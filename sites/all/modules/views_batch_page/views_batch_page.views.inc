<?php

/**
 * Implementation of hook_views_plugins
 */
function views_batch_page_views_plugins(){
  $views_path = drupal_get_path('module', 'views');
  return array(
    'module' => 'views_batch_page', // This just tells our themes are elsewhere.
    'display' => array(
      'parent' => array(
        'no ui' => TRUE,
        'handler' => 'views_plugin_display',
        'path' => "$views_path/plugins",
        'parent' => ''
      ),
      'page' => array(
        'no ui' => TRUE,
        'handler' => 'views_plugin_display_page',
        'path' => "$views_path/plugins",
        'parent' => 'parent'
      ),
      'views_batch_page' => array(
        'title' => t('Batch page'),
        'help' => t('A page view where the view has been loaded via the batch API'),
        'handler' => 'views_plugin_display_batch_page',
        'parent' => 'page',
        'theme' => 'views_view',
        'uses hook menu' => TRUE,
        'admin' => t('Batch page'),
        'help topic' => 'views-batch-page'
      )
    )
  );
}