<?php

/**
 * Menu callback
 * Copy of content_add_more_js() except uses slickgrid_json rather than drupal_json
 * @param string $type_name_url
 * @param string $field_name
 */
function slickgrid_content_add_more_js($type_name_url, $field_name){
  module_load_include('inc', 'node', 'node.pages');
  module_load_include('inc', 'content', 'includes/content.node_form');
  $type = content_types($type_name_url);
  $field = content_fields($field_name, $type['type']);
  if(($field['multiple'] != 1) || empty($_POST['form_build_id'])){
    // Invalid request.
    slickgrid_json(array(
      'data' => ''
    ));
    exit();
  }
  // Retrieve the cached form.
  $form_state = array(
    'submitted' => FALSE
  );
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  if(!$form){
    // Invalid form_build_id.
    slickgrid_json(array(
      'data' => ''
    ));
    exit();
  }
  // We don't simply return a new empty widget to append to existing ones, because
  // - ahah.js won't simply let us add a new row to a table
  // - attaching the 'draggable' behavior won't be easy
  // So we resort to rebuilding the whole table of widgets including the existing ones,
  // which makes us jump through a few hoops.
  // The form that we get from the cache is unbuilt. We need to build it so that
  // _value callbacks can be executed and $form_state['values'] populated.
  // We only want to affect $form_state['values'], not the $form itself
  // (built forms aren't supposed to enter the cache) nor the rest of $form_data,
  // so we use copies of $form and $form_data.
  $form_copy = $form;
  $form_state_copy = $form_state;
  $form_copy['#post'] = array();
  form_builder($_POST['form_id'], $form_copy, $form_state_copy);
  // Just grab the data we need.
  $form_state['values'] = $form_state_copy['values'];
  // Reset cached ids, so that they don't affect the actual form we output.
  form_clean_id(NULL, TRUE);
  // Sort the $form_state['values'] we just built *and* the incoming $_POST data
  // according to d-n-d reordering.
  unset($form_state['values'][$field_name][$field['field_name'] . '_add_more']);
  foreach($_POST[$field_name] as $delta => $item){
    $form_state['values'][$field_name][$delta]['_weight'] = $item['_weight'];
  }
  $form_state['values'][$field_name] = _content_sort_items($field, $form_state['values'][$field_name]);
  $_POST[$field_name] = _content_sort_items($field, $_POST[$field_name]);
  // Build our new form element for the whole field, asking for one more element.
  $form_state['item_count'] = array(
    $field_name => count($_POST[$field_name]) + 1
  );
  $form_element = content_field_form($form, $form_state, $field);
  // Let other modules alter it.
  drupal_alter('form', $form_element, array(), 'content_add_more_js');
  // Add the new element at the right place in the (original, unbuilt) form.
  if(module_exists('fieldgroup') && ($group_name = _fieldgroup_field_get_group($type['type'], $field_name))){
    $form[$group_name][$field_name] = $form_element[$field_name];
  }else{
    $form[$field_name] = $form_element[$field_name];
  }
  // Save the new definition of the form.
  $form_state['values'] = array();
  form_set_cache($form_build_id, $form, $form_state);
  // Build the new form against the incoming $_POST values so that we can
  // render the new element.
  $delta = max(array_keys($_POST[$field_name])) + 1;
  $_POST[$field_name][$delta]['_weight'] = $delta;
  $form_state = array(
    'submitted' => FALSE
  );
  $form += array(
    '#post' => $_POST,
    '#programmed' => FALSE
  );
  $form = form_builder($_POST['form_id'], $form, $form_state);
  // Render the new output.
  $field_form = (!empty($group_name)) ? $form[$group_name][$field_name] : $form[$field_name];
  // We add a div around the new content to receive the ahah effect.
  $field_form[$delta]['#prefix'] = '<div class="ahah-new-content">' . (isset($field_form[$delta]['#prefix']) ? $field_form[$delta]['#prefix'] : '');
  $field_form[$delta]['#suffix'] = (isset($field_form[$delta]['#suffix']) ? $field_form[$delta]['#suffix'] : '') . '</div>';
  // Prevent duplicate wrapper.
  unset($field_form['#prefix'], $field_form['#suffix']);
  // If a newly inserted widget contains AHAH behaviors, they normally won't
  // work because AHAH doesn't know about those - it just attaches to the exact
  // form elements that were initially specified in the Drupal.settings object.
  // The new ones didn't exist then, so we need to update Drupal.settings
  // by ourselves in order to let AHAH know about those new form elements.
  $javascript = drupal_add_js(NULL, NULL);
  $output_js = isset($javascript['setting']) ? '<script type="text/javascript">jQuery.extend(Drupal.settings, ' . drupal_to_js(call_user_func_array('array_merge_recursive', $javascript['setting'])) . ');</script>' : '';
  $output = theme('status_messages') . drupal_render($field_form) . $output_js;
  // Using drupal_json() breaks filefield's file upload, because the jQuery
  // Form plugin handles file uploads in a way that is not compatible with
  // 'text/javascript' response type.
  $GLOBALS['devel_shutdown'] = FALSE;
  print slickgrid_json(array(
    'status' => TRUE,
    'data' => $output
  ));
  exit();
}

