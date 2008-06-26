<?php

/**
 * @file
 * Support for the auto_nodetitle module.
 */

/**
 * Implementation of hook_node_import_prepare().
 */
function auto_nodetitle_node_import_prepare(&$node, $preview) {
  $setting = auto_nodetitle_get_setting($node->type);
  if (($setting == AUTO_NODETITLE_ENABLED) || ($setting == AUTO_NODETITLE_OPTIONAL && empty($node->title))) {
    token_get_values('node', $node, TRUE); // Flush first!
    auto_nodetitle_set_title($node);
  }
}

