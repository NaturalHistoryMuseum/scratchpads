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
    }
    $output .= '<li id="tidandvid-'.$term->tid.'-'.$term->vid.'"><span class="tui-nodeleaf ';
    if(is_array($branch)){
      $output .= 'tui-node-open';
    }
    else if($term_has_children){
      $output .= 'tui-node-closed';
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

/**
 * Get the tree (or part of), and return it for embedding in a page
 */
function tui_get_children($tid_and_vid){
  // Get the tree, then theme it, then return it.
  $parts = explode("-", $tid_and_vid);
  
  $tree = array();
  $results = db_query(db_rewrite_sql('SELECT tid FROM {term_data} t WHERE tid IN (SELECT tid FROM {term_hierarchy} WHERE parent = %d)', 't', 'tid'), $parts[1]);
  while($tid = db_result($results)){
    $tree[$tid] = db_result(db_query(db_rewrite_sql('SELECT COUNT(*) FROM {term_hierarchy} t WHERE parent = %d', 't', 'tid'), $tid));
  }
  echo theme('tui_tree', $tree);
}

/**
 * 
 */
function tui_get_tree(){
  $tids = array();
  foreach($_REQUEST as $key => $value){
    if(is_numeric($key) && $key == $value){
      $tids[] = $key;
    }
  }
  $tree = array();
  // We have a tid set, we need to open out to the tid, else we'll just show 
  // the very root of the tree.
  $num_tids = count($tids);
  for($i=0; $i<$num_tids; $i++){
    $branch = false;
    if(isset($tids[$i])){
      $parents = taxonomy_get_parents_all($tids[$i]);
      while($parents){
        $term = array_shift($parents);
        $key = array_search($term->tid, $tids);
        if($key){
          unset($tids[$key]);
        }
        if(is_array($branch)){
          foreach($branch as $tid => $stuff){
            $all_tids[$tid] = $tid;
          }
        }
        $branch = array($term->tid => $branch);
      }
      $tree[$term->tid] = array_pop($branch);
      $all_tids[$term->tid] = $term->tid;
    }
  }
  echo theme('tui_tree', $tree);
}