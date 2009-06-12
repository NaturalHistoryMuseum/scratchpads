<?php

/** 
* hook_menu callback; 
* Wrapper for all ahah / ajax callbacks
*/
function matrix_editor_callback($op){
  
  $func = 'matrix_editor_callback_'.$op;
  $json = $func($arg);
  drupal_json($json);
  
}


function matrix_editor_callback_column_resized(){
  
  $view_name = $_POST['view'];
  $width = $_POST['width'];
  $field = $_POST['field'];
  
  matrix_editor_callback_set_option($view_name, $field, $width);
   
}


function matrix_editor_callback_viewport_resized(){
  
  $view_name = $_POST['view'];
  $height = $_POST['height'];

  matrix_editor_callback_set_option($view_name, 'viewportHeight', $height);
   
}


function array_diff_assoc2_deep($array1, $array2) {
    $ret = array();
    foreach ($array1 as $k => $v) {
        if (!isset($array2[$k])) $ret[$k] = $v;
        else if (is_array($v) && is_array($array2[$k])) $ret[$k] = array_diff_assoc2_deep($v, $array2[$k]);
        else if ((string)$v != (string)$array2[$k]) $ret[$k] = $v;
    }
    return $ret;
}



function matrix_editor_callback_get_form_field(){
 
  module_load_include('inc', 'node', 'node.pages');

  $nid = $_POST['nid'];
  $field = $_POST['field'];
  $view_field = $_POST['view_field'];
  
  if($nid){
    
    $node = node_load($nid);

    $form_state = array();

    $form_id = $node->type .'_node_form';

    $form = drupal_retrieve_form($form_id, $form_state, $node);

    drupal_prepare_form($form_id, $form, $form_state);

    drupal_alter('form', $form, array(), $form_id);

    drupal_process_form($form_id, $form, $form_state);
    
    if($field == 'body'){
      $field .= '_field';
    }
    
    if($form[$field]){
      
      $form[$field]['#attributes'] = array('class' => 'matrix-editor-field');
      $output = drupal_render($form[$field]);
      
    }elseif(count($_POST['vids'])){
      
        foreach($_POST['vids'] as $vid){
          
          $output .= drupal_render($form['taxonomy'][$vid]);
          
        }
      
    }else{
      
      matrix_editor_get_form_element($field, $form, $form_element);

      if($form_element){
        
        $output .= drupal_render($form_element);
        
      }else{
        
        $output .= '<p>'.t('Sorry, this field cannot be found for this node type').'</p>';
        
      }

       
      
    }

  $form['nid']['#type'] = 'hidden';
   
  $output .= drupal_render($form['nid']); 
  
  $form['view_field'] = array(
    '#type' => 'hidden', 
    '#value' => $view_field, 
    '#id' => 'view_field', 
    '#name' => 'view_field'
  );
  
  $output .= drupal_render($form['view_field']);  
   
  return array('status' => TRUE, 'data' => $output); 
    
  }
  
}




function matrix_editor_get_form_element($field, &$form, &$element){
  
  static $count;
  
  if($element){
    return;
  }
  
  foreach(element_children($form) as $child_element){
    
    if($child_element === $field){

      if($form[$child_element]['#type']){
        $element = $form[$child_element];
        return;
      }
      
    }elseif(count(element_children($form[$child_element]))){
      
      $count++;  
      matrix_editor_get_form_element($field, $form[$child_element], $element);

    }
    
  }
  
}




function matrix_editor_callback_update_node(){
  
  $default_nid = $_POST['nid'];
  $view_name = $_POST['view'];
  $view_field = $_POST['view_field'];
  
  $nids = $_POST['nids'];
  $nids[] = $default_nid;
  
  if(count($nids)){
  
  module_load_include('inc', 'node', 'node.pages');
    
    foreach($nids as $nid){

      $_POST['nid'] = $nid;
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
       
        $form_id = $node->type .'_node_form';

        drupal_execute($form_id, $form_state, $node);
        
      }
      
      $errors = form_get_errors();
      
      if(count($errors)){
        break;
      }
      
    }
    
    if (!count($errors)) {

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
  
  return array('data' => $output, 'result' => theme('status_messages'));
  
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
  
  $view = views_get_view($view_name);
  
  $view->init_display();
  
  $display = $view->display['default'];
  
  $fields = $display->display_options['fields'];

  $columns = $_POST['cols'];

  if(count($columns)){
    
    foreach($columns as $column){
      
      $ordered_fields[$column] = $fields[$column];
      
    }
    
  }

  $display->handler->set_option('fields', $ordered_fields);

  $view->save();
   
}



?>