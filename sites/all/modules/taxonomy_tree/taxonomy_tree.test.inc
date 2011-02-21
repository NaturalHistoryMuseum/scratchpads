<?php

function taxonomy_tree_test_form(){
  
  $form = array();
  
  $form['taxonomy_tree'] = array(
    '#type' => 'taxonomy_tree',
    '#vid' => 1
  );
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Import'));
  
  return $form;
  
}

function taxonomy_tree_test_form_submit($form, &$form_state){


}

?>