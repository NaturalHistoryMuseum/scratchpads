<?php

DEFINE('SHORT_NAME_CHAR_LENGTH', 30); // Number of characters to shorten groups / character names to

/**
 * Prepare the matrix data
 */
function nexus_project_get_matrix_data($node, $editable = true) {
  
  $matrix_editor = new stdClass();
  
  // Define filter
  $matrix_editor->filter = 'nexusFilter';
  
  // Define options  
  $matrix_editor->options = array(
    'editable' => $editable, 
    'enableAddRow' => false, 
    'enableCellNavigation' => $editable, 
    'asyncEditorLoading' => false, 
    'fixedFirstColumn' => true, 
    'enableColumnReorder' => FALSE, 
    'viewportHeight' => ($node->settings['height'] ? $node->settings['height'] : 300), 
    'noTree' => false
  );
  
  // Prepare columns
  $matrix_editor->columns = array();
  
  // All the taxa terms in this vocabulary attached to the project node
  $taxa_vocabulary = taxonomy_vocabulary_load($node->taxa_vid);
  
  // First column is for the taxa
  $matrix_editor->columns[] = array(
    'id' => 'taxa', 
    'name' => $taxa_vocabulary->name, 
    'field' => 'taxa', 
    'width' => ($node->settings['taxa_column_width'] ? $node->settings['taxa_column_width'] : 350), 
    'cssClass' => 'cell-move-handle', 
    'formatter' => 'TaskNameFormatter', 
    'resizable' => ($editable ? 1 : 0)
  );
  
  // Get all the charcater groups attached to this project 
  $character_groups = nexus_get_characters($node->character_root_tid);
  
  // Get settings for these characters (column width & numeric)
  $character_settings = nexus_get_character_settings($node->character_root_tid);
  
  $counter = 1;
  
  $default_states = array(
    '?' => array(
      'state' => 'Unknown', 
      'nid' => '?'
    ), 
    '-' => array(
      'state' => 'Not applicable', 
      'nid' => '-'
    )
  )
  ;
  
  $character_terms = array();
  
  if (count($character_groups)) {
    
    foreach ($character_groups as $character_group_tid => $character_group) {
      
      $characters = nexus_get_characters($character_group_tid);
      
      if (count($characters)) {
        
        $character_terms += $characters;
        
        foreach ($characters as $character_tid => $character) {
          
          $column = array(
            'id' => $character_tid, 
            'field' => $character_tid, 
            'name' => '<span>' . $counter . '</span>', 
            'width' => ($character_settings['width'][$character_tid] ? $character_settings['width'][$character_tid] : 50), 
            'cssClass' => 'cell-title', 
            'editor' => 'nexusCellEditor', 
            'term' => $character->name,
            'short_term' => truncate_utf8($character->name, SHORT_NAME_CHAR_LENGTH, TRUE, TRUE),  
            'description' => addslashes($character->description), 
            'groupID' => $character_group_tid, 
            'group' => $character_group->name,
            'short_group' => truncate_utf8($character_group->name, SHORT_NAME_CHAR_LENGTH, TRUE, TRUE),  
            'resizable' => ($editable ? 1 : 0)
          );
          
          // Get options for this character
          $nexus_controlled_states = nexus_get_states_for_character($character_tid);
          
          if (count($nexus_controlled_states)) {
            
            $column['setValueHandler'] = 'nexusControlledStateUpdateItem';
            $column['validator'] = 'nexusControlledStateValidator';
            $column['type'] = 'controlled';
            
            $x = 0;
            
            $column['states'] = $default_states;
            
            foreach ($nexus_controlled_states as $nexus_controlled_state) {
              
              $column['states'][$x] = array(
                'state' => $nexus_controlled_state->title, 
                'state_description' => $nexus_controlled_state->body, 
                'nid' => $nexus_controlled_state->nid
              );
              
              // Save the order number for each nid so we can use it as the default value
              $state_order_number[$nexus_controlled_state->nid] = $x;
              $x++;
            
            }
          
          }
          else { // Is free entry
            

            $column['setValueHandler'] = 'nexusFreeEntryUpdateItem';
            $column['type'] = 'free';
            
            // Numeric validator
            

            if (isset($character_settings['numeric'][$character->tid])) {
              
              $column['validator'] = 'nexusNumericStateValidator';
              $column['type'] = 'quantitative';
            
            }
          
          }
          
          $matrix_editor->columns[] = $column;
          $counter++;
        
        }
      
      }
    
    }
  
  }
  
  $tree = array();
  
  if (count($node->taxonomy)) {
    
    // Put these tids in order.
    

    foreach ($node->taxonomy as $tid => $taxonomy) {
      
      if ($taxonomy->vid) {
        
        $parents = taxonomy_get_parents_all($tid);
        
        $parent_tids = array();
        
        $node->taxonomy[$tid]->parents = $parents;
        
        for ($i = count($parents); $i > 0; $i--) {
          
          if ($parents[$i]->tid) {
            
            $parent_tids[] = $parents[$i]->tid;
          
          }
        
        }
        
        $parent_tids[] = $tid;
        
        $branch = nexus_create_tree($parent_tids);
      
      }
      
      if (is_array($branch)) {
        
        $tree = nexus_array_merge_recursive($branch, $tree);
      
      }
    
    }
    
    $collapsed_taxa = nexus_get_collapsed_taxa($node->taxonomy);
  
  }
  
  $matrix_editor->intersection_info = array();
  $matrix_editor->data = array();
  
  // Get the relationship data for child terms
  $nexus_relationships = nexus_get_relationships($node->character_root_tid);
  
  nexus_matrix_editor_populate_data($tree, $node->taxonomy, $character_terms, $state_order_number, $nexus_relationships, $collapsed_taxa, $matrix_editor);
  
  if ($matrix_editor->options['noTree']) {
    $matrix_editor->columns[0]['formatter'] = 'selectorCellFormatter';
  }
  
  return $matrix_editor;

}