/**
 * 
 * Copy of filefield_js() using slickgrid_json() rather than drupal_to_js()
 * @param unknown_type $type_name
 * @param unknown_type $field_name
 * @param unknown_type $delta
 */
function slickgrid_filefield_js($type_name, $field_name, $delta){

  module_load_include('inc', 'node', 'node.pages');
  module_load_include('inc', 'content', 'includes/content.node_form');
  $field = content_fields($field_name, $type_name);
  if(empty($field) || empty($_POST['form_build_id'])){
    // Invalid request.
    drupal_set_message(t('An unrecoverable error occurred. The uploaded file likely exceeded the maximum file size (@size) that this server supports.', array(
      '@size' => format_size(file_upload_max_size())
    )), 'error');
    print slickgrid_json(array(
      'data' => theme('status_messages')
    ));
    exit();
  }
  // Build the new form.
  $form_state = array(
    'submitted' => FALSE
  );
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  if(!$form){
    // Invalid form_build_id.
    drupal_set_message(t('An unrecoverable error occurred. This form was missing from the server cache. Try reloading the page and submitting again.'), 'error');
    print slickgrid_json(array(
      'data' => theme('status_messages')
    ));
    exit();
  }
  // Build the form. This calls the file field's #value_callback function and
  // saves the uploaded file. Since this form is already marked as cached
  // (the #cache property is TRUE), the cache is updated automatically and we
  // don't need to call form_set_cache().
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form['#post'] = $_POST;
  $form = form_builder($form_id, $form, $form_state);
  // Update the cached form with the new element at the right place in the form.
  if(module_exists('fieldgroup') && ($group_name = _fieldgroup_field_get_group($type_name, $field_name))){
    if(isset($form['#multigroups']) && isset($form['#multigroups'][$group_name][$field_name])){
      $form_element = $form[$group_name][$delta][$field_name];
    }else{
      $form_element = $form[$group_name][$field_name][$delta];
    }
  }else{
    $form_element = $form[$field_name][$delta];
  }
  if(isset($form_element['_weight'])){
    unset($form_element['_weight']);
  }
  $output = drupal_render($form_element);
  // AHAH is not being nice to us and doesn't know the "other" button (that is,
  // either "Upload" or "Delete") yet. Which in turn causes it not to attach
  // AHAH behaviours after replacing the element. So we need to tell it first.
  // Loop through the JS settings and find the settings needed for our buttons.
  $javascript = drupal_add_js(NULL, NULL);
  $filefield_ahah_settings = array();
  if(isset($javascript['setting'])){
    foreach($javascript['setting'] as $settings){
      if(isset($settings['ahah'])){
        foreach($settings['ahah'] as $id => $ahah_settings){
          if(strpos($id, 'filefield-upload') || strpos($id, 'filefield-remove')){
            $filefield_ahah_settings[$id] = $ahah_settings;
          }
        }
      }
    }
  }
  // Add the AHAH settings needed for our new buttons.
  if(!empty($filefield_ahah_settings)){
    $output .= '<script type="text/javascript">jQuery.extend(Drupal.settings.ahah, ' . drupal_to_js($filefield_ahah_settings) . ');</script>';
  }
  $output = theme('status_messages') . $output;
  // For some reason, file uploads don't like drupal_json() with its manual
  // setting of the text/javascript HTTP header. So use this one instead.
  $GLOBALS['devel_shutdown'] = FALSE;
  // Use slickgrid_json() if removing a file, drupal_to_js() if adding
  if($_POST[$form_element['filefield_remove']['#name']]){
    slickgrid_json(array(
      'status' => TRUE,
      'data' => $output
    ));
  }else{
    print drupal_to_js(array(
      'status' => TRUE,
      'data' => $output
    ));
  }
  exit();
}


function slickgrid_nodereference_autocomplete($field_name, $string = '') {
  $fields = content_fields();
  $field = $fields[$field_name];
  $match = isset($field['widget']['autocomplete_match']) ? $field['widget']['autocomplete_match'] : 'contains';
  $matches = array();

  $references = _nodereference_potential_references($field, $string, $match, array(), 10);
  foreach ($references as $id => $row) {
    // Add a class wrapper for a few required CSS overrides.
    $matches[$row['title'] ." [nid:$id]"] = '<div class="reference-autocomplete">'. $row['rendered'] . '</div>';
  }
  slickgrid_json($matches);
}