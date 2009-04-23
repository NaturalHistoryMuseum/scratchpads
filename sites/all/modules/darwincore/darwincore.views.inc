<?php
/**
 * Implementation of hook_views_data()
 */
function darwincore_views_data() {
  $data = array(
    'darwincore' => array(
      'table' => array(
        'group' => 'Darwincore - Specimen',
        'join' => array(
          'node' => array(
            'left_field' => 'nid',
            'field' => 'nid'
          )
        )
      ),
      'collectioncode' => array(
        'title' => t('Collection code'),
        'help' => t("A specimen's collecion code"),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      )
    ),
    'darwincorelocation' => array(
      'table' => array(
        'group' => 'Darwincore - Location',
        'join' => array(
          'node' => array(
            'left_field' => 'nid',
            'field' => 'nid')
        )
      ),
      'island' => array(
        'title' => t('Island'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )
      )
    )
  );
  return $data;
}