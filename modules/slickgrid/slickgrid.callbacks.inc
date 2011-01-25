<?php
/** 
 * hook_menu callback; 
 * Universal callback - builds function name and returns json
 */
function slickgrid_callback($op){
  $func = 'slickgrid_callback_' . $op;
  $json = $func();
  drupal_json($json);
}

/**
 * 
 * Callback function - delete a node
 */
function slickgrid_callback_delete(){
	
	$node = node_load($_POST['nid']);
	
	// node_delete() has access check so it's safe just to try and delete the node
	node_delete($node->nid);
	
	return array(
      'status' => TRUE,
      'result' => t('@type deleted.', array('@type' => node_get_types('name', $node)))
    );
	
}

/**
 * 
 * Callback function - update a view's settings
 */
function slickgrid_callback_settings(){
	
	slickgrid_set_settings($_POST['view'], $_POST['setting'], $_POST['value']);

		return array(
      'status' => TRUE
    );
	
}