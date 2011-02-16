<?php

/**
 * Implementation of hook_views_default_views().
 * Loop through all the node types & define grid editor views for them all
 */
function slickgrid_views_default_views(){
  $node_types = node_get_types();
  foreach($node_types as $node_type){
    $view_name = 'slickgrid_' . $node_type->type;
    $view = new view();
    $view->name = $view_name;
    $view->description = 'Grid editor for "' . $node_type->name . '"';
    $view->tag = 'slickgrid';
    $view->base_table = 'node';
    $view->is_cacheable = FALSE;
    $view->api_version = 2;
    $view->disabled = FALSE;
    $handler = $view->new_display('default', 'Defaults', 'default');
    // Define the fields
    $views_fields = array();
    // If there's a title field, add it
    if($node_type->has_title){
      $views_fields['title'] = array(
        'label' => $node_type->title_label,
        'alter' => array(),
        'link_to_node' => 0,
        'exclude' => 0,
        'id' => 'title',
        'table' => 'node',
        'field' => 'title',
        'relationship' => 'none'
      );
      $columns['title'] = array(
        'width' => 100,
        'sortable' => true,
        'editor' => 'TextCellEditor',
        'validator' => 'requiredFieldValidator',
        'filter' => 'textFilter'
      );
    }
    // If there's a body field, add it
    if($node_type->has_body){
      $views_fields['body'] = array(
        'label' => $node_type->body_label,
        'alter' => array(),
        'link_to_node' => 0,
        'exclude' => 0,
        'id' => 'title',
        'table' => 'node_revisions',
        'field' => 'body'
      );
      $columns['body'] = array(
        'width' => 100,
        'sortable' => true,
        'editor' => 'nodeFormEditor'
      );
    }
    // Add the author field - not editable but used for the filters
    $views_fields['name'] = array(
      'label' => t('Author'),
      'link_to_user' => 0,
      'alter' => array(),
      'exclude' => 0,
      'id' => 'name',
      'table' => 'users',
      'field' => 'name'
    );
    $columns['name'] = array(
      'width' => 50,
      'sortable' => true,
      'filter' => 'textFilter'
    );
    // Add the date created field - not editable but used for the filters
    $views_fields['created'] = array(
      'label' => t('Date created'),
      'date_format' => 'custom',
      'custom_date_format' => 'd/m/y',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'created',
      'table' => 'node',
      'field' => 'created'
    );
    $columns['created'] = array(
      'width' => 50,
      'sortable' => true,
      'filter' => 'textFilter'
    );
    //    Vocabulary fields
    $vocabularies = taxonomy_get_vocabularies($node_type->type);
    if(count($vocabularies)){
      foreach($vocabularies as $vocabulary){
        $field_name = 'tid_' . $vocabulary->vid;
        $views_fields[$field_name] = array(
          'label' => $vocabulary->name,
          'alter' => array(),
          'type' => 'ul',
          'separator' => ', ',
          'empty' => '',
          'link_to_taxonomy' => 0,
          'limit' => 1,
          'vids' => array(
            $vocabulary->vid => $vocabulary->vid
          ),
          'exclude' => 0,
          'id' => 'tid_' . $vocabulary->vid,
          'table' => 'term_node',
          'field' => 'tid',
          'relationship' => 'none'
        );
        $columns[$field_name] = array(
          'width' => 100,
          'sortable' => true,
          'filter' => 'textFilter',
          'editor' => 'nodeFormEditor',
          'validator' => ($vocabulary->required ? 'requiredFieldValidator' : NULL)
        );
      }
    }
    // Add the CCK fields
    $content_type = content_types($node_type->type);
    if(is_array($fields = $content_type['fields'])){
      foreach($fields as $field){
        $field_id = $field['field_name'];
        $views_fields[$field_id] = array(
          'label' => $field['widget']['label'],
          'alter' => array(),
          'exclude' => 0,
          'id' => $field_id,
          'table' => 'node_data_' . $field['field_name'],
          'field' => $field_id . '_' . array_shift(array_keys($field['columns'])),
          'relationship' => 'none',
          'label_type' => 'widget'
        );
        $columns[$field_id] = array(
          'width' => 100,
          'editor' => 'nodeFormEditor'
        );
        switch($field['type']){
          case 'filefield': // If this is an image field, need to add thumbnail formatting & multi image handling
            if($field['widget']['type'] == 'imagefield_widget'){
              $views_fields[$field_id]['format'] = 'matrix_editor_thumbnail_default';
              $views_fields[$field_id]['multiple'] = array(
                'group' => 1,
                'multiple_number' => '',
                'multiple_from' => '',
                'multiple_reversed' => 0
              );
            }
            break;
          case 'text':
            if($field['required']){
              $columns[$field_id]['validator'] = 'requiredFieldValidator';
            }
            break;
        }
      }
    }
    // Add the fields to the view
    $handler->override_option('fields', $views_fields);
    // Sorting
    // Created date descending
    $handler->override_option('sorts', array(
      'created' => array(
        'order' => 'DESC',
        'granularity' => 'second',
        'id' => 'created',
        'table' => 'node',
        'field' => 'created',
        'relationship' => 'none'
      )
    ));
    // Define the filters
    // Node type filter
    $filters = array(
      'type' => array(
        'operator' => 'in',
        'value' => array(
          $node_type->type => $node_type->type
        ),
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
          'operator' => FALSE,
          'label' => ''
        ),
        'id' => 'type',
        'table' => 'node',
        'field' => 'type',
        'override' => array(
          'button' => 'Override'
        ),
        'relationship' => 'none'
      )
    );
    // If editor views exists, add the filter
    if(module_exists('editor_views')){
      $filters['edit_delete'] = array(
        'operator' => 'in',
        'value' => array(
          'update' => 'update'
        ),
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
          'operator' => FALSE,
          'label' => ''
        ),
        'id' => 'edit_delete',
        'table' => 'node_access',
        'field' => 'edit_delete',
        'relationship' => 'none'
      );
    }
    // add the filters to the view
    $handler->override_option('filters', $filters);
    // Set access options
    $handler->override_option('access', array(
      'type' => 'none'
    ));
    // Set cache options
    $handler->override_option('cache', array(
      'type' => 'none'
    ));
    // Set empty text
    $handler->override_option('empty', t('Sorry no %node_type nodes could be found.', array(
      '%node_type' => $node_type->name
    )));
    $handler->override_option('empty_format', '1');
    // Define the slickgrid plugin
    $handler->override_option('style_plugin', 'slickgrid');
    // And the slickgrid options
    $handler->override_option('style_options', array(
      'enableColumnResize' => true,
      'enableColumnReorder' => true,
      'select_columns' => true,
      'delete_nodes' => true,
      'asyncEditorLoading' => true,
      'forceFitColumns' => true,
      'multi_edit' => true,
      'undo' => true,
      'viewportHeight' => 500,
      'rowHeight' => 100,
      'columns' => $columns
    ));
    // Define the page
    $handler = $view->new_display('page', 'Page', 'page_1');
    $handler->override_option('path', 'grid/' . str_replace('_', '-', $node_type->type));
    // We do not want a menu item
    $handler->override_option('menu', array(
      'type' => 'none',
      'title' => '',
      'description' => '',
      'weight' => 0,
      'name' => 'navigation'
    ));
    // Set pager
    $handler->override_option('items_per_page', 50);
    $handler->override_option('use_pager', '1');
    $views[$view->name] = $view;
  } // End of foreach($node_types as $node_type){
  return $views;
}  