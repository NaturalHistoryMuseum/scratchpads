<?php

/**
 * Extending the view_plugin_style class to provide a matrix editor style.
 */
class matrix_editor_views_plugin extends views_plugin_style{

  /**
   * Set default options
   */
  function option_definition(){
    $options = parent::option_definition();
    $options['editable'] = array(
      'default' => TRUE
    );
    $options['enableColumnResize'] = array(
      'default' => TRUE
    );
    $options['enableColumnReorder'] = array(
      'default' => TRUE
    );
    $options['enableCellNavigation'] = array(
      'default' => TRUE
    );
    $options['asyncEditorLoading'] = array(
      'default' => TRUE
    );
    $options['fixedFirstColumn'] = array(
      'default' => FALSE
    );
    $options['viewportHeight'] = array(
      'default' => 200
    );
        $options['rowHeight'] = array(
      'default' => 24
    );
    return $options;
  }

  /**
   * Add settings for the particular timeline.
   */
  function options_form(&$form, &$form_state){
    parent::options_form($form, $form_state);
    $handlers = $this->display->handler->get_handlers('field');
    if(empty($handlers)){
      $form['error_markup'] = array(
        
        '#value' => t('You need at least one field before you can configure your table settings'), 
        '#prefix' => '<div class="error form-item description">', 
        '#suffix' => '</div>'
      );
      return;
    }
    // Create an array of allowed columns from the data we know:
    foreach($handlers as $field => $handler){
      if(!$name = $handler->label()){
        $name = $handler->ui_name();
      }
      $default = ($this->options['columns'][$field]['width'] ? $this->options['columns'][$field]['width'] : 50);
      $form['columns'][$field]['width'] = array(
        '#type' => 'textfield', 
        '#default_value' => $default, 
        '#size' => 10, '#maxlength' => 10
      );
      // markup for the field name
      $form['columns'][$field]['name'] = array(
        '#value' => $name
      );
    }
    $form['editable'] = array(
      '#type' => 'checkbox', 
      '#title' => t('Editable'), 
      '#default_value' => !empty($this->options['editable'])
    );
    $form['enableColumnResize'] = array(
      '#type' => 'checkbox', 
      '#title' => t('Enable resizing columns'), 
      '#default_value' => !empty($this->options['enableColumnResize'])
    );
    $form['enableColumnReorder'] = array(
      '#type' => 'checkbox', 
      '#title' => t('Enable reordering columns'), 
      '#default_value' => !empty($this->options['enableColumnReorder'])
    );
    $form['fixedFirstColumn'] = array(
      '#type' => 'checkbox', 
      '#title' => t('Use first column as a side header'), 
      '#default_value' => !empty($this->options['fixedFirstColumn'])
    );
    $form['asyncEditorLoading'] = array(
      '#type' => 'checkbox', 
      '#title' => t('A sync editor loading'), 
      '#default_value' => !empty($this->options['asyncEditorLoading'])
    );
    $form['viewportHeight'] = array(
      '#title' => t('Viewport height'), 
      '#type' => 'textfield', 
      '#default_value' => $this->options['viewportHeight'], 
      '#size' => 10, '#maxlength' => 10
    );
        $form['rowHeight'] = array(
      '#title' => t('Row height'), 
      '#type' => 'textfield', 
      '#default_value' => $this->options['rowHeight'], 
      '#size' => 10, '#maxlength' => 10
    );
    $form['#theme'] = 'matrix_editor_views_plugin_table';
  }

  function query(){
    // We always want the node type to be returned
    $this->view->query->add_field('node', 'type');
  }
}
?>