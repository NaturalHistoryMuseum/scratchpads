<?php


/**
 * Preprocess the matrix_editor theme hook.
 */
function nexus_project_get_matrix_data($node) {
  
  $matrix_editor = new stdClass();

  // Define filter
  $matrix_editor->filter = 'nexusFilter';
  
  // Define options  
  $matrix_editor->options = array(
    'editable' => true,
    'enableAddRow' => false,
    'enableCellNavigation' => true,
    'asyncEditorLoading' => false
  );
  
  $matrix_editor->search = true;
  
  // Prepare columns
  $matrix_editor->columns = array();
  
  // All the taxa terms in this vocabulary attached to the project node
  $taxa_vocabulary = taxonomy_vocabulary_load($node->taxa_vid);
   
   // First column is for the row selector / number
   $matrix_editor->columns[] = array(
     'id' => '#',
     'name' => '#',
     'width' => 60,
     'cssClass' => 'cell-move-handle',
     'formatter' =>'nexusSelectorCellFormatter',
   );
   
    // First column is for the taxa
    $matrix_editor->columns[] = array(
      'id' => 'taxa',
      'name' => $taxa_vocabulary->name,
      'field' => 'taxa',
      'width' => 400,
      'cssClass' => 'cell-title',
      'formatter' => 'TaskNameFormatter',
      'validator' => 'requiredFieldValidator',
      'setValueHandler' => 'updateItem',  
    );
   
    // Get all the charcater terms attached to this project 
    
    $character_vid = nexus_get_character_vid();
    
    $character_root_tid = nexus_get_project_character_root_tid($node->nid);

    $character_terms = taxonomy_get_tree($character_vid, $node->character_tid);
    
    // Array of all the characters / notes so we don't need to use a callback for display the info
    $matrix_editor->characters = array();
    $matrix_editor->character_group_class = array();
    $matrix_editor->notes = array();
    $counter = 0;
    
    $flip = array('even' => 'odd', 'odd' => 'even');
    $group_class = 'even';
    $character_groups = array();
       
    foreach($character_terms as $character_term){

      if($character_term->parents[0] == $character_root_tid){
        
        $character_groups[$character_term->tid] = $character_term->name;
        
        continue;
        
      }
      
      if($character_term->parents[0] != $previous_parent){
        
        $group_class = $flip[$group_class];
        
      }
      
      $previous_parent = $character_term->parents[0];
            
      $column = array(
        'id' => $character_term->tid,
        'field' => $character_term->tid,
        'name' => '<span class=\"group '.$group_class.'\" title=\"'.$character_groups[$character_term->parents[0]].'\"></span>'.$counter.'<span>'.$character_term->name.'</span>',
        'width' => 50,
        'cssClass' => 'cell-title char-group-'.$character_term->parents[0],
        'setValueHandler' => 'nexusUpdateItem',
      );
      
      $matrix_editor->character_group_class[$character_term->tid] = $group_class;
          
      // All the characters so we don't need to do a callback to display the info
      $matrix_editor->characters[$character_term->tid] = array(
        'name' => $character_term->name,
        'description' => $character_term->description
      );
      
      
      // Get all options for this character
      $nexus_states = nexus_get_states_for_character($character_term->tid);      
      
      if(count($nexus_states)){
        
        $column['editor'] = 'nexusCellEditor';
        $column['setValueHandler'] = 'nexusQuantitativeUpdateItem';

        $x = 1;

        foreach($nexus_states as $nexus_state){
          
          $matrix_editor->characters[$character_term->tid]['states'][$x] = $nexus_state; 

          // Save the order number for each nid so we can use it as the default value
          $state_order_number[$nexus_state['nid']] = $x;
          $x++;
          
        }

      }else{
        
        $column['editor'] = 'nexusCellEditor';
        $column['setValueHandler'] = 'nexusQualitativeUpdateItem';
               
      } 
       
      $matrix_editor->columns[] = $column;
      $counter++;

    }

    $tree = array();    
    
    foreach($node->taxonomy as $tid => $taxonomy){
      
      if($taxonomy->vid){
        
        $parents = taxonomy_get_parents_all($tid);

        $parent_tids = array();
        
        $node->taxonomy[$tid]->parents = $parents;
        
        for($i = count($parents); $i > 0; $i--){
          
          if($parents[$i]->tid){
            
            $parent_tids[] = $parents[$i]->tid;
          
          }
                    
        }
        
        $parent_tids[] = $tid;
        
        $branch = nexus_create_tree($parent_tids);
        
      }

      if(is_array($branch)){
        
        $tree = nexus_array_merge_recursive($branch, $tree);    
      
      }  
    
    }
    
    nexus_matrix_editor_populate_data($tree, $node->taxonomy, $data);
      
    $matrix_editor->data = $data;  
      
    
    //BTODO: Move into populate data
    
    // for($i = 0; $i < count($taxa_vocabulary_terms); $i++){
    //     
    //       $taxa_term = $taxa_vocabulary_terms[$i];
    //     
    //       $data_item = array(
    //         'id' => $taxa_term->tid,
    //         'taxa' => $taxa_term->name,
    //       );
    //       
    //       if($hidden_terms[$taxa_term->tid]){
    //         
    //         $data_item['_hidden'] = true;
    //         
    //       }
    //   
    //       foreach($character_terms as $character_term){
    //         
    //         // Are there any nodes at this intersection?
    //         // If there are, that's the default value
    //         $nexus_nodes = nexus_get_nodes_at_intersection(array($character_term->tid, $taxa_term->tid));
    //         
    //         if(count($nexus_nodes)){
    //           
    //           for($x = 0; $x < count($nexus_nodes); $x++){
    //             
    //             switch($nexus_nodes[$x]->type){
    //                             
    //               case 'nexus_state':
    //   
    //                 $data_item[$character_term->tid] = $state_order_number[$nexus_nodes[$x]->nid];                 
    //               
    //               break;
    //               
    //               case 'nexus_note':
    //   
    //                 $js_notes[$character_term->tid][$taxa_term->tid] = array(
    //                   'nid' => $nexus_nodes[$x]->nid,
    //                   'body' => $nexus_nodes[$x]->body,
    //                   );
    //               
    //               break;
    //               
    //               case 'nexus_free_state':
    //               
    //               $js_free_state[$character_term->tid][$taxa_term->tid] = $nexus_nodes[$x]->nid;
    //                   
    //               $data_item[$character_term->tid] = $nexus_nodes[$x]->title;
    //               
    //               break;
    //               
    //             }
    //             
    //           }      
    //                    
    //         }else{
    //           
    //           $data_item[$character_term->tid] = null;
    //           
    //         }
    //                 
    //       }
    //    }   
    
    
         
    // Add the notes data
    // drupal_add_js('var notes = '.drupal_to_js($js_notes), 'inline');
    //  
    //  // Add the characters data
    //  drupal_add_js('var characters = '.drupal_to_js($js_characters), 'inline');
    // 
    //  // Add the free_state data
    //  drupal_add_js('var free_states_data = '.drupal_to_js($js_free_state), 'inline');     
     
     
    return $matrix_editor;
    
}




