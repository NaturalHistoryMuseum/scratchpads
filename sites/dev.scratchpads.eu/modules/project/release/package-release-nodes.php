#!/usr/bin/php
<?php


/**
 * @file
 * Automated packaging script to generate tarballs from release nodes.
 *
 * @author Derek Wright (http://drupal.org/user/46549)
 */

// ------------------------------------------------------------
// Required customization
// ------------------------------------------------------------

// The root of your Drupal installation, so we can properly bootstrap
// Drupal. This should be the full path to the directory that holds
// your index.php file, the "includes" subdirectory, etc.
$drupal_root = '';

// The name of your site. Required so that when we bootstrap Drupal in
// this script, we find the right settings.php file in your sites folder.
// For example, on drupal.org:
// $site_name = 'drupal.org';
$site_name = '';

// Root of the temporary directory where you want packages to be
// made. Subdirectories will be created depending on the task.
$tmp_root = '';

// ------------------------------------------------------------
// Optional customization
// ------------------------------------------------------------

// ----------------
// File destination
// ----------------
// This assumes you want to install the packaged releases in the
// "files/projects" directory of your root Drupal installation. If
// that's not the case, you should customize these.
$dest_root = $drupal_root;
$dest_rel = 'files/projects';

// --------------
// External tools
// --------------
// If you want this program to always use absolute paths for all the
// tools it invokes, provide a full path for each one. Otherwise,
// the script will find these tools in your PATH.
$rm = '/bin/rm';
$php = '/usr/bin/php';

// If you are using project-release-create-history.php to generate XML release
// history files, if you include the full path to your copy of that script
// here, after all the packages are re(generated), this script will regenerate
// the XML release history files for any projects with new/updated releases.
$project_release_create_history = '';


// ------------------------------------------------------------
// Initialization
// (Real work begins here, nothing else to customize)
// ------------------------------------------------------------

// Check if all required variables are defined
$vars = array(
  'drupal_root' => $drupal_root,
  'site_name' => $site_name,
  'tmp_root' => $tmp_root,
);
foreach ($vars as $name => $val) {
  if (empty($val)) {
    print "ERROR: \"\$$name\" variable not set, aborting\n";
    $fatal_err = true;
  }
}
if (!empty($fatal_err)) {
  exit(1);
}

$script_name = $argv[0];

// Find what kind of packaging we need to do
if (!empty($argv[1])) {
  $task = $argv[1];
}
else {
  $task = 'tag';
}
switch($task) {
  case 'tag':
  case 'branch':
    break;

  default:
    print "ERROR: $argv[0] invoked with invalid argument: \"$task\"\n";
    exit (1);
}

$project_id = 0;
if (!empty($argv[2])) {
  $project_id = $argv[2];
}

// Setup variables for Drupal bootstrap
$_SERVER['HTTP_HOST'] = $site_name;
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
$_SERVER['REQUEST_METHOD']  = 'GET';
$_SERVER['REQUEST_URI'] = '/' . $script_name;
$_SERVER['SERVER_SOFTWARE'] = 'PHP CLI';
$_SERVER['QUERY_STRING']    = '';
$_SERVER['SCRIPT_NAME'] = '/' . $script_name;
$_SERVER['PHP_SELF'] = '/' . $script_name;
$_SERVER['SCRIPT_FILENAME'] = $_SERVER['PWD'] . '/' . $script_name;
$_SERVER['PATH_TRANSLATED'] = $_SERVER['SCRIPT_FILENAME'];


if (!chdir($drupal_root)) {
  print "ERROR: Can't chdir($drupal_root): aborting.\n";
  exit(1);
}

// Force the right umask while this script runs, so that everything is created
// with sane file permissions.
umask(0022);

require_once 'includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
// We have to initialize the theme() system before we leave $drupal_root
$hack = theme('placeholder', 'hack');

// Load the include file for packager-related helper functions.
module_load_include('inc', 'project_release', 'includes/packager');

initialize_tmp_dir($task);
package_releases($task, $project_id);
// Now that we're done, clean out the tmp/task dir we created
chdir($tmp_root);
drupal_exec("$rm -rf $tmp_dir");

if ($task == 'branch') {
  // Clear any cached data set to expire.
  cache_clear_all(NULL, 'cache_project_release');
}

// ------------------------------------------------------------
// Functions: main work
// ------------------------------------------------------------

