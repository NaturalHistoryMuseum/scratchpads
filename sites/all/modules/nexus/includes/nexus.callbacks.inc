<?php

/** 
* menu callback; wrapper for all ahah / ajax callbacks
*/
function nexus_callback($op){
  
  $func = 'nexus_callback_'.$op;
  $json = $func();
  
  // switch($op){
  //   
  //   case 'add_state':
  // 
  //     $json = nexus_callback_add_state();
  //       
  //   break;
  //   
  //   case 'save_note':
  // 
  //     $json = nexus_callback_save_note();
  //       
  //   break;
  //   
  //   case 'set_state':
  // 
  //     $json = nexus_callback_set_state();
  //       
  //   break;
  //   
  //   case 'set_free_state':
  //   
  //     $json = nexus_callback_set_free_state();
  //   
  //   break;
  //   
  //   case 'reorder_columns':
  //   
  //     $json = nexus_callback_reorder_columns();
  //   
  //   break;
  //   
  //   case 'get_row_data':
  //   
  //     $json = nexus_callback_get_row_data();
  //   
  //   break;
  //   
  //   case 'get_character_form':
  //   
  //     $json = nexus_callback_get_character_form();
  //   
  //   break;   
  //   
  //   case 'get_character_form':
  //   
  //     $json = nexus_callback_save_character();
  //   
  //   break; 
  //   
  //   case 'toggle_display':
  //   
  //     $json = nexus_callback_toggle_display();
  //   
  //   break;
  // 
  //   case 'update_project_settings':
  //   
  //     $json = nexus_callback_update_project_settings();
  //   
  //   break;
  //   
  //   case 'update_project_taxa':
  //   
  //     $json = nexus_callback_update_project_taxa();
  //   
  //   break;
  //   
  //   case 'get_taxonomy':
  //   
  //     $json = nexus_callback_get_taxonomy();
  //   
  //   break;
  //   
  // }
  
  drupal_json($json);
  
}

function nexus_callback_get_taxonomy(){

  include_once 'modules/node/node.pages.inc';
  
   $form_state = array('storage' => NULL, 'submitted' => TRUE);
   $form_build_id = $_POST['form_build_id'];
   $form = form_get_cache($form_build_id, $form_state);

   // Preparing for #5.
   $args = $form['#parameters'];
   $form_id = array_shift($args);
   $form_state['post'] = $form['#post'] = $_POST;
   $form['#programmed'] = $form['#redirect'] = FALSE;

   // Step #5.
   drupal_process_form($form_id, $form, $form_state);
   // Step #6 and #7 and #8.
   $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

   return array('status' => TRUE, 'data' => 'HI');

}


/** 
* Edit project form is being submitted (Settings)
*/
function nexus_callback_update_project_settings(){
  
  module_load_include('inc', 'node', 'node.pages');

  $form_state = nexus_callback_ahah_form_submit();
    
  $output = theme('status_messages');

  $output .= '<script>NEXUS.updateTitle("'.$form_state['post']['title'].'")</script>';
   
  return array('status' => TRUE, 'data' => $output);
  
}

function nexus_callback_update_project_taxa(){
  
  module_load_include('inc', 'node', 'node.pages');
  module_load_include('inc', 'nexus', 'includes/nexus.matrix');

  $form_state = nexus_callback_ahah_form_submit();    
  $output = theme('status_messages');
  
  $output .= nexus_regenerate_matrix_data($form_state['values']['nid']);
     
  return array('status' => TRUE, 'data' => $output);
  
}

function nexus_regenerate_matrix_data($nid){
  
  $node = node_load($nid);
  
  $matrix_editor = nexus_project_get_matrix_data($node);

  $output = '<script>var data = '.drupal_to_js($matrix_editor->data).'; initMatrixEditor()</script>';
  
  return $output;
  
}



function nexus_callback_ahah_form_submit(){
  
   $form_state = array('storage' => NULL, 'submitted' => FALSE);
   $form_build_id = $_POST['form_build_id'];
   $form = form_get_cache($form_build_id, $form_state);
   $args = $form['#parameters'];
   $form_id = array_shift($args);
   $form_state['post'] = $form['#post'] = $_POST;
   $form['#programmed'] = $form['#redirect'] = FALSE;

   drupal_process_form($form_id, $form, $form_state);
   $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
   
   return $form_state;
  
}

