<?php

/** 
 * hook_menu callback; 
 * Wrapper for all ahah / ajax callbacks
 */
function matrix_editor_callback($op){
  $func = 'matrix_editor_callback_' . $op;
  $json = $func();
  drupal_json($json);
}

function matrix_editor_callback_column_resized(){
  $view_name = $_POST['view'];
  $width = $_POST['width'];
  $field = $_POST['field'];
  matrix_editor_set_settings($view_name, $field . '_width', $width);
}

function matrix_editor_callback_viewport_resized(){
  $view_name = $_POST['view'];
  $height = $_POST['height'];
  matrix_editor_set_settings($view_name, 'viewportHeight', $height);
}

function array_diff_assoc2_deep($array1, $array2){
  $ret = array();
  foreach($array1 as $k => $v){
    if(!isset($array2[$k]))
      $ret[$k] = $v;
    else if(is_array($v) && is_array($array2[$k]))
      $ret[$k] = array_diff_assoc2_deep($v, $array2[$k]);
    else if((string)$v != (string)$array2[$k])
      $ret[$k] = $v;
  }
  return $ret;
}

function matrix_editor_callback_get_form_field(){
  global $conf;
  module_load_include('inc', 'node', 'node.pages');
  $nid = $_POST['nid'];
  $field = $_POST['field'];
  $view_field = $_POST['view_field'];
  // batax breaks the matrix editor, so disable it for all vids before getting the form
  foreach($_POST['vids'] as $vid){
    $conf['batax_disable_vids'][$vid] = 1;
  }
  if($nid){
    $node = node_load($nid);
    $form_id = $node->type . '_node_form';
    $form = matrix_editor_get_form($form_id, $node);
    if($field == 'body'){
      $field .= '_field';
    }
    if($form[$field]){
      $form[$field]['#attributes'] = array(
        'class' => 'matrix-editor-field'
      );
      $output = drupal_render($form[$field]);
    }elseif(count($_POST['vids'])){
      foreach($_POST['vids'] as $vid){
        if($form['taxonomy'][$vid]){
          $output .= drupal_render($form['taxonomy'][$vid]);
        }elseif($form['taxonomy']['tags'][$vid]){
          $output .= drupal_render($form['taxonomy']['tags'][$vid]);
        }
      }
    }else{
      matrix_editor_get_form_element($field, $form, $form_element);
      if($form_element){
        $output .= drupal_render($form_element);
      }else{
        $output .= '<p>' . t('Sorry, this field cannot be found for this node type') . '</p>';
      }
    }
    $form['nid']['#type'] = 'hidden';
    $output .= drupal_render($form['nid']);
    $output .= drupal_render($form['form_build_id']);
    $output .= drupal_render($form['form_id']);
    $form['fields']['view_field'] = array(
      '#type' => 'hidden',
      '#value' => $view_field,
      '#id' => 'view_field',
      '#name' => 'view_field'
    );
    $form['fields']['field_name'] = array(
      '#type' => 'hidden',
      '#value' => $field,
      '#id' => 'field_name',
      '#name' => 'field_name'
    );
    $output .= drupal_render($form['fields']);
    return array(
      'status' => TRUE,
      'data' => $output
    );
  }
}

/**
 * Same as drupal_get_form but returns an unrendered form
 * @param $form_id
 */
