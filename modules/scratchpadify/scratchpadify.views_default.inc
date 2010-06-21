<?php

function scratchpadify_views_default_views(){
  // Lets get all the content types
  if(!function_exists('content_types')){return;}
  $content_types = content_types();
  /*
   * What do we need from hook_ispecies array
   * 
   * name (will have module prepended, so only needs to be unique per module)
   * title
   * description
   * callback_url 
   */
  // Don't create the following views
  unset($content_types['ispecies']);
  unset($content_types['glossary']);
  unset($content_types['nexus_project']);
  unset($content_types['nexus_free_state']);
  unset($content_types['nexus_note']);
  unset($content_types['nexus_controlled_state']);
  foreach($content_types as $type => $type_details){
    // Begin copy and paste of output from the Export tab of a view.
    $view = new view();
    $view->name = $type;
    $view->description = "[Full nodes view] " . strip_tags($type_details['description']);
    $view->tag = '';
    $view->view_php = '';
    $view->base_table = 'node';
    $view->is_cacheable = FALSE;
    $view->api_version = 2;
    $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
    $handler = $view->new_display('default', 'Defaults', 'default');
    $handler->override_option('sorts', array(
      'weight' => array(
        'order' => 'ASC',
        'id' => 'weight',
        'table' => 'node_weight',
        'field' => 'weight',
        'relationship' => 'none'
      ),
      'changed' => array(
        'order' => 'DESC',
        'id' => 'changed',
        'table' => 'node',
        'field' => 'changed'
      )
    ));
    $handler->override_option('arguments', array(
      'tid' => array(
        'default_action' => 'not found',
        'style_plugin' => 'default_summary',
        'style_options' => array(),
        'wildcard' => 'all',
        'wildcard_substitution' => 'All',
        'title' => '',
        'default_argument_type' => 'fixed',
        'default_argument' => '',
        'validate_type' => 'none',
        'validate_fail' => 'not found',
        'break_phrase' => 0,
        'add_table' => 0,
        'require_value' => 0,
        'reduce_duplicates' => 0,
        'set_breadcrumb' => 0,
        'id' => 'tid',
        'table' => 'term_node',
        'field' => 'tid',
        'validate_user_argument_type' => 'uid',
        'validate_user_roles' => array(),
        'relationship' => 'none',
        'default_options_div_prefix' => '',
        'default_argument_user' => 0,
        'default_argument_fixed' => '',
        'default_argument_php' => '',
        'validate_argument_node_type' => array(),
        'validate_argument_node_access' => 0,
        'validate_argument_nid_type' => 'nid',
        'validate_argument_vocabulary' => array(),
        'validate_argument_type' => 'tid',
        'validate_argument_transform' => 0,
        'validate_user_restrict_roles' => 0,
        'validate_argument_php' => ''
      )
    ));
    $handler->override_option('filters', array(
      'type' => array(
        'operator' => 'in',
        'value' => array(
          'darwincore' => $type
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
        'relationship' => 'none'
      ),
      'status_extra' => array(
        'operator' => '=',
        'value' => '',
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
          'operator' => FALSE,
          'label' => ''
        ),
        'id' => 'status_extra',
        'table' => 'node',
        'field' => 'status_extra',
        'relationship' => 'none'
      )
    ));
    $handler->override_option('access', array(
      'type' => 'none'
    ));
    $handler->override_option('row_plugin', 'node');
    $handler->override_option('title', $type_details['name']);
    // Additional page without arguments
    $handler = $view->new_display('page', 'Page', 'page_1');
    $handler->override_option('title', $type_details['name']);
    $handler->override_option('arguments', array());
    $handler->override_option('path', 'content/' . $type);
    $handler->override_option('use_pager', '1');
    // Rename og as it causes issues
    if($view->name == 'og'){
      $view->name = 'scratchpadify_og';
    }
    $views[$view->name] = $view;
  }
  // ---------------------- NEXUS CHARACTER PROJECT ----------------------------
  $view = new view();
  $view->name = 'nexus_project';
  $view->description = '[Full nodes view] Character Projects.';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('sorts', array(
    'weight' => array(
      'order' => 'ASC',
      'id' => 'weight',
      'table' => 'node_weight',
      'field' => 'weight',
      'relationship' => 'none'
    ),
    'changed' => array(
      'order' => 'DESC',
      'id' => 'changed',
      'table' => 'node',
      'field' => 'changed'
    )
  ));
  $handler->override_option('fields', array(
    'title' => array(
      'label' => '',
      'alter' => array(),
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none'
    )
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'nexus_project' => 'nexus_project'
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
      'relationship' => 'none'
    ),
    'status' => array(
      'operator' => '=',
      'value' => '1',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => ''
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none'
    )
  ));
  $handler->override_option('title', 'Character projects');
  $handler->override_option('items_per_page', 100);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'list');
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'content/nexus_project');
  $views[$view->name] = $view;
  // ---------------------- BIBLIO TABLE ---------------------------------------
  $view = new view();
  $view->name = 'biblio_table';
  $view->description = 'Biblio nodes formatted in a table';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'biblio_year' => array(
      'label' => 'Year of Publication',
      'alter' => array(),
      'biblio_label' => 1,
      'exclude' => 0,
      'id' => 'biblio_year',
      'table' => 'biblio',
      'field' => 'biblio_year',
      'relationship' => 'none'
    ),
    'biblio_authors' => array(
      'label' => 'Authors',
      'alter' => array(),
      'biblio_label' => 1,
      'style_name' => 'cse',
      'exclude' => 0,
      'id' => 'biblio_authors',
      'table' => 'biblio',
      'field' => 'biblio_authors',
      'relationship' => 'none'
    ),
    'title' => array(
      'label' => 'Title',
      'alter' => array(),
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none'
    )
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'biblio' => 'biblio'
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none'
    )
  ));
  $handler->override_option('arguments', array(
    'term_node_tid_depth' => array(
      'default_action' => 'ignore',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'depth' => '0',
      'break_phrase' => 0,
      'set_breadcrumb' => 0,
      'id' => 'term_node_tid_depth',
      'table' => 'node',
      'field' => 'term_node_tid_depth',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
      'validate_argument_php' => ''
    )
  ));
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'biblio_authors' => 'biblio_authors',
      'title' => 'title',
      'biblio_year' => 'biblio_year'
    ),
    'info' => array(
      'biblio_authors' => array(
        'separator' => ''
      ),
      'title' => array(
        'sortable' => 0,
        'separator' => ''
      ),
      'biblio_year' => array(
        'separator' => ''
      )
    ),
    'default' => '-1'
  ));
  $handler->override_option('title', 'Bibliography');
  $handler->override_option('items_per_page', 0);
  $views[$view->name] = $view;
  // ---------------------- VOCABULARIES ---------------------------------------
  $view = new view();
  $view->name = 'vocabularies';
  $view->description = '';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'term_data';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'depth' => array(
      'label' => '',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'depth',
      'table' => 'leftandright',
      'field' => 'depth',
      'relationship' => 'none',
      'description' => 1
    )
  ));
  $handler->override_option('sorts', array(
    'lft' => array(
      'order' => 'ASC',
      'id' => 'lft',
      'table' => 'leftandright',
      'field' => 'lft',
      'relationship' => 'none'
    )
  ));
  $handler->override_option('arguments', array(
    'vid' => array(
      'default_action' => 'empty',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '%1',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'id' => 'vid',
      'table' => 'vocabulary',
      'field' => 'vid',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0
    )
  ));
  $handler->override_option('access', array(
    'type' => 'none'
  ));
  $handler->override_option('items_per_page', 0);
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'vocabularies');
  $views[$view->name] = $view;
  // ---------------------- MATRIX EDITOR: IMAGES ------------------------------
  $view = new view();
  $view->name = 'matrix_images';
  $view->description = 'Matrix editor images';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'title' => array(
      'label' => 'Title',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none'
    ),
    'tid_1' => array(
      'label' => 'All terms',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'type' => 'separator',
      'separator' => ', ',
      'empty' => '',
      'link_to_taxonomy' => 0,
      'limit' => 1,
      'vids' => array(
        '9' => 9,
        '1' => 0,
        '5' => 0,
        '18' => 0,
        '8' => 0,
        '3' => 0,
        '11' => 0,
        '2' => 0,
        '10' => 0,
        '17' => 0,
        '6' => 0,
        '4' => 0,
        '16' => 0,
        '13' => 0,
        '14' => 0,
        '7' => 0
      ),
      'exclude' => 0,
      'id' => 'tid_1',
      'table' => 'term_node',
      'field' => 'tid',
      'relationship' => 'none'
    ),
    'image_publication_nid' => array(
      'label' => 'Publication',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'link_to_node' => 0,
      'label_type' => 'widget',
      'format' => 'plain',
      'multiple' => array(
        'group' => TRUE,
        'multiple_number' => '',
        'multiple_from' => '',
        'multiple_reversed' => FALSE
      ),
      'exclude' => 0,
      'id' => 'image_publication_nid',
      'table' => 'node_data_image_publication',
      'field' => 'image_publication_nid',
      'relationship' => 'none'
    ),
    'tid' => array(
      'label' => 'All terms',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'type' => 'separator',
      'separator' => ', ',
      'empty' => '',
      'link_to_taxonomy' => 0,
      'limit' => 1,
      'vids' => array(
        '3' => 3,
        '1' => 0,
        '5' => 0,
        '18' => 0,
        '8' => 0,
        '9' => 0,
        '11' => 0,
        '2' => 0,
        '10' => 0,
        '17' => 0,
        '6' => 0,
        '4' => 0,
        '16' => 0,
        '13' => 0,
        '14' => 0,
        '7' => 0
      ),
      'exclude' => 0,
      'id' => 'tid',
      'table' => 'term_node',
      'field' => 'tid',
      'relationship' => 'none'
    ),
    'image_specimen_nid' => array(
      'label' => 'Specimen',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'link_to_node' => 0,
      'label_type' => 'widget',
      'format' => 'plain',
      'multiple' => array(
        'group' => TRUE,
        'multiple_number' => '',
        'multiple_from' => '',
        'multiple_reversed' => FALSE
      ),
      'exclude' => 0,
      'id' => 'image_specimen_nid',
      'table' => 'node_data_image_specimen',
      'field' => 'image_specimen_nid',
      'relationship' => 'none'
    )
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'image' => 'image'
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
      'relationship' => 'none'
    )
  ));
  $handler->override_option('access', array(
    'type' => 'none'
  ));
  $handler->override_option('style_plugin', 'matrix');
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'matrix/image');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation'
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0
  ));
  $views[$view->name] = $view;
  // ---------------------- MATRIX EDITOR: SPECIMENS ---------------------------
  $view = new view();
  $view->name = 'matrix_editor_specimens';
  $view->description = 'Matrix editor for specimen records';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'basisofrecord' => array(
      'label' => 'Basis of record',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'link_to_taxonomy' => 0,
      'exclude' => 0,
      'id' => 'basisofrecord',
      'table' => 'darwincore',
      'field' => 'basisofrecord',
      'relationship' => 'none'
    ),
    'cataloguenumbertext' => array(
      'label' => 'Catalogue number text',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'cataloguenumbertext',
      'table' => 'darwincore',
      'field' => 'cataloguenumbertext',
      'relationship' => 'none'
    ),
    'collector' => array(
      'label' => 'Collector',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'collector',
      'table' => 'darwincore',
      'field' => 'collector',
      'relationship' => 'none'
    ),
    'collectioncode' => array(
      'label' => 'Collection code',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'collectioncode',
      'table' => 'darwincore',
      'field' => 'collectioncode',
      'relationship' => 'none'
    ),
    'collectornumber' => array(
      'label' => 'Collector number',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'collectornumber',
      'table' => 'darwincore',
      'field' => 'collectornumber',
      'relationship' => 'none'
    ),
    'count' => array(
      'label' => 'Count',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'count',
      'table' => 'darwincore',
      'field' => 'count',
      'relationship' => 'none'
    ),
    'dateidentified' => array(
      'label' => 'Date identified',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'dateidentified',
      'table' => 'darwincore',
      'field' => 'dateidentified',
      'relationship' => 'none'
    ),
    'earliestdatecollected' => array(
      'label' => 'Earliest date collected',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'earliestdatecollected',
      'table' => 'darwincore',
      'field' => 'earliestdatecollected',
      'relationship' => 'none'
    ),
    'fieldnotes' => array(
      'label' => 'Field notes',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'fieldnotes',
      'table' => 'darwincore',
      'field' => 'fieldnotes',
      'relationship' => 'none'
    ),
    'fieldnumber' => array(
      'label' => 'Field number',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'fieldnumber',
      'table' => 'darwincore',
      'field' => 'fieldnumber',
      'relationship' => 'none'
    ),
    'genbanknum' => array(
      'label' => 'Genbank number',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'genbanknum',
      'table' => 'darwincore',
      'field' => 'genbanknum',
      'relationship' => 'none'
    ),
    'identificationqualifier' => array(
      'label' => 'Identification qualifier/modifier',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'identificationqualifier',
      'table' => 'darwincore',
      'field' => 'identificationqualifier',
      'relationship' => 'none'
    ),
    'identifiedby' => array(
      'label' => 'Identified by',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'identifiedby',
      'table' => 'darwincore',
      'field' => 'identifiedby',
      'relationship' => 'none'
    ),
    'institutioncode' => array(
      'label' => 'Institution code',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'institutioncode',
      'table' => 'darwincore',
      'field' => 'institutioncode',
      'relationship' => 'none'
    ),
    'latestdatecollected' => array(
      'label' => 'Latest date collected',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'latestdatecollected',
      'table' => 'darwincore',
      'field' => 'latestdatecollected',
      'relationship' => 'none'
    ),
    'lifestage' => array(
      'label' => 'Life stage',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'lifestage',
      'table' => 'darwincore',
      'field' => 'lifestage',
      'relationship' => 'none'
    ),
    'othercataloguenum' => array(
      'label' => 'Other cataloguenum',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'othercataloguenum',
      'table' => 'darwincore',
      'field' => 'othercataloguenum',
      'relationship' => 'none'
    ),
    'remarks' => array(
      'label' => 'Remarks',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'exclude' => 0,
      'id' => 'remarks',
      'table' => 'darwincore',
      'field' => 'remarks',
      'relationship' => 'none'
    ),
    'sex' => array(
      'label' => 'Sex',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'link_to_taxonomy' => 0,
      'exclude' => 0,
      'id' => 'sex',
      'table' => 'darwincore',
      'field' => 'sex',
      'relationship' => 'none'
    ),
    'taxonomicname' => array(
      'label' => 'Taxonomic name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'link_to_taxonomy' => 0,
      'exclude' => 0,
      'id' => 'taxonomicname',
      'table' => 'darwincore',
      'field' => 'taxonomicname',
      'relationship' => 'none'
    ),
    'typestatus' => array(
      'label' => 'Type status',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0
      ),
      'link_to_taxonomy' => 0,
      'exclude' => 0,
      'id' => 'typestatus',
      'table' => 'darwincore',
      'field' => 'typestatus',
      'relationship' => 'none'
    )
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'darwincore' => 'darwincore'
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
      'relationship' => 'none'
    )
  ));
  $handler->override_option('access', array(
    'type' => 'none'
  ));
  $handler->override_option('style_plugin', 'matrix');
  $handler->override_option('style_options', array(
    'columns' => array(
      'collectioncode' => array(
        'width' => '176'
      )
    )
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'matrix/darwincore');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation'
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0
  ));
  $views[$view->name] = $view;
  // ---------------------- MATRIX EDITOR: PAGES -------------------------------
  $view = new view();
  $view->name = 'matrix_page';
  $view->description = 'Matrix editor for "page"';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $fields = array(
    'title' => array(
      'label' => 'Title',
      'alter' => array(),
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title'
    ),
    'body' => array(
      'label' => 'Body',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'body',
      'table' => 'node_revisions',
      'field' => 'body'
    )
  );
  $vocabularies = taxonomy_get_vocabularies('page');
  if(count($vocabularies)){
    $counter = 0;
    foreach($vocabularies as $vid => $vocabulary){
      $vids = $default_vids;
      $vids[$vid] = $vid;
      $name = 'tid' . $field_name_extension;
      $fields[$name] = array(
        'label' => $vocabulary->name,
        'alter' => array(),
        'type' => 'separator',
        'separator' => ', ',
        'link_to_taxonomy' => 0,
        'limit' => 1,
        'vids' => array(
          $vid => $vid
        ),
        'id' => 'tid',
        'table' => 'term_node',
        'field' => 'tid'
      );
      $counter++;
      $field_name_extension = '_' . $counter;
    }
  }
  $handler->override_option('fields', $fields);
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'page' => 'page'
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
  $handler->override_option('style_plugin', 'matrix');
  $handler->override_option('style_options', array(
    'columns' => array(
      'body' => array(
        'width' => '478'
      ),
      'title' => array(
        'width' => '293'
      )
    ),
    'viewportHeight' => '432'
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'matrix/page');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation'
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0
  ));
  $views[$view->name] = $view;
  // ---------------------- IMAGE GALLERY THUMBNAILS ---------------------------
  $vid_of_image_gallery = variable_get('image_gallery_nav_vocabulary', 0);
  $view = new view();
  $view->name = 'image_thumbnails';
  $view->description = 'Image Gallery Thumbnails';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'field_imagefield_fid' => array(
      'label' => '',
      'format' => 'imagefield__lightbox2__thumbnail_square__preview',
      'exclude' => 0,
      'id' => 'field_imagefield_fid',
      'table' => 'node_data_field_imagefield',
      'field' => 'field_imagefield_fid',
      'relationship' => 'none',
      'label_type' => 'none'
    ),
    'title' => array(
      'label' => '',
      'alter' => array(
        'trim' => 1,
        'max_length' => '12',
        'word_boundary' => 0
      ),
      'link_to_node' => 1,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none'
    )
  ));
  $handler->override_option('arguments', array(
    'tid' => array(
      'default_action' => 'empty',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '',
      'breadcrumb' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'taxonomy_term',
      'validate_fail' => 'not found',
      'break_phrase' => 0,
      'add_table' => 0,
      'require_value' => 0,
      'reduce_duplicates' => 0,
      'set_breadcrumb' => 1,
      'id' => 'tid',
      'table' => 'term_node',
      'field' => 'tid',
      'validate_argument_vocabulary' => array(
        $vid_of_image_gallery => $vid_of_image_gallery
      )
    )
  ));
  $handler->override_option('items_per_page', 24);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'list');
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'image');
  $views[$view->name] = $view;
  // At the end, return array of default views.
  return $views;
}
