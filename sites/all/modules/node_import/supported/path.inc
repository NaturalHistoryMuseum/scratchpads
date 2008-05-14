<?php
// $Id: path.inc,v 1.1 2006/07/10 09:22:10 robrechtj Exp $

/**
 * Implementation of hook_node_import_fields().
 */
function path_node_import_fields($type) {
  return array('path' => t('Path: URL alias'));
}
