<?php
/**
 * Form for creating a new character
 */

function nexus_character_form(&$form_state, $nid, $tid = null){
  
  if($tid){
    $term = taxonomy_get_term($tid);
  }
  
  
  $form = array();
  
  // Are there any state states for this character?
  
  $form['name'] = array(
    '#type' => 'textarea',
    '#title' => t('Character title'),
    '#size' => 60,
    '#default_value' => $term->name,
    '#maxlength' => 255,
    '#required' => TRUE,
  );
  
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Character description'),
    '#required' => FALSE,
    '#default_value' => $term->description,
  );
  
  $character_tid = nexus_get_project_character_root_tid($nid);

  $character_vid = nexus_get_character_vid();
  
  $form['character_vid'] = array('#type' => 'hidden', '#value' => $character_vid);
  
  $form['character_tid'] = array('#type' => 'hidden', '#value' => $character_tid);
  
  $character_groups = taxonomy_get_tree($character_vid, $character_tid, -1, 1);
  
  $form['group']['new_group'] = array(
    '#type' => 'textfield',
    '#title' => t('Create and add to new group'),
    '#size' => 60,
    '#maxlength' => 128,
    '#weight' => 1
  );
  
  $character_group_options[0] = t('Add new group');
  
  if(count($character_groups)){
    
    foreach($character_groups as $character_group){
      
      $character_group_options[$character_group->tid] = $character_group->name;
      
    }
    
  }else{
    
    $form['group']['new_group']['#required'] = true;
    
  }
  
  $form['group']['character_group'] = array(
    '#type' => 'select',
    '#title' => t('Character group'),
    '#options' => $character_group_options,
    '#description' => t('Please select a group to add this character to.'),
  );
  
  $form['type'] = array(
    '#type' => 'radios',
    '#title' => t('Type of character'),
    '#default_value' => 'text',
    '#options' => array(
      'text' => t('Text (free text entry)'), 
      'numeric' => t('Numeric (free text entry)'), 
      'controlled' => t('Controlled character')),
    '#description' => t('Please select the type of character. For controlled characters, you will create a list of states to select from; for free entry you can enter numeric or textual data for this character.'),
    '#required' => TRUE,    
  );

  // $states_count = (isset($form_state['values']['states']) ? count($form_state['values']['states']) : 1);

  if (isset($form_state['states_count'])) {
    $states_count = $form_state['states_count'];
  }else{
    $states_count = (count($form_state['values']['states']) ? count($form_state['values']['states']) : 1);
  }
    
  // Add a wrapper for the choices and more button.
  $form['states_wrapper'] = array(
    '#tree' => FALSE,
    '#prefix' => '<div class="clear-block" id="nexus-state-wrapper">',
    '#suffix' => '</div>',
  );

  // Container for just the poll choices.
  $form['states_wrapper']['states'] = array(
    '#prefix' => '<div id="nexus-states">',
    '#suffix' => '</div>'
  );

  $states = $form_state['values']['states'];

  // Add the current choices to the form.
  for ($delta = 0; $delta < $states_count; $delta++) {
    
    $state = isset($states[$delta]['state']) ? $states[$delta]['state'] : '';
    $state_description = isset($states[$delta]['state_description']) ? $states[$delta]['state_description'] : '';
    
    $form['states_wrapper']['states'][$delta] = _nexus_state_form($delta, $state, $state_description);
  
  }
  
  $form['states_wrapper']['add_state'] = array(
    '#type' => 'button',
    '#value' => t('Add more states'),
    '#description' => t("If the amount of boxes above isn't enough, click here to add more states."),
    '#weight' => 1,
    '#submit' => array('nexus_state_form_submit'), // If no javascript action.
    '#ahah' => array( 
      'path' => 'nexus/callback/add_state', 
      'wrapper' => 'nexus-states',
      'method' => 'replace',
      'effect' => 'slide',
    ),
  );
  
  $form['nid'] = array('#type' => 'hidden', '#value' => $nid);
  
  $form['save'] = array(
    '#type' => 'button', 
    '#value' => t('Add'),
    '#ahah' => array( 
      'path' => 'nexus/callback/save_character', 
      'wrapper' => 'nexus-messages',
      'method' => 'replace',
      'effect' => 'slide',
    ),
    
  );
  
  $form['#validate'] = array('nexus_add_character_form_validate');

  
  return $form;
   
}