function nexus_callback_add_state(){
  
  module_load_include('inc', 'nexus', 'includes/nexus.forms');
  
  nexus_callback_ahah_form_submit();
    
  // Renexusr the new output.
  $states_form = $form['states_wrapper']['states'];
  unset($states_form['#prefix'], $states_form['#suffix']); // Prevent duplicate wrappers.
  $output = theme('status_messages') . drupal_render($states_form);

  return array('status' => TRUE, 'data' => $output);

}


function nexus_callback_toggle_display(){
  
  if(count($_POST['tids'])){
  
    switch($_POST['op']){
    
      case 'show': // Show items
      
        $sql =  'DELETE FROM nexus_hidden_terms WHERE nid = %d AND tid = %d';
        
      break;
    
      case 'hide': // Hide items

        $sql =  'INSERT INTO nexus_hidden_terms (nid, tid) VALUES (%d, %d)';
    
      break;
    
    }
    
    foreach($_POST['tids'] as $tid){

      $args = array($_POST['nid'], $tid);

      db_query($sql, $args);

    }  
  
  }

}



function nexus_callback_get_row_data(){
  
  $tid = $_POST['tid'];

  $term = taxonomy_get_term($tid);

  return array(
    'status' => TRUE,
    'data' => theme('nexus_row_data', $term) 
  );
  
}

function nexus_callback_get_character_form(){
  
  module_load_include('inc', 'nexus', 'includes/nexus.forms');
  
  $tid = $_POST['tid'];
  $nid = $_POST['nid'];
  
  $output = drupal_get_form('nexus_character_form', $nid, $tid);

  return array(
    'status' => TRUE,
    'data' => $output
  );
  
}

function nexus_callback_save_character(){
  
  module_load_include('inc', 'nexus', 'includes/nexus.forms');
  
  $tid = $_POST['tid'];
  $nid = $_POST['nid'];

  return array(
    'status' => TRUE,
    'data' => theme('status_messages')
  );
  
}




function nexus_callback_reorder_columns(){
  
  $tid = $_POST['tid'];
  
  if(is_numeric( $_POST['prev'])){
    $prev = $_POST['prev'];
  }
  
  $next = $_POST['next'];
  
  $moved_term_parent = array_shift(taxonomy_get_parents($tid));
  
  
  if($prev){
    
    $previous_term_parent = array_shift(taxonomy_get_parents($prev));

    if($previous_term_parent->tid == $moved_term_parent->tid){
      
      $parent_tid = $moved_term_parent->tid;
      
    }
    
  }
  
  if(!$parent_tid && $next){
    
    
    $next_term_parent = array_shift(taxonomy_get_parents($next));

    if($next_term_parent->tid == $moved_term_parent->tid){
      
      $parent_tid = $moved_term_parent->tid;
      
    }
    
  }

  
  
  if(!$parent_tid){

    if($prev){ // If it doesn't and there's a previous term, use that
      
      $parent_tid = $previous_term_parent->tid;
      $adjacent_tid = $prev;
    
    }elseif($next){
    
      $parent_tid = $next_term_parent->tid;
      $adjacent_tid = $next;
    
    }
    
  }
  
  if($parent_tid){
    
    $moved_term = taxonomy_get_term($tid);
    $moved_term->parent[0] = $parent_tid;
    
    // Get the new part of the tree
    $tree = taxonomy_get_tree($moved_term->vid, $parent_tid);

    // Loop through the tree changing all the weights

    if($prev){ // Are there terms previous to this one?

      foreach($tree as $term){
        
          if($term->tid == $prev){
            
            $moved_term->weight = $term->weight + 1;
            nexus_callback_update_weight($moved_term);
          
          }elseif($moved_term_weight){
          
            $term->weight = $term->weight + 2;
            nexus_callback_update_weight($term);
          
          }

      }

    }else{
 
      $moved_term->weight = 0;
      nexus_callback_update_weight($moved_term);
      
      foreach($tree as $term){
      
        if($term->tid != $tid){
        
          $term->weight++;
          nexus_callback_update_weight($term);
        
        }
      
      }

    }
    
  }else{
    
    drupal_set_message('error', 'Saving the character order failed. Please contact your administer.');
    
  }
    
  return array(
    'status' => TRUE,
    'data' => theme('status_messages'),
    'settings' => array(
      'tid' => $tid, 
      'adjacentTid' => $adjacent_tid
    )
  );
  
}