function package_releases($type, $project_id = 0) {
  global $drupal_root, $dest_root, $dest_rel, $tmp_dir, $wd_err_msg;
  global $php, $project_release_create_history;

  if (!empty($project_id)) {
    if (is_numeric($project_id)) {
      $project_nid = $project_id;
    }
    else {
      $project_nid = db_result(db_query("SELECT nid FROM {project_projects} WHERE uri = '%s'", $project_id));
    }
    // We repeatedly clear the node_load() cache, but we have our own cache
    // for loading the project nodes since those tend to repeat and we need to
    // load less of them.
    $project_node = project_release_packager_node_load($project_nid);
    if (empty($project_node)) {
      wd_err('ERROR: Project ID %id not found', array('%id' => $project_id));
      return FALSE;
    }
  }

  $rel_node_join = '';
  $where_args = array();
  if ($type == 'tag') {
    $where = " AND (prn.rebuild = %d) AND (f.filepath IS NULL OR f.filepath = '')";
    $where_args[] = 0;  // prn.rebuild
    $plural = t('tags');
  }
  elseif ($type == 'branch') {
    $rel_node_join = " INNER JOIN {node} nr ON prn.nid = nr.nid";
    $where = " AND (prn.rebuild = %d) AND ((f.filepath IS NULL) OR (f.filepath = '') OR (nr.status = %d))";
    $where_args[] = 1;  // prn.rebuild
    $where_args[] = 1;  // nr.status
    $plural = t('branches');
    if (empty($project_node)) {
      wd_msg("Starting to package all snapshot releases.");
    }
    else {
      wd_msg("Starting to package snapshot releases for project id: %project_short_name.", array('%project_short_name' => $project_node->project['uri']), l(t('view'), 'node/' . $project_node->nid));
    }
  }
  else {
    wd_err("ERROR: package_releases() called with unknown type: %type", array('%type' => $type));
    return FALSE;
  }
  $args = array();
  $args[] = 1;    // Account for np.status = 1.
  $args[] = 1;    // Account for prp.releases = 1.
  if (!empty($project_node)) {
    $where .= ' AND prn.pid = %d';
    $where_args[] = $project_node->nid;
  }
  $args = array_merge($args, $where_args);
  $query = db_query("SELECT prn.nid FROM {project_release_nodes} prn $rel_node_join LEFT JOIN {project_release_file} prf ON prn.nid = prf.nid LEFT JOIN {files} f ON prf.fid = f.fid INNER JOIN {project_projects} pp ON prn.pid = pp.nid INNER JOIN {node} np ON prn.pid = np.nid INNER JOIN {project_release_projects} prp ON prp.nid = prn.pid WHERE np.status = %d AND prp.releases = %d " . $where . ' ORDER BY pp.uri', $args);

  $num_built = 0;
  $num_considered = 0;
  $project_nids = array();

  // Read everything out of the query immediately so that we don't leave the
  // query object/connection open while doing other queries.
  $releases = array();
  while ($release = db_fetch_object($query)) {
    // This query could pull multiple rows of the same release since multiple
    // files per release node are allowed. Account for this by keying on
    // release nid.
    $releases[$release->nid] = $release->nid;
  }
  foreach ($releases as $release_nid) {
    $wd_err_msg = array();

    // We don't want to waste too much RAM by leaving all these loaded nodes
    // in RAM, so we reset the node_load() cache each time we call it.
    $release_node = node_load($release_nid, NULL, TRUE);
    if (empty($release_node)) {
      wd_err("ERROR: Can't load release node for release ID %nid", array('%nid' => $release_nid));
      continue;
    }

    $packager = project_release_get_packager_plugin($release_node, $dest_root, $dest_rel, $tmp_dir);
    if (empty($packager)) {
      wd_err("ERROR: Can't find packager plugin to use for %release", array('%release' => $release_node->title));
      continue;
    }

    db_query("DELETE FROM {project_release_package_errors} WHERE nid = %d", $release_node->nid);

    chdir($drupal_root);
    $files = array();
    $contents = array();
    $rval = $packager->createPackage($files, $contents);
    $num_considered++;
    chdir($drupal_root);

    switch ($rval) {
      case 'success':
      case 'rebuild':
        project_release_packager_update_node($release_node, $dest_root, $files, $contents);
        module_invoke_all('project_release_create_package', $project_node, $release_node);
        $num_built++;
        $packager->cleanupSuccessfulBuild();
        $release_pid = $release_node->project_release['pid'];
        $project_nids[$release_pid] = TRUE;
        $release_node_view_link = l(t('View'), 'node/' . $release_node->nid);
        if ($rval == 'rebuild') {
          $msg = '%release_title has changed, re-packaged.';
        }
        else {
          $msg = 'Packaged %release_title.';
        }
        wd_msg($msg, array('%release_title' => $release_node->title), $release_node_view_link);
        break;

      case 'error':
        $packager->cleanupFailedBuild();
        break;

    }

    if (count($wd_err_msg)) {
      db_query("INSERT INTO {project_release_package_errors} (nid, messages) values (%d, '%s')", $release_node->nid, serialize($wd_err_msg));
    }
  }

  if ($num_built || $type == 'branch') {
    if (!empty($project_node)) {
      wd_msg("Done packaging releases for @project_short_name from !plural: !num_built built, !num_considered considered.", array('@project_short_name' => $project_node->project['uri'], '!plural' => $plural, '!num_built' => $num_built, '!num_considered' => $num_considered));
    }
    else {
      wd_msg("Done packaging releases from !plural: !num_built built, !num_considered considered.", array('!plural' => $plural, '!num_built' => $num_built, '!num_considered' => $num_considered));
    }
  }

  // Finally, regenerate release history XML files for all projects we touched.
  if (!empty($project_nids) && !empty($project_release_create_history)) {
    wd_msg('Re-generating release history XML files');
    $i = $fails = 0;
    foreach ($project_nids as $project_nid => $value) {
      if (drupal_exec("$php $project_release_create_history $project_nid")) {
        $i++;
      }
      else {
        $fails++;
      }
    }
    if (!empty($fails)) {
      wd_msg('ERROR: Failed to re-generate release history XML files for !num project(s)', array('!num' => $fails));
    }
    wd_msg('Done re-generating release history XML files for !num project(s)', array('!num' => $i));
  }
}

