<?php

function taxonomy_tree_admin_settings_form(){
  
  $form = array();
  
  $vocabularies = taxonomy_get_vocabularies();
  
  if(count($vocabularies)){
    
    foreach($vocabularies as $vid => $vocabulary){

      $vocabulary_options[$vid] = $vocabulary->name;

    }

    $form['taxonomy_tree_vocabularies'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Vocabularies'),
      '#default_value' => variable_get('taxonomy_tree_vocabularies', array()),
      '#options' => $vocabulary_options,
      '#description' => t('Please select which vocabularies should use the taxonomy tree.')
    );
    
  }
  
  $node_types = node_get_types();
  
  if(count($node_types)){

    foreach($node_types as $node_type){

      $node_type_options[$node_type->type] = $node_type->name;

    }

    $form['taxonomy_tree_node_types'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Node types'),
      '#default_value' => variable_get('taxonomy_tree_node_types', array()),
      '#options' => $node_type_options,
      '#description' => t('Select nodes which will use the taxonomy tree for <em>all</em> vocabularies')
    );

  }
  
  return system_settings_form($form);
  
}







