<?php 
/*
 * Creates a task and returns it's node id if successful
 */
function createTask($task){
	
 	return node_save($task);	
 	
}

/*
 * Get Task Status
 */
function getTask($nid){
	return node_load(array('nid' => $nid));		
}


?>