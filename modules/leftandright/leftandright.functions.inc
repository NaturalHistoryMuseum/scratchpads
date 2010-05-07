<?php
function _leftandright_taxonomy_update($array){
  // Get the right plus one value for this term (what it should be)
  $new_right = _leftandright_get_new_right($array);
  $new_left = _leftandright_get_new_left($array);
  
  // Get what it is set as now.
  $current_right = array_pop(db_fetch_array(db_query('SELECT rgt FROM {leftandright} WHERE tid = %d', $array['tid'])));
  $current_left = array_pop(db_fetch_array(db_query('SELECT lft FROM {leftandright} WHERE tid = %d', $array['tid'])));
  
  // If we have a change, then we'd best do stuff to reflect this change.
  //if($current_right != ($new_right -1)){
  if($current_right != $new_right || $current_left != $new_left){
    // We have three possible movements:
    // - left
    // - right
    // - up
    // Lets work out which one we've got.  If we calculate whether or not
    // we've moved up, and we haven't, then left is 
    // $current_right>$new_right
    // else
    // $current_right<$new_right
    // Get current ancestors (from leftandright)
    $result = db_query('SELECT tid FROM {leftandright} WHERE lft<(SELECT lft FROM {leftandright} WHERE tid = %d) AND rgt>(SELECT rgt FROM {leftandright} WHERE tid = %d ORDER BY lft DESC)', $array['tid'], $array['tid']);
    // Throw away the first result, if it hasn't moved up, then this will
    // still be the parent
    db_fetch_array($result);
    $shift = ($current_right - $current_left) + 1;
    if($new_right > $current_right){
      // We're moving right
      // Firstly, move the branch to the right position (but with
      // negative values).
      $new_left = $new_right - ($current_right - $current_left);
      $branch_change = $new_left - $current_left;
      db_query('UPDATE {leftandright} SET rgt = -(rgt + %d), lft = -(lft + %d) WHERE lft>=%d AND rgt<=%d', $branch_change, $branch_change, $current_left, $current_right);
      // Then close make the space moving stuff to the left that is 
      // between where we were, and where we will be.
      db_query('UPDATE {leftandright} SET rgt = (rgt - %d) WHERE rgt > %d AND rgt <= %d', $shift, $current_right, $new_right);
      db_query('UPDATE {leftandright} SET lft = (lft - %d) WHERE lft > %d AND lft <= %d', $shift, $current_right, $new_left);
    } else {
      // We're moving left
      $new_right = $new_left + ($current_right - $current_left);
      $branch_change = $current_left - $new_left;
      db_query('UPDATE {leftandright} SET rgt = -(rgt - %d), lft = -(lft - %d) WHERE lft>=%d AND rgt<=%d', $branch_change, $branch_change, $current_left, $current_right);
      db_query('UPDATE {leftandright} SET rgt = (rgt + %d) WHERE rgt >= %d AND rgt < %d', $shift, $new_left, $current_left);
      db_query('UPDATE {leftandright} SET lft = (lft + %d) WHERE lft >= %d AND lft < %d', $shift, $new_left, $current_left);
    }
    // Ensure the depth is correctly set.
    if(is_array($array['parent'])){
      $parent_tid = array_pop($array['parent']);
    } else {
      $parent_tid = $array['parent'];
    }
    if($parent_tid){
      $new_depth = array_pop(db_fetch_array(db_query('SELECT depth FROM {leftandright} WHERE tid = %d', $parent_tid))) + 1;
    } else {
      $new_depth = 0;
    }
    $current_depth = array_pop(db_fetch_array(db_query('SELECT depth FROM {leftandright} WHERE tid = %d', $array['tid'])));
    if($current_depth != $new_depth){
      // Depth has changed, lets do something about it
      $depth_change = $new_depth - $current_depth;
      db_query('UPDATE {leftandright} SET depth = (depth + %d) WHERE lft < 0 AND rgt < 0', $depth_change);
    }    
    // Finally restore the branch
    db_query('UPDATE {leftandright} SET rgt = -rgt, lft = -lft WHERE lft < 0 AND rgt < 0');
  }
}

