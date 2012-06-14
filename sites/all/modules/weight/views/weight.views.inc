<?php
/**
 * @file
 * Views2 support for Weight module.
 */

/**
 * Implementation of hook_views_data().
 */
function weight_views_data() {
  $data['node_weight'] = array(
    'table' => array(
      'group' => t('Weight'),
      'join' => array(
        'node' => array(
          'table' => 'node',
          'left_field' => 'nid',
          'field' => 'nid',
        ),
      ),
    ),
    'weight' => array(
      'real field' => 'sticky',
      'title' => t('Weight'), // The item it appears as on the UI,
      'help' => t('The node weight.'), // The help that appears on the UI,
      'field' => array(
        'handler' => 'weight_handler_field_sticky',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'weight_handler_filter_weight',
        'label' => t('Weight'),
      ),
      'sort' => array(
        'handler' => 'weight_handler_sort',
      ),
    ),
  );

  $data['node_weight_stick'] = array(
    'table' => array(
      'group' => t('Weight'),
      'join' => array(
        'node' => array(
          'table' => 'node',
          'left_field' => 'nid',
          'field' => 'nid',
        ),
      ),
    ),
    'weight_stick' => array(
      'real field' => 'sticky',
      'title' => t('Weighted Sticky'), // The item it appears as on the UI,
      'help' => t('Checks if something really is sticky.'), // The help that appears on the UI,
      'filter' => array(
        'handler' => 'weight_handler_filter_sticky',
        'label' => t('Weighted Sticky'),
        'type' => 'yes-no',
      ),
    ),
  );

  return $data;
}


/**
 * Implementation of hook_views_plugins();
 */
function weight_views_plugins() {
  return array(
    'style' => array(
      'weight' => array(
        'title' => t('Weight Changer'),
        'help' => t('Displays rows in a table which allows weight change.  Be sure to add the Weight field and Sort by Weight.'),
        'handler' => 'weight_plugin_style',
        'parent' => 'table',
        'path' => drupal_get_path('module', 'weight') . '/views',
        'uses row plugin' => FALSE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_handlers().
 */
function weight_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'weight') .'/views',
    ),
    'handlers' => array(
      'weight_handler_field_sticky' => array(
        'parent' => 'views_handler_field_numeric',
      ),
      'weight_handler_sort' => array(
        'parent' => 'views_handler_sort',
      ),
      'weight_handler_filter_sticky' => array(
        'parent' => 'views_handler_filter_boolean_operator',
      ),
      'weight_handler_filter_weight' => array(
        'parent' => 'views_handler_filter_numeric',
      ),
    ),
  );
}
