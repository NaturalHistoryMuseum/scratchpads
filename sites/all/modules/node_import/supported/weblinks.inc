<?php
// $Id$

/**
 * @file
 * Support file for the weblinks module.
 */

/**
 * Implementation of hook_node_import_fields().
 */
function weblinks_node_import_fields($type) {
  if ($type == 'weblinks') {
    return array(
      'url' => t('Web link: URL'),
      'weight' => t('Web link: weight'),
    );
  }
}

