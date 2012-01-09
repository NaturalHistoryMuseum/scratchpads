<?php

/**
 * @file Code required during release node packaging.
 *
 * In addition to some shared helper functions, this file defines the
 * interface that all CTools release_packager plugins must implement.
 *
 */

interface ProjectReleasePackagerInterface {
  /**
   * Constructor for a packager plugin class.
   *
   * @param stdClass $release_node
   *   The fully-loaded release node to create package(s) for.
   * @param string $file_destination_root
   *   The full path to the root of the directory tree where package files
   *   should be written. Often this is the same as the site root directory.
   * @param string $file_destination_subdirectory
   *   The subdirectory under $file_destination_root where the package files
   *   should be written. For example, 'files/projects', or
   *   'sites/default/files/projects'.
   * @param string $temp_directory
   *   The full path to a temporary directory to use during packaging.
   */
  public function __construct($release_node, $file_destination_root, $file_destination_subdir, $temp_directory);

  /**
   * Create package(s) for the given release.
   *
   * @param array $files
   *   Reference to an associative array of files created for this release.
   *   The keys are the filename (relative to the root of the Drupal
   *   installation) and the values are the {project_release_files}.weight for
   *   each file.
   * @param array $contents
   *   Reference to an array of contents included in this release. This is
   *   optional, and is only used if the project_package module is enabled.
   *   The keys are ignored, and the values should be node IDs of other
   *   release nodes packaged with this release.
   *
   * @return string
   *   The results of the packaging attempt. Must be one of:
   *   - 'success': Everything worked and new package(s) were created
   *   - 'rebuilt': Everything worked and package(s) were rebuilt
   *   - 'error': There was an error generating package(s)
   *   - 'no-op': There was nothing to do to package the given release
   */
  public function createPackage(&$files, &$contents);

  /**
   * Clean up the results of a failed packaging build.
   *
   * This function will be invoked if createPackage() returns 'error'.
   */
  public function cleanupFailedBuild();

  /**
   * Clean up the results of a successful packaging build.
   *
   * This function will be invoked if createPackage() returns anything other
   * than 'error'.
   */
  public function cleanupSuccessfulBuild();
}

/**
 * Find the approriate packager CTools plugin for the given release.
 *
 * First, we see if $release_node->project_release['packager_plugin'] is
 * defined, and if so, we use that. Otherwise, we load the project the
 * release is attached to, and check in the project node to see if
 * $project_node->project_release['packager_plugin'] is defined. Then, we see
 * if there's a project-type-specific site-wide packager plugin variable
 * defined. Finally, we look for a global site-wide default plugin variable.
 *
 * Once we find the appropriate plugin name to use, we ask CTools to give us
 * the right class to use for that plugin, and try to instantiate an object.
 *
 * @param $release_node
 *   The fully-loaded release node to find the packaging plugin for.
 * @param string $file_destination_root
 *   The full path to the root of the directory tree where package files
 *   should be written. Often this is the same as the site root directory.
 * @param string $file_destination_subdirectory
 *   The subdirectory under $file_destination_root where the package files
 *   should be written. For example, 'files/projects', or
 *   'sites/default/files/projects'.
 * @param string $temp_directory
 *   The full path to a temporary directory to use during packaging.
 * @return
 *   An instantiated packager plugin for the given release node, or FALSE if
 *   we couldn't find or instantiate the right plugin.
 */
function project_release_get_packager_plugin($release_node, $file_destination_root, $file_destination_subdir, $temp_directory) {
  $packager_plugin = '';
  // If this particular release has its own plugin defined, use it.
  if (!empty($release_node->project_release['packager_plugin'])) {
    $packager_plugin = $release_node->project_release['packager_plugin'];
  }
  else {
    $project_node = project_release_packager_node_load($release_node->project_release['pid']);
    // If this particular project has its own plugin defined, use it.
    if (!empty($project_node->project_release['packager_plugin'])) {
      $packager_plugin = $project_node->project_release['packager_plugin'];
    }
    // Otherwise, look for a project-type-specific pluging variable.
    elseif (project_use_taxonomy()) {
      $project_types = project_get_project_types();
      if (!empty($project_node->taxonomy)) {
        foreach ($project_node->taxonomy as $tid => $term) {
          if (!empty($project_types[$tid])) {
            $packager_plugin = variable_get('project_release_packager_plugin_' . $tid, '');
            break;
          }
        }
      }
    }
    // If we still don't know the plugin to use, try the site-wide default.
    if (empty($packager_plugin)) {
      $packager_plugin = variable_get('project_release_packager_plugin', '');
    }
  }
  if (!empty($packager_plugin)) {
    ctools_include('plugins');
    if ($class = ctools_plugin_load_class('project_release', 'release_packager', $packager_plugin, 'packager')) {
      // Make sure the plugin implements the right interface.
      if (in_array('ProjectReleasePackagerInterface', class_implements($class, TRUE))) {
        return new $class($release_node, $file_destination_root, $file_destination_subdir, $temp_directory);
      }
    }
  }
  return FALSE;
}

