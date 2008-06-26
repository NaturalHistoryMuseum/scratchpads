<?php
// $Id$

/**
 * @file
 * Panels integration for Bio module.
 */

/**
 * Plugin to provide a relationship handler for node from user.
 */
function bio_panels_relationships() {
  $bio_type = bio_get_type();
  $args['node_from_user'] = array(
    'title' => t('User @type', array('@type' => $bio_type)),
    'keyword' => 'node',
    'description' => t("Adds a user's @type node.", array('@type' => $bio_type)),
    'required context' => new panels_required_context(t('User'), 'user'),
    'context' => 'bio_panels_context',
  );

  return $args;
}

/**
 * Return a new context based on an existing context.
 */
function bio_panels_context($context = NULL, $conf) {
  // If unset it wants a generic, unfilled context, which is just NULL.
  if (empty($context->data)) {
    return panels_context_create_empty('node', NULL);
  }

  if (isset($context->data->uid)) {
    // Load the NID for the user's bio node.
    $nid = bio_for_user($context->data->uid);
    
    // Load the node object itself.
    $bionode = node_load(array('nid' => $nid));
    
    // Send it to Panels.
    return panels_context_create('node', $bionode);
  }
  else {
    return panels_context_create_empty('node', NULL);
  }   
}

