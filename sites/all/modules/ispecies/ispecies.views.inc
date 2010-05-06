<?php
/**
 * Implementation of hook_views_data()
 */
function ispecies_views_data() {
  $data = array(
    'ispecies' => array(
      'table' => array(
        'group' => 'iSpecies',
        'title' => 'iSpecies',
        'join' => array(
          'node' => array(
            'left_field' => 'nid',
            'field' => 'nid',
            //'handler' => 'views_handler_filter_string'
          )
        )
      ),
      'ispeciestype' => array(
        'title' => t('Type'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )
      )
    )
  );
  return $data;
}