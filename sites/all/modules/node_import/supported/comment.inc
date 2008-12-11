<?php
/* $Id: comment.inc,v 1.1.2.1 2008/02/03 10:01:31 robrechtj Exp $ */

/**
 * Implementation of hook_node_import_fields().
 */
function comment_node_import_fields($type) {
  $fields = array();
  if (user_access('administer nodes')) {
    $fields = array(
      'comment' => t('Node: Comment options'),
    );
  }
  return $fields;
}

/**
 * Implementation of hook_node_import_prepare().
 */
function comment_node_import_prepare(&$node, $preview = FALSE) {
  $errors = array();

  if (isset($node->comment) && !empty($node->comment)) {
    // TODO: find a way to use the comment constants COMMENT_NODE_DISABLED/READ_ONLY/READ_WRITE
    if (!is_numeric($node->comment) or ($node->comment < 0 || $node->comment > 2)) {
      $errors[] = t('The comment option %comment is not a valid option. Valid options are: 0 (disabled), 1 (read only), 2 (read/write).', array('%comment' => $node->comment));
    }
  }
  else {
    $node->comment = variable_get('comment_'. $node->type, COMMENT_NODE_READ_WRITE);
  }

  return $errors;
}
