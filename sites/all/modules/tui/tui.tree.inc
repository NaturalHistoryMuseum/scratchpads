<?php

/**
 * Theme the tree
 */
function theme_tui_tree($taxonomy_tree){
  // We need to employ a recursive element here, and it may as well be this 
  // function that does it.  We'll recurse into the tree, magicing things to 
  // where they should be.
  $output = '<ul>';
  foreach($taxonomy_tree as $tid => $branch){
    $term = taxonomy_get_term($tid);
    $term_has_children = FALSE;
    if(!is_array($branch)){
      // Does this term have children
      $term_has_children = db_result(db_query('SELECT COUNT(*) FROM {term_hierarchy} WHERE parent = %d', $term->tid));
    } else {
      $term_has_children = TRUE;
    }
    $output .= '<li><span class="';
    if($term_has_children){
      $output .= 'tui-node';
    } else {
      $output .= 'tui-leaf';
    }
    $output .= '">&nbsp;</span><span class="tui-term" id="tid-'.$term->tid.'">'.check_plain($term->name).'</span>';
    if(is_array($branch)){
      $output .= theme_tui_tree($branch);
    }
    $output .= '</li>';
  }
  return $output . '</ul>';
}