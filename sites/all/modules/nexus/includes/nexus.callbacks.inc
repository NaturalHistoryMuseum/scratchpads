<?php

/** 
 * hook_menu callback; 
 * Wrapper for all ahah / ajax callbacks
 */
function nexus_callback($op, $arg = null){
  $func = 'nexus_callback_' . $op;
  $json = $func($arg);
  drupal_json($json);
}

/** 
 * Edit project form is being submitted (Settings)
 */
function nexus_callback_project_settings_form_submit(){
  module_load_include('inc', 'node', 'node.pages');
  $submitted_form = nexus_callback_ahah_form_submit();
  $output = theme('status_messages');
  return array('status' => TRUE, 'data' => $output, 
    'action' => 'NEXUS.updateProjectCallback', 
    'title' => $submitted_form['form_state']['post']['title']);
}

function nexus_callback_update_project_setting(){
  $nid = $_POST['projectNid'];
  $setting = $_POST['setting'];
  $value = $_POST['value'];
  $node = node_load($nid);
  $node->settings[$setting] = $value;
  node_save($node);
}

/** 
 * Save taxa settings
 */
function nexus_callback_project_taxonomy_form_submit(){
  module_load_include('inc', 'node', 'node.pages');
  $submitted_form = nexus_callback_ahah_form_submit();
  $output = theme('status_messages');
  $output .= nexus_callback_regenerate_matrix_data($submitted_form['form_state']['values']['nid']);
  return array('status' => TRUE, 'data' => $output, 
    'action' => 'NEXUS.updateTaxonomyCallback');
}

/**
 * Rebuild the matrix data and re-initialise the editor
 */
function nexus_callback_regenerate_matrix_data($nid){
  module_load_include('inc', 'nexus', 'includes/nexus.matrix');
  $node = node_load($nid);
  $matrix_editor = nexus_project_get_matrix_data($node);
  $matrix_editor->options['regenerated'] = true;
  $js .= "var options = " . drupal_to_js($matrix_editor->options) . ";\r";
  $js .= "var data = " . drupal_to_js($matrix_editor->data) . ";\r";
  $js .= "var columns = " . matrix_editor_to_js($matrix_editor->columns) . ";\r";
  $js .= "initMatrixEditor(); NEXUS.init({$nid});\n";
  $output = '<script type="text/javascript">' . $js . '</script>';
  return $output;
}

/** 
 * Generic AHAH form submission
 */
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
  return array('form' => $form, 'form_state' => $form_state);
}

/** 
 * Add new state to the character form
 */
function nexus_callback_state_form_submit(){
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  $form['#programmed'] = $form['#redirect'] = FALSE;
  // Skip validation in project issue.
  $form_state['ahah_state_submit'] = TRUE;
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  // Get the new output.
  $states_form = $form['states_wrapper']['states'];
  unset($states_form['#prefix'], $states_form['#suffix']); // Prevent duplicate wrappers.
  $output = drupal_render($states_form);
  return array('status' => TRUE, 'data' => $output, 
    'action' => 'NEXUS.addStateCallback');
}

/** 
 * Remove state
 */
function nexus_callback_delete_state(){
  $state_nid = $_POST['state_nid'];
  $delta = $_POST['delta'];
  $form_build_id = $_POST['form_build_id'];
  // Need to ensure the cached form remains the same as the html form, so remove delta from cache
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form = form_get_cache($form_build_id, $form_state);
  $count = count(element_children($form['states_wrapper']['states']));
  $form['states_count']--;
  unset($form['states_wrapper']['states'][$count]);
  form_set_cache($form_build_id, $form, array());
  return array('status' => TRUE, 'data' => theme('status_messages'));
}

/** 
 * Row selected so return Taxa info
 */
function nexus_callback_get_row_data(){
  $taxa_tid = $_POST['taxa_tid'];
  $project_nid = $_POST['project_nid'];
  $no_tree = $_POST['no_tree'];
  $term = taxonomy_get_term($taxa_tid);
  return array('status' => TRUE, 
    'data' => theme('nexus_row_data', $term, $project_nid, $no_tree));
}

