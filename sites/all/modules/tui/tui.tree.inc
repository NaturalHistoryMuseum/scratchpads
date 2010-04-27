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
    $output .= '"><img src="'.base_path().drupal_get_path('module','tui').'/images/empty.gif" width="9px" height="9px"></span><span class="tui-term" id="tid-'.$term->tid.'">'.check_plain($term->name).'</span>';
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
 * Move a term in a tree
 * 
 * Method is one of child, or sibling, whilst from $term is the term to move,
 * and to is the position to move to (child of, or next term after). 
 */
function tui_move_term($method, $term, $to){
  // Only continue if term != to, as JS can mess things up occasionaly
  if($term == $to){
    return;
  }
  $term_bits = explode("-", $term);
  $term = taxonomy_get_term($term_bits[1]);
  $to_bits = explode("-", $to);
  $to = taxonomy_get_term($to_bits[1]);
  $to->parent = db_result(db_query("SELECT parent FROM {term_hierarchy} WHERE tid = %d", $to->tid));
  if($to && $term){
    switch($method){
      case 'child':
        // This will be the first child of the parent, so we will increase the
        // weight of the siblings by one, and give this term the minimum weight.
        // FIXME - This works, but it may break other drupal stuff.
        $weight = db_result(db_query("SELECT MIN(weight) FROM {term_data} WHERE tid IN (SELECT tid FROM {term_hierarchy} WHERE parent = %d)", $to->tid));
        db_query("UPDATE {term_data} SET weight = weight +1 WHERE tid IN (SELECT tid FROM {term_hierarchy} WHERE parent = %d)", $to->tid);
        $term->weight = $weight;
        $term->parent = $to->tid;
        $form_values = (array)$term;
        taxonomy_save_term($form_values);
        break;      
      case 'sibling':
        // FIXME - This fails if the tree is set to alphapetical.
        db_query("UPDATE {term_data} SET weight = weight +2 WHERE tid IN (SELECT tid FROM {term_hierarchy} WHERE parent = %d) AND weight > %d", $to->parent, $to->weight);
        $term->weight = $to->weight + 1;
        $term->parent = $to->parent;
        $form_values = (array)$term;
        taxonomy_save_term($form_values);
        break;
    }
  }
}

/**
 * 
 */
function tui_delete_term($term_id){
  $term_parts = explode("-", $term_id);
  if($term_parts[0] == 'tid' && is_numeric($term_parts[1])){
    taxonomy_del_term($term_parts[1]);
    // We'll unset the annoying message that gets set!
    drupal_get_messages('status');
  }
}

function tui_get_tree($vid, $tids = array(), &$all_tids=array()){
  $tree = array();
  $all_tids = array();
  $results = db_query(db_rewrite_sql('SELECT tid FROM {term_data} t WHERE tid IN (SELECT tid FROM {term_hierarchy} WHERE parent = 0) AND vid = %d', 't', 'tid'), $vid);
  while($tid = db_result($results)){
    $tree[$tid] = db_result(db_query(db_rewrite_sql('SELECT COUNT(*) FROM {term_hierarchy} t WHERE parent = %d', 't', 'tid'), $tid));
    $all_tids[$tid] = $tid;
  }
  $num_tids = count($tids);
  foreach($tids as $tid){
    // Get the term, just to make sure it still exists
    $term = taxonomy_get_term($tid);
    if($term){
      // Term exists, now we should get all its ancestors
      $ancestors = taxonomy_get_parents_all($tid);
      // If we have more than one ancestor, then we need to do stuff
      if(count($ancestors)>1){
        // For every term in the list, we need to show all the children of that
        // term (except for the very last one).
        array_shift($ancestors);
        while($ancestors){
          $term = array_shift($ancestors);
          $result = db_query('SELECT tid FROM {term_hierarchy} WHERE parent = %d AND tid IN (SELECT tid FROM {term_data} WHERE vid = %d)', $term->tid, $vid);
          $previous_branch = $branch;
          $branch = array();
          while($row = db_fetch_array($result)){
            $all_tids[$row['tid']] = $row['tid'];
            if($row['tid'] == $previous_tid){
              $branch[$row['tid']] = $previous_branch;
            } else {
              $branch[$row['tid']] = false;
            }
          }
          $previous_tid = $term->tid;
        }
        if(is_array($tree[$term->tid])){
          $tree[$term->tid] = array_merge_recursive_distinct($branch, $tree[$term->tid]);
        } else {
          $tree[$term->tid] = $branch;
        }
      }
    }
  }
  return $tree;
}

/**
 * Straight from php.net.
 * 
 * http://www.php.net/manual/en/function.array-merge-recursive.php#96201
 * 
 * Thanks must go to mark dot roduner at gmail dot com
 * RANT
 * Why doesn't the default array_merge_recursive do this anyway.
 * /RANT
 */
function array_merge_recursive_distinct(){
  $arrays = func_get_args();
  $base = array_shift($arrays);
  if(!is_array($base)) $base = empty($base) ? array() : array($base);
  foreach($arrays as $append) {
    if(!is_array($append)) $append = array($append);
    foreach($append as $key => $value) {
      if(!array_key_exists($key, $base) and !is_numeric($key)) {
        $base[$key] = $append[$key];
        continue;
      }
      if(is_array($value) or is_array($base[$key])) {
        $base[$key] = array_merge_recursive_distinct($base[$key], $append[$key]);
      } else if(is_numeric($key)) {
        if(!in_array($value, $base)) $base[] = $value;
      } else {
        $base[$key] = $value;
      }
    }
  }
  return $base;
}

/**
 * 
 */
function tui_get_tree_themed(){
  $tids = array();
  foreach($_REQUEST as $key => $value){
    if(is_numeric($key) && $key == $value){
      $tids[] = $key;
    }
  }
  $term = taxonomy_get_term($tids[0]);
  $tree = tui_get_tree($term->vid, $tids);
  echo theme('tui_tree', $tree);
}