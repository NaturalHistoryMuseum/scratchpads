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

function matrix_editor_callback_get_form_field(){
 
  module_load_include('inc', 'node', 'node.pages');
 
  $nid = $_POST['nid'];
  $field = $_POST['field'];
  
  $node = node_load($nid);
  
  
  switch ($field){
    
    case 'body':
    
      $form_state['body'] = 'hi';
      $form = node_content_form($node, $form_state);
      $output = drupal_render($form['body_field']['body']);
      
    break;
    
    case 'title':
    
      $form = node_content_form($node, array());
      
      print_r($form);
      
      $form['title']['#value'] = $node->title;
      
      // print_r($form);
      
      // $output = drupal_render($form['title']);
      
    break;
    
    default:
      
      module_load_include('inc', 'content', 'includes/content.node_form');
            
      $content_type = content_types($node->type);

      $form = array();
      $form_state = array();
      $field = content_field_form($form, $form_state, $content_type['fields']['field_b_cck_text']);

      // print_r($field);

      $output = drupal_render($field['field_b_cck_text'][0]);
    
      // $output = content_view_field($content_type['fields']['field_b_cck_text'], $node);
    
    break;
    
  }
  
  return array('status' => TRUE, 'data' => $output);
  
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