/** 
 * Callback; Inherit states has been selected
 * This term should inherit all controlled states from it's children
 */
function nexus_callback_inherit_states(){
  $taxa_tid = $_POST['taxa_tid'];
  $project_nid = $_POST['project_nid'];
  $taxa_tid_children = nexus_get_children_in_project($project_nid, $taxa_tid);
  $controlled_characters = nexus_get_controlled_characters_in_project($project_nid);
  $count = 0;
  if(count($controlled_characters)){
    foreach($controlled_characters as $character_tid){
      $states_updated = false;
      // Delete any currently selected states
      $currently_selected_states = nexus_get_nodes_at_intersection(array(
        $taxa_tid, $character_tid), array(
        'nexus_controlled_state'));
      if(count($currently_selected_states)){
        continue;
      }
      $selected_states = array();
      foreach($taxa_tid_children as $child_taxa_tid){
        $intersection_nodes = nexus_get_nodes_at_intersection(array(
          $child_taxa_tid, 
          $character_tid), array(
          'nexus_controlled_state'));
        if(count($intersection_nodes)){
          foreach($intersection_nodes as $intersection_node){
            // Use NID for key to remove duplication
            $selected_states[$intersection_node->nid] = $intersection_node->nid;
          }
          foreach($selected_states as $nid => $selected_state){
            nexus_select_state_for_taxa($taxa_tid, $selected_state, $nid);
            $states_updated = true;
          }
          if($selected_states > 1){
            // All inherited characters are OR by default
            nexus_insert_relationship('/', $character_tid, $taxa_tid);
          }
        }
      }
      if($states_updated){
        $count++;
      }
    }
  }
  if($count){
    drupal_set_message(format_plural($count, '@count state updated', '@count states updated'));
  }else{
    drupal_set_message(t('No states were updated'), 'warning');
  }
  $output .= theme('status_messages');
  $output .= nexus_callback_regenerate_matrix_data($project_nid);
  return array('status' => TRUE, 'data' => $output);
}

/** 
 * Get the group form
 */
function nexus_callback_get_group_form(){
  $group_tid = $_POST['group_tid'];
  $project_nid = $_POST['project_nid'];
  return array('status' => TRUE, 
    'data' => drupal_get_form('nexus_group_form', $group_tid, $project_nid), 
    'ahah' => nexus_callback_get_ahah());
}

/** 
 * Save taxa settings
 */
function nexus_callback_group_form_submit(){
  $project_nid = $_POST['project_nid'];
  $submitted_form = nexus_callback_ahah_form_submit();
  $output = theme('status_messages');
  $output .= nexus_callback_regenerate_matrix_data($project_nid);
  return array('status' => TRUE, 'data' => $output, 
    'action' => 'NEXUS.displayDialog');
}

/** 
 * Get the character form
 */
function nexus_callback_get_character_form(){
  $character_tid = $_POST['character_tid'];
  $project_nid = $_POST['project_nid'];
  $output = drupal_get_form('nexus_character_form', $project_nid, $character_tid);
  return array('status' => TRUE, 'data' => $output, 
    'ahah' => nexus_callback_get_ahah());
}

/** 
 * Get all the AHAH js stuff 
 * Used to add AHAH events to dynamic forms
 */
function nexus_callback_get_ahah(){
  $js = drupal_add_js();
  $ahah = array();
  foreach($js['setting'] as $js_setting){
    if(key($js_setting) == 'ahah'){
      $ahah += $js_setting['ahah'];
    }
  }
  return $ahah;
}

/** 
 * Save character
 */
function nexus_callback_character_form_submit(){
  $return = array('status' => true);
  $project_nid = $_POST['project_nid'];
  $character_tid = $_POST['character_tid'];
  $submitted_form = nexus_callback_ahah_form_submit();
  $return['data'] = theme('status_messages');
  $return['action'] = 'NEXUS.saveCharacterCallback';
  $form_state = $submitted_form['form_state'];
  if($form_state['validated']){
    if(!is_numeric($character_tid)){
      // If a new group has been created, we need to add it to the options
      if($form_state['new_group_term']){
        $return['group_tid'] = $form_state['new_group_term']['tid'];
        $return['group_name'] = $form_state['new_group_term']['name'];
      }
      $return['action'] = 'NEXUS.addCharacterCallback';
    }
    $return['data'] .= nexus_callback_regenerate_matrix_data($project_nid);
  }
  return $return;
}

