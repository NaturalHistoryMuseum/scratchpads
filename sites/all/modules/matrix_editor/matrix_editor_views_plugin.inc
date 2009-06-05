<?php

/**
 * Extending the view_plugin_style class to provide a matrix editor style.
 */
class matrix_editor_views_plugin extends views_plugin_style {
  /**
   * Set default options
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['editable'] = array('default' => TRUE);
    $options['enableAddRow'] = array('default' => FALSE);
    $options['enableCellNavigation'] = array('default' => TRUE);
    $options['asyncEditorLoading'] = array('default' => TRUE);
    
    $options['enableColumnReorder'] = array('default' => FALSE);
    $options['columnWidth'] = array('default' => 50);
    $options['fixedFirstColumn'] = array('default' => TRUE);   
    $options['firstColumnWidth'] = array('default' => 50);
    
    return $options;
    
  }

  /**
   * Add settings for the particular timeline.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $handlers = $this->display->handler->get_handlers('field');
    if (empty($handlers)) {
      $form['error_markup'] = array(
        '#value' => t('You need at least one field before you can configure your table settings'),
        '#prefix' => '<div class="error form-item description">',
        '#suffix' => '</div>',
      );
      return;
    }

    // Create an array of allowed columns from the data we know:
    foreach ($handlers as $field => $handler) {

      if (!$name = $handler->label()) {
        $name = $handler->ui_name();
      }
      
      $default = ($this->options['columns'][$field]['width'] ? $this->options['columns'][$field]['width'] : 50);
      
      $form['columns'][$field]['width'] = array(
        '#type' => 'textfield',
        '#default_value' => $default,
        '#size' => 10,
        '#maxlength' => 10,
      );
      
      // markup for the field name
      $form['columns'][$field]['name'] = array(
        '#value' => $name,
      );

    }
    
    $form['editable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Editable'),
      '#default_value' => $this->options['editable'],
    );
    
    $form['enableAddRow'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable add row'),
      '#default_value' => $this->options['enableAddRow'],
    );

    $form['enableCellNavigation'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable cell navigation'),
      '#default_value' => $this->options['enableCellNavigation'],
    );
    
    $form['asyncEditorLoading'] = array(
      '#type' => 'checkbox',
      '#title' => t('A sync editor loading'),
      '#default_value' => $this->options['asyncEditorLoading'],
    );

    $form['enableColumnReorder'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable column reorder'),
      '#default_value' => $this->options['enableColumnReorder'],
    ); 
    
    $form['fixedFirstColumn'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use first column as headers'),
      '#default_value' => $this->options['fixedFirstColumn'],
    );
    
    $form['#theme'] = 'matrix_editor_views_plugin_table';
  

  }
  
}

?>