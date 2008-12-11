<?php
// $Id: usernodes.inc,v 1.1.4.1 2008/03/17 13:02:50 robrechtj Exp $

/**
 * @file
 * Support file for usernodes module.
 */

/**
 * Implementation of hook_node_import_prepare().
 */
function usernodes_node_import_prepare(&$node, $preview = FALSE) {
  $errors = array();
  if (_usernodes_at_max($node->type, $node->uid)) {
    $user = user_load(array('uid' => $node->uid));
    $errors[] = t('The user %name can not create any more nodes of this type.', array('%name' => $user->name));
  }
  return $errors;
}
