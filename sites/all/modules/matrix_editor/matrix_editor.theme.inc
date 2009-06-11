<?php






/**
 * Display a view as a table style.
 */
function matrix_editor_preprocess_views_view_matrix(&$vars) {
    
  $view = $vars['view'];
  
  drupal_add_js(array('matrixEditorCallbackPath' => url('matrix_editor/callback/'.$view->name)), 'setting');
  drupal_add_js(array('matrixEditorPath' => drupal_get_path('module', 'matrix_editor')), 'setting');
  
  drupal_add_js(array('matrixEditorViewName' => $view->name), 'setting');
  
  $result   = $vars['rows'];

  $options  = $view->style_plugin->options;
  
  $options['enableCellNavigation'] = $options['editable'];
  
  $handler  = $view->style_plugin;
  
  $handlers = $view->style_plugin->display->handler->get_handlers('field');
  
  $fields   = &$view->field;
  
  if($options['fixedFirstColumn']){
    
    $first_field = key($handlers);
  
  }

  $options['enableAddRow'] = false;
  
  foreach ($handlers as $field => $handler) {
  
    if (!$name = $handler->label()) {
      $name = $handler->ui_name();
    }
    
    ($handler->content_field['field_name'] ? $id = $handler->content_field['field_name'] : $id = $field);

    $column = array(
      'id' => $field,
      'name' => $name,
      'field' => $field,
      'width' => ($options['columns'][$field]['width'] ? $options['columns'][$field]['width'] : 100),
      'cssClass' => 'cell-title',
      'resizable' => ($options['enableColumnResize'] ? 1 : 0),
      'formatter' => 'selectorCellFormatter',
      'editor' => 'MatrixCellEditor',
      'args' => ''  
    );

    if((isset($first_field) && $first_field == $field)){
      
      unset($first_field);
    
    }elseif($field != 'nid'){
      
      // Is this a CCK field?
      if($handler->content_field['widget']['type']){
        
        $column['id'] = $handler->content_field['field_name'];
        
      }elseif($field == 'tid'){
        
        foreach($handler->options['vids'] as $vid){
       
          if($vid){
            
            $column['args'] .= 'vids[]='.$vid.'&';
            
          }
          
        }
 
      }
      
    }

    $columns[] = $column;
    
  }
  
  $renders = array();
  $keys = array_keys($view->field);
  
  $node_types = array();
  $data = array();
  
  module_load_include('inc', 'node', 'node.pages');

  foreach ($result as $count => $row) {
    
    foreach ($keys as $id) {

      $data[$count][$id] = $view->field[$id]->theme($row);

    }

    if(!$node_types[$row->node_type]){
      
      matrix_editor_add_node_form_js($row->node_type, $row->nid);
      $node_types[$row->node_type] = $row->nid;
      
    }

    $data[$count]['id'] = $row->nid;
    
  }

  $vars['matrix'] = theme('matrix_editor', $options, $columns, $data);
  
  $path = drupal_get_path('module', 'nexus');

   $form['save_changes'] = array(
    '#type' => 'image_button',
    '#src' => $path.'/images/save_changes.gif',
    '#value' => t('Save note'),
    );
  
   $vars['matrix'] .= '<div id="matrix-editor-panel"><form>';
   $vars['matrix'] .= '<div id="matrix-editor-field"></div>';
   $vars['matrix'] .= '<div id="save-field"> '.drupal_render($form).'</div>';
   $vars['matrix'] .= '</form></div>';
   $vars['matrix'] .= '<div id="matrix-editor-result"></div>';
  
 
}

/**
 * Load and pseudo-render the form - ensures all js & css files are available
 */
function matrix_editor_add_node_form_js($node_type, $nid){

  $form_state = array();
  $node = node_load($nid);

  $form_id = $node_type .'_node_form';

  $form = drupal_retrieve_form($form_id, $form_state, $node);

  drupal_prepare_form($form_id, $form, $form_state);

  drupal_alter('form', $form, array(), $form_id);

  drupal_process_form($form_id, $form, $form_state);
  
  drupal_render_form($form_id, $form);
  
}


function theme_matrix_editor($options, $columns, $data, $filter = null){
  
  _matrix_editor_add_files();
  
  drupal_add_js('var options = '.drupal_to_js($options), 'inline');    
  drupal_add_js('var data = []; data = '.drupal_to_js($data), 'inline');
   
  if($filter){

    drupal_add_js('var filter = '.$filter, 'inline');
    
  }

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



