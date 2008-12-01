<?php

/**
 * Function to build menu page and classification landing page
 */
function classification_menu_block_page() {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  if (system_admin_compact_mode()) {
  	$output = '<h2>Manage Classification</h2>';
    $output .= '<ul class="menu">';
    foreach ($content as $item) {
      $output .= '<li class="leaf">'. l($item['title'], $item['href'], $item['localized_options']) .'</li>';
    }
    $output .= '</ul>';
  }
  else {
  	$output = '<h2>Manage Classification</h2>';
    $output .= '<dl class="admin-list">';
    foreach ($content as $item) {
      $output .= '<dt>'. l($item['title'], $item['href'], $item['localized_options']) .'</dt>';
      $output .= '<dd>'. $item['description'] .'</dd>';
    }
    $output .= '</dl>';
  }
  
  return $output;
}