<?php
/* $Id: premium.inc,v 1.1 2007/02/01 11:02:28 robrechtj Exp $ */

/**
 * Implementation of hook_node_import_fields().
 */
function premium_node_import_fields($type) {
  if (user_access('administer nodes')) {
    return array(
      'premium' => t('Premium: Access restricted for non-premium users'),
    );
  }
}

/**
 * Implementation of hook_node_import_prepare().
 */
function premium_node_import_prepare(&$node, $preview = FALSE) {
  $errors = array();

  // The user may have enabled it for this content type.
  $premium = in_array('premium', (array)variable_get('node_options_'. $node->type, array())) ? 1 : 0;

  // Alternatively, she may have enabled it during import.
  if (isset($node->node_import_premium)) {
    $premium = $node->node_import_premium;
    unset($node->node_import_premium);
  }

  // If the premium field was mapped, use that value instead.
  if (isset($node->premium) && strlen($node->premium) > 0) {
    $premium = ($node->premium ? 1 : 0);
  }

  $node->premium = $premium;

  return $errors;
}

/**
 * Implementation of hook_node_import_global().
 */
function premium_node_import_global($type, $global_values) {
  if (user_access('administer nodes')) {
    if (!isset($global_values['node_import_premium'])) {
      $defaults = variable_get('node_options_'. $type, array());
      $premium = (in_array('premium', $defaults) ? 1 : 0);
    }
    else {
      $premium = $global_values['node_import_premium'];
    }

    $form = array();
    $form['node_import_premium'] = array(
      '#type' => 'checkbox',
      '#default_value' => $premium,
      '#title' => t('Access restricted for non-premium users'),
    );
  return $form;
  }
}
