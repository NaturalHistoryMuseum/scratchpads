<?php
// $Id: template.php,v 1.1.2.2 2009/09/29 04:19:00 sociotech Exp $

/**
 * Changed breadcrumb separator
 */
function acquia_prosper_breadcrumb($breadcrumb) {
  if (!empty($breadcrumb)) {
    return '<div class="breadcrumb">'. implode(' &rarr; ', $breadcrumb) .'</div>';
  }
}

/**
 * Node preprocessing
 */
function acquia_prosper_preprocess_node(&$vars) {
  // Render Ubercart fields into separate variables
  if ($vars['template_files'][0] == 'node-product') {
    $node = node_build_content(node_load($vars['nid']));
    $vars['fusion_uc_image'] = drupal_render($node->content['image']);
    $vars['fusion_uc_body'] = drupal_render($node->content['body']);
    $vars['fusion_uc_display_price'] = drupal_render($node->content['display_price']);
    $vars['fusion_uc_add_to_cart'] = drupal_render($node->content['add_to_cart']);
    $vars['fusion_uc_weight'] = drupal_render($node->content['weight']);
    $vars['fusion_uc_dimensions'] = drupal_render($node->content['dimensions']);
    $vars['fusion_uc_model'] = drupal_render($node->content['model']);
    $vars['fusion_uc_list_price'] = drupal_render($node->content['list_price']);
    $vars['fusion_uc_sell_price'] = drupal_render($node->content['sell_price']);
    $vars['fusion_uc_cost'] = drupal_render($node->content['cost']);
    $vars['fusion_uc_additional'] = drupal_render($node->content);
  }
}
