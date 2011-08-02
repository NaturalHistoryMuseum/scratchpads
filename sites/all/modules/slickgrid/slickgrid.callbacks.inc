<?php

/** 
 * hook_menu callback; 
 * Universal callback - builds function name and returns error = true 
 */
function slickgrid_callback($op){
  $func = 'slickgrid_callback_' . $op;
  // Basic required json 
  $json = array(
    'status' => TRUE,
    'op' => $op
  );
  // The callback can append extra json
  if(is_array($additonal_json = $func())){
    $json += $additonal_json;
  }
  // Add any drupal messages that have been created
  foreach(drupal_get_messages() as $type => $messages){
    foreach($messages as $message){
      if($type == 'error'){
        $json['error'] = true;
      }
      $json['messages'][] = array(
        'type' => $type,
        'message' => $message
      );
    }
  }
  // Return the json
  slickgrid_json($json);
}

/**
 * Callback function - delete a node
 */
function slickgrid_callback_delete(){
  $node = node_load($_POST['nid']);
  // node_delete() has access check so it's safe to just try and delete the node
  node_delete($node->nid);
}

function slickgrid_callback_update(){
  // TODO - Batch processing
  $field_name = $_POST['field_name'];
  $field_id = $_POST['field_id'];
  module_load_include('inc', 'node', 'node.pages');
  $updated = array(); // Array of NIDs that have been updated
  foreach($_POST['nids'] as $nid){
    // $form_state gets changed by the form submission so re-intialise for every loop
    $form_state = array(
      'values' => $_POST
    );
    $form_state['values']['op'] = t('Save');
    $form_state['submitted'] = true;
    $form_state['submitted'] = 1;
    $node = node_load($nid);
    if(node_access('update', $node)){
      // Get the vid before it is updated
      $revision_vid = $node->vid;
      $form_id = $node->type . '_node_form';
      // Execute the node form      
      drupal_execute($form_id, $form_state, $node);
      // Has there been an error?
      if($err = form_get_errors()){
        $errors[$nid] = $err;
      }else{
        // If not, mark it as updated nids
        $updated_nodes[$nid] = array(
          'vid' => $revision_vid,
          'value' => ($_POST['reload_from_view'] ? '' : $_POST[$field_name]) // If not being reloaded from the view, populate the value  
        );
      }
    }else{
      $errors[$nid] = t('You do not have access to edit @node', array(
        '@node' => $node->title
      ));
    }
  }
  // If we want to reload the value from the view, get it now
  if(count($updated_nodes) && $_POST['reload_from_view']){
    $view = slickgrid_callback_get_view($_POST['view'], $_POST['display_id'], array_keys($updated_nodes));
    foreach($view->result as $count => $row){
      $updated_nodes[$row->nid]['value'] = $view->render_field($field_id, $count);
    }
  }
  // Clear the message queue
  drupal_get_messages();
  // Add our own messages
  if($count_updated = count($updated_nodes)){
    drupal_set_message(format_plural($count_updated, '@title was updated succesfully.', '@count nodes were updated succesfully.', array(
      '@title' => $node->title
    )));
  }
  if($count_errors = count($errors)){
    $error_message = format_plural($count_errors, 'There was 1 error', 'There were @count errors');
    drupal_set_message($error_message, 'error');
    $error_message .= ': ';
    foreach($errors as $error){
      foreach($error as $err){
        $error_message .= $err . '<br />';
      }
    }
    watchdog('slickgrid', $error_message, array(), WATCHDOG_ERROR);
  }
  // If batch page exists, we need to clear the cache
  if(module_exists('views_batch_page') && strpos($_POST['display_id'], 'views_batch_page') !== false){
    // If we haven't already retrieved the view, build a dummy object to build the cache id from
    if(!isset($view)){
      $view = new stdClass();
      $view->name = $_POST['view'];
      $view->current_display = $_POST['display_id'];
    }
    views_batch_page_cache_clear($view);
  }
  return array(
    'errors' => $errors,
    'updated' => $updated_nodes,
    'field_name' => $field_name,
    'field_id' => $field_id
  );
}

