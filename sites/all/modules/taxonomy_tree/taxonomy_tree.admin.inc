<?php

function taxonomy_tree_admin_settings_form(){
  
  $form = array();
  
  $vocabularies = taxonomy_get_vocabularies();
  
  $node_types = node_get_types();
  
  if(count($vocabularies)){
    
    foreach($vocabularies as $vocabulary){

      if($vocabulary->multiple && count($vocabulary->nodes)){

        $options = array();

        foreach($vocabulary->nodes as $vocabulary_node){
          
          $options[$vocabulary_node] = '';
          
        }
        
        $form['taxonomy_tree_'.$vocabulary->vid] = array(
          '#type' => 'checkboxes',
          '#default_value' => variable_get('taxonomy_tree_'.$vocabulary->vid, array()),
          '#options' => $options
        );
        
      }

    }
    
  }
  
  $form['#prefix'] = '<p>'.t('Please select for which vocabularies & on which node forms you want to use taxonomy tree').'</p>';
  $form['#suffix'] = '<p class="warning">Please note: Only \'multiple\' vocabularies can use taxonomy tree, so unless a vocabulary is marked as \'multiple\' it will not be displayed in this list.</p>';
  
  $form = system_settings_form($form);
  
  $form['#theme'] = 'taxonomy_tree_admin_settings_form';
  
  return $form;
  
}



function theme_taxonomy_tree_admin_settings_form($form){
  
  $rows = array();
  $header = array(t('Vocabularies'));
  
  $vocabularies = taxonomy_get_vocabularies();
  
  $node_type_info = node_get_types();
  
  $node_types = array();
  
  foreach($vocabularies as $vocabulary){
    
    if(count($vocabulary->nodes)){
      
      $node_types += $vocabulary->nodes;
      
    }

  }
  
  foreach($vocabularies as $vocabulary){
    
    $row = array($vocabulary->name);
    
    foreach($node_types as $node_type){

      $header[$node_type] = $node_type_info[$node_type]->name;

      if($vocabulary->nodes[$node_type]){

         $row[] = drupal_render($form['taxonomy_tree_'.$vocabulary->vid][$node_type]);

       }else{
     
         $row[] = '<span title="This content type does not use this vocabulary">N/A</span>';
     
       }

    }
    
    $rows[] = $row;
    
  }

  $output = theme('table', $header, $rows);
   
  $output .= drupal_render($form);
  
  return $output;
  
}





