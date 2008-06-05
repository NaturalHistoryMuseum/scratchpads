<?php
// $Id$

/**
 * @file
 * Support file for userlink module (from the links module package).
 */

/**
 * Implementation of hook_node_import_types().
 */
function userlink_node_import_types() {
  return array(
    'userlink' => t('Userlink'),
  );
}

/**
 * Implementation of hook_node_import_fields().
 */
function userlink_node_import_fields($type) {
  if ($type == 'userlink') {
    return array(
      'title' => t('Title'),
      'body' => t('Body'),
      'url' => t('Userlink: URL'),
      'private' => t('Userlink: Private'),
    );
  }
}

