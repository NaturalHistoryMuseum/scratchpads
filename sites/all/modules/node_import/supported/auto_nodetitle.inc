<?php

/**
 * @file
 * Support for the auto_nodetitle module.
 */

/**
 * Implementation of hook_node_import_prepare().
 */
function auto_nodetitle_node_import_prepare(&$node, $preview) {
  $errors = array();

  $setting = auto_nodetitle_get_setting($node->type);
  if (($setting == AUTO_NODETITLE_ENABLED) || ($setting == AUTO_NODETITLE_OPTIONAL && $node->title == $node->auto_nodetitle)) {
    // Flush the tokens and create a new title.
    token_get_values('node', $node, TRUE);
    auto_nodetitle_set_title($node);
    unset($node->auto_nodetitle);
  }

  // Since we provided a static random title value the test in
  // supported/node.inc for unique titles does no longer work.
  if ($setting == AUTO_NODETITLE_ENABLED || $setting == AUTO_NODETITLE_OPTIONAL) {
    if ($node->node_import_node['unique_title']) {
      $count = db_fetch_object(db_query("SELECT count(*) AS cnt FROM {node} WHERE title = '%s' AND type = '%s'", $node->title, $node->type));
      if ($count->cnt > 0) {
        $errors[] = t('The node title %title is not unique for this node type.', array('%title' => $node->title));
      }
    }
  }

  return $errors;
}

/**
 * Implementation of hook_node_import_static().
 */
function auto_nodetitle_node_import_static($type) {
  $setting = auto_nodetitle_get_setting($type);
  if ($setting == AUTO_NODETITLE_ENABLED || $setting == AUTO_NODETITLE_OPTIONAL) {
    $title = md5(rand());
    return array(
      'title' => $title,
      'auto_nodetitle' => $title,
    );
  }
}

