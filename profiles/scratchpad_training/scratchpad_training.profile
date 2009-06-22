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

function scratchpad_training_form_alter(&$form, $form_state, $form_id){
  if($form_id == 'install_configure_form'){
    $form_state = array (
      'storage' => NULL, 
      'submitted' => true, 
      'values' => array (
        'site_name' => 'Drupal6 Too', 
        'site_mail' => 'scratchpad@nhm.ac.uk', 
        'account' => array (
          'name' => 'user',
          'mail' => 'scratchpad@nhm.ac.uk', 
          'pass' => 'password' ), 
        'date_default_timezone' => '3600', 
        'clean_url' => '1', 
        'update_status_module' => array (
          1 => 1 ), 
        'op' => 'Save and continue', 
        'submit' => 'Save and continue', 
        'form_build_id' => 'form-efd87430cbc3216b5485d28cecbd6975', 
        'form_id' => 'install_configure_form', 
        'hashcash' => '1:090529:install_configure_form:157.140.4.52:.drupal6too:rCtWOHrU7aIWdsuRG0Ei' ), 
      'clicked_button' => array (
        '#type' => 'submit', 
        '#value' => 'Save and continue', 
        '#weight' => 15, 
        '#post' => array (
          'site_name' => 'Drupal6 Too', 
          'site_mail' => 'scratchpad@nhm.ac.uk',
          'account' => array (
            'name' => 'user',
            'mail' => 'scratchpad@nhm.ac.uk', 
            'pass' => 'password' ),  
          'date_default_timezone' => '3600', 
          'clean_url' => '1', 
          'update_status_module' => array (
            1 => '1' ), 
          'form_build_id' => 'form-3d6dc9ab849bed34ff077b89da1217ed', 
          'form_id' => 'install_configure_form', 
          'hashcash' => '1:090529:install_configure_form:157.140.4.52:.drupal6too:rCtWOHrU7aIWdsuRG0Ei', 
          'op' => 'Save and continue' ), 
        '#programmed' => false, 
        '#tree' => false, 
        '#parents' => array (
          0 => 'submit' ), 
        '#array_parents' => array (
          0 => 'submit' ), 
        '#processed' => false, 
        '#description' => NULL, 
        '#attributes' => array (), 
        '#required' => false, 
        '#input' => true, 
        '#name' => 'op', 
        '#button_type' => 'submit', 
        '#executes_submit_callback' => true, 
        '#process' => array (
                         
          0 => 'form_expand_ahah' ), 
        '#id' => 'edit-submit' ), 
      'redirect' => NULL );
    scratchpadify_install_configure_form_submit($form, $form_state);
  }
}