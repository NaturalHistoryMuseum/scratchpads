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
        'leftandright_rebuild_tree', array($vid, 20)
      );
      $operations[] = array(
        'leftandright_move_rebuilt_tree', array($vid)
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
 * Move the rebuilt tree into the leftandright table
 */
function leftandright_rebuild_finished($success, $results, $operations){
  if($success){
    drupal_set_message(t('Your vocabulary has been rebuilt'));
  } else {
    drupal_set_message(t('Your vocabulary has <strong>not</strong> been rebuilt'), 'error');    
  }
}

/**
 * Move the rebuilt tree
 */
function leftandright_move_rebuilt_tree($vid){
  db_query('INSERT INTO {leftandright} (tid, vid, lft, rgt, depth) SELECT l.tid,l.vid,lft,rgt,depth FROM {leftandright_rebuild_left} l, {leftandright_rebuild_right} r WHERE l.tid = r.tid AND l.vid = %d', $vid);
}

/**
 * Heavy lifting function, does the rebuilding.
 */
function leftandright_rebuild_tree($vid, $count, &$context){
  if(!isset($context['sandbox']['max'])){
    $context['sandbox'] = array(
      'progress' => 1,
      'max' => db_result(db_query('SELECT COUNT(*)*2 FROM {term_data} WHERE vid = %d', $vid))+1
    );
    // While we're at it, we'll wipe the previous tree, and initialise the
    // tree
    db_query('DELETE FROM {leftandright} WHERE vid = %d', $vid);
    db_query('DELETE FROM {leftandright_rebuild_left} WHERE vid = %d', $vid);
    db_query('DELETE FROM {leftandright_rebuild_right} WHERE vid = %d', $vid);
  }
  if($context['sandbox']['progress']>1){
    $result = db_query('SELECT * FROM (SELECT lft as leftright, tid, depth, \'LEFT\' AS leftorright FROM {leftandright_rebuild_left} WHERE vid = %d UNION SELECT rgt AS leftright, r.tid, depth, \'RIGHT\' AS leftorright FROM {leftandright_rebuild_left} l, {leftandright_rebuild_right} r WHERE l.tid = r.tid AND l.vid = %d) AS something ORDER BY leftright DESC LIMIT 1', $vid, $vid);
    $row = db_fetch_array($result);
    if($row['leftorright'] == 'RIGHT'){
      // We finished with this term, we need to move up or along (not down).
      $siblings = _leftandright_get_sibling_tids($row['tid'], $vid);
      if(count($siblings)){
        $depth = $row['depth'];
        $tid = array_shift($siblings);
        $left = TRUE;
      } else {
        // We need to move up
        $ancestors = _leftandright_get_ancestors_as_array_of_tids($row['tid']);
        array_shift($ancestors); // Remove this term
        $tid = array_shift($ancestors);
        $depth = $row['depth'] - 1;
        $left = FALSE; 
      }
    } else {
      $depth = $row['depth'];
      $tid = $row['tid'];
      $left = FALSE;
    }
  } else {
    // Get the root tid as the left
    $tid = db_result(db_query('SELECT t.tid FROM {term_data} t, {term_hierarchy} h WHERE t.tid = h.tid AND parent = 0 AND vid = %d ORDER BY weight ASC, name', $vid));
    $left = TRUE;
    $depth = 0;
  }
  global $stop_at_leftright;
  global $leftright;
  global $max;
  global $original_tid_call_ancestors;
  $original_tid_call_ancestors = _leftandright_get_ancestors_as_array_of_tids($tid);
  $max = $context['sandbox']['max'];
  $stop_at_leftright = $context['sandbox']['progress'] + $count;
  $leftright = $context['sandbox']['progress']; 
  
  // Build the part
  _leftandright_rebuild_tree_part($vid, $tid, $depth, $left);
  
  $context['sandbox']['progress'] = $leftright;
  $context['finished'] = $context['sandbox']['progress']/$context['sandbox']['max'];
}

function _leftandright_rebuild_tree_part($vid, $tid, $depth, $left = TRUE){//, $from_sibling = FALSE){
  // Simply walk through a tree, getting the children, followed by parents so
  // that we can set the leftandright values.
  global $stop_at_leftright;
  global $leftright;
  global $max;
  global $original_tid_call_ancestors;
  
  // Lets make sure that we've not equalled the max, or that we've passed the
  // stop point.
  if($leftright > $stop_at_leftright || $leftright > $max){
    // Tell the parent calls to stop
    return FALSE; 
  }
  if($left){
    // Else we're safe to continue, lets get the term we're working on, and set
    // the left value
    db_query('INSERT INTO {leftandright_rebuild_left} (tid, lft, depth) VALUES (%d, %d, %d)', $tid, $leftright, $depth);
    $leftright++;
  }
  
  // Next get the children, and call this function for them
  $children = _leftandright_get_children_tids($tid);
  foreach($children as $child){
    if(!_leftandright_rebuild_tree_part($vid, $child, $depth+1)){
      return FALSE;
    }
  }
  
  // Set the right value
  db_query('INSERT INTO {leftandright_rebuild_right} (tid, rgt) VALUES (%d, %d)', $tid, $leftright);
  $leftright++;
  
  // Now we need to move back up the tree.  We can do this by either returning
  // to the function that called us, or if this is the tid (or one of its
  // ancestors) that was originally called, then we'll need to call the 
  // function but with left set to false.
  if(isset($original_tid_call_ancestors[$tid])){
    watchdog('batch', 'We"re using ancestors');
    // We're an ancestor, and we're moving up, we can use the ancestors array
    // to work out the call
    $current_found = FALSE;
    foreach($original_tid_call_ancestors as $ancestor){
      if($current_found){
        _leftandright_rebuild_tree_part($vid, $ancestor, $depth-1, FALSE);
      }
      if($ancestor == $tid){
        $current_found = TRUE;
      }
    }
  }
  // Move back up the tree
  return TRUE;
}

function _leftandright_get_ancestors_as_array_of_tids($tid){
  $ancestors = taxonomy_get_parents_all($tid);
  $ancestors_tids = array();
  foreach($ancestors as $ancestor){
    $ancestors_tids[$ancestor->tid] = $ancestor->tid;
  }
  return $ancestors_tids;
}

function _leftandright_get_children_tids($tid){
  $result = db_query('SELECT t.tid FROM {term_data} t, {term_hierarchy} h WHERE t.tid = h.tid AND parent = %d AND t.tid NOT IN (SELECT tid FROM {leftandright_rebuild_right}) ORDER BY weight ASC, name', $tid);
  $children = array();
  while($tid = db_result($result)){
    $children[] = $tid;
  }
  return $children;
}

function _leftandright_get_sibling_tids($tid, $vid){
  // This function only returns siblings AFTER the current term
  $result = db_query('SELECT t.tid FROM {term_data} t, {term_hierarchy} h WHERE parent = (SELECT parent FROM {term_hierarchy} WHERE tid = %d) AND vid = %d AND t.tid = h.tid ORDER BY weight ASC, name', $tid, $vid);
  $siblings = array();
  while($sibling = db_result($result)){
    if($this_tid_reached){
      $siblings[] = $sibling;
    }
    if($tid == $sibling){
      $this_tid_reached = TRUE;
    }
  }
  return $siblings;
}