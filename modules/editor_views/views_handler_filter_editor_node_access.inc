<?php
// $Id: views_handler_filter_editor_node_access.inc,v 1.1 2010/03/22 10:22:11 b3n Exp $

/**
 * @file
 * Provides editor view node access filter
 */
 
class views_handler_filter_editor_node_access extends views_handler_filter_in_operator {
  function can_expose() {
    return FALSE;
  }

  function operators() {
    return array(
      'in' => array(
        'title' => t('User can access'),
        'short' => t('access'),
        'method' => 'op_simple',
        'values' => 1,
      ),
    );
  } 

  function get_value_options() {
     $this->value_options = array(
      'view' => 'View',
      'update' => 'Edit',
      'delete' => 'Delete',
    );
  }

 function query() {

    if (!user_access('administer nodes')) {

      $where = _editor_views_where_sql($this->value);

      $this->query->add_where('AND', $where['sql'], $where['args']);

    }
  
  } 

}