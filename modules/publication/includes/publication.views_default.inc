<?php

/**
 * Implementation of hook_views_default_views().
 */
function publication_views_default_views() {

$view = new view;
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
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'link_to_node' => 0,
    'exclude' => 0,
    'id' => 'title',
    'table' => 'node',
    'field' => 'title',
    'relationship' => 'none',
  ),
));
$handler->override_option('sorts', array(
  'title' => array(
    'order' => 'ASC',
    'id' => 'title',
    'table' => 'node',
    'field' => 'title',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
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
      'label' => '',
    ),
    'relationship' => 'none',
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
      'reduce' => 0,
    ),
    'id' => 'group_nid',
    'table' => 'og_ancestry',
    'field' => 'group_nid',
    'relationship' => 'none',
    'reduce_duplicates' => 0,
  ),
  'tid' => array(
    'operator' => 'or',
    'value' => array(),
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 0,
      'operator' => 'tid_op',
      'identifier' => 'tid',
      'label' => 'Term name',
      'optional' => 1,
      'single' => 0,
      'remember' => 0,
      'reduce' => 0,
      'selective' => 0,
    ),
    'type' => 'select',
    'limit' => TRUE,
    'vid' => '1',
    'id' => 'tid',
    'table' => 'term_node',
    'field' => 'tid',
    'hierarchy' => 1,
    'relationship' => 'none',
    'reduce_duplicates' => 0,
  ),
));
$handler->override_option('access', array(
  'type' => 'none',
  'role' => array(),
  'perm' => '',
));
$handler->override_option('cache', array(
  'type' => 'none',
));
$handler->override_option('empty', 'Sorry, no items were found.');
$handler->override_option('empty_format', '1');
$handler->override_option('use_ajax', TRUE);
$handler->override_option('items_per_page', 20);
$handler->override_option('use_pager', '1');
$handler->override_option('style_plugin', 'grid');
$handler = $view->new_display('page', 'Images', 'page_table');
$handler->override_option('fields', array(
  'image_image' => array(
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
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'link_to_node' => 0,
    'image_derivative' => 'thumbnail',
    'exclude' => 0,
    'id' => 'image_image',
    'table' => 'node',
    'field' => 'image_image',
    'relationship' => 'none',
  ),
));
$handler->override_option('path', 'noderelationships/noderelationships-noderef/page-table');
$handler->override_option('menu', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));
$handler->override_option('tab_options', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));
$handler = $view->new_display('page', 'Biblio', 'page_grid');
$handler->override_option('fields', array(
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
      'strip_tags' => 0,
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
      'button' => 'Use default',
    ),
    'relationship' => 'none',
  ),
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
      'strip_tags' => 0,
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
      'button' => 'Use default',
    ),
    'relationship' => 'none',
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
      'strip_tags' => 0,
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
      'button' => 'Use default',
    ),
    'relationship' => 'none',
  ),
));
$handler->override_option('sorts', array(
  'lastname' => array(
    'order' => 'ASC',
    'id' => 'lastname',
    'table' => 'biblio_contributor_data',
    'field' => 'lastname',
    'override' => array(
      'button' => 'Use default',
    ),
    'relationship' => 'none',
  ),
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
      'label' => '',
    ),
    'relationship' => 'none',
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
      'reduce' => 0,
    ),
    'id' => 'group_nid',
    'table' => 'og_ancestry',
    'field' => 'group_nid',
    'relationship' => 'none',
    'reduce_duplicates' => 0,
  ),
  'tid' => array(
    'operator' => 'or',
    'value' => array(),
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 0,
      'operator' => 'tid_op',
      'identifier' => 'tid',
      'label' => 'Term name',
      'optional' => 1,
      'single' => 0,
      'remember' => 0,
      'reduce' => 0,
      'selective' => 0,
    ),
    'type' => 'select',
    'limit' => TRUE,
    'vid' => '1',
    'id' => 'tid',
    'table' => 'term_node',
    'field' => 'tid',
    'hierarchy' => 1,
    'relationship' => 'none',
    'reduce_duplicates' => 0,
  ),
  'biblio_year' => array(
    'operator' => '=',
    'value' => array(
      'value' => '',
      'min' => '',
      'max' => '',
    ),
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 0,
      'operator' => 'biblio_year_op',
      'identifier' => 'biblio_year',
      'label' => 'Year of Publication',
      'optional' => 1,
      'remember' => 0,
    ),
    'id' => 'biblio_year',
    'table' => 'biblio',
    'field' => 'biblio_year',
    'override' => array(
      'button' => 'Use default',
    ),
    'relationship' => 'none',
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
      'remember' => 0,
    ),
    'case' => 1,
    'id' => 'title',
    'table' => 'node',
    'field' => 'title',
    'override' => array(
      'button' => 'Use default',
    ),
    'relationship' => 'none',
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
      'single' => 1,
      'remember' => 0,
      'reduce' => 0,
    ),
    'id' => 'lastname',
    'table' => 'biblio_contributor_data',
    'field' => 'lastname',
    'override' => array(
      'button' => 'Use default',
    ),
    'relationship' => 'none',
    'reduce_duplicates' => 0,
  ),
));
$handler->override_option('style_options', array(
  'grouping' => '',
  'columns' => '4',
  'alignment' => 'horizontal',
));
$handler->override_option('path', 'noderelationships/noderelationships-noderef/page-grid');
$handler->override_option('menu', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));
$handler->override_option('tab_options', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
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
      'strip_tags' => 0,
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
      'button' => 'Use default',
    ),
    'relationship' => 'none',
  ),
));
$handler->override_option('sorts', array(
  'field_familyname_value' => array(
    'order' => 'ASC',
    'delta' => -1,
    'id' => 'field_familyname_value',
    'table' => 'node_data_field_familyname',
    'field' => 'field_familyname_value',
    'override' => array(
      'button' => 'Use default',
    ),
    'relationship' => 'none',
  ),
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
      'label' => '',
    ),
    'relationship' => 'none',
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
      'remember' => 0,
    ),
    'id' => 'node_author_in_group',
    'table' => 'og_uid',
    'field' => 'node_author_in_group',
    'override' => array(
      'button' => 'Use default',
    ),
    'relationship' => 'none',
  ),
));
$handler->override_option('path', 'noderelationships/pub-noderef/page-1');
$handler->override_option('menu', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));
$handler->override_option('tab_options', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));


