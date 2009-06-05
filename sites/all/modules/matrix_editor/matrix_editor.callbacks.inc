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


?>