/**
 * Simple form editing / adding nexus_note nodes
 */

function nexus_note_form($form){
  
  global $user;
  
  $form['uid'] = array('#type' => 'hidden', '#value' => $user->uid);
  $form['type'] = array('#type' => 'hidden', '#value' => 'nexus_note');

  $form['nid'] = array('#type' => 'hidden');
  $form['character_tid'] = array('#type' => 'hidden');
  $form['taxa_tid'] = array('#type' => 'hidden');

  
  $form['note'] = array(
    '#type' => 'fieldset',
    '#title' => t('Note'),
    '#weight' => 5,
  );
  
  $form['note']['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Notes'),
  );
    
  $form['note']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#ahah' => array(
        'path' => 'nexus/callback/save_note',
        'wrapper' => 'nexus-status-messages',
        'method' => 'replace',
        'effect' => 'fade',
        'callback' => 'hello'
      )
    );
      
  return $form;
  
}

function nexus_matrix_editor_populate_data($tree, $node_taxonomy, &$data){

  static $count = 0;
  static $term_history;

  foreach($tree as $tid => $branch){
    
    // Is it a selected item for this node..?
    if(!$node_taxonomy[$tid]){
      continue;
    }
    
    $data_item = array(
      'id' => $tid,
      'taxa' => $node_taxonomy[$tid]->name
    );
    
    for($x = count($node_taxonomy[$tid]->parents); $x > 0; $x--){
      
      $parent_tid = $node_taxonomy[$tid]->parents[$x]->tid;
      
      if($term_history[$parent_tid]){
        $data_item['parent'] = $term_history[$parent_tid]['key'];
        $data_item['indent'] = $term_history[$parent_tid]['indent'] + 1;
      }else{
        $data_item['indent'] = 0;
      }
      
      
    }
 
     $term_history[$tid] = array(
       'key' => $count,
       'indent' => $data_item['indent'],
     );

    $data[$count] = $data_item;
    $count++;
    
    if(count($branch)){
      
      nexus_matrix_editor_populate_data($branch, $node_taxonomy, $data);
      
    }
    
  }
  
}



