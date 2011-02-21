<?php
// $Id: editor_views.views.inc,v 1.1 2010/03/22 10:22:11 b3n Exp $

/**
 * @file
 * Provide views data and handlers for editor view
 */
 
/**
 * Implementation of hook_views_data()
 */
function editor_views_views_data() {
  // Basic table information.
  $data = array();
  
  $data['node_access']['edit_delete'] = array(
    'title' => t('Edit / Delete'),
    'help' => t('Filter by edit / delete access.'),
    'filter' => array(
      'handler' => 'views_handler_filter_editor_node_access',
      'help' => t('Filter for nodes by edit / delete access.'),
    ),
  );

  return $data;
}

/**
 * Implementation of hook_views_handlers() to register all of the basic handlers
 * views uses.
 */
function editor_views_views_handlers() {
  
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'editor_views'),
    ),
    'handlers' => array(
      // field handlers
      'views_handler_filter_editor_node_access' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    )
  );
}