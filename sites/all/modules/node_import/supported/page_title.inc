<?php
// $Id$

/**
 * @file
 * Support file for the page_title module.
 */

/**
 * Implementation of hook_node_import_fields().
 */
function page_title_node_import_fields($type) {
  return array(
    'page_title' => t('Page title'),
  );
}