function slickgrid_execute($form_id, &$form_state){
  $args = func_get_args();
  // Make sure $form_state is passed around by reference.
  $args[1] = &$form_state;
  $form = call_user_func_array('drupal_retrieve_form', $args);
  $form['#post'] = $form_state['values'];
  drupal_prepare_form($form_id, $form, $form_state);
  $form['#validate'][] = 'slickgrid_callback_form_validate';
  drupal_process_form($form_id, $form, $form_state);
}

function slickgrid_callback_form_validate($form, &$form_state){
  form_set_error(null, null, true);
}

/**
 * 
 * Get a views filtered by NIDs 
 * @param string $view_name
 * @param string $display_id
 * @param array $nids
 */
function slickgrid_callback_get_view($view_name, $display_id, $nids){
  $view = views_get_view($view_name);
  $view->set_display($display_id);
  // Remove all existing arguments - we'll limit result set by NID only
  foreach($view->get_items('argument') as $id => $arg){
    $view->set_item($display_id, 'argument', $id, NULL);
  }
  // Add an argument to limit the view to only nids being updated	
  $options = array(
    'table' => 'node',
    'field' => nid,
    'break_phrase' => 1, // Allow multiple nids,
    'validate_argument_nid_type' => 'nid'
  );
  $view->add_item($display_id, 'argument', 'node', 'nid', $options);
  $view->set_arguments(array(
    implode('+', $nids)
  ));
  $view->pre_execute();
  $view->execute();
  $view->render();
  return $view;
}

/**
 * 
 * Callback function - update a view's settings
 */
function slickgrid_callback_settings(){
  global $user;
  slickgrid_set_settings($user->uid, $_POST['view'], array(
    $_POST['setting'] => $_POST['value']
  ));
}

/**
 * Callback function - log an message 
 */
function slickgrid_callback_log(){
  watchdog('slickgrid', $_POST['error'], array(), WATCHDOG_ERROR);
}

/**
 * Callback function - retrieve a form 
 */
function slickgrid_callback_get_form(){
  module_load_include('inc', 'node', 'node.pages');
  $node = node_load($_POST['nid']);
  if(node_access('update', $node)){
    $html = drupal_get_form($node->type . '_node_form', $node);
    $data = new stdClass();
    $data->content = $html;
    // Register the JavaScript callback for this module.
    $data->__callbacks = array();
    // Allow other modules to extend the data returned.
    drupal_alter('ajax_data', $data, 'slickgrid', $html);
    return array(
      'data' => $data
    );
  }else{
    drupal_set_message(t("You do not have access to edit @title", array(
      '@title' => $node->title
    )), 'error');
  }
}

/**
 * Callback function - log an message 
 */
function slickgrid_callback_undo(){
  $field_name = $_POST['field_name'];
  $field_id = $_POST['field_id'];
  $updated_nodes = $_POST['updated'];
  if(is_array($updated_nodes)){
    foreach($_POST['updated'] as $nid => $node){
      $node_revision = node_load($nid, $node['vid']);
      $node_revision->revision = 1;
      $node_revision->log = t('Copy of the revision from %date.', array(
        '%date' => format_date($node_revision->revision_timestamp)
      ));
      node_save($node_revision);
    }
    // All nodes have been reverted - need to get the values from the view
    $view = slickgrid_callback_get_view($_POST['view'], $_POST['display_id'], array_keys($updated_nodes));
    foreach($view->result as $count => $row){
      $updated_nodes[$row->nid]['value'] = $view->render_field($field_id, $count);
    }
  }
  return array(
    'updated' => $updated_nodes,
    'field_name' => $field_name,
    'field_id' => $field_id
  );
}
