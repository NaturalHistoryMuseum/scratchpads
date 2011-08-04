<?php
// Increase the timelimit, as this profile is SSSSLLLLOOOOWWWWW!
// Note, this is done outside any functions, to ensure that it is executed when
// the profile is loaded.
set_time_limit(300);
// Load the emonocot functions, and all the scratchpad_profile files.
require_once (dirname(__FILE__) . '/profile_functions.inc');
require_once (dirname(__FILE__) . '/../scratchpad_profile/profile_functions.inc');
require_once (dirname(__FILE__) . '/../scratchpad_profile/scratchpad_profile.profile');
require_once (dirname(__FILE__) . '/../scratchpad_profile/profile_forms.inc');

/**
 * The emonocot profile.
 * 
 * Profile name: "emonocot".
 * 
 * Note, this profile was renamed to avoid a conflict with the module of the
 * same name.
 */
/**
 * Details about this module
 * 
 * Implementation of hook_profile_details
 */
function emonocot_profile_profile_details(){
  return array(
    'name' => 'eMonocot Scratchpad',
    'description' => 'Provides a customised Scratchpad for the eMonocot project.'
  );
}

/**
 * Modules that this profile would like installing
 * 
 * Implementation of hook_profile_modules
 */
function emonocot_profile_profile_modules(){
  $modules_to_disable = array(
    'blog',
    'color',
    'simplenews',
    'poll',
    'webform',
    'nexus',
    'tree',
    'publication',
  );
  $emonocot_modules = array();
  $scratchpad_modules = scratchpad_profile_profile_modules();
  foreach($scratchpad_modules as $module){
    if(!in_array($module, $modules_to_disable)){
      $emonocot_modules[] = $module;
    }
  }
  $emonocot_modules[] = 'emonocot';
  $emonocot_modules[] = 'taxonlist';
  $emonocot_modules[] = 'wcm_import';
  return $emonocot_modules;
}

/**
 * Implementation of hook_profile_task_list
 */
function emonocot_profile_profile_task_list(){
  return scratchpad_profile_profile_task_list();
}

/**
 * Pull out all the functions into the following numbered functions so that the
 * scratchpad_training module can reuse them.
 */
function emonocot_profile_profile_tasks_1(){
  //Set variable to say this is an eMonocot Scratchpad
  variable_set('emonocot_is_emonocot_site', TRUE);
  scratchpad_profile_profile_tasks_1();
  // Rename "Page" content type to "Static Page"
  db_query("UPDATE {node_type} SET name = 'Static Page' WHERE type = 'page'");
}

function emonocot_profile_profile_tasks_2(){
  scratchpad_profile_set_settings();
  emonocot_profile_set_conditions();
  scratchpad_profile_set_issue_feed('eMonocot Scratchpads Issue Queue', 'http://dev.scratchpads.eu/project/issues/rss/emonocot');
  variable_set('remote_issue_tab_redirect_path', 'project/issues/emonocot');
  scratchpad_profile_profile_set_perms();
  emonocot_profile_set_blocks();
  emonocot_profile_tweak_menu();
  emonocot_profile_apache_solr();
}
/**
 * Code for the tasks
 * 
 * Implementation of hook_profile_tasks
 */
function emonocot_profile_profile_tasks(&$task, $url){
  if($task == 'profile'){
    $task = 'personal';
    emonocot_profile_profile_tasks_1();
  }
  $data = array();
  $output = scratchpad_profile_do_tasks($task, $url, $data);
  if($output){
    return $output;
  }
  if($task == 'scratchpadcleanup'){
    emonocot_profile_profile_tasks_2();
    // The above function moves the UID 1 to UID 2.  Now we can update that user
    // if we're installing user Aegir.  This means the next funciton will email
    // the right person! WOOT!
    if(is_array($data) && count($data)){
      db_query("UPDATE {users} SET mail = '%s', name = '%s' WHERE uid = 2", $data['client_email'], $data['fullname']);
    }
    scratchpad_profile_set_theme('emonocot_sp');
    $task = 'profile-finished';
  }
}