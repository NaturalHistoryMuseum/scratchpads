<?php

function publication_export_form($form_state, $node){
  $form = array();
  $options = array('' => '- None -');  
  foreach (module_implements('publication_info') as $module) {
    $info = module_invoke($module, 'publication_info'); 
    $options[$module] = $info['name'];
  }
  $form['#action'] = url('node/'.$node->nid.'/publication-export');
  $form['module'] = array(
    '#type' => 'select', 
    '#title' => t('Export publication'), 
    '#options' => $options,
    '#description' => t('Please select the publication export format.'),
    '#ahah' => array(
      'path' => 'publication/export/js/'.$node->nid,
      'wrapper' => 'export-form',
      'method' => 'replace',
      'effect' => 'fade',
      'progress' => array('type' =>  'bar', 'message' => t('Loading export form...')),
    ),
    '#suffix' => '<div id="export-form"></div>'
  );
  $form['submit'] = array(
    '#weight' => 1000,
    '#type' => 'submit',
    '#value' => 'Export'
  );
  return $form;  
}

function publication_export_js(){
  $form = publication_ahah_form();
  drupal_json(array(
    'status' => TRUE, 'data' => drupal_render($form[$_REQUEST['module']])
  ));
}

function publication_export_form_submit($form, &$form_state){
  if(isset($form_state['values']['module']) && $form_state['values']['module'] && function_exists($form_state['values']['module'].'_publication_export_form_submit')){
    call_user_func_array($form_state['values']['module'].'_publication_export_form_submit', array($form, $form_state));
  }
}