function nexus_callback_update_weight($term, $weight = null){
  
  
  // When you get term data from taxonomy_get_tree it comes as parents, but you have to save it parent!!!
  if($term->parents){
    $term->parent = $term->parents;
  }

  $term_array = (array) $term;
  
  taxonomy_save_term($term_array);
  
}

function nexus_callback_set_state(){
  
  $tids = array($_POST['character_tid'], $_POST['taxa_tid']);
  
  $current_state_nodes = nexus_get_nodes_at_intersection($tids, array('nexus_state'));

  if(count($current_state_nodes)){
    
    foreach($current_state_nodes as $current_state_node){
       
      $node = node_load($current_state_node->nid);
      
      unset($node->taxonomy[$_POST['taxa_tid']]);
      
      node_save($node);
      
    }
    
  }
  
  //If an option has been selected, load the option node, and save it with the taxa term
  if($_POST['nid']){
    
    $new_state_node = node_load($_POST['nid']);
    
    $taxa_term = taxonomy_get_term($_POST['taxa_tid']);

    $new_state_node->taxonomy[$_POST['taxa_tid']] = $taxa_term;
    
    node_save($new_state_node);
    
  }
  
}


function nexus_callback_set_free_state(){
  
  global $user;

  if($_POST['nid']){ // Is there a nid (does a note already exist)?
    
    if(!strlen($_POST['title'])){ // No text so delete the comment
      
      node_delete($_POST['nid']);
      
    }else{
      
      $node = node_load($_POST['nid']);
      $node->title = $_POST['title'];
      node_save($node);
      
    }
    
  }else{
    
    $node = (object)$_POST;
    
    $character_term = taxonomy_get_term($node->character_tid);
    $taxa_term = taxonomy_get_term($node->taxa_tid);

    $node->title = $_POST['title'];
    $node->type = 'nexus_free_state';
    $node->uid = $user->uid;
    
    // Format the taxonomies
    $node->taxonomy = array(
      $character_term->vid = $node->character_tid,
      $taxa_term->vid = $node->taxa_tid,
    );
    
    node_save($node);
    
  }
  
  return array(
    'status' => TRUE
  );

  
}



function nexus_callback_save_note(){
  
  global $user;
  
  if($_POST['nid']){ // Is there a nid (does a note already exist)?
    
    $nid = $_POST['nid'];
    
    if(!strlen($_POST['body'])){ // No text so delete the comment
      
      node_delete($_POST['nid']);
      $output = t('Note deleted');
      $op = 'delete';
      
    }else{
      
      $node = node_load($nid);
      $node->body = $_POST['body'];
      node_save($node);
      
      $output = t('Note saved');
      $op = 'updated';
      
    }
    
  }else{
    
    $node = (object)$_POST;
    
    $character_term = taxonomy_get_term($node->character_tid);
    $taxa_term = taxonomy_get_term($node->taxa_tid);

    $node->title = 'NOTE: '.$taxa_term->name.' ('.$character_term->name.')';

    // Format the taxonomies
    $node->taxonomy = array(
      $character_term->vid = $node->character_tid,
      $taxa_term->vid = $node->taxa_tid,
    );
    
    $node->uid = $user->uid;
    
    node_save($node);
    
    $nid = $node->nid;
    
    $output = t('Note saved');
    $op = 'add';
    
  }
  
  return array(
    'status' => TRUE, 
    'data' => $output, 
    'action' => 'nexusAhahCallbackNoteUpdated',
    'op' => $op, 
    'nid' => $nid, 
    'characterTid' => $_POST['character_tid'], 
    'taxaTid' => $_POST['taxa_tid']
  );

  
}
