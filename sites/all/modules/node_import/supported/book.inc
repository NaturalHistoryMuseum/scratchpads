<?php
// $Id: book.inc,v 1.1.2.1.2.2 2008/03/17 13:02:50 robrechtj Exp $

/**
 * @file
 * Support file for book module.
 */

/**
 * Implementation of hook_node_import_fields().
 */
function book_node_import_fields($type) {
  if ($type == 'book') {
    return array(
      'node_import_book_parent' => t('Book page: Parent link'),
      'weight' => t('Book page: Page weight'),
    );
  }
}

/**
 * Implementation of hook_node_import_prepare().
 */
function book_node_import_prepare(&$node, $preview = FALSE) {
  $errors = array();
  if (isset($node->node_import_book_parent)) {
    $parent = trim($node->node_import_book_parent);
    unset($node->node_import_book_parent);
    if (strlen($parent) == 0) {
      $node->parent = 0;
    }
    else if (($nid = node_import_nodereference($parent))) {
      $node->parent = $nid;
    }
    else {
      $errors[] = t('Unable to set parent of book page: no node matching %title found.', array('%title' => $parent));
    }
  }
  return $errors;
}

/**
 * Implementation of hook_node_import_static().
 */
function book_node_import_static($type) {
  if ($type == 'book') {
    return array(
      'parent' => 0,
      'weight' => 0,
    );
  }
}

