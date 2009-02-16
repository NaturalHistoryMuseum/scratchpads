<?php
/**
 * Implementation of hook_views_data()
 */
function ispecies_views_data() {
  $data['ispecies'] = array(
    'table' => array(
      'group' => t('iSpecies'),
      'join' => array(
        'node' => array(
          'table' => 'node',
          'left_field' => 'nid',
          'field' => 'nid',
        ),
      ),
    ),
    'ispecies' => array(
      'real field' => 'type',
      'title' => t('iSpecies provider'),
      'filter' => array(
        'help' => t('Select the remote provider that you require'),
        'handler' => 'views_handler_filter_string',
      )
    )
  );
  return $data;
}