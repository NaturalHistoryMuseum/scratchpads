<?php


/**
 * The Scratchpad profile.  This will replace the default profile so that all
 * sites installed will become Scratchpads.
 */

/**
 * Details about this module
 */
function sandbox_profile_details(){
  return array(
    'name' => 'Scratchpad Sandbox',
    'description' => 'Provides the Scratchpad Sandbox.'
  );
}

/**
 * Modules that this profile would like installing
 */
function sandbox_profile_modules(){
  require_once("./profiles/scratchpad/scratchpad.profile");
  return array_merge(scratchpad_profile_modules(), array('sandbox'));
}

/**
 * Code for the tasks
 */
function sandbox_profile_tasks(&$task, $url){
  require_once("./profiles/scratchpad/scratchpad.profile");
  if($task == 'profile'){
    scratchpad_profile_tasks_1();
    
    db_query("INSERT INTO {profile_values} (fid, uid, value) VALUES 
      (1,2,'Mr/Mrs/Miss'), 
      (2,2,'Scratchpad'), 
      (3,2,'Trainer'),
      (4,2,'My Institution'),
      (5,2,'Life')");
    $values = array('values'=>array('gmapkey'=>'INSERT KEY HERE'));
    scratchpad_gmapkey_submit(array(), $values);
        
    variable_set('site_mission','This site has been created for the <a href="http://scratchpads.eu/training">Scratchpad Training Courses</a>');
  
    scratchpad_profile_tasks_2();
    
    // N.B. The following is normally executed by tasks_3, but we don't want an
    // automatic password, nor do we want the mail message being sent.
    db_query("UPDATE {users} SET pass = MD5('password') , status = 1 WHERE uid = 2");
    
    // Update the menu router information.
    menu_rebuild();    
    $task = 'profile-finished';
  }
}