<?php

/**
* Implementation of hook_views_plugins()
*/
function matrix_editor_views_plugins() {
  return array(
    'style' => array(
      'matrix' => array(
        'title' => t('Matrix'),
        'help' => t('Displays data in a speadsheet-style editable matrix.'),
        'handler' => 'matrix_editor_views_plugin',
        'theme' => 'views_view_matrix',
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'uses grouping' => FALSE,
        'type' => 'normal'
      ),
    ),
  );
}