/**
 * Recursively populate the date in the tree
 */
function nexus_matrix_editor_populate_data($tree, $taxa, &$character_terms, &$state_order_number, &$nexus_relationships, &$collapsed_taxa, &$matrix_editor) {
  
  static $count = 0;
  static $term_history;
  
  $tids = array_keys($tree);
  
  $ordered_tids = nexus_order_tids_by_weight($tids);
  
  foreach ($ordered_tids as $ordered_tid) {
    
    // Is it a selected item for this node..?
    if ($taxa[$ordered_tid]) {
      
      $data_item = array(
        'id' => $ordered_tid, 
        'taxa' => $taxa[$ordered_tid]->name
      );
      
      $parent_count = count($taxa[$ordered_tid]->parents);
      
      for ($x = count($taxa[$ordered_tid]->parents); $x > 0; $x--) {
        
        $parent_tid = $taxa[$ordered_tid]->parents[$x]->tid;
        
        if ($term_history[$parent_tid]) {
          $data_item['parent'] = $term_history[$parent_tid]['key'];
          $data_item['indent'] = $term_history[$parent_tid]['indent'] + 1;
          $data_item['_collapsed'] = ($collapsed_taxa[$ordered_tid] ? 1 : 0);
        }
        else {
          $data_item['indent'] = 0;
          
          if ($parent_tid) {
            $matrix_editor->options['noTree'] = true;
          }
        
        }
      
      }
      
      // Populate all the cells - loop through all the characters and see if there's data
      

      foreach ($character_terms as $character_term) {
        
        if ($nexus_relationships[$character_term->tid][$ordered_tid] == '?') {
          
          $data_item[$character_term->tid] = '?';
        
        }
        elseif ($nexus_relationships[$character_term->tid][$ordered_tid] == '-') {
          
          $data_item[$character_term->tid] = '-';
        
        }
        
        // Are there nodes?
        $nexus_nodes = nexus_get_nodes_at_intersection(array(
          $character_term->tid, 
          $ordered_tid
        ));
        
        $conj = '';
        
        if (count($nexus_nodes)) {
          
          for ($x = 0; $x < count($nexus_nodes); $x++) {
            
            switch ($nexus_nodes[$x]->type) {
              
              case 'nexus_controlled_state':
                
                $data_item[$character_term->tid] .= $conj . $state_order_number[$nexus_nodes[$x]->nid];
                
                if ($nexus_relationships[$character_term->tid][$ordered_tid] == '/') {
                  
                  $conj = '/';
                
                }
                else {
                  
                  $conj = '+';
                
                }
                
                break;
              
              case 'nexus_note':
                
                $matrix_editor->intersection_info[$character_term->tid][$ordered_tid]['note'] = array(
                  'nid' => $nexus_nodes[$x]->nid, 
                  'body' => $nexus_nodes[$x]->body
                );
                
                break;
              
              case 'nexus_free_state':
                
                $matrix_editor->intersection_info[$character_term->tid][$ordered_tid]['state_nid'] = $nexus_nodes[$x]->nid;
                
                $data_item[$character_term->tid] = $nexus_nodes[$x]->body;
                
                break;
            
            }
          
          }
        
        }
      
      }
      
      $term_history[$ordered_tid] = array(
        'key' => $count, 
        'indent' => $data_item['indent']
      );
      
      $matrix_editor->data[$count] = $data_item;
      $count++;
    
    }
    
    if (count($tree[$ordered_tid])) {
      
      nexus_matrix_editor_populate_data($tree[$ordered_tid], $taxa, $character_terms, $state_order_number, $nexus_relationships, $collapsed_taxa, $matrix_editor);
    
    }
  
  }

}

function nexus_order_tids_by_weight($unordered_tids) {
  
  $tids_placeholders = db_placeholders($unordered_tids, 'int');
  
  $sql = 'SELECT DISTINCT t.tid FROM {term_data} t WHERE t.tid IN  (' . $tids_placeholders . ') ORDER BY t.weight, t.name';
  
  $result = db_query($sql, $unordered_tids);
  
  while ($tid = db_fetch_object($result)) {
    
    $ordered_tids[] = $tid->tid;
  
  }
  
  return $ordered_tids;

}

/**
 * Create an multidimensional array using the term's parents
 */
function nexus_create_tree($array) {
  
  if (! is_array($array)) {
    return false;
  }
  if (count($array) == 1) {
    $key = array_shift($array);
    return array(
      $key => array()
    );
  }
  else {
    $key = array_shift($array);
    return array(
      $key => nexus_create_tree($array)
    );
  }
}

/**
 * Recursively merge an array, preserving numeric keys
 */
function nexus_array_merge_recursive($array1, $array2) {
  $arrays = func_get_args();
  $array_count = count($arrays);
  
  // the first array is in the output set in every case
  $merged_array = $arrays[0];
  
  for ($i = 1; $i < $array_count; $i++) {
    
    foreach ($arrays[$i] as $key => $value) {
      
      if (is_array($value) && isset($merged_array[$key])) {
        
        $merged_array[$key] = nexus_array_merge_recursive($merged_array[$key], $value);
      
      }
      else {
        $merged_array[$key] = $value;
      }
    
    }
  }
  
  return $merged_array;

}






