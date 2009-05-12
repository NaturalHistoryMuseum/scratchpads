<?php
function bio_image_views_default_views() {
  $view = new view;
  $view->name = 'biblio_nodes';
  $view->description = '';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'biblio_authors' => array(
      'label' => '',
      'alter' => array(),
      'biblio_label' => 0,
      'style_name' => 'cse',
      'exclude' => 0,
      'id' => 'biblio_authors',
      'table' => 'biblio',
      'field' => 'biblio_authors',
      'relationship' => 'none',
    ),
    'biblio_year' => array(
      'label' => '',
      'alter' => array(),
      'biblio_label' => 0,
      'exclude' => 0,
      'id' => 'biblio_year',
      'table' => 'biblio',
      'field' => 'biblio_year',
      'relationship' => 'none',
    ),
    'title' => array(
      'label' => '',
      'alter' => array(),
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'biblio' => 'biblio',
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
  
  // FIXME -- We need a sort in here.  Unfortunately Biblio doesn't yet support
  // sorting by Biblio fields, so we'll have to wait.
  
  $handler->override_option('items_per_page', 0);
  
  $views[$view->name] = $view;
  
  // At the end, return array of default views.
  return $views;
}