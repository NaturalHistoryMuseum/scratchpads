<?php
/**
 * rebuild form
 */
function leftandright_rebuild_form($form_state){
  $vocabularies = taxonomy_get_vocabularies();
  $form['vocabularies'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Vocabulary to rebuild'),
    '#description' => t('Select which vocabularies you would like to rebuild, and then click Rebuild')
  );  
  foreach ($vocabularies as $vocabulary) {
    $form['vocabularies']['#options'][$vocabulary->vid] = check_plain($vocabulary->name);
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  return $form;
}

/**
 * Ensure a vocabulary has been selected
 */
function leftandright_rebuild_form_validate($form, $form_state){
  foreach($form_state['values']['vocabularies'] as $vid => $set){
    if($set){
      return;
    }
  }
  form_set_error('vocabularies', t('Please select a vocabulary'));
}

/**
 * Submit the rebuild form, and start the batch.
 *
 * @param unknown_type $form
 * @param unknown_type $form_state
 */
function leftandright_rebuild_form_submit($form, $form_state){
  // Lets 
  $operations = array();
  foreach($form_state['values']['vocabularies'] as $key => $vid){
    if($vid){
      $operations[] = array(
        'leftandright_rebuild_tree', array($vid, 2000)
      );
    }
  }
  $batch = array(
    'operations' => $operations,
    'finished' => 'leftandright_rebuild_finished',
    'title' => t('Rebuilding Vocabularies'),
    'init_message' => t('The rebuilding of your vocabularies is starting.'),
    'progress_message' => t('Written record @current of @total'),
    'error_message' => t('Something went wrong, please contact an administrator'),
    'file' => drupal_get_path('module','leftandright').'/leftandright.rebuild.inc'
  );
  batch_set($batch);
}

/**
 * Heavy lifting function, does the rebuilding.
 */
function leftandright_rebuild_tree($vid, $count, &$context){
  if(!isset($context['sandbox']['max'])){
    $context['sandbox'] = array(
      'progress' => 0,
      'max' => db_result(db_query('SELECT COUNT(*)*2 FROM {term_data} WHERE vid = %d', $vid))
    );
    // While we're at it, we'll wipe the previous tree, and initialise the
    // tree
    db_query('DELETE FROM {leftandright} WHERE vid = %d', $vid);
    db_query('DELETE FROM {leftandright_rebuild_left} WHERE tid IN (SELECT tid FROM {term_data} WHERE vid = %d) OR tid = 0', $vid); // Remove the = 0
    db_query('DELETE FROM {leftandright_rebuild_right} WHERE tid IN (SELECT tid FROM {term_data} WHERE vid = %d) OR tid = 0', $vid); // Remove the = 0
  }
  if($context['sandbox']['progress']){
    $tid = db_result(db_query('SELECT tid FROM {leftandright_rebuild_left} WHERE lft = %d UNION SELECT tid FROM {leftandright_rebuild_right} WHERE rgt = %d', $context['sandbox']['progress']-1, $context['sandbox']['progress']-1));
    watchdog('batch','Second run tid is: '.$tid);
  } else {
    $tid = 0;
  }
  $context['sandbox']['progress'] = _leftandright_get_tree_part($vid, $tid, $count, $context['sandbox']['progress'], $context['sandbox']['max']);
  if($context['sandbox']['progress']){
    $context['finished'] = $context['sandbox']['progress']/$context['sandbox']['max'];
  } else {
    $context['sandbox']['progress'] = 0;
  }
}

function _leftandright_get_tree_part($vid, $tid, $count, $leftright, $max){ // CHANGE NAME - FIXME!
  // This function gets the parts of a tree after a term (going back up the tree
  // if required).
  $tid_lft_values = array();
  $tid_rgt_values = array();
  if($tid){
    // We've got a $tid, we need a depth
    $depth = db_result(db_query('SELECT depth FROM {leftandright_rebuild_left} WHERE tid = %d', $tid));
    // Also populate the branches array
    $branches = _leftandright_add_parents_to_array(array(), $tid, $vid);
    $branches = _leftandright_add_children_to_array($branches, $tid, $vid);
    // Set $tid_lft_values and $tid_rgt_values with previous highest entry
    $tid_lft_values[$tid] = db_fetch_array(db_query('SELECT tid, lft, depth, 1 AS readyset FROM {leftandright_rebuild_left} ORDER BY lft DESC LIMIT 1'));
    $tid_rgt_values[$tid] = db_fetch_array(db_query('SELECT tid, rgt, 1 AS readyset FROM {leftandright_rebuild_right} ORDER BY rgt DESC LIMIT 1'));
  } else {
    // We've got zero, we're at the start, depth = 0, and lft = 1    
    $branches = _leftandright_add_parents_to_array(array(), $tid, $vid);
    $tid = $branches[0][0];
    $branches = _leftandright_add_children_to_array($branches, $tid, $vid);
    $depth = 0;
  }
  // We've now got a branch which we can work with, lets move down through it 
  // (and along and up) setting the lft and rgt values as we go.
  
  if($leftright == 0){
    $tid_lft_values[$branches[0][0]] = array('tid' => $tid, 'lft' => 1, 'depth' => 0);
    $leftright = 2;
  }
  $nextstepup = $leftright + $count;
  while($leftright <= $max && $leftright < $nextstepup){
    if(isset($branches[$depth+1])){
      // We're moving down the array
      if(!isset($tid_lft_values[$branches[$depth+1][0]])){
        $tid_lft_values[$branches[$depth+1][0]] = array('tid' => $branches[$depth+1][0], 'lft' => $leftright, 'depth' => $depth+1);
      }
      $depth ++;
      $leftright ++;
    } elseif(isset($branches[$depth])) {
      // We're moving along, lets set the right value, and then pop the tid from
      // the array
      $tid = array_shift($branches[$depth]);
      // Set left first
      if(!isset($tid_lft_values[$tid])){
        $tid_lft_values[$tid] = array('tid' => $tid, 'lft' => $leftright, 'depth' => $depth);
        $leftright ++;
      }
      $tid_rgt_values[$tid] = array('tid' => $tid, 'rgt' => $leftright);
      $leftright ++;
      // If there are other tids on this level, then we'll need to get the 
      // children of the next tid
      if(count($branches[$depth])){
        $branches = _leftandright_add_children_to_array($branches, $branches[$depth][0]);
      } else {
        // else we'll need to move up on the next cycle
        unset($branches[$depth]);
        $depth --;
      }
    } else {
      $depth --;
      if(!is_array($branches[$depth])){
        watchdog('batch','Branch is not an array at depth '.$depth.': '.print_r($branches));
        exit;
      }
      $tid = array_shift($branches[$depth]);
      $tid_rgt_values[$tid] = $leftright;
      $leftright ++;
      // If there are other tids on this level, then we'll need to get the 
      // children of the next tid
      if(count($branches[$depth])){
        $branches = _leftandright_add_children_to_array($branches, $branches[$depth][0]);
      } else {
        // else we'll need to move up on the next cycle
        unset($branches[$depth]);
        $depth --;
      }
    }
  }
  // Save the stuff to the leftandright_rebuild_x tables
  if(count($tid_lft_values)){
    $lft_placeholders = implode(',', array_fill(0, count($tid_lft_values), '(%d,%d,%d)'));
    foreach($tid_lft_values as $tid => $values){
      if(!isset($values['readyset'])){
        foreach($values as $value){
          $lft_values[] = $value;
        }
      }
    }
    //watchdog('batch', print_r($lft_values, TRUE));
    //watchdog('batch', print_r($lft_placeholders, TRUE));
    db_query('INSERT INTO {leftandright_rebuild_left} (tid,lft,depth) VALUES '.$lft_placeholders, $lft_values);
  }
  if(count($tid_rgt_values)){
    $rgt_placeholders = implode(',', array_fill(0, count($tid_rgt_values), '(%d,%d)'));
    foreach($tid_rgt_values as $tid => $values){
      if(!isset($values['readyset'])){
        foreach($values as $value){
          $rgt_values[] = $value;
        }
      }
    }
    db_query('INSERT INTO {leftandright_rebuild_right} (tid,rgt) VALUES '.$rgt_placeholders, $rgt_values);
  }
  /*watchdog('batch',$leftright);
  exit;*/
  return $leftright;
}

function _leftandright_add_parents_to_array($branches, $tid, $vid){
  if(!$tid){
    $result = db_query('SELECT t.tid FROM {term_hierarchy} h, {term_data} t  WHERE parent = 0 AND t.tid = h.tid AND vid = %d ORDER BY weight ASC, name', $vid);
    while($branch_tid = db_result($result)){
      $branches[0][] = $branch_tid;
    }
    return $branches;
  }
  // We also need to populate the array above this term, so we'll do that here
  $ancestors = taxonomy_get_parents_all($tid);
  $ancestors = array_reverse($ancestors);
  $root = new stdClass();
  $root->tid = 0;
  array_unshift($ancestors, $root);
  $depth = 0;
  foreach($ancestors as $ancestor){
    $ancestor_reached = FALSE;
    if($ancestor->tid == $tid){
      break;
    }
    $result = db_query('SELECT t.tid FROM {term_hierarchy} h, {term_data} t  WHERE parent = %d AND t.tid = h.tid AND vid = %d AND t.tid NOT IN (SELECT tid FROM {leftandright_rebuild_right}) ORDER BY weight ASC, name', $ancestor->tid, $vid);  
    while($branch_tid = db_result($result)){
      if($branch_tid == $ancestors[$depth+1]->tid){
        $ancestor_reached = TRUE;
      }
      if($ancestor_reached){
        $branches[$depth][] = $branch_tid;
      }
    }
    $depth ++;
  }
  return $branches;
}

function _leftandright_add_children_to_array($branches, $tid){
  // We don't want to add anything that is already in the 
  // leftandright_rebuild_right table, so lets not!
  $depth = count($branches);
  $result = db_query('SELECT t.tid FROM {term_data} t INNER JOIN {term_hierarchy} h ON h.tid = t.tid WHERE parent = %d AND t.tid NOT IN (SELECT tid FROM {leftandright_rebuild_right}) ORDER BY weight, name', $tid);
  while ($additional_tid = db_result($result)){
    $branches[$depth][] = $additional_tid;
  }
  if(isset($branches[$depth])){
    $branches = _leftandright_add_children_to_array($branches, $branches[$depth][0]);
  }
  return $branches;
}
