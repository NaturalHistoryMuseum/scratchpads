<?php
/**
 * Describe table structure to Views.
 */
function publication_views_data() {

  // Example plain text field.
  $data['og_uid']['node_author_in_group'] = array(
    'title' => t('Node author in group'),
    'help' => t('Node author in group.'),
    'filter' => array(
      'handler' => 'views_handler_filter_author_in_group',
    ),
  );

  return $data;
  
}

/**
 * Implementation of hook_views_handlers() to register all of the basic handlers
 * views uses.
 */
function publication_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'publication'),
    ),
    'handlers' => array(
      // field handlers
      // filters
      'views_handler_filter_author_in_group' => array(
        'parent' => 'views_handler_filter_numeric',
      ),
    ),
  );
}

?>