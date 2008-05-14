<?php
// $Id: weblinks.inc,v 1.1.2.1.2.1 2007/05/14 08:31:49 robrechtj Exp $

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

