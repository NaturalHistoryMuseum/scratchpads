<?php
// Increase the timelimit, as this profile is SSSSLLLLOOOOWWWWW!
// Note, this is done outside any functions, to ensure that it is executed when
// the profile is loaded.
set_time_limit(300);
// Load the profile_functions.inc file
require_once (dirname(__FILE__) . '/profile_functions.inc');
// Load the forms (even though they may not be required).
require_once (dirname(__FILE__) . '/profile_forms.inc');

/**
 * The Scratchpad profile.  This will replace the default profile so that all
 * sites installed will become Scratchpads.
 * 
 * Profile name: "scratchpad_profile".
 * 
 * Note, this profile was renamed to avoid a conflict with the module of the
 * same name.
 */
/**
 * Details about this module
 * 
 * Implementation of hook_profile_details
 */
function scratchpad_profile_profile_details(){
  return array(
    'name' => 'Scratchpad',
    'description' => 'Select this profile to enable a whole suite of modules to make entering biodiversity and taxonomic information on to your site easier.'
  );
}

/**
 * Modules that this profile would like installing
 * 
 * Implementation of hook_profile_modules
 */
function scratchpad_profile_profile_modules(){
  return array(
    'blog',
    'color',
    'comment',
    'contact',
    'locale',
    'dblog',
    'help',
    'menu',
    'openid',
    'path',
    'poll',
    'search',
    'taxonomy',
    'trigger',
    'upload',
    'forum',
    'translation',
    'tracker',
    'biblio',
    'boost',
    'node_import',
    'creativecommons_lite',
    'simplenews',
    'advanced_help',
    'auto_nodetitle',
    'checkbox_validate',
    'clone',
    'modalframe',
    'globalredirect',
    'legal',
    'path_redirect',
    'pathauto',
    'quote',
    'statistics',
    'roleassign',
    'search_files',
    'thickbox',
    'token',
    'vertical_tabs',
    'weight',
    'jstools',
    'wysiwyg',
    'print',
    'imce',
    'imce_wysiwyg',
    'ajax',
    'mollom',
    'hashcash',
    'jquery_update',
    'jquery_ui',
    'content',
    'number',
    'optionwidgets',
    'text',
    'date_api',
    'date',
    'date_popup',
    'nodereference',
    'date_timezone',
    'fieldgroup',
    'date_php4',
    'nodereferrer',
    'noderelationships',
    'imagecache',
    'imageapi',
    'imageapi_imagemagick',
    'filefield',
    'imagefield',
    'lightbox2',
    'plupload',
    'location',
    'gmap',
    'gmap_location',
    'location_user',
    'location_node',
    'messaging',
    'notifications',
    'notifications_lite',
    'notifications_content',
    'notifications_autosubscribe',
    'messaging_mail',
    'og',
    'og_access',
    'og_user_roles',
    'og_views',
    'og_notifications',
    'og_forum',
    'views',
    'views_ui',
    'views_xml',
    'content_profile',
    'content_profile_registration',
    'webform',
    'countriesmap',
    'citation',
    'backup',
    'batax',
    'ispecies',
    'bhl',
    'darwincore',
    'fixperms',
    'flickr',
    'gbifmap',
    'googlescholar',
    'lowername',
    'mado',
    'ncbi',
    'node_term_edit',
    'autotag',
    'nbnmap',
    'foundation',
    'content_taxonomy',
    'content_taxonomy_autocomplete',
    'taxtab',
    'tinytax',
    'morphbank',
    'view_sort',
    'wikipedia',
    'yahooimages',
    'scratchpadify',
    'scratchpadify_help',
    'tablesorter',
    'nagger',
    'itis_term',
    'term_node',
    'spm',
    'taxonomy_tree',
    'ahah_action',
    'matrix_editor',
    'nexus',
    'tree',
    'femail',
    'term_node',
    'scratchpad',
    'tui',
    'classification_import',
    'csv_import',
    'tcs_import',
    'eol_import',
    'publication',
    'nodereferencebiblio',
    'nodereferencethumb',
    'aggregator',
    'uwho_client',
    'remote_issue_tab',
    'comment_upload',
    'commentmail',
    'beautytips',
    'slickgrid',
    'views_batch_page',
    'ajax_load',
    'editor_views',
    'scratchpad_slickgrid',
    'apachesolr',
    'apachesolr_search',
    'apachesolr_multisitesearch'
  );
}

/**
 * Additional taks
 * 
 * Implementation of hook_profile_task_list
 */
function scratchpad_profile_profile_task_list(){
  return array(
    'personal' => st('Personal information'),
    'gmapkey' => st('Google Maps API Key'),
    'clustrmap' => st('ClustrMaps HTML Code'),
    'mission' => st('Mission statement')
  );
}

/**
 * Pull out all the functions into the following numbered functions so that the
 * scratchpad_training module can reuse them.
 */
function scratchpad_profile_profile_tasks_1(){
  scratchpad_profile_create_content_types();
  scratchpad_profile_profile_install_profile();
}

function scratchpad_profile_profile_tasks_2(){
  scratchpad_profile_set_settings();
  scratchpad_profile_set_conditions();
  scratchpad_profile_set_issue_feed('Scratchpads Issue Queue', 'http://dev.scratchpads.eu/project/issues/rss/scratchpads');
  scratchpad_profile_profile_set_perms();
  scratchpad_profile_set_blocks();
  scratchpad_profile_tweak_menu();
}

/**
 * Code for the tasks
 * 
 * Implementation of hook_profile_tasks
 */
function scratchpad_profile_profile_tasks(&$task, $url){
  if($task == 'profile'){
    $task = 'personal';
    scratchpad_profile_profile_tasks_1();
  }
  $data = array();
  $output = scratchpad_profile_do_tasks($task, $url, $data);
  if($output){
    return $output;
  }
  if($task == 'scratchpadcleanup'){
    scratchpad_profile_profile_tasks_2();
    // The above function moves the UID 1 to UID 2.  Now we can update that user
    // if we're installing user Aegir.  This means the next funciton will email
    // the right person! WOOT!
    if(is_array($data) && count($data)){
      db_query("UPDATE {users} SET mail = '%s', name = '%s' WHERE uid = 2", $data['client_email'], $data['fullname']);
    }
    scratchpad_profile_set_theme('garland');
    $task = 'profile-finished';
  }
}