function nexus_order_tids_by_weight($tids){
  
  $tid_placeholders = db_placeholders($tids, 'integer');
     
  $sql = 'SELECT t.tid FROM {term_data} t WHERE t.tid IN  ('.$tid_placeholders.') ORDER BY weight';

    while ($node = db_fetch_object($result)) {

     $nodes[] = $node;

    }
  
  return $nodes;
  
}

function nexus_create_tree($array){

    if(!is_array($array)){
        return false;
    }
    if(count($array)==1){
        $key = array_shift($array);
        return array($key => array());
    } else {
        $key = array_shift($array);
        return array($key => nexus_create_tree($array));
    }
}





function nexus_project_settings_form($form_state){
  
  module_load_include('inc', 'node', 'node.pages');
  
  $form = array();
  
  $nid = arg(1);
  
  $node = node_load($nid);
  
  $form = node_form($form_state, $node);
  
  $taxa_vid = $form['taxa']['taxa_vid']['#default_value'];
  $form['taxa']['taxa_vid'] =  array('#type' => 'hidden', '#value' => $taxa_vid);
  
  unset($form['body_filter']['filter']);
  unset($form['revision_information']);

  $form['name'] = array('#type' => 'hidden', '#value' => $form['author']['name']['#default_value']);
  $form['date'] = array('#type' => 'hidden', '#value' => $form['author']['date']['#default_value']);
  
  unset($form['author']);
  
  $form['status'] = array('#type' => 'hidden', '#value' => $form['options']['status']['#default_value']);
  $form['promote'] = array('#type' => 'hidden', '#value' => $form['options']['promote']['#default_value']);
  $form['sticky'] = array('#type' => 'hidden', '#value' => $form['options']['sticky']['#default_value']);
      
  unset($form['options']);    
      
  unset($form['buttons']['preview']);
  unset($form['buttons']['delete']);
  
  $form['buttons']['submit']['#value'] = t('Save changes');
  $form['buttons']['submit']['#ahah'] = 
    array(
      'path' => 'nexus/callback/update_project',
      'wrapper' => 'update-result',
      'method' => 'replace',
      'effect' => 'fade',
    );

  $form['buttons']['submit']['#suffix'] = '<div id="update-result"></div>';

  return $form;
  
}

function nexus_array_merge_recursive($array1, $array2)
{
    $arrays = func_get_args();
    $array_count = count($arrays);
   
    // the first array is in the output set in every case
    $merged_array = $arrays[0];

    for ($i = 1; $i < $array_count; $i ++) {
      
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