function _leftandright_taxonomy_insert($array){
  // Work out correct position - if no siblings, then this is easy, else
  // we have to do some calculating.
  foreach($array['parent'] as $parent_tid){
    $result = db_query('SELECT lft,rgt FROM {leftandright} WHERE tid = %d', $parent_tid);
    $row = db_fetch_array($result);
    if($row && $row['lft']+1 == $row['rgt']){
      // We have no siblings, lft and rgt are easy to set
      $lft_position = $row['lft']+1;
    } else {
      // We have siblings, so lets get the previous sibling (or parent)
      // to set the correct leftandright
      $result = db_query('SELECT tid FROM {term_data} WHERE tid IN (SELECT tid FROM {term_hierarchy} WHERE parent = %d) AND vid = %d ORDER BY weight ASC, name', $parent_tid, $array['vid']);
      $previous_sibling = FALSE;
      while($row = db_fetch_array($result)){
        if($row['tid'] == $array['tid']){
          // We should now have the previous sibling
          break;
        }
        $previous_sibling = $row['tid'];
      }
      if($previous_sibling){
        // Get the right of this sibling, then we set out leftandright
        // relative to that
        $result = db_query('SELECT rgt AS leftof FROM {leftandright} WHERE tid = %d', $previous_sibling); 
      } else {
        // Else it's the left of the parent.
        $result = db_query('SELECT lft AS leftof FROM {leftandright} WHERE tid = %d', $parent_tid);
      }
      $row = db_fetch_array($result);
      $lft_position = $row['leftof']+1;
    }
    db_query('UPDATE {leftandright} SET rgt = rgt+2 WHERE rgt >= %d AND vid = %d', $lft_position, $array['vid']);
    db_query('UPDATE {leftandright} SET lft = lft+2 WHERE lft >= %d AND vid = %d', $lft_position, $array['vid']);
    $result = db_query('SELECT depth FROM {leftandright} WHERE tid = %d', $parent_tid);
    $row = db_fetch_array($result);
    if($row){
      $depth = $row['depth'] + 1;
    } else {
      $depth = 0;
    }
    db_query('INSERT INTO {leftandright} (lft, rgt, depth, tid, vid) VALUES (%d, %d, %d, %d, %d)', $lft_position, $lft_position+1, $depth, $array['tid'], $array['vid']);
  }
}

/**
 * Following function returns an int which reflects the value to the right of
 * the insert position for the term.  This is the next sibling's left value, or
 * the parent's right value (depending on the order/presence of siblings).
 *
 * @param int $tid
 */
function _leftandright_get_new_right($term){
  $result = db_query('SELECT tid FROM {term_data} WHERE tid IN (SELECT tid FROM {term_hierarchy} WHERE parent = (SELECT parent FROM {term_hierarchy} WHERE tid = %d LIMIT 1)) AND vid = %d ORDER BY weight ASC, name', $term['tid'], $term['vid']);
  $next_value = FALSE;
  while($row = db_fetch_array($result)){
    if($next_value){
      return array_pop(db_fetch_array(db_query('SELECT lft FROM {leftandright} WHERE tid = %d', $row['tid']))) - 1;
    }
    if($row['tid'] == $term['tid']){
      $next_value = TRUE;
    }
  }
  // If we've reached this point, the term has no siblings (or none to the 
  // right) so we return the rgt value of the parent
  // First get the parent and check that it isn't "0"
  $parent = array_pop(db_fetch_array(db_query('SELECT parent FROM {term_hierarchy} WHERE tid = %d LIMIT 1', $term['tid'])));
  if($parent != 0){
    return array_pop(db_fetch_array(db_query('SELECT rgt FROM {leftandright} WHERE tid = %d', $parent))) - 1;
  } else {
    // We've got a Zero parent, and we're the far right root, so our right value
    // is calculated from the total number of terms in the tree *2
    return array_pop(db_fetch_array(db_query('SELECT COUNT(*)*2 FROM term_data WHERE vid = %d', $term['vid'])));
  }
}

/**
 * Following function returns an int which reflects the value to the left of
 * the insert position for the term.  This is the previous sibling's right 
 * value, or the parent's left value (depending on the order/presence of 
 * siblings).
 *
 * @param int $tid
 */
function _leftandright_get_new_left($term){
  $result = db_query('SELECT tid FROM {term_data} WHERE tid IN (SELECT tid FROM {term_hierarchy} WHERE parent = (SELECT parent FROM {term_hierarchy} WHERE tid = %d LIMIT 1)) AND vid = %d ORDER BY weight ASC, name', $term['tid'], $term['vid']);  
  $previous_value = FALSE;
  while($row = db_fetch_array($result)){
    if($row['tid'] == $term['tid']){
      if($previous_value){
        return array_pop(db_fetch_array(db_query('SELECT rgt FROM {leftandright} WHERE tid = %d', $previous_value))) + 1;
      } else {
        // Return the parent left
        break;
      }
    }
    $previous_value = $row['tid'];
  }
  // If we've reached this point, the term has no siblings (or none to the left)
  // so we return the right value of the parent
  // First get the parent and check that it isn't "0"
  $parent = array_pop(db_fetch_array(db_query('SELECT parent FROM {term_hierarchy} WHERE tid = %d LIMIT 1', $term['tid'])));
  if($parent != 0){
    return array_pop(db_fetch_array(db_query('SELECT lft FROM {leftandright} WHERE tid = %d', $parent))) + 1;
  } else {
    // We've got a Zero parent, and we're the far left root, so our left value
    // is 1
    return 1;
  }
}