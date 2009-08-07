<?php
function leftandright_hook_taxonomy_helper_insert($array){ 
  // Lets shove the stuff into leftandright.  Tricky bit here is getting the right
  // values;
  
  // Firstly we get the current values of leftandright - these will be used
  // for updating children of this term
  $result = db_query("SELECT lft,rgt,depth FROM leftandright WHERE tid = %d", $array['tid']);
  if($result){
    $row = db_fetch_array($result);
    $current_left = $row['lft'];
    $current_right = $row['rgt'];
    $current_depth = $row['depth'];
  }
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
      $max_right_result_array = db_fetch_array(db_query("SELECT lft FROM leftandright l,term_lowername t WHERE l.tid=t.tid AND lft > %f AND rgt < %f AND depth = %d AND lowername > LOWER('%s') ORDER BY lft ASC LIMIT 1",$parent['lft'],$parent['rgt'],$parent['depth']+1,$array['name']));
      $min_left_result_array = db_fetch_array(db_query("SELECT rgt FROM leftandright l,term_lowername t WHERE l.tid=t.tid AND lft > %f AND rgt < %f AND depth = %d AND lowername < LOWER('%s') ORDER BY lft DESC LIMIT 1",$parent['lft'],$parent['rgt'],$parent['depth']+1,$array['name']));
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
        if($diff < 0.0000000001){
          drupal_set_message(t('The tree for this vocabulary needs <a href="!url">rebuilding</a>.  YOU WILL EXPERIENCE ERRORS OTHERWISE.', array('!url' => url("leftandright/update/".$array['vid'], array('absolute' => TRUE)))),'error');
        }
        else if($diff < 0.00000001){
          // Farp, the tree needs rebuilding - give an error message
          drupal_set_message(t('The tree for this vocabulary needs <a href="!url">rebuilding</a>.', array('!url' => url("leftandright/update/".$array['vid'], array('absolute' => TRUE)))),'error');
        }
        $rgt = $max_right - $diff;
        $lft = $rgt - $diff;
      }
      db_query("INSERT INTO {leftandright} (lft,rgt,tid,vid,depth) VALUES (%f,%f,%d,%d,%d)",$lft,$rgt,$array['tid'],$array['vid'],$parent['depth']+1);
      // Finally we need to update the children - this only needs to be done if
      // we're updating (are current left and right set?)
      if(isset($current_left) && isset($current_right)){
        $depth_change = ($parent['depth'] + 1) - $current_depth; 
        $insert_factor = ($rgt - $lft) / ($current_right - $current_left);
        if($insert_factor < 0.0000000001){
          drupal_set_message(t('The tree for this vocabulary needs <a href="!url">rebuilding</a>.  YOU WILL EXPERIENCE ERRORS OTHERWISE.', array('!url' => url("leftandright/update/".$array['vid'], array('absolute' => TRUE)))),'error');
        }
        else if($insert_factor < 0.00000001){
          // Farp, the tree needs rebuilding - give an error message
          drupal_set_message(t('The tree for this vocabulary needs <a href="!url">rebuilding</a>.', array('!url' => url("leftandright/update/".$array['vid'], array('absolute' => TRUE)))),'error');
        }
        drupal_set_message("Insert factor: $insert_factor - Current left: $current_left - Current right: $current_right - Left: $lft - Right: $rgt");
        db_query("UPDATE {leftandright} SET lft = ((lft - %f) * %f) + %f , rgt = ((rgt - %f) * %f) + %f , depth = depth + %d WHERE lft > %f and rgt < %f", $current_left, $insert_factor, $lft, $current_left, $insert_factor, $lft, $depth_change, $current_left, $current_right);
      }
    }
  }
}