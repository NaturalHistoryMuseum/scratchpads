<?php
// $Id$

function menu_activemenu() {
  $items = array();
  $root_menus = menu_get_root_menus();
  foreach (array_keys($root_menus) as $mid) {
    $items['#block-menu-'. $mid] = 'activemenu/menu';
  }
  return $items;
}