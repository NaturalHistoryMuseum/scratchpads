<?php
function leftandright_hook_taxonomy_helper_insert($array){
  
  
  // Lets shove the stuff into leftandright.  Tricky bit here is getting the right
  // values
  // If the term is a root term we handle this completely differently
  if($array['parent'] == array(0=>0)){
    $lft = array_pop(db_fetch_array(db_query("SELECT rgt FROM {leftandright} ORDER BY rgt DESC LIMIT 1")))+1;
    $rgt = $lft+1;
    db_query("INSERT INTO {leftandright} (lft,rgt,tid,vid,depth) VALUES (%f,%f,%d,%d,%d)",$lft,$rgt,$array['tid'],$array['vid'],0);
  } 
  
  
  else 
  
  
  {
    // If the term has multiple parents, it gets multiple entries in the table.  Will this work?
    if(!is_array($array['parent'])){
      $array['parent'] = array($array['parent']);
    }
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
}