<?php

function darwincore_views_default_views() {
  
  // We're going to create just ONE view, for a Table view of specimens
  
  // Begin copy and paste of output from the Export tab of a view.
  $view = new view;
  $view->name = 'specimen_table';
  $view->description = 'Darwincore specimen table (Institution/Collection/Catalogue codes, Taxonomic name)';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
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
  $handler->override_option('title', t('Specimens'));
  $handler->override_option('items_per_page', 0);
  $views[$view->name] = $view;
  
  // ---------------------------------------------------------------------------
  $view = new view;
  $view->name = 'specimen_table_full_monty';
  $view->description = 'Darwincore specimen table (All fields)';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'locationnode' => array(
      'label' => 'Darwincore location node',
      'required' => 0,
      'id' => 'locationnode',
      'table' => 'darwincore',
      'field' => 'locationnode',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('fields', array(
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
    'basisofrecord' => array(
      'label' => 'Basis of record',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'basisofrecord',
      'table' => 'darwincore',
      'field' => 'basisofrecord',
      'relationship' => 'none',
    ),
    'cataloguenumbertext' => array(
      'label' => 'Catalogue number text',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'cataloguenumbertext',
      'table' => 'darwincore',
      'field' => 'cataloguenumbertext',
      'relationship' => 'none',
    ),
    'collectioncode' => array(
      'label' => 'Collection code',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'collectioncode',
      'table' => 'darwincore',
      'field' => 'collectioncode',
      'relationship' => 'none',
    ),
    'collector' => array(
      'label' => 'Collector',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'collector',
      'table' => 'darwincore',
      'field' => 'collector',
      'relationship' => 'none',
    ),
    'collectornumber' => array(
      'label' => 'Collector number',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'collectornumber',
      'table' => 'darwincore',
      'field' => 'collectornumber',
      'relationship' => 'none',
    ),
    'count' => array(
      'id' => 'count',
      'table' => 'darwincore',
      'field' => 'count',
    ),
    'dateidentifiedyear' => array(
      'label' => 'Date identified',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'dateidentifiedyear',
      'table' => 'darwincore',
      'field' => 'dateidentifiedyear',
      'relationship' => 'none',
    ),
    'dateidentifiedmonth' => array(
      'label' => 'Date identified',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'dateidentifiedmonth',
      'table' => 'darwincore',
      'field' => 'dateidentifiedmonth',
      'relationship' => 'none',
    ),
    'dateidentifiedday' => array(
      'label' => 'Date identified',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'dateidentifiedday',
      'table' => 'darwincore',
      'field' => 'dateidentifiedday',
      'relationship' => 'none',
    ),
    'earliestdatecollected' => array(
      'label' => 'Earliest date collected',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'earliestdatecollected',
      'table' => 'darwincore',
      'field' => 'earliestdatecollected',
      'relationship' => 'none',
    ),
    'fieldnotes' => array(
      'label' => 'Field notes',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'fieldnotes',
      'table' => 'darwincore',
      'field' => 'fieldnotes',
      'relationship' => 'none',
    ),
    'fieldnumber' => array(
      'label' => 'Field number',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'fieldnumber',
      'table' => 'darwincore',
      'field' => 'fieldnumber',
      'relationship' => 'none',
    ),
    'genbanknum' => array(
      'label' => 'Genbank number',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'genbanknum',
      'table' => 'darwincore',
      'field' => 'genbanknum',
      'relationship' => 'none',
    ),
    'identificationqualifier' => array(
      'label' => 'Identification qualifier/modifier',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'identificationqualifier',
      'table' => 'darwincore',
      'field' => 'identificationqualifier',
      'relationship' => 'none',
    ),
    'identifiedby' => array(
      'label' => 'Identified by',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'identifiedby',
      'table' => 'darwincore',
      'field' => 'identifiedby',
      'relationship' => 'none',
    ),
    'institutioncode' => array(
      'label' => 'Institution code',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'institutioncode',
      'table' => 'darwincore',
      'field' => 'institutioncode',
      'relationship' => 'none',
    ),
    'latestdatecollected' => array(
      'label' => 'Latest date collected',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'latestdatecollected',
      'table' => 'darwincore',
      'field' => 'latestdatecollected',
      'relationship' => 'none',
    ),
    'lifestage' => array(
      'label' => 'Life stage',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'lifestage',
      'table' => 'darwincore',
      'field' => 'lifestage',
      'relationship' => 'none',
    ),
    'othercataloguenum' => array(
      'label' => 'Other cataloguenum',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'othercataloguenum',
      'table' => 'darwincore',
      'field' => 'othercataloguenum',
      'relationship' => 'none',
    ),
    'remarks' => array(
      'label' => 'Remarks',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'remarks',
      'table' => 'darwincore',
      'field' => 'remarks',
      'relationship' => 'none',
    ),
    'sex' => array(
      'label' => 'Sex',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'sex',
      'table' => 'darwincore',
      'field' => 'sex',
      'relationship' => 'none',
    ),
    'typestatus' => array(
      'id' => 'typestatus',
      'table' => 'darwincore',
      'field' => 'typestatus',
    ),
    'continentocean' => array(
      'label' => 'Continent/BodyOfWater',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'continentocean',
      'table' => 'darwincorelocation',
      'field' => 'continentocean',
      'relationship' => 'locationnode',
    ),
    'coordinateuncertainty' => array(
      'label' => 'Coordinate uncertainty in meters',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'coordinateuncertainty',
      'table' => 'darwincorelocation',
      'field' => 'coordinateuncertainty',
      'relationship' => 'locationnode',
    ),
    'county' => array(
      'label' => 'County',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'county',
      'table' => 'darwincorelocation',
      'field' => 'county',
      'relationship' => 'locationnode',
    ),
    'geodeticdatum' => array(
      'label' => 'Geodetic Datum',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'geodeticdatum',
      'table' => 'darwincorelocation',
      'field' => 'geodeticdatum',
      'relationship' => 'locationnode',
    ),
    'georeferenceprotocol' => array(
      'label' => 'Georeference Protocol',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'georeferenceprotocol',
      'table' => 'darwincorelocation',
      'field' => 'georeferenceprotocol',
      'relationship' => 'locationnode',
    ),
    'georeferenceremarks' => array(
      'label' => 'Georeference remarks',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'georeferenceremarks',
      'table' => 'darwincorelocation',
      'field' => 'georeferenceremarks',
      'relationship' => 'locationnode',
    ),
    'island' => array(
      'label' => 'Island',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'island',
      'table' => 'darwincorelocation',
      'field' => 'island',
      'relationship' => 'locationnode',
    ),
    'islandgroup' => array(
      'label' => 'Island group',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'islandgroup',
      'table' => 'darwincorelocation',
      'field' => 'islandgroup',
      'relationship' => 'locationnode',
    ),
    'maxdepth' => array(
      'label' => 'Maximum depth in meters',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'maxdepth',
      'table' => 'darwincorelocation',
      'field' => 'maxdepth',
      'relationship' => 'locationnode',
    ),
    'maxelevation' => array(
      'label' => 'Maximum elevation in meters',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'maxelevation',
      'table' => 'darwincorelocation',
      'field' => 'maxelevation',
      'relationship' => 'locationnode',
    ),
    'mindepth' => array(
      'label' => 'Minimum depth in meters',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'mindepth',
      'table' => 'darwincorelocation',
      'field' => 'mindepth',
      'relationship' => 'locationnode',
    ),
    'minelevation' => array(
      'label' => 'Minimum elevation in meters',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'minelevation',
      'table' => 'darwincorelocation',
      'field' => 'minelevation',
      'relationship' => 'locationnode',
    ),
    'stateprovince' => array(
      'label' => 'State province',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'stateprovince',
      'table' => 'darwincorelocation',
      'field' => 'stateprovince',
      'relationship' => 'locationnode',
    ),
    'verbatimcoordinatesystem' => array(
      'label' => 'Verbatim Coordinate System ',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'verbatimcoordinatesystem',
      'table' => 'darwincorelocation',
      'field' => 'verbatimcoordinatesystem',
      'relationship' => 'locationnode',
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
  
  // ---------------------------------------------------------------------------
  
  
  // We're going to create just ONE view, for a Table view of specimens
  
  // Begin copy and paste of output from the Export tab of a view.
  $view = new view;
  $view->name = 'specimen_map';
  $view->description = 'Darwincore specimen map';
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
      'alter' => array(),
      'link_to_node' => 1,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'taxonomicname' => array(
      'label' => '',
      'alter' => array(),
      'link_to_taxonomy' => 1,
      'id' => 'taxonomicname',
      'table' => 'darwincore',
      'field' => 'taxonomicname',
      'relationship' => 'none',
    ),
    'latitude' => array(
      'label' => 'Latitude',
      'alter' => array(),
      'style' => 'dms',
      'exclude' => 1,
      'id' => 'latitude',
      'table' => 'location',
      'field' => 'latitude',
      'relationship' => 'none',
    ),
    'longitude' => array(
      'label' => 'Longitude',
      'alter' => array(),
      'style' => 'dms',
      'exclude' => 1,
      'id' => 'longitude',
      'table' => 'location',
      'field' => 'longitude',
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
  $handler->override_option('title', 'Specimens map');
  $handler->override_option('items_per_page', 0);
  $handler->override_option('style_plugin', 'gmap');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'macro' => '[gmap ]',
    'datasource' => 'location',
    'markers' => 'static',
    'markertype' => 'white',
  ));
  
  $views[$view->name] = $view;
  
  // ---------------------------------------------------------------------------
  return $views;
}