<?php


/**
 * The Scratchpad profile.  This will replace the default profile so that all
 * sites installed will become Scratchpads.
 */

/**
 * Details about this module
 */
function scratchpad_training_profile_details(){
  return array(
    'name' => 'Scratchpad Training',
    'description' => 'This profile automatically installs everything, no user input is required.'
  );
}

/**
 * Modules that this profile would like installing
 */
function scratchpad_training_profile_modules(){
  require_once("./profiles/scratchpad/scratchpad.profile");
  return array_merge(scratchpad_profile_modules(), array('scratchpad_training'));
}

/**
 * Code for the tasks
 */
function scratchpad_training_profile_tasks(&$task, $url){
  require_once("./profiles/scratchpad/scratchpad.profile");
  if($task == 'profile'){
    scratchpad_profile_tasks_1();

    $node = new stdClass();
    $node->type = 'profile';
    $node->uid = 2;
    $node->field_title = array(array('value'=>'Mr/Mrs/Miss'));
    $node->field_givennames = array(array('value'=>'Scratchpad'));
    $node->field_familyname = array(array('value'=>'Trainer'));
    $node->title = "Mr/Mrs/Miss Scratchpad Trainee";
    $node->auto_nodetitle_applied = TRUE;  
    $node->field_institution = array(array('value'=>'My Institution'));
    $node->field_taxonomicinterest = array(array('value'=>'Life'));
    node_save($node);
    $values = array('values'=>array('gmapkey'=>'INSERT KEY HERE'));
    scratchpad_gmapkey_submit(array(), $values);
        
    variable_set('site_mission','This site has been created for the <a href="http://scratchpads.eu/training">Scratchpad Training Courses</a>');
  
    scratchpad_profile_tasks_2();
    
    // N.B. The following is normally executed by tasks_3, but we don't want an
    // automatic password, nor do we want the mail message being sent.
    db_query("UPDATE {users} SET pass = MD5('password') , status = 1 WHERE uid = 2");
    
    // Update the menu router information.
    drupal_rebuild_theme_registry();
    node_types_rebuild();
    menu_rebuild();
    cache_clear_all('schema', 'cache');
    $task = 'profile-finished';
  }
}