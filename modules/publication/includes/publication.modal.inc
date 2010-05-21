<?php

/**
 * Add modal files
 *
 * @param string $type child | parent
 * @return void
 * @author Ben Scott
 */
function publication_add_modal_child_files(){

  $path = drupal_get_path('module', 'publication');

  modalframe_child_js();
  drupal_add_js($path . '/js/publication.modal.child.js');
  drupal_add_css($path . '/css/publication.css');
  
}


function publication_modal_image_caption_node_form($image_nid){

  $publication_nid = $_GET['publication_nid'];

  $node = publication_modal_image_caption_node_load($image_nid, $publication_nid);

  module_load_include('inc', 'node', 'node.pages');
  
  // See if a caption exists which is related to both $image_nid & $publication_nid
  $output = drupal_get_form('publication_image_caption_node_form', $node, array('#modal' => true));
  
  return $output;
  
}


/**
 * Modal version of node_form_delete_submit()
 * Redirect to the same delete from with a modal flag set
 *
 * @param array $form 
 * @param array $form_state 
 * @return void
 * @author Ben Scott
 */
function publication_node_delete_submit($form, &$form_state){
  
  $form_state['redirect'] = array('publication/'. $form['#node']->nid .'/delete');
  
}



/**
 * Menu callback -- ask for confirmation of node deletion
 */
function publication_modal_node_delete_confirm(&$form_state, $node){
  
  publication_add_modal_child_files();
  
  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );
  
  $form['type'] = array(
    '#type' => 'value',
    '#value' => $node->type,
  );
  
  $form['publication_form_build_id'] = array(
    '#type' => 'value',
    '#value' => $_GET['publication_form_build_id'],
  );


  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $node->title)),
    'node/'. $node->nid,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
  
}


/**
 * Delete nodes - remove references to them, but keep the nodes themselves for revisions
 *
 * @param string $form 
 * @param string $form_state 
 * @return void
 * @author Ben Scott
 */
function publication_modal_node_delete_confirm_submit($form, &$form_state){

  $publication_form_build_id = $form_state['values']['publication_form_build_id'];

  // Delete the reference, not the node.
  $nid = $form_state['values']['nid'];
  
  switch($form_state['values']['type']){
    
    case 'publication_section':
    
      // If the form's being cached, remove the element to keep cached version up to date
      $publication_form_state = array('storage' => NULL, 'submitted' => FALSE, 'rebuild' => TRUE);

      if($publication_form = form_get_cache($publication_form_build_id, $publication_form_state)){
      
        unset($publication_form['field_publication_section'][$nid]);
        
        form_set_cache($publication_form_build_id, $publication_form, $publication_form_state);
      
      }
    
    break;
    
  }
  
  $args = array(
    'op' => 'Delete',
    'nid' => $nid,
    'type' =>  $form_state['values']['type']
  );
  
  $form_state['rebuild'] = true;
  
  modalframe_close_dialog($args);
  
}


function publication_modal_image_caption_node_form_submit(&$form, &$form_state){
  
  $args = array(
    'type' => $form['#node']->type,
    'nid' => $nid,
    'op' =>  (empty($form['#node']->nid) ? 'Created' : 'Updated')
  );

   drupal_get_messages(null, true);
 
   drupal_set_message(t('@op caption for image <em>@image_node_title</em>.', 
     array(
       '@op' => $args['op'],
       '@image_node_title' => $form['#image_node_title'],
     )
   ));
  
  $form_state['rebuild'] = true;
  
   modalframe_close_dialog($args);
  
}


function publication_section_modal_node_form_submit(&$form, &$form_state){

  node_form_submit($form, &$form_state);

  $nid = $form_state['nid'];
  
  $args = array(
    'type' => $form['#node']->type,
    'nid' => $nid,
  );
  
  if(empty($form['#node']->nid)){ // Has a new node been created?
    
    $args['op'] = 'Created';
    
    // Build a new row to go in the sections table
    $form_build_id = $form_state['values']['publication_form_build_id'];
  
    $publication_form_state = array('storage' => NULL, 'submitted' => FALSE, 'rebuild' => TRUE);
  
    $publication_form = form_get_cache($form_build_id, $publication_form_state);
  
    $nids = array(array('nid' => $nid));
  
    function array_filter_helper($property){
       
     return !is_array($property);
     
    }
    
    $publication_form['field_publication_section'] = array_filter($publication_form['field_publication_section'], 'array_filter_helper');
  
    publication_field_publication_section_alter($publication_form['field_publication_section'], $nids);
  
    drupal_process_form('publication_node_form', $publication_form, $publication_form_state);
  
    $args['output'] = drupal_render($publication_form['field_publication_section']);    
   
  }else{ // Node has been updated
    
    $args['op'] = 'Updated';
    
  }
  
  $form_state['rebuild'] = true;
  modalframe_close_dialog($args);
  
}

function publication_taxon_section_modal_node_form_submit(&$form, &$form_state){
  
  $args = array(
    'type' => $form['#node']->type,
    'nid' => $nid,
  );
  
  if($form_state['values']['reset_manuscript_name']){
  
    $form_state['values']['title'] = $form_state['values']['term']->name;
    $args['op'] = 'Reset';
    $args['title'] = $form_state['values']['term']->name;
    
  }else{
    
    $args['op'] = 'Update';
    $args['title'] = $form_state['node']['title'];
    
  }

  node_form_submit($form, &$form_state);

  $nid = $form_state['nid'];
  
  $form_state['rebuild'] = true;
  modalframe_close_dialog($args);
  
}


function publication_taxon_description_modal_node_form_submit(&$form, &$form_state){
  
  $args = array(
    'type' => $form['#node']->type,
    'nid' => $form_state['nid'],
    'op' => 'Updated',
  );
  
  $form_state['rebuild'] = true;
  modalframe_close_dialog($args);
  
}







/**
 * Load the image caption node associated with the image & publication nid.
 * If no node is found, create a placeholder node
 *
 * @param integer $nid 
 * @return object $node
 * @author Ben Scott
 */
function publication_modal_image_caption_node_load($image_nid, $publication_nid){
  
  static $node;
  
  if(!isset($node)){

    $cck_params = array(
      'publication_caption_image' => $image_nid, 
      'publication_caption_parent' => $publication_nid
    );

    if(!$node = publication_content_node_load(NULL, $cck_params)){
      
      // Set up a dummy node
      $node = array(
        'uid' => $user->uid, 
        'name' => (isset($user->name) ? $user->name : ''), 
        'type' => 'publication_image_caption', 
        'language' => ''
      );
      
    }
    
  }

  return $node;
  
}










?>