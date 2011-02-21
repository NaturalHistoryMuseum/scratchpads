<?php

/**
 * Implementation of hook_views_default_views().
 */
function publication_views_default_views(){
  $view = new view();
  $view->name = 'publication_noderefences';
  $view->description = '';
  $view->tag = 'noderelationships_noderef';
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
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none'
    )
  ));
  $handler->override_option('sorts', array(
    'title' => array(
      'order' => 'ASC',
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'override' => array(
        'button' => 'Override'
      ),
      'relationship' => 'none'
    )
  ));
  $handler->override_option('filters', array(
    'status_extra' => array(
      'id' => 'status_extra',
      'table' => 'node',
      'field' => 'status_extra',
      'operator' => '=',
      'value' => '',
      'group' => 0,
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => ''
      ),
      'relationship' => 'none'
    ),
    'group_nid' => array(
      'operator' => 'or',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'group_nid_op',
        'identifier' => 'group_nid',
        'label' => 'Content is part of publication group',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0
      ),
      'id' => 'group_nid',
      'table' => 'og_ancestry',
      'field' => 'group_nid',
      'relationship' => 'none',
      'reduce_duplicates' => 0
    ),
    'tid' => array(
      'operator' => 'or',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'operator' => 'tid_op',
        'label' => 'Term name',
        'use_operator' => 0,
        'identifier' => 'tid',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
        'selective' => 0
      ),
      'type' => 'textfield',
      'limit' => TRUE,
      'vid' => '1',
      'id' => 'tid',
      'table' => 'term_node',
      'field' => 'tid',
      'hierarchy' => 1,
      'relationship' => 'none',
      'reduce_duplicates' => 0
    )
  ));
  $handler->override_option('access', array(
    'type' => 'none',
    'role' => array(),
    'perm' => ''
  ));
  $handler->override_option('cache', array(
    'type' => 'none'
  ));
  $handler->override_option('empty', 'Sorry, no items were found.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('use_ajax', TRUE);
  $handler->override_option('items_per_page', 20);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'grid');
  $handler = $view->new_display('page', 'Images', 'page_table');
  $handler->override_option('fields', array(
    'field_imagefield_fid' => array(
      'label' => '',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 0,
      'label_type' => 'none',
      'format' => 'default',
      'multiple' => array(
        'group' => 0,
        'multiple_number' => '',
        'multiple_from' => '',
        'multiple_reversed' => 0
      ),
      'exclude' => 0,
      'id' => 'field_imagefield_fid',
      'table' => 'node_data_field_imagefield',
      'field' => 'field_imagefield_fid',
      'override' => array(
        'button' => 'Use default'
      ),
      'relationship' => 'none'
    )
  ));
  $handler->override_option('path', 'noderelationships/noderelationships-noderef/page-table');
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
    'weight' => 0,
    'name' => 'navigation'
  ));
  $handler = $view->new_display('page', 'Biblio', 'page_grid');
  $handler->override_option('fields', array(
    'biblio_authors' => array(
      'label' => '',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'biblio_label' => 0,
      'style_name' => 'cse',
      'exclude' => 0,
      'id' => 'biblio_authors',
      'table' => 'biblio',
      'field' => 'biblio_authors',
      'override' => array(
        'button' => 'Use default'
      ),
      'relationship' => 'none'
    ),
    'biblio_year' => array(
      'label' => '',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'biblio_label' => 0,
      'exclude' => 0,
      'id' => 'biblio_year',
      'table' => 'biblio',
      'field' => 'biblio_year',
      'override' => array(
        'button' => 'Use default'
      ),
      'relationship' => 'none'
    ),
    'title' => array(
      'label' => '',
      'alter' => array(
        'alter_text' => 0,
        'text' => '[biblio_authors]<em>[title]</em>',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'override' => array(
        'button' => 'Use default'
      ),
      'relationship' => 'none'
    )
  ));
  $handler->override_option('sorts', array(
    'lastname' => array(
      'order' => 'ASC',
      'id' => 'lastname',
      'table' => 'biblio_contributor_data',
      'field' => 'lastname',
      'override' => array(
        'button' => 'Use default'
      ),
      'relationship' => 'none'
    )
  ));
  $handler->override_option('filters', array(
    'status_extra' => array(
      'id' => 'status_extra',
      'table' => 'node',
      'field' => 'status_extra',
      'operator' => '=',
      'value' => '',
      'group' => 0,
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => ''
      ),
      'relationship' => 'none'
    ),
    'group_nid' => array(
      'operator' => 'or',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'group_nid_op',
        'identifier' => 'group_nid',
        'label' => 'Content is part of publication group',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0
      ),
      'id' => 'group_nid',
      'table' => 'og_ancestry',
      'field' => 'group_nid',
      'relationship' => 'none',
      'reduce_duplicates' => 0
    ),
    'tid' => array(
      'operator' => 'or',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'operator' => 'tid_op',
        'label' => 'Term name',
        'use_operator' => 0,
        'identifier' => 'tid',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
        'selective' => 0
      ),
      'type' => 'textfield',
      'limit' => TRUE,
      'vid' => '1',
      'id' => 'tid',
      'table' => 'term_node',
      'field' => 'tid',
      'hierarchy' => 1,
      'relationship' => 'none',
      'reduce_duplicates' => 0
    ),
    'biblio_year' => array(
      'operator' => '=',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => ''
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'biblio_year_op',
        'identifier' => 'biblio_year',
        'label' => 'Year of Publication',
        'optional' => 1,
        'remember' => 0
      ),
      'id' => 'biblio_year',
      'table' => 'biblio',
      'field' => 'biblio_year',
      'override' => array(
        'button' => 'Use default'
      ),
      'relationship' => 'none'
    ),
    'title' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'title_op',
        'identifier' => 'title',
        'label' => 'Title',
        'optional' => 1,
        'remember' => 0
      ),
      'case' => 1,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'override' => array(
        'button' => 'Use default'
      ),
      'relationship' => 'none'
    ),
    'lastname' => array(
      'operator' => 'or',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'lastname_op',
        'identifier' => 'author',
        'label' => 'Author',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0
      ),
      'id' => 'lastname',
      'table' => 'biblio_contributor_data',
      'field' => 'lastname',
      'override' => array(
        'button' => 'Use default'
      ),
      'relationship' => 'none',
      'reduce_duplicates' => 0
    )
  ));
  $handler->override_option('style_options', array(
    'grouping' => '',
    'columns' => '4',
    'alignment' => 'horizontal'
  ));
  $handler->override_option('path', 'noderelationships/noderelationships-noderef/page-grid');
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
    'weight' => 0,
    'name' => 'navigation'
  ));
  $handler = $view->new_display('page', 'Authors', 'page_1');
  $handler->override_option('fields', array(
    'title' => array(
      'label' => '',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'override' => array(
        'button' => 'Use default'
      ),
      'relationship' => 'none'
    )
  ));
  $handler->override_option('sorts', array(
    'field_familyname_value' => array(
      'order' => 'ASC',
      'delta' => -1,
      'id' => 'field_familyname_value',
      'table' => 'node_data_field_familyname',
      'field' => 'field_familyname_value',
      'override' => array(
        'button' => 'Use default'
      ),
      'relationship' => 'none'
    )
  ));
  $handler->override_option('filters', array(
    'status_extra' => array(
      'id' => 'status_extra',
      'table' => 'node',
      'field' => 'status_extra',
      'operator' => '=',
      'value' => '',
      'group' => 0,
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => ''
      ),
      'relationship' => 'none'
    ),
    'node_author_in_group' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'operator' => '',
        'identifier' => 'node_author_in_group',
        'label' => 'Author in publication group',
        'optional' => 1,
        'remember' => 0
      ),
      'id' => 'node_author_in_group',
      'table' => 'og_uid',
      'field' => 'node_author_in_group',
      'override' => array(
        'button' => 'Use default'
      ),
      'relationship' => 'none'
    )
  ));
  $handler->override_option('path', 'noderelationships/pub-noderef/page-1');
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
    'weight' => 0,
    'name' => 'navigation'
  ));
  $views[$view->name] = $view;
  return $views;
}
?>