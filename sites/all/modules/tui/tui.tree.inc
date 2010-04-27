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
  if($num_tids){
    // We have a tid set, we need to open out to the tid, else we'll just show 
    // the very root of the tree.
    $num_tids = count($tids);
    for($i=0; $i<$num_tids; $i++){
      $branch = false;
      if(isset($tids[$i]) && is_numeric($tids[$i])){
        // Check it also hasn't been deleted.
        $term = taxonomy_get_term($tids[$i]);
        if(!$term){
          continue;
        }
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
  }
  print_r($tree);exit;
  return $tree;
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