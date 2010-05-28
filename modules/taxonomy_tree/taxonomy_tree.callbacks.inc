<?php

function taxonomy_tree_callback($nid = null){

  // If root = source, the tree should already be populated  
  if($_GET['root'] == 'source'){
   return; 
  }
  
  $exploded_root = explode('-', $_GET['root']);
  
  $parent_tid = $exploded_root[0];
  $select_all = $exploded_root[1];
  
	$parent_term = taxonomy_get_term($parent_tid);

  $selected_tids = array();

  $treeview_items = taxonomy_tree_get_treeview_items($parent_term->vid, $selected_tids, $parent_tid, $select_all);

  return drupal_json($treeview_items);
   
}



