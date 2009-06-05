<?php

function theme_matrix_editor($options, $columns, $data, $filter = 'matrixFilter'){
  
  _matrix_editor_add_files();

  drupal_add_js('var options = '.drupal_to_js($options), 'inline');    
  drupal_add_js('var data = []; data = '.drupal_to_js($data), 'inline');
   
  drupal_add_js('var filter = '.$filter, 'inline');
  drupal_add_js('var columns = '.matrix_editor_to_js($columns).';', 'inline');

  $output = '<div id="resizableWrapper">
  <div id="myGrid" style="width:100%;height:300px;" class="hideCols hideRows '.($options['editable'] ? 'editable' : '').'">
  </div></div>';
    
  drupal_add_js("$(function(){\rinitMatrixEditor();\n});", 'inline');
  
  return $output;
  
  
}

/**
 * Theme the form for the matrix style plugin
 */
function theme_matrix_editor_views_plugin_table($form) {
  
  $output = drupal_render($form['description_markup']);

  $header = array(
    t('Field'),
    t('Width'),
  );
  
  $rows = array();
  
  foreach (element_children($form['columns']) as $id) {
    $row = array();
    $row[] = drupal_render($form['columns'][$id]['name']);
    $row[] = drupal_render($form['columns'][$id]['width']);
    $rows[] = $row;
  }

  $output .= theme('table', $header, $rows);
  $output .= drupal_render($form);

  return $output;

}



/**
 * Display a view as a table style.
 */
function matrix_editor_preprocess_views_view_matrix(&$vars) {
  
  $view = $vars['view'];
  
  $result   = $vars['rows'];

  $options  = $view->style_plugin->options;
  $handler  = $view->style_plugin;
  
  $handlers = $view->style_plugin->display->handler->get_handlers('field');
  
  $fields   = &$view->field;
  
  foreach ($handlers as $field => $handler) {
  
    if (!$name = $handler->label()) {
      $name = $handler->ui_name();
    }
    
    $columns[] = array(
      'id' => $fields[$field]->field_alias,
      'name' => $name,
      'field' => $fields[$field]->field_alias,
      'width' => $options['columns'][$field]['width'],
      'cssClass' => 'cell-title',
      'editor' => 'TextCellEditor',
    );
  
  }

  
  foreach ($result as $num => $row) {
    
    $data[$num] = (array) $row;
    
  }
  
  
  // print_r($options);
  
  $options['fixedFirstColumn'] = true;
  
  $vars['matrix'] = theme('matrix_editor', $options, $columns, $data);


}