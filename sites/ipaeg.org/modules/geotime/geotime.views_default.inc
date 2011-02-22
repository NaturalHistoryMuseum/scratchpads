<?php

function geotime_views_default_views() {
	$view = new view;
$view->name = 'geotime_lookup';
$view->description = 'Provides lookup for  adding geological time data to nodes';
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
  'field_geotime_base_age_value' => array(
    'id' => 'field_geotime_base_age_value',
    'table' => 'node_data_field_geotime_base_age',
    'field' => 'field_geotime_base_age_value',
  ),
  'field_geotime_top_age_value' => array(
    'id' => 'field_geotime_top_age_value',
    'table' => 'node_data_field_geotime_top_age',
    'field' => 'field_geotime_top_age_value',
  ),
  'field_geotime_age_value' => array(
    'id' => 'field_geotime_age_value',
    'table' => 'node_data_field_geotime_age',
    'field' => 'field_geotime_age_value',
  ),
));
$handler->override_option('filters', array(
  'type' => array(
    'operator' => 'in',
    'value' => array(
      'geotime' => 'geotime',
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
));
$handler->override_option('access', array(
  'type' => 'none',
));
$handler->override_option('cache', array(
  'type' => 'none',
));
$handler->override_option('style_plugin', 'table');
	
	$views[$view->name] = $view;
	
	return $views;
}