// ------------------------------------------------------------
// Functions: utility methods
// ------------------------------------------------------------

/**
 * Wrapper for exec() that logs errors to the watchdog.
 * @param $cmd
 *   String of the command to execute (assumed to be safe, the caller is
 *   responsible for calling escapeshellcmd() if necessary).
 * @return true if the command was successful (0 exit status), else false.
 */
function drupal_exec($cmd) {
  // Made sure we grab stderr, too...
  exec("$cmd 2>&1", $output, $rval);
  if ($rval) {
    wd_err("ERROR: %cmd failed with status !rval" . '<pre>' . implode("\n", array_map('htmlspecialchars', $output)), array('%cmd' => $cmd, '!rval' => $rval));
    return false;
  }
  return true;
}

/**
 * Wrapper for chdir() that logs errors to the watchdog.
 * @param $dir Directory to change into.
 * @return true if the command was successful (0 exit status), else false.
 */
function drupal_chdir($dir) {
  if (!chdir($dir)) {
    wd_err("ERROR: Can't chdir(@dir)", array('@dir' => $dir));
    return false;
  }
  return true;
}

/// TODO: remove this before the final script goes live -- debugging only.
function wprint($var) {
  watchdog('package_debug', '<pre>' . var_export($var, TRUE));
}

/**
 * Wrapper function for watchdog() to log notice messages. Uses a
 * different watchdog message type depending on the task (branch vs. tag).
 */
function wd_msg($msg, $variables = array(), $link = NULL) {
  global $task;
  watchdog('package_' . $task, $msg, $variables, WATCHDOG_NOTICE, $link);
  echo t($msg, $variables) . "\n";
}

/**
 * Wrapper function for watchdog() to log error messages.
 */
function wd_err($msg, $variables = array(), $link = NULL) {
  global $wd_err_msg;
  if (!isset($wd_err_msg)) {
    $wd_err_msg = array();
  }
  watchdog('package_error', $msg, $variables, WATCHDOG_ERROR, $link);
  echo t($msg, $variables) . "\n";
  $wd_err_msg[] = t($msg, $variables);
}

/**
 * Initialize the tmp directory. Use different subdirs for building
 * snapshots than official tags, so there's no potential directory
 * collisions and race conditions if both are running at the same time
 * (due to how long it takes to complete a branch snapshot run, and
 * how often we run this for tag-based releases).
 */
function initialize_tmp_dir($task) {
  global $tmp_dir, $tmp_root, $rm;

  if (!is_dir($tmp_root) && !@mkdir($tmp_root, 0777, TRUE)) {
    wd_err("ERROR: mkdir(@dir) (tmp_root) failed", array('@dir' => $tmp_root));
    exit(1);
  }

  // Use a tmp directory *specific* to this invocation, so that we don't
  // clobber other runs if the script is invoked twice (e.g. via cron and
  // manually, etc).
  $tmp_dir = $tmp_root . '/' . $task . '.' . getmypid();
  if (is_dir($tmp_dir)) {
    // Make sure we start with a clean slate
    drupal_exec("$rm -rf $tmp_dir/*");
  }
  else if (!@mkdir($tmp_dir, 0777, TRUE)) {
    wd_err("ERROR: mkdir(@dir) failed", array('@dir' => $tmp_dir));
    exit(1);
  }
}
