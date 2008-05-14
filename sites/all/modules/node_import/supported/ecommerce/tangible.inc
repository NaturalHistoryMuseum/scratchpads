<?php
// $Id: tangible.inc,v 1.1.4.1 2008/03/17 13:02:51 robrechtj Exp $

/**
 * @file
 * Support file for tangible.module of the e-commerce module
 * bundle.
 */

/**
 * Implementation of hook_node_import_types().
 */
function tangible_node_import_types() {
  return array(
    'tangible' => t('Shippable product'),
  );
}

/**
 * Implementation of hook_node_import_fields().
 */
function tangible_node_import_fields($type) {
  if ($type == 'tangible') {
    return array(
      'manage_stock' => t('Shippable product: Inventory management enabled?'),
      'stock' => t('Shippable product: Number of items in stock'),
      'availability' => t('Shippable product: Availability estimate'),
    );
  }
}

/**
 * Implementation of hook_node_import_prepare().
 */
function tangible_node_import_prepare(&$node, $preview = FALSE) {
  $errors = array();
  if ($node->type == 'product' && $node->ptype == 'tangible') {
    if (isset($node->manage_stock) && strlen($node->manage_stock) > 0) {
      switch (strtolower($node->manage_stock)) {
        case 'enabled':
          $node->manage_stock = 1;
          break;
        case 'disabled':
          $node->manage_stock = 0;
          break;
        default:
          $node->manage_stock = $node->manage_stock ? 1 : 0;
          break;
      }
    }
    else {
      $node->manage_stock = 0;
    }

    if (isset($node->stock) && strlen($node->stock) > 0) {
      if (!is_numeric($node->stock) || $node->stock < 0) {
        $errors[] = t('The number of items in stock (%value) is not valid.', array('%value' => $node->stock));
      }
    }

    if (isset($node->availability) && strlen($node->availability) > 0) {
      $options = array_flip(array_map('strtolower', availability_build_messages()));
      if (isset($options[$node->availability])) {
        $node->availability = $options[$node->availability];
      }
      else if (!is_numeric($node->availability) || $node->availability < 1 || $node->availability > count($options)) {
        $errors[] = t('The availability estimate (%value) is not valid.', array('%value' => $node->availability));
      }
    }
    else {
      $node->availability = 1;
    }
  }
  return $errors;
}