function theme_nexus_character_form($form){
  
  $path = drupal_get_path('module', 'nexus');
    
  drupal_add_js($path.'/js/nexus.js'); 
  drupal_add_css($path.'/css/nexus.css');
  
  $output = drupal_render($form); 
  return $output;
  
}


/**
 * User has submitted add option form
 * Passed validation so increase option count to display new row when form is renexusred
 */
function nexus_state_form_submit($form, &$form_state) {
  // Set the form to rebuild and run submit handlers.

  // Make the changes we want to the form state.
  if ($form_state['values']['add_state']) {
    $form_state['states_count'] = count($form_state['values']['states']) + 1;    
  }

}




function _nexus_state_form($delta, $state = '', $state_description = 0) {

  $form = array(
    '#tree' => TRUE,
  );

  // We'll manually set the #parents property of these fields so that
  // their values appear in the $form_state['values']['choice'] array.
 
  $nexus_node_info = nexus_node_info();
  
  $form['state'] = array(
    '#type' => 'textfield',
    '#title' => $nexus_node_info['nexus_state']['title_label'],
    '#size' => 10,
    '#maxlength' => 128,
    '#required' => FALSE,
    '#parents' => array('states', $delta, 'state'),
    '#default_value' => $state
  );
  
  $form['state_description'] = array(
    '#type' => 'textarea',
    '#title' => $nexus_node_info['nexus_state']['body_label'],
    '#size' => 5,
    '#maxlength' => 7,
    '#required' => FALSE,
    '#parents' => array('states', $delta, 'state_description'),
    '#default_value' => $state_description
  );
  
  return $form;

}





/**
 * Validate form - remove errors for state data if free_state is selected 
 */
function nexus_character_form_validate($form, &$form_state){
  
  if(!$form_state['values']['character_group']){
    
    if(empty($form_state['values']['new_group'])){
      
      form_set_error('new_group', t('Please enter a name for the new character group'));
      
    }
    
    
  }
  
  
  
  // Is this quantative data? If it is remove all the errors for free_state node data
  if($form_state['values']['type'] == 'controlled'){
    
    $errors = form_get_errors();
    
      if(count($errors)){
    
        // Reset the errors
        form_set_error(null, null, true);
        drupal_get_messages('error', TRUE);
        
        foreach($errors as $name => $error){
          
          if(strpos($name, 'states][') === false){
            
            form_set_error($name, $error);
            
          }
          
        }
        
      }
    
  }
  

  
}


function nexus_character_form_submit($form, &$form_state){
  
  global $user;
  
  // Make the changes we want to the form state.
  if ($form_state['values']['add_state']) {
    $form_state['states_count'] = count($form_state['values']['choice']) + 1;
  }
  
  $nid = $form_state['values']['nid'];
  $character_vid = $form_state['values']['character_vid'];
  $character_tid = $form_state['values']['character_tid'];

  if($form_state['values']['character_group']){
    
    $parent_tid = $form_state['values']['character_group'];
    
  }else{
    
    $group_term = array(
      'name' => $form_state['values']['new_group'],
      'parent' => $character_tid,
      'vid' => $character_vid 
    );

    taxonomy_save_term($group_term);
    
    $parent_tid = $group_term['tid'];
    
  }
  
  $node = node_load($nid);
    
  // Create a term for the new character  
  $term = array(
    'name' => $form['#post']['name'],
    'description' => $form['#post']['description'],
    'parent' => $parent_tid,
    'vid' => $character_vid 
  );
   
  taxonomy_save_term($term);
  
  // If there's states, create the states
  if(count($form_state['values']['states'])){
    
    $vocabularies = taxonomy_get_vocabularies('nexus_state');

    // Set all vocabularies for this node type to 0 - 
    // if we don't this node will be associated with first term in all taxonomies 
    // for this node type (ie; any other char taxonomies)
    
    foreach($vocabularies as $vocabulary){
      
      $node_taxonomy[$vocabulary->vid] = 0;
      
    }
    
    // Assign this node to the character taxonomy
    $node_taxonomy[$character_tid] = $term['tid'];
    
    foreach($form_state['values']['states'] as $state){
       
      $character_node = new stdClass();
      $character_node->uid = $user->uid;
      $character_node->type = 'nexus_state';
      $character_node->title = $state['state'];
      $character_node->body = $state['state_description'];
      $character_node->status = 1;
      $character_node->name = $user->name;
      $character_node->taxonomy = $node_taxonomy;
      
      node_save($character_node);

    }
    
  }
  
  drupal_set_message('Created new character');
  
  
  
}