$views[$view->name] = $view;

$view = new view;
$view->name = 'publication_display';
$view->description = '';
$view->tag = '';
$view->view_php = '';
$view->base_table = 'node';
$view->is_cacheable = FALSE;
$view->api_version = 2;
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
$handler = $view->new_display('default', 'Defaults', 'default');
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
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'link_to_node' => 0,
    'exclude' => 0,
    'id' => 'title',
    'table' => 'node',
    'field' => 'title',
    'relationship' => 'none',
  ),
));
$handler->override_option('arguments', array(
  'nid' => array(
    'default_action' => 'not found',
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
    'break_phrase' => 1,
    'not' => 0,
    'id' => 'nid',
    'table' => 'node',
    'field' => 'nid',
    'validate_user_argument_type' => 'uid',
    'validate_user_roles' => array(
      '2' => 0,
      '3' => 0,
      '4' => 0,
      '5' => 0,
    ),
    'relationship' => 'none',
    'default_options_div_prefix' => '',
    'default_argument_fixed' => '',
    'default_argument_user' => 0,
    'default_argument_image_size' => '_original',
    'default_argument_php' => '',
    'validate_argument_node_type' => array(
      'webform' => 0,
      'blog' => 0,
      'poll' => 0,
      'countriesmap' => 0,
      'image' => 0,
      'ispecies' => 0,
      'nexus_project' => 0,
      'nexus_controlled_state' => 0,
      'nexus_note' => 0,
      'nexus_free_state' => 0,
      'publication' => 0,
      'publication_image_caption' => 0,
      'publication_section' => 0,
      'spm' => 0,
      'tree' => 0,
      'forum' => 0,
      'biblio' => 0,
      'darwincore' => 0,
      'darwincorelocation' => 0,
      'group' => 0,
      'page' => 0,
      'profile' => 0,
      'simplenews' => 0,
    ),
    'validate_argument_node_access' => 0,
    'validate_argument_nid_type' => 'nid',
    'validate_argument_vocabulary' => array(
      '1' => 0,
      '5' => 0,
      '12' => 0,
      '8' => 0,
      '3' => 0,
      '9' => 0,
      '11' => 0,
      '2' => 0,
      '10' => 0,
      '6' => 0,
      '4' => 0,
      '7' => 0,
    ),
    'validate_argument_type' => 'tid',
    'validate_argument_transform' => 0,
    'validate_user_restrict_roles' => 0,
    'image_size' => array(
      '_original' => '_original',
      'thumbnail' => 'thumbnail',
      'preview' => 'preview',
    ),
    'validate_argument_is_member' => 0,
    'validate_argument_php' => '',
  ),
));
$handler->override_option('access', array(
  'type' => 'none',
));
$handler->override_option('cache', array(
  'type' => 'none',
));
$handler->override_option('items_per_page', 0);
$handler->override_option('style_plugin', 'list');
$handler = $view->new_display('page', 'Biblio', 'page_1');
$handler->override_option('path', 'publications/biblio');
$handler->override_option('menu', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));
$handler->override_option('tab_options', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));
$handler = $view->new_display('page', 'Contributors', 'page_2');
$handler->override_option('path', 'publications/contrib');
$handler->override_option('menu', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));
$handler->override_option('tab_options', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));

$views[$view->name] = $view;

return $views;
  
}


?>