<?php

/**
 * Implementation of hook_views_handlers
 */
function leftandright_views_handlers(){
  return array(
    'info' => array(
      'path' => drupal_get_path('module','leftandright') . '/views'
    ),
    'handlers' => array(
      'leftandright_handler_field_depth' => array(
        'parent' => 'views_handler_field'
      )
    )
  );
}

/**
 * Implementation of hook_views_data()
 */
function leftandright_views_data() {
  $data = array(
    'leftandright' => array(
      'table' => array(
        'group' => 'Leftandright',
        'join' => array(
          'term_data' => array(
            'left_field' => 'tid',
            'field' => 'tid'
          )
        )
      ),
      'lft' => array(
        'title' => t('Left'),
        'help' => t('The Left ID from the Leftandright table'),
        'sort' => array(
          'handler' => 'views_handler_sort'
        ),
        'field' => array(
          'handler' => 'views_handler_field'
        )
      ),
      'depth' => array(
        'title' => t('Depth'),
        'help' => t('The depth of the term from the top of the tree.'),
        'sort' => array(
          'handler' => 'views_handler_sort'
        ),
        'field' => array(
          'handler' => 'leftandright_handler_field_depth'
        )/*,
          * TODO - Add a filter for terms of specific depths
        'filter' => array(
          'handler' => 'views_handler_filter_term_node_tid'
        )
        */
      )
    )
  );
  return $data;
}