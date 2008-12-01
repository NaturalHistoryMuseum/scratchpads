<?php

/**
 * Classification settings form
 */
function classification_administration_form(&$form_state) {
	$module_path = drupal_get_path('module', 'classification') .'/';
  drupal_add_css($module_path .'css/classification.css');

  $form['names_display_viewer'] = array(
    '#type' => 'fieldset',
    '#title' => t('Classification Browser Display Options'),
  );

  $default_viewer = variable_get('classification_viewer_names',2);
  $options_viewer = array(0 => t('Show all names (vernaculars, synonyms, <em>etc.</em>)'), 1 => t('Show valid names and synonyms (hide vernaculars)'), 2 => t('Only show valid names'));
  $default_canonicals = variable_get('classification_viewer_canonicals',1);
  $options_canonicals = array(1 => t('Yes'), 0 => t('No'));

  $form['names_display_viewer']['display_options_viewer'] = array(
    '#type' => 'radios',
    '#title' => t('Visibility of names in the public classification browser'),
    '#options' => $options_viewer,
    '#default_value' => $default_viewer,
  );

  $form['names_display_viewer']['display_canonicals'] = array(
    '#type' => 'radios',
    '#title' => t('Convert names to canonical versions in the public classification browser?'),
    '#description' => t('Canonical names are versions of names without the authority information.'),
    '#options' => $options_canonicals,
    '#default_value' => $default_canonicals,
  );

  $form['relationships'] = array(
    '#type' => 'fieldset',
    '#title' => t('Name Relationship Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $sql = db_query("SELECT rtid, name FROM {term_relation_types} ORDER BY rtid");
  while ($row = db_fetch_object($sql)) {
     $_relationships .= $row->name . "<br>";
  }
  
  $form['relationships']['relationships_list'] = array(
    '#type' => 'markup',
    '#value' => '<p>' . $_relationships . '</p>',
  );
  
  $form['relationships']['relationships_add'] = array(
    '#title' => t('One relationship per line'),
    '#type' => 'textarea',
    '#disabled' => TRUE,
  );
  
  $form['submit'] = array(
  	'#type' => 'submit',
  	'#value' => t('Save Configuration'),
  	'#attributes' => array('style' => 'margin-top:20px', 'class' => 'classification-buttons save'),
  );
  
  return $form;
}

/**
 * Classification settings form submit
 */
function classification_administration_form_submit($form, &$form_state) {
	switch ($form_state['values']['op']) {
		case t('Save Configuration'):
		  
		  variable_set('classification_viewer_names',$form_state['values']['display_options_viewer']);
		  variable_set('classification_viewer_canonicals',$form_state['values']['display_canonicals']);

      $relationships = explode("\n",$form_state['values']['relationships_list']);
      for ($i=0;$i<=count($relationships);$i++) {
      	$value = str_replace("\r","",trim($relationships[$i]));
//      	db_query("INSERT INTO {term_relation_types} (name,namespace,description) VALUES ('%s', ' ', ' ')",$value);
      }
		  drupal_set_message(t('Configuration options have been saved.'),'status');
      break;
  }
}
