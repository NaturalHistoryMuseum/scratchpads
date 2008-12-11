<?php
// $Id: page_title.inc,v 1.1.2.1 2007/02/06 09:53:34 robrechtj Exp $

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