/** 
 * Delete callback - either character or group
 */
function nexus_callback_confirm_delete_form_submit(){
  $form_id = $_POST['form_id'];
  $output = array('status' => true);
  switch($form_id){
    case 'nexus_group_form' :
      $output += §nexus_delete_group();
      break;
    case 'nexus_character_form' :
      $output += nexus_delete_character();
      break;
  }
  $project_nid = $_POST['project_nid'];
  $output['data'] = theme('status_messages');
  $output['data'] .= nexus_callback_regenerate_matrix_data($project_nid);
  return $output;
}

function nexus_callback_update_term_setting(){
  nexus_update_term_setting($_POST['tid'], $_POST['setting'], $_POST['value']);
}

/** 
 * Delete character
 */
function nexus_delete_character(){
  $character_tid = $_POST['character_tid'];
  $nexus_nodes = nexus_get_nodes_at_intersection(array($character_tid));
  // Need to delete the nodes here as hook_taxonomy is called after the node relationships have been removed
  nexus_delete_nodes($nexus_nodes);
  taxonomy_del_term($character_tid);
  // Clear mesage queue
  drupal_get_messages();
  drupal_set_message('Character deleted');
  return array('action' => 'NEXUS.deleteCharacterCallback');
}

/** 
 * Delete character
 */
function nexus_delete_group(){
  $group_tid = $_POST['tid'];
  taxonomy_del_term($group_tid);
  drupal_set_message('Character group deleted');
  return array('action' => 'NEXUS.deleteGroupCallback');
}

/** 
 * Column order has been changed so save to DB
 */
function nexus_callback_reorder_groups(){
  $groups = $_POST['groups'];
  $project_nid = $_POST['project_nid'];
  $active_group = $_POST['active_group'];
  $root_character_tid = nexus_get_project_character_root_tid($project_nid);
  if(count($groups)){
    foreach($groups as $weight => $group){
      $term = taxonomy_get_term($group);
      $term->parent = array($root_character_tid);
      $term->weight = $weight;
      _nexus_callback_save_term($term);
    }
  }
  $output .= nexus_callback_regenerate_matrix_data($project_nid);
  return array('status' => TRUE, 'data' => $output, 
    'active_group' => $active_group);
}

/** 
 * Column order has been changed so save to DB
 */
function nexus_callback_reorder_columns(){
  $characters = $_POST['c'];
  $parent_tid = $_POST['groupID'];
  if(count($characters)){
    foreach($characters as $weight => $character){
      $character_term = taxonomy_get_term($character);
      $character_term->parent = $parent_tid;
      $character_term->weight = $weight;
      _nexus_callback_save_term($character_term);
    }
  }else{
    drupal_set_message('error', 'Saving the character order failed. Please contact your administer.');
  }
  return array('status' => TRUE, 'data' => theme('status_messages'));
}

/** 
 * Update a taxonomy term
 */
function _nexus_callback_save_term($term){
  // When you get term data from taxonomy_get_tree it comes as parents, but you have to save it parent!!!
  if($term->parents){
    $term->parent = $term->parents;
  }
  $term_array = (array)$term;
  taxonomy_save_term($term_array);
}

/** 
 * Set the state for character / taxa
 */
