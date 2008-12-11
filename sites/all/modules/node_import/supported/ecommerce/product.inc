<?php
// $Id: product.inc,v 1.2.4.1 2008/03/17 13:02:51 robrechtj Exp $

/**
 * @file
 * Support file for product.module of the e-commerce module
 * bundle.
 */

/**
 * Implementation of hook_node_import_fields().
 */
function product_node_import_fields($type) {
  if (_product_node_import_is_product($type)) {
    $fields = array(
      'title' => t('Product: Name'),
      'body' => t('Product: Description'),
      'price' => t('Product: Price'),
      'sku' => t('Product: Product identifier (SKU)'),
      'hide_cart_link' => t('Product: "Add to cart" link'),
    );

    if (variable_get('payment_recurring', 0)) {
      $fields['price_interval'] = t('Product: Billing cycle - price interval');
      $fields['price_unit'] = t('Product: Billing cycle - interval unit');
      $fields['price_cycle'] = t('Product: Billing cycle - number of payments cycles');
    }

    if ($type != 'product') {
      $fields = array_merge($fields, node_import_fields('product'));
    }

    return $fields;
  }
}

/**
 * Implementation of hook_node_import_prepare().
 */
function product_node_import_prepare(&$node, $preview = FALSE) {
  $errors = array();
  if ($node->type == 'product') {
    if (isset($node->price)) {
      $price = normalize_price($node->price);
      if ($price === FALSE) {
        $errors[] = t('Price (%value) is not valid. It must only contain integers, decimal point and a currency symbol.', array('%value' => $node->price));
      }
      else {
        $node->price = $price;
      }
    }
    else {
      $node->price = 0;
    }

    if (isset($node->hide_cart_link) && strlen($node->hide_cart_link) > 0) {
      switch (strtolower($node->hide_cart_link)) {
        case 'visible':
          $node->hide_cart_link = 0;
          break;
        case 'hidden':
          $node->hide_cart_link = 1;
          break;
        default:
          $node->hide_cart_link = $node->hide_cart_link ? 1 : 0;
          break;
      }
    }
    else {
      $node->hide_cart_link = 0;
    }

    if (variable_get('payment_recurring', 0)) {

      if (isset($node->price_interval) && strlen($node->price_interval) > 0) {
        if (!is_numeric($node->price_interval) || $node->price_interval < 0 || $node->price_interval > 31) {
          $errors[] = t('Price interval (%value) is not valid. It must be an integer between 0 and 31.', array('%value' => $node->price_interval));
        }
      }

      if (isset($node->price_unit) && strlen($node->price_unit) > 0) {
        $options = array(
          t('days') => 'D', t('day(s)') => 'D', t('day') => 'D', 'd' => 'D',
          t('weeks') => 'W', t('week(s)') => 'W', t('week') => 'W', 'w' => 'W',
          t('months') => 'M', t('month(s)') => 'M', t('month') => 'M', 'm' => 'M',
          t('years') => 'Y', t('year(s)') => 'Y', t('year') => 'Y', 'y' => 'Y',
        );
        if (!isset($options[strtolower($node->price_unit)])) {
          $errors[] = t('Price interval unit (%value) is not valid. It must either be "d" (days), "w" (weeks), "m" (months) or "y" (years).', array('%value' => $node->price_unit));
        }
        else {
          $node->price_unit = $options[strtolower($node->price_unit)];
        }
      }

      if (isset($node->price_cycle) && strlen($node->price_cycle) > 0) {
        if (!is_numeric($node->price_cycle) || $node->price_cycle < 0 || $node->price_cycle > 31) {
          $errors[] = t('Number of payment cycles (%value) is not valid. It must be an integer between 0 and 31.', array('%value' => $node->price_cycle));
        }
      }
    }
  }
  return $errors;
}

/**
 * Implementation of hook_node_import_global().
 */
function product_node_import_global($type, $global_values) {
  if (_product_node_import_is_product($type) && $type != 'product') {
    return module_invoke_all('node_import_global', 'product', $global_values);
  }
}

/**
 * Implementation of hook_node_import_static().
 */
function product_node_import_static($type) {
  if (_product_node_import_is_product($type) && $type != 'product') {
    return array_merge(array('type' => 'product', 'ptype' => $type), module_invoke_all('node_import_static', 'product'));
  }
}

/**
 * Check whether the node_import $type provided to the hooks is
 * actually a $node->ptype.
 */
function _product_node_import_is_product($type) {
  static $ptypes;

  if (!isset($ptypes)) {
    $ptypes = product_get_ptypes();
  }

  if (isset($ptypes[$type])) {
    return TRUE;
  }

  return FALSE;
}
