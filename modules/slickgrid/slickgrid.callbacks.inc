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
  // node_delete() has access check so it's safe just to try and delete the node
  node_delete($node->nid);
}

/**
 * 
 * Callback function - update a node
 */
function slickgrid_callback_update(){
  // TODO - Batch processing
  // TODO - Access checks
  $field_name = $_POST['field_name'];
  $field_id = $_POST['field_id'];
  $updated = array(); // Array of NIDs that have been updated
  $form_state = array(
    'values' => $_POST
  );
  dd($_POST);
  foreach($_POST['nids'] as $nid){
    $node = node_load($nid);
    if($field = content_fields($field_name)){ // Is this a CKK field    	
    //      $form_state['values'][$field_name][0] = array(
    //        'value' => $_POST[$field_name]
    //      );
    }
    // Get the vid before it is updated
    $revision_vid = $node->vid;
    if(is_array($_POST['taxonomy'])){
      $vid = key($_POST['taxonomy']);
    }
    // Need to create new revision
    drupal_execute('slickgrid_field_form', $form_state, $node, $field_name, $vid);
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
  }
  // If we want to get the value from the view, load it now
  if($_POST['reload_from_view']){
    $view = slickgrid_callback_get_view($_POST['view'], $_POST['display_id'], $_POST['nids']);
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
    drupal_set_message(format_plural($count_errors, 'There was 1 error.', 'There were @count errors.'), 'error');
  }
  return array(
    'errors' => $errors,
    'updated' => $updated_nodes,
    'field_name' => $field_name,
    'field_id' => $field_id
  );
}

function slickgrid_callback_get_view($view_name, $display_id, $nids){
  $view = views_get_view($view_name);
  $view->set_display($display_id);
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
 * Builds a form for a particular field
 */
function slickgrid_field_form(&$form_state, $node, $field_name, $vid){
  module_load_include('inc', 'node', 'node.pages');
  module_load_include('inc', 'content', 'includes/content.node_form');
  // Basic form settings - need to have the node in the submit handler
  $form = array(
    '#node' => $node,
    '#field_name' => $field_name
  );
  if(!node_access('update', $node)){
    drupal_set_message(t('Access denied'), 'error');
    return;
  }
  // If $vid has been passed in, this is a taxonomy field
  if($vid){
    $form['taxonomy'][$vid] = taxonomy_form($vid, array_keys($node->taxonomy));
    $form['#tree'] = true;
     // Does the field exist for this node?
  // Or if it's body change it to body_field
  }elseif(($field_name == 'body' && $field_name = 'body_field') || property_exists($node, $field_name)){
    // Is the a CCK field?
    if($field = content_fields($field_name)){
      // Check access for this field
      if(!content_access('edit', $field)){
        drupal_set_message(t('Access denied'), 'error');
        return;
      }
      $form += content_field_form($form, $form_state, $field);
    }else{ // Not CCK - try and extract the form from the node form
      $node_form = node_form($form_state, $node);
      if($element = _slickgrid_callback_get_form_element($node_form, $field_name)){
        $form += $element;
      }else{
        drupal_set_message(t('Field not found'), 'error');
        return;
      }
    }
  }else{
    drupal_set_message(t('Field not found'), 'error');
    return;
  }
  return $form;
}

/**
 * Recursively extract a field from a form array
 * @param array $form
 * @param string $field_name
 */
function _slickgrid_callback_get_form_element($form, $field_name){
  static $element;
  foreach(element_children($form) as $key){
    if($key == $field_name){
      $element[$key] = $form[$key];
      return $element;
    }else{
      _slickgrid_callback_get_form_element($form[$key], $field_name);
    }
    if(isset($element)){return $element;}
  }
}

/**
 * Submit handler for slickgrid_field_form()
 */
function slickgrid_field_form_submit($form, &$form_state){
  $node = $form['#node'];
  if($form_state['values']['taxonomy']){
  	// TODO - this wipes all taxonomy
  	$node->taxonomy = array();
    $vid = key($form_state['values']['taxonomy']);
    foreach($form_state['values']['taxonomy'][$vid] as $tid){
      $node->taxonomy[$tid] = new stdClass();
      $node->taxonomy[$tid]->tid = $tid;
      $node->taxonomy[$tid]->vid = $vid;
    }
  }else{
    $node->{$form['#field_name']} = $form_state['values'][$form['#field_name']];
  }
  // If undo is enabled, create a new node revision	
  if($_POST['undo']){
    $node->revision = true;
  }
  node_save($node);
}

/**
 * 
 * Callback function - update a view's settings
 */
function slickgrid_callback_settings(){
  slickgrid_set_settings($_POST['view'], $_POST['setting'], $_POST['value']);
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
  $node = node_load($_POST['nid']);
  $field_name = $_POST['field_name'];
  // taxonomy vid (passed in if this a taxonomy field)
  $vid = $_POST['vid'];
  $html = drupal_get_form('slickgrid_field_form', $node, $field_name, $vid);
  $data = new stdClass();
  $data->content = $html;
  // Register the JavaScript callback for this module.
  $data->__callbacks = array();
  // Allow other modules to extend the data returned.
  drupal_alter('ajax_data', $data, 'slickgrid', $html);
  return array(
    'data' => $data
  );
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
