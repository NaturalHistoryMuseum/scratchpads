<?php

/**
 * Implementation of hook_views_default_views().
 * Loop through all the node types & define grid editor views for them all
 */
function scratchpad_slickgrid_views_default_views(){
  $node_types = node_get_types();
  $batch_node_types = scratchpad_slickgrid_get_batch_node_types();
  foreach($node_types as $node_type){
    $style_option_settings = array();
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
    // Are there batches for this content type
    if(in_array($node_type->type, $batch_node_types)){
      $views_fields['batch_created'] = array(
        'label' => t('Batch created'),
        'alter' => array(),
        'exclude' => 0,
        'date_format' => 'custom',
        'custom_date_format' => 'd/m/y H:i',
        'id' => 'batch_created',
        'table' => 'scratchpad_slickgrid_batches',
        'field' => 'created',
        'relationship' => 'none'
      );
      $columns['batch_created'] = array(
        'width' => 50,
        'sortable' => true,
        'filter' => 'textFilter'
      );
    }
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
    if($node_type->has_body && $node_type->type != 'darwincore' && $node_type->type != 'biblio'){ // DWC hides the body field??
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
    // Add the groups field - not editable but used for the filters
    $views_fields['group_nid'] = array(
      'label' => t('Groups'),
      'type' => 'ul',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'group_nid',
      'table' => 'og_ancestry',
      'field' => 'group_nid'
    );
    $columns['group_nid'] = array(
      'width' => 50,
      'sortable' => true,
      'filter' => 'textFilter',
      'editor' => 'nodeFormEditor'
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
          'label_type' => 'widget',
          'link_to_node' => 0
        );
        $columns[$field_id] = array(
          'width' => 100,
          'editor' => 'nodeFormEditor',
          'sortable' => true,
          'filter' => 'textFilter'
        );
        switch($field['type']){
          case 'filefield': // If this is an image field, need to add thumbnail formatting & multi image handling
            if($field['widget']['type'] == 'imagefield_widget'){
              $views_fields[$field_id]['format'] = 'slickgrid_thumbnail_default';
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
    // Get additional fields from the views definition?
    $views_data = views_fetch_data($node_type->type);
    if(is_array($views_data)){
      foreach($views_data as $id => $views_data_item){
        if(isset($views_data_item['field'])){
          $views_fields[$id] = array(
            'label' => $views_data_item['title'],
            'alter' => array(),
            'exclude' => 0,
            'id' => $id,
            'table' => $node_type->type,
            'field' => $id,
            'relationship' => 'none',
            'label_type' => 'widget'
          );
          $columns[$id] = array(
            'width' => 100,
            'editor' => 'nodeFormEditor',
            'sortable' => true,
            'filter' => 'textFilter'
          );
        }
      }
    }
    $style_option_settings['hidden_columns'] = scratchpad_slickgrid_hidden_fields($node_type->type);
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
    // Arguments
    $arguments['bid'] = array(
      'default_action' => 'ignore',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '',
      'breadcrumb' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'break_phrase' => 0,
      'not' => 0,
      'id' => 'bid',
      'table' => 'scratchpad_slickgrid_batches',
      'field' => 'bid'
    );
    // add the arguments to the view
    $handler->override_option('arguments', $arguments);
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
    $handler->override_option('distinct', 1);
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
      'autoEdit' => true,
      'viewportHeight' => 500,
      'rowHeight' => 100,
      'columns' => $columns,
      'settings' => $style_option_settings
    ));
    // Define the page
    $handler = $view->new_display('views_batch_page', 'Batch page', 'views_batch_page_1');
    $handler->override_option('path', 'matrix-editor/' . str_replace('_', '-', $node_type->type));
    // We do not want a menu item
    $handler->override_option('menu', array(
      'type' => 'none',
      'title' => '',
      'description' => '',
      'weight' => 0,
      'name' => 'navigation'
    ));
    // Set pager
    $handler->override_option('items_per_page', 0);
    $handler->override_option('use_pager', '1');
    $views[$view->name] = $view;
  } // End of foreach($node_types as $node_type){
  return $views;
}

function scratchpad_slickgrid_hidden_fields($node_type){
  $hidden_fields = array(
    'image' => array(
      'batch_created',
      'name',
      'created',
      'group_nid'
    ),
    'darwincore' => array(
      'sex',
      'basisofrecord',
      'typestatus',
      'taxonomicname',
      'identificationqualifier',
      'identifiedby',
      'dateidentifiedyear',
      'dateidentifiedmonth',
      'dateidentifiedday',
      'collectornumber',
      'fieldnumber',
      'collector',
      'earliestdatecollected',
      'latestdatecollected',
      'fieldnotes',
      'othercataloguenum',
      'lifestage',
      'count',
      'genbanknum',
      'remarks',
      'name',
      'created',
      'group_nid'
    ),
    'biblio' => array(
      'citation',
      'biblio_secondary_authors',
      'biblio_tertiary_authors',
      'biblio_subsidiary_authors',
      'biblio_corp_authors',
      'biblio_secondary_title',
      'biblio_tertiary_title',
      'biblio_accession_number',
      'biblio_call_number',
      'biblio_other_number',
      'biblio_other_author_affiliations',
      'biblio_publisher',
      'biblio_place_published',
      'biblio_edition',
      'biblio_volume',
      'biblio_number',
      'biblio_pages',
      'biblio_date',
      'biblio_lang',
      'biblio_abst_e',
      'biblio_abst_f',
      'biblio_type_of_work',
      'biblio_url',
      'biblio_issue',
      'biblio_research_notes',
      'biblio_custom1',
      'biblio_custom2',
      'biblio_custom3',
      'biblio_custom4',
      'biblio_custom5',
      'biblio_custom6',
      'biblio_custom7',
      'biblio_number_of_volumes',
      'biblio_short_title',
      'biblio_alternate_title',
      'biblio_translated_title',
      'biblio_original_publication',
      'biblio_reprint_edition',
      'biblio_section',
      'biblio_citekey',
      'biblio_coins',
      'biblio_issn',
      'biblio_doi',
      'biblio_auth_address',
      'biblio_remote_db_name',
      'biblio_remote_db_provider',
      'biblio_label',
      'biblio_access_date',
      'biblio_refereed',
      'name',
      'created',
      'group_nid'
    )
  );
  if($hidden_fields[$node_type]){
    return $hidden_fields[$node_type];
  }else{
    // Default hidden fields
    return array(
      'name',
      'created',
      'group_nid'
    );
  }
}

