<?php
// $Id$

/**
 * @file
 * Support file for links_weblink module (from the links module package).
 */

/**
 * Implementation of hook_node_import_fields().
 */
function links_weblink_node_import_fields($type) {
  if ($type == 'weblink') {
    return array(
      'links_weblink_url' => t('Weblink URL'),
    );
  }
}

