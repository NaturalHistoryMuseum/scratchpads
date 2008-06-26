<?php
// $Id$

/**
 * Implementation of hook_node_import_fields().
 */
function path_node_import_fields($type) {
  return array('path' => t('Path: URL alias'));
}
