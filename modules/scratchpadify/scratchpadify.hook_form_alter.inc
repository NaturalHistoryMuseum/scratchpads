<?php
function _scratchpadify_form_alter(&$form, &$form_state, $form_id){
  if(isset($form['og_nodeapi'])){
    $form['og_nodeapi']['#type'] = 'fieldset';
    $form['og_nodeapi']['#title'] = t('Access');
  }
  // Prevent displaying of admin user on user list.
  if($form_id == 'block_admin_display_form'){
    unset($form['scratchpadify_4']);
  }
  if($form_id == 'node_import_add_form'  && isset($_GET['type']) && !$form_state['submitted']){
    $form_state = array (
      'process_input' => TRUE,
      'storage' => array ( 
        'page' => 'file', 
        'back_button' => 'Back', 
        'reload_button' => 'Reload page', 
        'reset_button' => 'Reset page', 
        'op' => 'Reload page', 
        'next_button' => 'Next',  
        'form_id' => 'node_import_add_form', 
        'type' => 'node:'.$_GET['type'], 
        'file_upload' => '', 
        'file_upload_button' => 'Upload file', 
      ), 
      'submitted' => true, 
      'values' => array ( 
        'file_upload' => '', 
        'file_upload_button' => 'Upload file', 
        'back_button' => 'Back', 
        'op' => 'Reload page', 
        'reload_button' => 'Reload page', 
        'reset_button' => 'Reset page', 
        'next_button' => 'Next', 
        'form_id' => 'node_import_add_form', 
      ), 
      'clicked_button' => array ( 
        '#type' => 'submit',
        '#value' => 'Reload page',
        '#submit' => array ( 
          0 => 'node_import_add_form_submit_reload', 
        ), 
        '#disabled' => false, 
        '#post' => array ( 
          'op' => 'Reload page', 
          'form_id' => 'node_import_add_form', 
        ), 
        '#programmed' => false, 
        '#tree' => false, 
        '#parents' => array ( 
          0 => 'reload_button', 
        ), 
        '#array_parents' => array ( 
          0 => 'buttons-top', 
          1 => 'reload_button', 
        ), 
        '#weight' => 0.001, 
        '#processed' => false, 
        '#description' => NULL, 
        '#attributes' => array ( ), 
        '#required' => false, 
        '#input' => true, 
        '#name' => 'op', 
        '#button_type' => 'submit', 
        '#executes_submit_callback' => true, 
        '#process' => array ( 
          0 => 'form_expand_ahah', 
        ), 
        '#id' => 'edit-reload-button-1', 
      ), 
      'submit_handlers' => array ( 
        0 => 'node_import_add_form_submit_reload', 
      ), 
      'redirect' => NULL, 
      'rebuild' => true, 
    );
    unset($_GET['type']);
  }
  else if($form_id == 'user_admin_account'){
    $form_parts = array('status','name','roles','member_for','last_access','operations');
    foreach($form_parts as $form_part){
      unset($form[$form_part][1]);
    }
    unset($form['accounts']['#options'][1]);
  }
  // Prevent editing admin user
  else if($form_id == 'user_profile_form'){
    if($form['#uid'] == 1){
      drupal_set_message(t('You can not edit the admin user'),'error');
      if(isset($_GET['destination'])){
        drupal_goto($_GET['destination']);
      } else {
        drupal_goto(referer_uri());
      }      
    }
  }
  else if($form['#id'] == 'node-form'){
    if(isset($form['attachments']['wrapper']['new']['upload']['#description'])){
      global $user;
      $limits = _upload_file_limits($user);
      global $base_url;
      $form['attachments']['wrapper']['new']['upload']['#description'] .= " ".t("You are currently using <b>!using</b> of your <b>!quota</b> quota. If you would like this quota increasing, please <a href=\"mailto:scratchpad@nhm.ac.uk?subject=Increase upload quota&body=Scratchpad Administrators! Please can you increase the quota for !user on <!site>.\">mail</a> us.", array('!using' => format_size(upload_space_used($user->uid)), '!quota' => format_size($limits['user_size']), '!user' => print_r($user->name, true), '!site' => $base_url));
    }
    if($form['type']['#value'] == 'image'){
      $vid = variable_get('image_gallery_nav_vocabulary',0);
      if(is_array($form['taxonomy'][$vid]) && (!isset($form['taxonomy'][$vid]['#description']) || trim($form['taxonomy'][$vid]['#description']) == '')){
        $vid = variable_get('image_gallery_nav_vocabulary', FALSE);
        if($vid){
          $form['taxonomy'][$vid]['#description'] = l('Add a new image gallery', 'admin/content/taxonomy/'.$vid.'/add/term') . ".  (".t('N.B. Any changes you have made on this page will be lost').")";
        }
      }
    }
    else if($form['type']['#value'] == 'biblio'){
      if(isset($form['contributors2_wrapper'])){
        $form['contributors2_wrapper']['#collapsed'] = 1;
      }
      if(isset($form['contributors3_wrapper'])){
        $form['contributors3_wrapper']['#collapsed'] = 1;
      }
      if(isset($form['contributors4_wrapper'])){
        $form['contributors4_wrapper']['#collapsed'] = 1;
      }
      if(isset($form['contributors5_wrapper'])){
        $form['contributors5_wrapper']['#collapsed'] = 1;
      }
    }
  }
  else if($form_id == 'taxonomy_form_vocabulary'){
    if(!isset($form['vid']) && isset($form['settings']['multiple']['#default_value'])){
      // Set default value of multiple to checked.
      $form['settings']['multiple']['#default_value'] = 1;
      // Check all content types.
      $form['content_types']['nodes']['#default_value'] = array_keys($form['content_types']['nodes']['#options']);
      // Collapse the content types and settings fieldsets.
      $form['content_types']['#collapsed'] = TRUE;
      $form['settings']['#collapsed'] = TRUE;
    }
  }
  else if($form_id == 'install_configure_form'){
    $form['server_settings']['#collapsed'] = TRUE;
    $form['server_settings']['#collapsible'] = TRUE;
    $form['admin_account']['account']['pass'] = array(
      '#type' => 'hidden',
      '#value' => user_password() // Not ideal, but it's only set for seconds!
    );
    $form['#submit'] = array('scratchpadify_install_configure_form_submit');
  }
  else if($form_id == 'node_type_form'){
    // Add vocabulary options to the node type form
    // We're going to reorder the form, and not by weight
    foreach($form as $key => $value){
      $new_form[$key] = $value;
      if($key == 'identity'){
        $vocabularies = taxonomy_get_vocabularies();
        foreach($vocabularies as $key => $value){
          if($value->module == 'taxonomy'){ // FIXME - may need to add others.
            $options[$key] = $value->name;
          }
        }
        // Are we editing an already created one, or creating a new one?
        if($form['#node_type']->type){
          // Editing
          foreach($vocabularies as $key => $value){
            if(isset($value->nodes[$form['#node_type']->type]) && $value->nodes[$form['#node_type']->type]){
              $default_value[] = $key;
            }
          }
        } else {
          // New one
          $default_value = array();
        }
        $new_form['vocabulary'] = array(
          '#type' => 'fieldset',
          '#title' => t('Vocabularies'),
          '#description' => t('Select the vocabularies to associate with this content type'),
          '#collapsed' => FALSE,
          '#collapsible' => TRUE,
          'vocabularies' => array(
            '#type' => 'checkboxes',
            '#title' => t('Vocabularies'),
            '#options' => $options,
            '#default_value' => $default_value
          )
        );
        $dont_autotag = variable_get('autotag_dont_autotag', array());
        if(isset($dont_autotag[$form['#node_type']->type]) && $dont_autotag[$form['#node_type']->type]){
          $dont_autotag_value = 1;
        }
        $new_form['autotag_setting'] = array(
          '#type' => 'fieldset',
          '#title' => t('Autotag settings'),
          '#description' => t('Decide whether or not content of this type should be automatically tagged.'),
          '#collapsed' => TRUE,
          '#collapsible' => TRUE,
          'autotag_settings' => array(
            '#type' => 'checkbox',
            '#title' => t("Never Autotag this content type"),
            '#default_value' => $dont_autotag_value
          )
        );
      }
    }
    $form = $new_form;
    if(is_array($form['#submit'])){
      $form['#submit'][] = 'scratchpadify_node_type_form_submit';
    } else {
      $form['#submit'] = array('scratchpadify_node_type_form_submit');
    }
  }
}

/**
 * Submit function for above altered form
 */
function scratchpadify_node_type_form_submit($form, &$form_state){
  foreach($form_state['values']['vocabularies'] as $vid => $value){
    $vocabulary = (array)taxonomy_vocabulary_load($vid);
    if($value){
      $vocabulary['nodes'][$form_state['values']['type']] = $form_state['values']['type'];
    } else {
      unset($vocabulary['nodes'][$form_state['values']['type']]);
    }
    taxonomy_save_vocabulary($vocabulary);
  }
  $dont_autotag = variable_get('autotag_dont_autotag', array());
  unset($dont_autotag[$form_state['values']['type']]);
  if($form_state['values']['autotag_settings']){
    $dont_autotag[$form_state['values']['type']] = $form_state['values']['type'];
  }
  variable_set('autotag_dont_autotag' , $dont_autotag);
}