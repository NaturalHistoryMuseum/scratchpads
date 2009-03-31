<?php

/**
* Implementation of hook_settings().
*/
function classification_scratchpads_admin_settings() {
 
 $vocabs = taxonomy_get_vocabularies();
 
 foreach($vocabs as $vid => $vocab){
   
   $options[$vid] = $vocab->name;
   
 }
 
  $form['classification_scratchpads'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Taxonomies to use as classifications'),
    '#default_value' => variable_get('classification_scratchpads', array()),
    '#options' => $options,
    '#description' => t('Please select which taxonomies you want to use as taxonomies.'),
  );
  
  return system_settings_form($form);
  
}



