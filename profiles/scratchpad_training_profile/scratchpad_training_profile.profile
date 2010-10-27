<?php


/**
 * The Scratchpad profile.  This will replace the default profile so that all
 * sites installed will become Scratchpads. 
 * 
 * Profile name: "scratchpad_training_profile".
 * 
 * Note, this profile was renamed to avoid a conflict with the module of the
 * same name.
 */

/**
 * Details about this module
 * 
 * hook_profile_details
 */
function scratchpad_training_profile_profile_details(){
  return array(
    'name' => 'Scratchpad Training',
    'description' => 'This profile automatically installs everything, no user input is required.'
  );
}

/**
 * Modules that this profile would like installing
 * 
 * hook_profile_modules
 */
function scratchpad_training_profile_profile_modules(){
  require_once("./profiles/scratchpad_profile/scratchpad_profile.profile");
  return array_merge(scratchpad_profile_profile_modules(), array('scratchpad_training'));
}

/**
 * Code for the tasks
 * 
 * hook_profile_tasks
 */
function scratchpad_training_profile_profile_tasks(&$task, $url){
  require_once("./profiles/scratchpad_profile/scratchpad_profile.profile");
  if($task == 'profile'){
    scratchpad_profile_profile_tasks_1();

    // Set the last reported variable, so that this site doesn't
    // get included in the sites list.
    variable_set('scratchpad_last_reported',10000000000000000000);

    // Set the title of the site.
    $profile_details = scratchpad_training_profile_profile_details();
    variable_set('site_name', $profile_details['name']);

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
  
    scratchpad_profile_profile_tasks_2();
    // Change the cache back to "disabled"
    variable_set('cache', 0);
    
    // N.B. The following is normally executed by tasks_3, but we don't want an
    // automatic password, nor do we want the mail message being sent.
    db_query("UPDATE {users} SET pass = MD5('password') , status = 1 WHERE uid = 2");
    
    // Update for Aegir to ensure the UID 2 is correct, and site name/email.
    variable_set('site_name', 'Scratchpad Training');
    variable_set('site_mail', 'scratchpad@nhm.ac.uk');
    db_query("UPDATE {users} SET mail = 'scratchpad@nhm.ac.uk', name = 'username', password = MD5('password') WHERE uid = 2");
    
    // Add the special training site block
    // Note, this should probably get added automatically, but something
    // is screwy about the site.
    db_query("INSERT INTO {blocks} (module, delta, theme, status, weight, region, cache) VALUES ('scratchpad_training', 'pointless_string', 'garland', 1, -100, 'left', 8)");
    scratchpad_profile_profile_tasks_4();
    $task = 'profile-finished';
  }
}
