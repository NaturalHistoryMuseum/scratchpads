<?php
function emonocot_views_default_views () {
$view = new view;
$view->name = 'specimen_table_em';
$view->description = 'Darwincore specimen table (Institution/Collection/Catalogue codes, Taxonomic name)';
$view->tag = '';
$view->view_php = '';
$view->base_table = 'node';
$view->is_cacheable = FALSE;
$view->api_version = 2;
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
$handler = $view->new_display('default', 'Defaults', 'default');
$handler->override_option('fields', array(
  'typestatus' => array(
    'label' => 'Type status',
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
    'link_to_taxonomy' => 1,
    'exclude' => 0,
    'id' => 'typestatus',
    'table' => 'darwincore',
    'field' => 'typestatus',
    'relationship' => 'none',
  ),
  'title' => array(
    'label' => 'Institution/Collection/Catalogue',
    'alter' => array(),
    'link_to_node' => 1,
    'exclude' => 0,
    'id' => 'title',
    'table' => 'node',
    'field' => 'title',
    'relationship' => 'none',
  ),
  'taxonomicname' => array(
    'label' => 'Taxonomic name',
    'alter' => array(),
    'exclude' => 0,
    'id' => 'taxonomicname',
    'table' => 'darwincore',
    'field' => 'taxonomicname',
    'relationship' => 'none',
  ),
));
$handler->override_option('arguments', array(
  'tid' => array(
    'default_action' => 'empty',
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
    'validate_argument_php' => '',
  ),
));
$handler->override_option('filters', array(
  'type' => array(
    'operator' => 'in',
    'value' => array(
      'darwincore' => 'darwincore',
    ),
    'group' => '0',
    'exposed' => FALSE,
    'expose' => array(
      'operator' => FALSE,
      'label' => '',
    ),
    'id' => 'type',
    'table' => 'node',
    'field' => 'type',
    'relationship' => 'none',
  ),
  'tid' => array(
    'operator' => 'or',
    'value' => '',
    'group' => '0',
    'exposed' => FALSE,
    'expose' => array(
      'operator' => FALSE,
      'label' => '',
    ),
    'type' => 'textfield',
    'limit' => TRUE,
    'vid' => '9',
    'id' => 'tid',
    'table' => 'term_node',
    'field' => 'tid',
    'hierarchy' => 0,
    'relationship' => 'none',
    'reduce_duplicates' => 0,
  ),
));
$handler->override_option('access', array(
  'type' => 'none',
));
$handler->override_option('cache', array(
  'type' => 'none',
));
$handler->override_option('title', 'Specimens');
$handler->override_option('items_per_page', 0);
$handler->override_option('style_plugin', 'table');
$handler->override_option('style_options', array(
  'grouping' => '',
  'override' => 1,
  'sticky' => 0,
  'order' => 'asc',
  'columns' => array(
    'title' => 'title',
    'collectioncode' => 'collectioncode',
  ),
  'info' => array(
    'title' => array(
      'sortable' => 0,
      'separator' => '',
    ),
    'collectioncode' => array(
      'sortable' => 0,
      'separator' => '',
    ),
  ),
  'default' => '-1',
));

$views[$view->name] = $view;

$view = new view;
$view->name = 'spec_image_types';
$view->description = '[Full nodes view] Images of a taxon including type status';
$view->tag = '';
$view->view_php = '';
$view->base_table = 'node';
$view->is_cacheable = FALSE;
$view->api_version = 2;
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
$handler = $view->new_display('default', 'Defaults', 'default');
$handler->override_option('relationships', array(
  'image_specimen_nid' => array(
    'label' => 'Specimen',
    'required' => 0,
    'delta' => -1,
    'id' => 'image_specimen_nid',
    'table' => 'node_data_image_specimen',
    'field' => 'image_specimen_nid',
    'relationship' => 'none',
  ),
));
$handler->override_option('fields', array(
  'field_imagefield_fid' => array(
    'label' => '',
    'alter' => array(
      'alter_text' => 0,
      'text' => '<div style="padding:5px">[field_imagefield_fid]</div>',
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
    'label_type' => 'none',
    'format' => 'imagefield__lightbox2__thumbnail_square__preview',
    'multiple' => array(
      'group' => 0,
      'multiple_number' => '',
      'multiple_from' => '',
      'multiple_reversed' => 0,
    ),
    'exclude' => 0,
    'id' => 'field_imagefield_fid',
    'table' => 'node_data_field_imagefield',
    'field' => 'field_imagefield_fid',
    'relationship' => 'none',
  ),
  'typestatus' => array(
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
    'link_to_taxonomy' => 0,
    'exclude' => 1,
    'id' => 'typestatus',
    'table' => 'darwincore',
    'field' => 'typestatus',
    'relationship' => 'image_specimen_nid',
  ),
));
$handler->override_option('sorts', array(
  'weight' => array(
    'order' => 'ASC',
    'id' => 'weight',
    'table' => 'node_weight',
    'field' => 'weight',
    'relationship' => 'none',
  ),
  'changed' => array(
    'order' => 'DESC',
    'id' => 'changed',
    'table' => 'node',
    'field' => 'changed',
  ),
));
$handler->override_option('arguments', array(
  'tid' => array(
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
    'add_table' => 0,
    'require_value' => 0,
    'reduce_duplicates' => 0,
    'set_breadcrumb' => 0,
    'id' => 'tid',
    'table' => 'term_node',
    'field' => 'tid',
    'validate_user_argument_type' => 'uid',
    'validate_user_roles' => array(
      '2' => 0,
      '3' => 0,
      '4' => 0,
      '5' => 0,
    ),
    'relationship' => 'none',
    'default_options_div_prefix' => '',
    'default_argument_user' => 0,
    'default_argument_fixed' => '',
    'default_argument_php' => '',
    'validate_argument_node_type' => array(
      'countriesmap' => 0,
      'ispecies' => 0,
      'spm' => 0,
      'forum' => 0,
      'biblio' => 0,
      'darwincore' => 0,
      'darwincorelocation' => 0,
      'itis_term' => 0,
      'group' => 0,
      'image' => 0,
      'page' => 0,
      'profile' => 0,
    ),
    'validate_argument_node_access' => 0,
    'validate_argument_nid_type' => 'nid',
    'validate_argument_vocabulary' => array(
      '1' => 0,
      '2' => 0,
      '10' => 0,
      '5' => 0,
      '6' => 0,
      '7' => 0,
      '9' => 0,
      '8' => 0,
      '3' => 0,
      '11' => 0,
      '4' => 0,
    ),
    'validate_argument_type' => 'tid',
    'validate_argument_transform' => 0,
    'validate_user_restrict_roles' => 0,
    'validate_argument_php' => '',
    'validate_argument_is_member' => 'OG_VIEWS_DO_NOT_VALIDATE_MEMBERSHIP',
    'validate_argument_group_node_type' => array(
      'group' => 0,
    ),
  ),
));
$handler->override_option('filters', array(
  'type' => array(
    'operator' => 'in',
    'value' => array(
      'image' => 'image',
    ),
    'group' => '0',
    'exposed' => FALSE,
    'expose' => array(
      'operator' => 'type_op',
      'label' => 'Node: Type',
      'use_operator' => FALSE,
      'identifier' => 'type',
      'remember' => FALSE,
      'single' => TRUE,
      'optional' => TRUE,
      'reduce' => FALSE,
    ),
    'id' => 'type',
    'table' => 'node',
    'field' => 'type',
    'relationship' => 'none',
  ),
  'status_extra' => array(
    'operator' => '=',
    'value' => '',
    'group' => '0',
    'exposed' => FALSE,
    'expose' => array(
      'operator' => FALSE,
      'label' => '',
    ),
    'id' => 'status_extra',
    'table' => 'node',
    'field' => 'status_extra',
    'relationship' => 'none',
  ),
));
$handler->override_option('access', array(
  'type' => 'none',
));
$handler->override_option('cache', array(
  'type' => 'none',
));
$handler->override_option('title', 'Image');
$handler->override_option('style_plugin', 'grid');
$handler->override_option('style_options', array(
  'grouping' => 'typestatus',
  'columns' => '4',
  'alignment' => 'horizontal',
  'fill_single_line' => 1,
));
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->override_option('arguments', array());
$handler->override_option('use_pager', '1');
$handler->override_option('path', 'content/image');
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