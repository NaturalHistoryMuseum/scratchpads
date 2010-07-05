<?php

/**
 * Implementation of hook_views_default_views().
 */
function matrix_editor_views_default_views(){
  module_load_include('inc', 'content', 'includes/content.crud');
  $node_types = node_get_types();
  foreach($node_types as $node_type){
    $view_name = 'matrix_editor_' . $node_type->type;
    $view = new view();
    $view->name = $view_name;
    $view->description = 'Matrix editor for "' . $node_type->name . '"';
    $view->tag = 'matrix_editor';
    $view->view_php = '';
    $view->base_table = 'node';
    $view->is_cacheable = FALSE;
    $view->api_version = 2;
    $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
    $handler = $view->new_display('default', 'Defaults', 'default');
    // Create the fields
    $views_fields = array();
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
    }
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
    }
    //    Vocabulary fields
    $vocabularies = taxonomy_get_vocabularies($node_type->type);
    if(count($vocabularies)){
      foreach($vocabularies as $vocabulary){
        $views_fields['tid_' . $vocabulary->vid] = array(
          'label' => $vocabulary->name, 
          'alter' => array(), 
          'type' => 'separator', 
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
      }
    }
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
        switch($field['type']){
          case 'filefield':
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
        }
      }
    }
    $handler->override_option('fields', $views_fields);
    // Add node type filter
    $handler->override_option('filters', array(
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
    ));
    $handler->override_option('access', array(
      'type' => 'none'
    ));
    $handler->override_option('cache', array(
      'type' => 'none'
    ));

    $handler->override_option('empty', t('Sorry no %node_type nodes could be found.', array(
      '%node_type' => $node_type->name
    )));
    $handler->override_option('empty_format', '1');
    $handler->override_option('items_per_page', 0);
    // Define the matrix plugin
    $handler->override_option('style_plugin', 'matrix');
    $handler->override_option('style_options', array(
      'editable' => 1, 
      'enableColumnResize' => 1, 
      'enableColumnReorder' => 1, 
      'fixedFirstColumn' => 0, 
      'asyncEditorLoading' => 1, 
      'viewportHeight' => 500, 
      'rowHeight' => 100
    ));
    $handler = $view->new_display('page', 'Page', 'page_1');
    $type_url_str = str_replace('_', '-', $node_type->type);
    $handler->override_option('path', 'matrix-editor/' . $type_url_str);
    $handler->override_option('menu', array(
      'type' => 'none', 
      'title' => '', 
      'description' => '', 
      'weight' => 0, 
      'name' => 'navigation'
    ));
    $views[$view->name] = $view;
  } // EO For each node type loop
  return $views;
}  

