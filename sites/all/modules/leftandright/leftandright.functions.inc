<?php
function leftandright_hook_taxonomy_helper_insert($array){
  // Lets shove the stuff into leftandright.  Tricky bit here is getting the right
  // values
  // If the term is a root term we handle this completely differently
  if($array['parent'] == array(0=>0)){
    // Get the max left and max right off all children, we then check to see if we can slot in over this
    if(array_pop(db_fetch_array(db_query("SELECT COUNT(*) FROM {term_hierarchy} WHERE parent=%d", $array['tid'])))){
      $max_left = array_pop(db_fetch_array(db_query("SELECT lft FROM {leftandright} l,{term_hierarchy} h WHERE l.tid=h.tid AND parent=%d ORDER BY lft ASC LIMIT 1", $array['tid'])));
      $min_right = array_pop(db_fetch_array(db_query("SELECT rgt FROM {leftandright} l,{term_hierarchy} h WHERE l.tid=h.tid AND parent=%d ORDER BY rgt DESC LIMIT 1", $array['tid'])));
      // Check that we can simply use +1 and -1 of these values
      $count = array_pop(db_fetch_array(db_query("SELECT COUNT(*) FROM {leftandright} WHERE rgt<%d AND rgt>%d AND lft>%d AND lft<%d",$max_left,$max_left-1,$min_right,$min_right+1)));
      if($count==0){
        // Count is zero, makes our job easy
        $lft = $max_left-1;
        $rgt = $min_right+1;
      } else {
        // We're going to need a decimal lft and/or rgt
        $min_left = array_pop(db_fetch_array(db_query("SELECT rgt FROM {leftandright} l WHERE rgt<%d ORDER BY rgt DESC LIMIT 1", $max_left)));
        $max_right = array_pop(db_fetch_array(db_query("SELECT lft FROM {leftandright} l WHERE lft>%d ORDER BY lft DESC LIMIT 1", $min_right)));
        
        // Set the decimal left
        $leftdiff=0.1;
        while($max_left-(2*$leftdiff)<$min_left){
          $leftdiff = $leftdiff/10;
        }
        $lft = $max_left - $leftdiff;
        // Set the decimal right
        $rightdiff=0.1;
        while($min_right+(2*$rightdiff)>$max_right){
          $rightdiff = $rightdiff/10;
        }
        $rgt = $min_right + $rightdiff;
        // Give a warning if either diff is low
        if($rightdiff < 0.000001 || $leftdiff < 0.000001){
          // Farp, the tree needs rebuilding - give an error message
          drupal_set_message(t('The tree for this vocabulary needs <a href="!url">rebuilding</a>.', array('!url' => url("leftandright/update/".$array['vid'], array('absolute' => TRUE)))),'error');
        }              
      }            
    } else {
      // term has no children, and no parents, just slap it on the end after all the siblings
      $lft = array_pop(db_fetch_array(db_query("SELECT rgt FROM {leftandright} ORDER BY rgt DESC LIMIT 1")))+1;
      $rgt = $lft+1;
    }
    $parent['depth'] = -1;
  } else {
    // If the term has multiple parents, it gets multiple entries in the table.  Will this work?
    foreach($array['parent'] as $parent){
      // Get the parent info, get direct descendants, and see if there are gaps
      $result = db_query('SELECT lft,rgt,depth FROM {leftandright} WHERE tid=%d', $parent);
      $parent = db_fetch_array($result);
      $result = db_query('SELECT lft,rgt FROM {leftandright} WHERE lft>%d AND rgt<%d AND depth=%d ORDER BY lft',$parent['lft'],$parent['rgt'],$parent['depth']+1);
      $sibling_lefts = array();
      $sibling_rights = array();
      while($row = db_fetch_array($result)){
        $sibling_lefts[] = $row['lft'];
        $sibling_rights[] = $row['rgt'];
      }
      $lft = false;
      $rgt = false;
      for($i=$parent['lft']+1; $i<$parent['rgt']-1; $i++){
        if(!in_array($i,$sibling_lefts) && !in_array($i+1,$sibling_rights)){
          // We've found a gap!
          // Also need to make sure that this isn't within child values
          $num_to_left = 0;
          $num_to_right = 0;
          foreach($sibling_lefts as $left){
            if($left<$i){
              $num_to_left++;
            } else {
              $num_to_right++;
            }
          }
          foreach($sibling_rights as $left){
            if($left<$i){
              $num_to_left++;
            } else {
              $num_to_right++;
            }
          }
          if($num_to_left%2===0 && $num_to_right%2===0){
            drupal_set_message("$i - Num to left: $num_to_left + Num to right: $num_to_right");
            $lft = $i;
            $rgt = $i+1;
            break;
          }
        }
      }
      // if lft and rgt are both false, then we need decimal values
      if(!$lft){
        drupal_set_message("SETTING DECIMAL CHILD");
        // If there are siblings, then place this term between the parent and the first sibling
        if(count($sibling_lefts)){
          $max_right = array_shift($sibling_lefts);
        } else {
          // Else we can place this evenly between the parents
          $max_right = $parent['rgt'];
        }
        //$diff = round(($max_right - $parent['lft'])/3,10);
        $diff=0.1;
        while($parent['lft']+(2*$diff)>$max_right){
          $diff = $diff/10;
        }
        if($diff < 0.000001){
          // Farp, the tree needs rebuilding - give an error message
          drupal_set_message(t('The tree for this vocabulary needs <a href="!url">rebuilding</a>.', array('!url' => url("leftandright/update/".$array['vid'], array('absolute' => TRUE)))),'error');
        }
        $rgt = $max_right - $diff;
        $lft = $rgt - $diff;
      }
    }
  }
  db_query("INSERT INTO {leftandright} (lft,rgt,tid,vid,depth,lowername) VALUES (%f,%f,%d,%d,%d,LOWER('%s'))",$lft,$rgt,$array['tid'],$array['vid'],$parent['depth']+1,$array['name']);
  // Check to see if we have multiple weights, if so, the tree most likely needs rebuilding (gah!)
  if(array_pop(db_fetch_array(db_query("SELECT COUNT(DISTINCT weight) FROM {term_data} WHERE vid = %d",$array['vid'])))>1){
    // Check to see if the tree has a small number of terms (less than 1000), if so, rebuild        
    if(leftandright_get_tree_count($array['vid'],0)<1000){            
      leftandright_rebuild_tree($array['vid']);
    } else {
      // Else we give a message warning that things may not be as expected
      drupal_set_message(t('You have terms with different weights in your vocabulary, you may need to <a href="@rebuild">rebuild</a> your tree.', array('@rebuild' => url('leftandright/update/'.$array['vid']))));
    }
  }
}