<?php
function leftandright_hook_taxonomy_helper_insert($array){  
  // Lets shove the stuff into leftandright.  Tricky bit here is getting the right
  // values;
  
  // First things first, delete the current values.  I know this is a rather
  // poor way of doing things as updates are better. FIXME  
  db_query('DELETE FROM {leftandright} WHERE tid=%d',$array['tid']);
  
  if($array['parent'] == array(0=>0)){
    // If the term has no parents, just shove it on the end - Note, this won't be
    // alphabetical
    $lft = array_pop(db_fetch_array(db_query("SELECT rgt FROM {leftandright} ORDER BY rgt DESC LIMIT 1")))+1;
    $rgt = $lft+1;
    db_query("INSERT INTO {leftandright} (lft,rgt,tid,vid,depth) VALUES (%f,%f,%d,%d,%d)",$lft,$rgt,$array['tid'],$array['vid'],0);
  } else {
    // The term has at least one parent lets run through them all
    if(!is_array($array['parent'])){
      $array['parent'] = array($array['parent']);
    }
    foreach($array['parent'] as $parent){
      // We need to place this term at the right place below the parents, lets
      // find the direct descendants of this parent directly before and after
      // the term that we're trying to insert      
      // We've already deleted the current values for this term from the
      // database which should make things a little simpler
      // Get the parent info, get direct descendants, and see if there are gaps
      $result = db_query('SELECT lft,rgt,depth FROM {leftandright} WHERE tid=%d', $parent);
      $parent = db_fetch_array($result);
      $max_right_result_array = db_fetch_array(db_query("SELECT lft FROM leftandright l,term_lowername t WHERE l.tid=t.tid AND lft > %d AND rgt < %d AND depth = %d AND lowername > LOWER('%s') ORDER BY lft ASC LIMIT 1",$parent['lft'],$parent['rgt'],$parent['depth']+1,$array['name']));
      $min_left_result_array = db_fetch_array(db_query("SELECT rgt FROM leftandright l,term_lowername t WHERE l.tid=t.tid AND lft > %d AND rgt < %d AND depth = %d AND lowername < LOWER('%s') ORDER BY lft DESC LIMIT 1",$parent['lft'],$parent['rgt'],$parent['depth']+1,$array['name']));
      if($min_left_result_array){
        $min_left = array_pop($min_left_result_array);
      } else {
        $min_left = $parent['lft'];
      }
      if($max_right_result_array){
        $max_right = array_pop($max_right_result_array);
      } else {
        $max_right = $parent['rgt'];
      }
      if($max_right-$min_left>=3){
        $lft = round($min_left+1);
        $rgt = round($max_right-1);
      } else {
        $diff = round(($max_right - $min_left)/3,10);
        if($diff < 0.00000001){
          // Farp, the tree needs rebuilding - give an error message
          drupal_set_message(t('The tree for this vocabulary needs <a href="!url">rebuilding</a>.', array('!url' => url("leftandright/update/".$array['vid'], array('absolute' => TRUE)))),'error');
        }
        if($diff < 0.0000000001){
          drupal_set_message(t('The tree for this vocabulary needs <a href="!url">rebuilding</a>.  YOU WILL EXPERIENCE ERRORS OTHERWISE.', array('!url' => url("leftandright/update/".$array['vid'], array('absolute' => TRUE)))),'error');
        }
        $rgt = $max_right - $diff;
        $lft = $rgt - $diff;
      }
      db_query("INSERT INTO {leftandright} (lft,rgt,tid,vid,depth) VALUES (%f,%f,%d,%d,%d)",$lft,$rgt,$array['tid'],$array['vid'],$parent['depth']+1);
    }
  }
  // Finally we need to move the children, we can do this by calling this very
  // function - YEAH!
  // Lets get the direct descendants of this term
  $children = taxonomy_get_children($array['tid']);
  foreach($children as $child){
    // Need to call this function with $array set with tid,vid and parent values
    // parent should be easy enough, as it's the current term (we DO NOT need to 
    // worry about multiple parents here - WOOT!)
    $child_array = array(
      'tid' => $child->tid,
      'vid' => $child->vid,
      'parent' => $array['tid']    
    );
    leftandright_hook_taxonomy_helper_insert($child_array);
  }
}