/**
 * Update the DB with the new file info for a given release node.
 *
 * @param $release_node
 *   The fully loaded release node to update.
 * @param $file_destination_directory
 *   The full path to the directory where packaged release files are stored.
 * @param $files
 *   Array of files to add to the release node. The keys are filenames, and
 *   the values are the weight for the {project_release_file}.weight column.
 * @param $package_contents
 *   Optional. Array of nids of releases contained in a release package.
 */
function project_release_packager_update_node($release_node, $file_destination_directory = '', $files = array(), $package_contents = array()) {

  // If the site is using DB replication, force this node_load() to use the
  // primary database to avoid node_load() failures.
  if (function_exists('db_set_ignore_slave')) {
    db_set_ignore_slave();
  }

  if (!empty($files)) {
    // PHP will cache the results of stat() and give us stale answers
    // here, unless we manually tell it otherwise!
    clearstatcache();

    foreach ($files as $file_path => $file_weight) {
      // Compute the metadata for this file that we care about.
      $full_path = $file_destination_directory . '/' . $file_path;
      $file_name = basename($file_path);
      $file_date = filemtime($full_path);
      $file_size = filesize($full_path);
      $file_hash = md5_file($full_path);
      $file_mime = file_get_mimetype($full_path);

      // First, see if we already have this file for this release node
      $file_data = db_fetch_object(db_query("SELECT prf.* FROM {project_release_file} prf INNER JOIN {files} f ON prf.fid = f.fid WHERE prf.nid = %d AND f.filename = '%s'", $release_node->nid, $file_name));

      // Insert or update the record in the DB as need.
      if (empty($file_data)) {
        // Don't have this file, insert a new record.
        db_query("INSERT INTO {files} (uid, filename, filepath, filemime, filesize, status, timestamp) VALUES (%d, '%s', '%s', '%s', %d, %d, %d)", $release_node->uid, $file_name, $file_path, $file_mime, $file_size, FILE_STATUS_PERMANENT, $file_date);
        $fid = db_last_insert_id('files', 'fid');
        db_query("INSERT INTO {project_release_file} (fid, nid, filehash, weight) VALUES (%d, %d, '%s', %d)", $fid, $release_node->nid, $file_hash, $file_weight);
      }
      else {
        // Already have this file for this release, update it.
        db_query("UPDATE {files} SET uid = %d, filename = '%s', filepath = '%s', filemime = '%s', filesize = %d, status = %d, timestamp = %d WHERE fid = %d", $release_node->uid, $file_name, $file_path, $file_mime, $file_size, FILE_STATUS_PERMANENT, $file_date, $file_data->fid);
        db_query("UPDATE {project_release_file} SET filehash = '%s', weight = %d WHERE fid = %d", $file_hash, $file_weight, $file_data->fid);
      }
    }
  }

  // Store package contents if necessary.
  if (!empty($package_contents) && module_exists('project_package')) {
    foreach ($package_contents as $item_nid) {
      db_query("INSERT INTO {project_package_local_release_item} (package_nid, item_nid) VALUES (%d, %d)", $release_node->nid, $item_nid);
    }
  }

  // Don't auto-publish security updates.
  $security_update_tid = variable_get('project_release_security_update_tid', 0);
  if (empty($release_node->project_release['rebuild']) && !empty($release_node->taxonomy[$security_update_tid])) {
    watchdog('package_security', 'Not auto-publishing security update release.', array(), WATCHDOG_NOTICE, l(t('view'), 'node/' . $release_node->nid));
    return;
  }

  // Finally publish the node if it is currently unpublished. Instead of
  // directly updating {node}.status, we use node_save() so that other modules
  // which implement hook_nodeapi() will know that this node is now published.
  if (empty($release_node->status)) {
    $release_node->status = 1;
    node_save($release_node);
  }
}

/**
 * Load a node.
 *
 * This function maintains its own static cache of loaded nodes.
 *
 * @param integer $nid
 *   The node ID of the node to load.
 *
 * @return
 *   The fully-loaded node for the given node ID.
 *
 * @see node_load()
 */
function project_release_packager_node_load($nid) {
  static $nodes = array();
  if (empty($nodes[$nid])) {
    $nodes[$nid] = node_load($nid);
  }
  return $nodes[$nid];
}
