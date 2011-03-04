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
    $options['grouping_field'] = array(
      'default' => NULL
    );
    $options['collapse_groups_by_default'] = array(
      'default' => FALSE
    );
    $options['collapsible_taxonomy_field'] = array(
      'default' => NULL
    );
    // maps to slickgrid option enableColumnResize
    $options['enableColumnResize'] = array(
      'default' => TRUE
    );
    // maps to slickgrid option enableColumnReorder
    $options['enableColumnReorder'] = array(
      'default' => TRUE
    );
    $options['select_columns'] = array(
      'default' => TRUE
    );
    $options['delete_nodes'] = array(
      'default' => TRUE
    );
    // maps to slickgrid option asyncEditorLoading
    $options['asyncEditorLoading'] = array(
      'default' => TRUE
    );
    // maps to slickgrid option enableCellNavigation
    $options['autoEdit'] = array(
      'default' => FALSE
    );
    // maps to slickgrid option forceFitColumns
    $options['forceFitColumns'] = array(
      'default' => false
    );
    // maps to slickgrid option forceFitColumns
    $options['multi_edit'] = array(
      'default' => false
    );
    // maps to slickgrid option forceFitColumns
    $options['undo'] = array(
      'default' => false
    );
    $options['viewport_height'] = array(
      'default' => 500
    );
    // maps to slickgrid option rowHeight
    $options['rowHeight'] = array(
      'default' => 30
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
    if(is_array($columns)){
      foreach($columns as $field => $column){
        // first, make sure the field still exists.
        if(!isset($sanitized[$field])){
          continue;
        }
        $column_name = key($column);
        // If the field is the column, mark it so, or the column
        // it's set to is a column, that's ok
        if($field == $column_name || $columns[$column_name] == $column && !empty($sanitized[$column_name])){
          $sanitized[$field] = $column;
        }
         // Since we set the field to itself initially, ignoring
      // the condition is ok; the field will get its column
      // status back.
      }
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
        '#value' => t('You need at least one field before you can configure your slickgrid settings'),
        '#prefix' => '<div class="error form-item description">',
        '#suffix' => '</div>'
      );
      return;
    }
    // Array of node term fields that can be selected to be a collapsible taxonomy 
    $collapsible_taxonomy_options = array(
      '' => t('<None>')
    );
    // Create an array of allowed columns from the data we know:
    foreach($columns as $field => $column){
      if($handlers[$field]->options['exclude']){
        continue;
      }
      // Field for setting a column's width
      $form['columns'][$field]['width'] = array(
        '#type' => 'textfield',
        '#default_value' => ($this->options['columns'][$field]['width'] ? $this->options['columns'][$field]['width'] : 100),
        '#size' => 10,
        '#maxlength' => 10
      );
      // Can this field be sorted?
      if($handlers[$field]->click_sortable()){
        // Field for setting if a column is sortable
        $form['columns'][$field]['sortable'] = array(
          '#type' => 'checkbox',
          '#default_value' => ($this->options['columns'][$field]['sortable'] ? $this->options['columns'][$field]['sortable'] : false),
          '#size' => 10
        );
      }
      $filters = array();
      // Is this a CCK field?
      // If it is we know its editable
      if(isset($handlers[$field]->content_field['widget']['type'])){
        $filters['field_type'] = $handlers[$field]->content_field['widget']['type'];
      }else{
        // If it's not a CCK field, make a field_type from {table}_{field}
        // We can then define editors for node fields (eg: node_title) and any other fields
        $filters['field_type'] = $handlers[$field]->table . '_' . $handlers[$field]->real_field;
      }
      // Is this a term node type?
      if($filters['field_type'] == 'term_node_tid'){
        // If it is, this can be provided as "collapsible taxonomy" option
        $collapsible_taxonomy_options[$field] = $field_names[$field];
      }
      foreach(slickgrid_get_plugin_types() as $plugin_type => $plugin_label){
        // Only allow editing (and therefore validation) if this is an editable field
        if(($plugin_type == 'editor' || $plugin_type == 'validator') && !$this->is_editable($handlers[$field], $filters['field_type'])){
          // Field locked so do not try and find an editor
          // Add HTML warning & continue() to next to plugin type
          $form['columns'][$field][$plugin_type] = array(
            '#value' => t('This field cannot be edited.')
          );
          continue;
        }
        $filters['plugin_type'] = $plugin_type;
        $plugins = slickgrid_get_plugins($filters);
        // Are there plugins we can use for this field?
        if(count($plugins)){
          $form['columns'][$field][$plugin_type] = array(
            '#type' => 'select',
            '#default_value' => ($this->options['columns'][$field][$plugin_type] ? $this->options['columns'][$field][$plugin_type] : ''),
            '#options' => $this->get_plugin_options($plugins)
          );
        }else{
          $form['columns'][$field][$plugin_type] = array(
            '#value' => t('No %plugin_label plugins for this field.', array(
              '%plugin_label' => strtolower($plugin_label)
            ))
          );
        }
      }
      // Field for setting if a column is editable
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
          '#default_value' => $this->options['collapse_groups_by_default']
        );
      }
      if(count($collapsible_taxonomy_options) > 1){ // Default value of <none> so needs to be greater than 1 if there's actually any fields
        $form['collapsible_taxonomy_field'] = array(
          '#type' => 'select',
          '#title' => t('Collapsible taxonomy field'),
          '#options' => $collapsible_taxonomy_options,
          '#default_value' => $this->options['collapsible_taxonomy_field'],
          '#description' => t('You may optionally specify a collapsible taxonomy field. Setting this will add a taxonomy parent relationship & ordering to the view.')
        );
      }
    }
    $form['enableColumnResize'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable resizing columns'),
      '#default_value' => $this->options['enableColumnResize']
    );
    $form['enableColumnReorder'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable reordering columns'),
      '#default_value' => $this->options['enableColumnReorder']
    );
    $form['select_columns'] = array(
      '#type' => 'checkbox',
      '#title' => t('Column visibility'),
      '#description' => t('Allow users to show &amp; hide columns'),
      '#default_value' => $this->options['select_columns']
    );
    $form['delete_nodes'] = array(
      '#title' => t('Delete nodes'),
      '#description' => t('Allow users to delete nodes from within the slickgrid.'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['delete_nodes']
    );
    $form['asyncEditorLoading'] = array(
      '#type' => 'checkbox',
      '#title' => t('A sync editor loading'),
      '#default_value' => $this->options['asyncEditorLoading']
    );
    $form['autoEdit'] = array(
      '#type' => 'checkbox',
      '#title' => t('Auto edit'),
      '#default_value' => $this->options['autoEdit'],
      '#description' => t('Activate edit on entry to cell, otherwise double click to edit.')
    );
    $form['forceFitColumns'] = array(
      '#type' => 'checkbox',
      '#title' => t('Force fit columns'),
      '#default_value' => $this->options['forceFitColumns'],
      '#description' => t('Force column widths to fit the grid.')
    );
    $form['multi_edit'] = array(
      '#type' => 'checkbox',
      '#title' => t('Multi-edit'),
      '#description' => t('Users can select & edit multiple nodes at once.'),
      '#default_value' => $this->options['multi_edit']
    );
    $form['undo'] = array(
      '#type' => 'checkbox',
      '#title' => t('Undo'),
      '#description' => t('Allow users to undo updates. Warning: if turned on, all updates will create a node revision.'),
      '#default_value' => $this->options['undo']
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

  function options_submit(){
    // If admin updates the node, delete the user customisation
    slickgrid_delete_settings(array(
      'view_name' => $this->view->name
    ));
  }

  function query(){
    // We always want the node type to be returned
    $this->view->query->add_field('node', 'type');
  }

  // Is this field editable? Lock any fields which may break the editor
  function is_editable($handler, $field_type){
    // Is this a taxonomy field?  	
    if(in_array($handler->table, array(
      'term_node',
      'term_data',
      'vocabulary'
    ))){
      // If it is a taxonomy field, it is only editable if it has one VID selected      
      if(!(is_array($handler->options['vids']) && slickgrid_taxonomy_field_get_vid($handler))){return FALSE;}
    }
    // Is this a locked field?
    // NID & node type should never be editable. Are there any other fields that need to be locked?
    if(in_array($field_type, array(
      'node_nid',
      'node_type'
    ))){return FALSE;}
    return TRUE;
  }

  // Get a FAPI select list optiuons from an array of plugins
  function get_plugin_options($plugins){
    $options = array(
      '' => 'None'
    );
    // Just want the plugin description in the option field
    // Use array_map and anonymous funciton to retrieve the description
    return $options + (array)array_map(create_function('$plugins', 'return $plugins["description"];'), $plugins);
  }
}
?>