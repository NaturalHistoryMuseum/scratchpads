<?php

function node_term_weight_admin_settings(){
  
  $form = array();
  
  $node_types = node_get_types();
  
    function array_map_node_types($node_type){
      return $node_type->name;
    }

    $form['node_term_weight_node_types'] = array(
      '#type' => 'checkboxes', 
      '#title' => t('Allow term ordering on'), 
      '#default_value' => variable_get('node_term_weight_node_types', array()),
      '#options' => array_map('array_map_node_types', $node_types)
    );  
  
  return system_settings_form($form);
  
}

?>