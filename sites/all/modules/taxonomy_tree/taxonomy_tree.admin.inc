<?php

function taxonomy_tree_admin_settings_form(){
  
  $form = array();
  
  $vocabularies = taxonomy_get_vocabularies();
  
  if(count($vocabularies)){
    
    $node_types = node_get_types();

    foreach($vocabularies as $vocabulary){

      if($vocabulary->multiple && count($vocabulary->nodes)){

        $options = array();

        $form[$vocabulary->vid] = array(
          '#type' => 'fieldset',
          '#title' => $vocabulary->name,
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
        );

        foreach($vocabulary->nodes as $vocabulary_node){
          
          $options[$vocabulary_node] = $node_types[$vocabulary_node]->name;
          
        }
        
        $form[$vocabulary->vid]['taxonomy_tree_'.$vocabulary->vid] = array(
          '#type' => 'checkboxes',
          '#title' => t('Node types'),
          '#default_value' => variable_get('taxonomy_tree_'.$vocabulary->vid, array()),
          '#options' => $options,
        );
        
      }

    }
    
  }
  
  return system_settings_form($form);
  
}



