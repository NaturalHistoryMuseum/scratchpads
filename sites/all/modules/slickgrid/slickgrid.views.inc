<?php

/**
 * Implementation of hook_views_plugins()
 */
function slickgrid_views_plugins(){
  return array(
    'style' => array(
      'slickgrid' => array(
        'title' => t('Slickgrid'),
        'help' => t('Displays data in a spreadsheet-style grid.'),
        'handler' => 'slickgrid_views_plugin',
        'theme' => 'views_view_slickgrid',
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'uses grouping' => FALSE,
        'use ajax' => FALSE,
        'type' => 'normal'
      )
    )
  );
}




