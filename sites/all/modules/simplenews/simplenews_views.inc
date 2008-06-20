<?php
// $Id: simplenews_views.inc,v 1.1.2.1 2008/02/23 14:38:58 sutharsan Exp $

/**
 * @file simplenews_views.inc
 * Views support for simplenews
 */

/**
 * Implementation of hook_views_tables
 */
function simplenews_views_tables() {
  $tables['simplenews_newsletters'] = array(
    'name' => 'simplenews_newsletters',
    'fields' => array(
      's_status' => array(
        'name' => t('Simplenews: Send status'),
        'handler' => 'simplenews_handler_send_status',
        'help' => t('Newsletter send status: Not send, Pending (being send or waiting for cron to run), Send. '),
        'sortable' => TRUE,
      ),
    ),
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'nid',
      ),
      'right' => array(
        'field' => 'nid',
      ),
    ),
    'sorts' => array(
      's_status' => array(
        'name' => t('Simplenews: Send status'),
        'help' => t('Sort by send status; Ascending = Not send > Pending > Send'),
      ),
    ),
    'filters' => array(
      's_status' => array(
        'name' => t('Simplenews: Send status'),
        'list' => 'simplenews_handler_filter_send_status',
        'list-type' => 'list',
        'operator' => 'views_handler_operator_or',
        'value-type' => 'array',
        'help' => t('Include or exclude status of the selected state.'),
      ),
    ),
  );
  return $tables;
}

/**
 * Views field handler for newsletter send status
 */
function simplenews_handler_send_status($fieldinfo, $fielddata, $value, $data) {
  $status = _simplenews_handler_send_status_data();
  return $status[$value];
}

/**
 * Views filter handler for newsletter send status
 */
function simplenews_handler_filter_send_status($op) {
  return _simplenews_handler_send_status_data();
}

/**
 * Helper function for send status handlers
 */
function _simplenews_handler_send_status_data() {
  return array(
    0 => t('Not send'),
    1 => t('Pending'),
    2 => t('Send'),
  );
}