function nexus_callback_save_state(){
  $character_tid = $_POST['character_tid'];
  $taxa_tid = $_POST['taxa_tid'];
  $nids = $_POST['nids'];
  $or = $_POST['or'];
  // Get all the states currently selected
  $current_state_nodes = nexus_get_nodes_at_intersection(array(
    $character_tid, $taxa_tid), array('nexus_controlled_state'));
  if(count($current_state_nodes)){
    foreach($current_state_nodes as $current_state_node){
      // Is this state still selected?
      if($nids[$current_state_node->nid]){
        // Don't bother saving it again
        unset($nids[$current_state_node->nid]);
      }else{ // Unselect it by updating the taxonomy
        $node = node_load($current_state_node->nid);
        unset($node->taxonomy[$taxa_tid]);
        node_save($node);
      }
    }
  }
  if($nids[0] == '?' || $nids[0] == '-'){
    nexus_insert_relationship($nids[0], $character_tid, $taxa_tid);
    return;
  }elseif($or){ // Is this an or relationship
    nexus_insert_relationship('/', $character_tid, $taxa_tid);
  }else{ // It's AND (default), so remove all ORs if they exist
    nexus_delete_relationship($character_tid, $taxa_tid);
  }
  //If there's any nids left, update the taxonomy
  if(count($nids)){
    foreach($nids as $nid){
      nexus_select_state_for_taxa($taxa_tid, $character_tid, $nid);
    }
  }
}

/** 
 * Select a state for a taxa
 */
function nexus_select_state_for_taxa($taxa_tid, $character_tid, $nid){
  $selected_state = node_load($nid);
  $selected_state->taxonomy[$taxa_tid] = taxonomy_get_term($taxa_tid);
  $selected_state->taxonomy[$character_tid] = taxonomy_get_term($character_tid);
  node_save($selected_state);
}

/** 
 * Remove selected states for a taxa
 */
function nexus_deselect_states_for_taxa($taxa_tid, $nid){
  $selected_state = node_load($nid);
  $taxa_term = taxonomy_get_term($taxa_tid);
  unset($selected_state->taxonomy[$taxa_tid]);
  node_save($selected_state);
}

/** 
 * Set free state for character / taxa
 */
function nexus_callback_save_free_state(){
  global $user;
  $character_tid = $_POST['character_tid'];
  $taxa_tid = $_POST['taxa_tid'];
  $body = $_POST['body'];
  $nid = $_POST['nid'];
  $character_term = taxonomy_get_term($character_tid);
  $taxa_term = taxonomy_get_term($taxa_tid);
  if($body == '-' || $body == '?'){
    $current_state_nodes = nexus_get_nodes_at_intersection(array(
      $character_tid, $taxa_tid), array(
      'nexus_free_state'));
    if(count($current_state_nodes)){
      foreach($current_state_nodes as $current_state_node){
        node_delete($current_state_node->nid);
      }
    }
    nexus_insert_relationship($body, $character_tid, $taxa_tid);
  }elseif($nid){ // Is there a nid (does a note already exist)?
    if(!strlen($body)){ // No text so delete the comment
      node_delete($nid);
    }else{
      $node = node_load($nid);
      $node->body = $body;
      node_save($node);
    }
  }else{
    $node = new stdClass();
    $node->type = 'nexus_free_state';
    $node->uid = $user->uid;
    $node->title = t('NEXUS STATE: ') . $taxa_term->name . ' (' . $character_term->name . ')';
    $node->body = $body;
    $character_term = taxonomy_get_term($character_tid);
    $taxa_term = taxonomy_get_term($taxa_tid);
    // Format the taxonomies
    $node->taxonomy = array(
      $character_term->vid = $character_tid, 
      $taxa_term->vid = $taxa_tid);
    node_save($node);
  }
  return array('status' => TRUE, 'data' => theme('status_messages'));
}

/** 
 * Save a note for a character / taxa
 */
function nexus_callback_note_node_form_submit(){
  module_load_include('inc', 'node', 'node.pages');
  $character_tid = $_POST['character_tid'];
  $taxa_tid = $_POST['taxa_tid'];
  $submitted_form = nexus_callback_ahah_form_submit();
  return array('status' => TRUE, 'data' => theme('status_messages'), 
    'action' => 'NEXUS.saveNoteCallback', 
    'op' => $submitted_form['form_state']['op'], 
    'new_note' => $submitted_form['form_state']['new_note'], 
    'character_tid' => $character_tid, 'taxa_tid' => $taxa_tid);
}