function matrix_editor_get_form($form_id){
  $form_state = array(
    'storage' => NULL,
    'submitted' => FALSE
  );
  $args = func_get_args();
  $cacheable = FALSE;
  if(isset($_SESSION['batch_form_state'])){
    // We've been redirected here after a batch processing : the form has
    // already been processed, so we grab the post-process $form_state value
    // and move on to form display. See _batch_finished() function.
    $form_state = $_SESSION['batch_form_state'];
    unset($_SESSION['batch_form_state']);
  }else{
    // If the incoming $_POST contains a form_build_id, we'll check the
    // cache for a copy of the form in question. If it's there, we don't
    // have to rebuild the form to proceed. In addition, if there is stored
    // form_state data from a previous step, we'll retrieve it so it can
    // be passed on to the form processing code.
    if(isset($_POST['form_id']) && $_POST['form_id'] == $form_id && !empty($_POST['form_build_id'])){
      $form = form_get_cache($_POST['form_build_id'], $form_state);
    }
    // If the previous bit of code didn't result in a populated $form
    // object, we're hitting the form for the first time and we need
    // to build it from scratch.
    if(!isset($form)){
      $form_state['post'] = $_POST;
      // Use a copy of the function's arguments for manipulation
      $args_temp = $args;
      $args_temp[0] = &$form_state;
      array_unshift($args_temp, $form_id);
      $form = call_user_func_array('drupal_retrieve_form', $args_temp);
      $form_build_id = 'form-' . md5(uniqid(mt_rand(), true));
      $form['#build_id'] = $form_build_id;
      drupal_prepare_form($form_id, $form, $form_state);
      // Store a copy of the unprocessed form for caching and indicate that it
      // is cacheable if #cache will be set.
      $original_form = $form;
      $cacheable = TRUE;
      unset($form_state['post']);
    }
    $form['#post'] = $_POST;
    // Now that we know we have a form, we'll process it (validating,
    // submitting, and handling the results returned by its submission
    // handlers. Submit handlers accumulate data in the form_state by
    // altering the $form_state variable, which is passed into them by
    // reference.
    drupal_process_form($form_id, $form, $form_state);
    if($cacheable && !empty($form['#cache'])){
      // Caching is done past drupal_process_form so #process callbacks can
      // set #cache.
      form_set_cache($form_build_id, $original_form, $form_state);
    }
  }
  // Most simple, single-step forms will be finished by this point --
  // drupal_process_form() usually redirects to another page (or to
  // a 'fresh' copy of the form) once processing is complete. If one
  // of the form's handlers has set $form_state['redirect'] to FALSE,
  // the form will simply be re-rendered with the values still in its
  // fields.
  //
  // If $form_state['storage'] or $form_state['rebuild'] has been set
  // and input has been processed, we know that we're in a complex
  // multi-part process of some sort and the form's workflow is NOT
  // complete. We need to construct a fresh copy of the form, passing
  // in the latest $form_state in addition to any other variables passed
  // into drupal_get_form().
  if((!empty($form_state['storage']) || !empty($form_state['rebuild'])) && !empty($form_state['process_input']) && !form_get_errors()){
    $form = drupal_rebuild_form($form_id, $form_state, $args);
  }
  // If we haven't redirected to a new location by now, we want to
  // render whatever form array is currently in hand.
  return $form;
}

function matrix_editor_get_form_element($field, &$form, &$element){
  if($element){return;}
  foreach(element_children($form) as $child_element){
    if($child_element === $field){
      if($form[$child_element]['#type']){
        $element = $form[$child_element];
        return;
      }
    }elseif(count(element_children($form[$child_element]))){
      matrix_editor_get_form_element($field, $form[$child_element], $element);
    }
  }
}

function matrix_editor_callback_update_node(){
  $default_nid = $_POST['nid'];
  $view_name = $_POST['view'];
  $view_field = $_POST['view_field'];
  $field_name = $_POST['field_name'];
  $nids = $_POST['nids'];
  $nids[] = $default_nid;
  if(count($nids)){
    module_load_include('inc', 'node', 'node.pages');
    foreach($nids as $nid){
      $node = node_load($nid);
      //      if(property_exists($node, $field_name)){
      //        $node->$field_name = $_POST[$field_name];
      //        node_save($node);
      //      }else{
      $_POST['nid'] = $nid;
      $form_state['values']['nid'] = $nid;
      $_POST['op'] = t('Save');
      $form_state = array();
      $form_state['values'] = $_POST;
      $form_state['values']['op'] = t('Save');
      $form_state['submitted'] = true;
      $node = node_load($nid);
      if(!$form_state['values']['taxonomy']){
        $form_state['taxonomy_tree_override'] = true;
      }elseif(is_array($_POST['taxonomy'])){
        $vid = key($_POST['taxonomy']);
        foreach($node->taxonomy as $tid => $term){
          if($term->vid == $vid){
            unset($node->taxonomy[$tid]);
          }
        }
      }
      if($node->type){
        $form_id = $node->type . '_node_form';
        drupal_execute($form_id, $form_state, $node);
      }
      $errors = form_get_errors();
      if(count($errors)){
        break;
      }
       //      }
    }
    if(!count($errors)){
      // Get the new view field content to add to the matrix
      // Double check view name exists - FF2 with firebug seems sometimes to lose it...
      if($view_name){
        $view = views_get_view($view_name);
        $view->is_cacheable = 0;
        $view->matrix_editor_nid = $default_nid;
        $view->execute();
        $view->render();
        $output = $view->render_field($view_field, 0);
      }
    }
  }
  return array(
    'data' => $output,
    'result' => theme('status_messages')
  );
}

function matrix_editor_callback_set_option($view_name, $option, $value){
  $view = views_get_view($view_name);
  $view->init_display();
  $display = $view->display['default'];
  $options = $display->handler->options['style_options'];
  if($option == 'viewportHeight'){
    $options['viewportHeight'] = $value;
  }else{
    $options['columns'][$option]['width'] = $value;
  }
  $display->handler->set_option('style_options', $options);
  $view->save();
}

function matrix_editor_callback_reorder_columns(){
  $view_name = $_POST['view'];
  matrix_editor_set_settings($view_name, 'column_order', $_POST['cols']);
}
?>