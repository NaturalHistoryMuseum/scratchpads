<?php


/**
 * Menu callback; displays all nexus projects
 */
function nexus_list_projects_page() {

  $project_nodes = nexus_get_project_nodes();

  if(count($project_nodes)){

    foreach($project_nodes as $project_node){

      $project_list[] = theme('nexus_project_list_item', nexus_view($project_node, TRUE, FALSE)); 

    }

    $output = theme('item_list', $project_list);
    
  }else{
    
    $output = t('There are not yet any projects.');
    
  }
  
  return $output;



}



