<?php

/**
 * Extending the view_plugin_style class to provide a slickgrid style.
 */
class slickgrid_views_plugin extends views_plugin_style{

  /**
   * Set default options
   */
  function option_definition(){
    $options = parent::option_definition();
    // These options map directly to slickgrid options
    // maps to slickgrid option enableColumnResize
    $options['enableColumnResize'] = array(
      'default' => TRUE
    );
    // maps to slickgrid option enableColumnReorder
    $options['enableColumnReorder'] = array(
      'default' => TRUE
    );
    // maps to slickgrid option enableCellNavigation
    $options['enableCellNavigation'] = array(
      'default' => TRUE
    );
    // maps to slickgrid option asyncEditorLoading
    $options['asyncEditorLoading'] = array(
      'default' => TRUE
    );
    // maps to slickgrid option rowHeight
    $options['rowHeight'] = array(
      'default' => 24
    );
    // The following options are not internal to slickgrid & are handled by this module
    $options['select_row_checkbox'] = array(
      'default' => FALSE
    );
    $options['viewport_height'] = array(
      'default' => 200
    );
    return $options;
  }

  /**
   * Normalize a list of columns based upon the fields that are
   * available. This compares the fields stored in the style handler
   * to the list of fields actually in the view, removing fields that
   * have been removed and adding new fields in their own column.
   *
   * - Each field must be in a column.
   * - Each column must be based upon a field, and that field
   * is somewhere in the column.
   * - Any fields not currently represented must be added.
   * - Columns must be re-ordered to match the fields.
   *
   * @param $columns
   * An array of all fields; the key is the id of the field and the
   * value is the id of the column the field should be in.
   * @param $fields
   * The fields to use for the columns. If not provided, they will
   * be requested from the current display. The running render should
   * send the fields through, as they may be different than what the
   * display has listed due to access control or other changes.
   */
  function sanitize_columns($columns, $fields = NULL){
    $sanitized = array();
    if($fields === NULL){
      $fields = $this->display->handler->get_option('fields');
    }
    // Preconfigure the sanitized array so that the order is retained.
    foreach($fields as $field => $info){
      // Set to itself so that if it isn't touched, it gets column
      // status automatically.
      $sanitized[$field] = $field;
    }
    foreach($columns as $field => $column){
      // first, make sure the field still exists.
      if(!isset($sanitized[$field])){
        continue;
      }
      // If the field is the column, mark it so, or the column
      // it's set to is a column, that's ok
      if($field == $column || $columns[$column] == $column && !empty($sanitized[$column])){
        $sanitized[$field] = $column;
      }
       // Since we set the field to itself initially, ignoring
    // the condition is ok; the field will get its column
    // status back.
    }
    return $sanitized;
  }

  /**
   * Add settings for the particular slickgrid.
   */
  function options_form(&$form, &$form_state){
    parent::options_form($form, $form_state);
    $handlers = $this->display->handler->get_handlers('field');
    $columns = $this->sanitize_columns($this->options['columns']);
    $field_names = $this->display->handler->get_field_labels();
    if(empty($columns)){
      $form['error_markup'] = array(
        '#value' => t('You need at least one field before you can configure your table settings'),
        '#prefix' => '<div class="error form-item description">',
        '#suffix' => '</div>'
      );
      return;
    }
    // Create an array of allowed columns from the data we know:
    foreach($columns as $field => $column){
      // Field for setting a column's width
      $form['columns'][$field]['width'] = array(
        '#type' => 'textfield',
        '#default_value' => ($this->options['columns'][$field]['width'] ? $this->options['columns'][$field]['width'] : 50),
        '#size' => 10,
        '#maxlength' => 10
      );
      // Field for setting a column's width
      $form['columns'][$field]['filter'] = array(
        '#type' => 'checkbox',
        '#default_value' => $this->options['columns'][$field]['filter'],
        '#size' => 10
      );
      // Can this field be sorted?
      if($handlers[$field]->click_sortable()){
        // Field for setting if a column is sortable
        $form['columns'][$field]['sortable'] = array(
          '#type' => 'checkbox',
          '#default_value' => $this->options['columns'][$field]['sortable'],
          '#size' => 10
        );
      }
      // markup for the field name
      $form['columns'][$field]['name'] = array(
        '#value' => $field_names[$field]
      );
    }
    if($this->uses_fields()){
      $options = array(
        '' => t('<None>')
      );
      $options += $this->display->handler->get_field_labels();
      // If there are no fields, we can't group on them.
      if(count($options) > 1){
        $form['grouping_field'] = array(
          '#type' => 'select',
          '#title' => t('Grouping field'),
          '#options' => $options,
          '#default_value' => $this->options['grouping_field'],
          '#description' => t('You may optionally specify a field by which to group the records. Leave blank to not group.')
        );
        $form['collapse_groups_by_default'] = array(
          '#type' => 'checkbox',
          '#title' => t('Collapse groups'),
          '#description' => t('All groups should be collapsed by default.'),
          '#default_value' => !empty($this->options['collapse_groups_by_default'])
        );
      }
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
    $form['asyncEditorLoading'] = array(
      '#type' => 'checkbox',
      '#title' => t('A sync editor loading'),
      '#default_value' => !empty($this->options['asyncEditorLoading'])
    );
    $form['select_row_checkbox'] = array(
      '#type' => 'checkbox',
      '#title' => t('Row selection'),
      '#description' => t('Add a checkbox to select rows.'),
      '#default_value' => !empty($this->options['select_row_checkbox'])
    );
    $form['select_columns'] = array(
      '#type' => 'checkbox',
      '#title' => t('Column visibility'),
      '#description' => t('Allow users to show &amp; hide columns'),
      '#default_value' => !empty($this->options['select_columns'])
    );
    $form['row_delete'] = array(
      '#title' => t('Delete rows'),
      '#description' => t('Allow users to delete rows'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['row_delete']
    );
    $form['viewport_height'] = array(
      '#title' => t('Viewport height'),
      '#type' => 'textfield',
      '#default_value' => $this->options['viewport_height'],
      '#size' => 10,
      '#maxlength' => 10
    );
    $form['rowHeight'] = array(
      '#title' => t('Row height'),
      '#type' => 'textfield',
      '#default_value' => $this->options['rowHeight'],
      '#size' => 10,
      '#maxlength' => 10
    );
    $form['#theme'] = 'slickgrid_views_plugin_table';
  }

  function query(){
    // We always want the node type to be returned
    $this->view->query->add_field('node', 'type');
  }
}
?>