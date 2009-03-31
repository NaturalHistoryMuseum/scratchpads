<?php

/**
 * Classification settings form
 */
function classification_administration_form(&$form_state) {
	$module_path = drupal_get_path('module', 'classification') .'/';
  drupal_add_css($module_path .'css/classification.css');

  $form['#cache'] = FALSE;

  $form['title'] = array(
    '#type' => 'markup',
    '#value' => '<h2>' . t('Content Settings') . '</h2>',
  );
  
  $form['content_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content Type - Name Associations'),
    '#description' => t('Choose whether newly created content may be tagged with one name from your site\'s primary classification, multiple names, or disabled (tagging with a name not enabled).'),
  );
  
  $node_types = node_get_types('names');
  
  $result = db_query("SELECT type, select_option, weight, required FROM {classification_node_types}");
  while ($node = db_fetch_object($result)) {
  	$nodes[$node->type] = array(
  	    'select_option' => $node->select_option,
  	    'weight' => $node->weight,
  	    'required' => $node->required
  	    );
  }
  
  $select_options = array(0 => t('Disabled for this content type'), 1 => t('Single taxon'), 2 => t('Multiple taxa'), 3 => t('Tree Selector'));
  $weight_options = array();
  for($i=-10;$i<=10;$i++) {
  	$weight_options[$i] = $i;
  }
  
  foreach ($node_types as $key => $node) {
  	if($key == 'taxon_description') {
  		$disabled = TRUE;
  	}
  	else {
  		$disabled = FALSE;
  	}

  		$form['content_types'][$key] = array(
  		  '#type' => 'fieldset',
  		  '#title' => check_plain($node),
  		  '#collapsible' => TRUE,
        '#collapsed' => TRUE,
  		  '#prefix' => '<div style="float:left;width:50%;padding-top:5px">',
  		  '#suffix' => '</div>',
  		);
  		
  		$form['content_types'][$key]['option'] = array(
  		  '#type' => 'radios',
  		  '#title' => t('Name tagging options for ') . check_plain($node),
  		  '#options' => $select_options,
  		  '#parents' => array($key,'option'),
  		  '#default_value' => isset($nodes[$key]['select_option']) ? $nodes[$key]['select_option'] : 0,
  		  '#required' => TRUE,
  		  '#disabled' => $disabled,
  		);
  		
  		if($nodes[$key]['required'] == 1) { $default = 1; } else { $default = 0; }

  		$form['content_types'][$key]['required'] = array(
  		  '#type' => 'checkbox',
  		  '#title' => t('Required'),
  		  '#parents' => array($key, 'required'),
  		  '#default_value' => $default,
  		  '#disabled' => $disabled,
  		);
  		
  		$form['content_types'][$key]['weight'] = array(
  		  '#type' => 'select',
  		  '#title' => t('Weight'),
  		  '#description' => t('Smaller values position the name selection box higher on the page'),
  		  '#options' => $weight_options,
  		  '#parents' => array($key, 'weight'),
  		  '#default_value' => isset($nodes[$key]['weight']) ? $nodes[$key]['weight'] : 0,
  		  '#disabled' => $disabled,
  		);
  }
  
  $form['content_types']['autocomplete_size'] = array(
    '#type' => 'select',
    '#title' => t('Number of typed characters required in the autocomplete drop-down box'),
    '#default_value' => variable_get('classification_autocomplete_size',3),
    '#options' => array(3 => 3, 4 => 4, 5 => 5),
    '#prefix' => '<div style="clear:both"></div>',
  );

  $form['names_display_viewer'] = array(
    '#type' => 'fieldset',
    '#title' => t('Frontpage Classification Browser'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $default_viewer = variable_get('classification_viewer_names',2);
  $options_viewer = array(0 => t('Show all names (vernaculars, synonyms, <em>etc.</em>)'), 1 => t('Show valid names and synonyms (hide vernaculars)'), 2 => t('Only show valid names'));
  $default_canonicals = variable_get('classification_viewer_canonicals',1);
  $options_canonicals = array(1 => t('Yes'), 0 => t('No'));

  $form['names_display_viewer']['display_options_viewer'] = array(
    '#type' => 'radios',
    '#title' => t('Visibility of names'),
    '#options' => $options_viewer,
    '#default_value' => $default_viewer,
  );

  $form['names_display_viewer']['display_canonicals'] = array(
    '#type' => 'radios',
    '#title' => t('Convert names to canonical versions?'),
    '#description' => t('Canonical names are versions of names without the authority information.'),
    '#options' => $options_canonicals,
    '#default_value' => $default_canonicals,
  );

/*

Relationships configuration commented out because we don't yet know how to deal with these
There are instances in the editor where relationship types are hard-coded

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
*/

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
		  variable_set('classification_autocomplete_size',$form_state['values']['autocomplete_size']);
/*
      Commented out here until the above is resolved

      $relationships = explode("\n",$form_state['values']['relationships_list']);
      for ($i=0;$i<=count($relationships);$i++) {
      	$value = str_replace("\r","",trim($relationships[$i]));
      	db_query("INSERT INTO {term_relation_types} (name,namespace,description) VALUES ('%s', ' ', ' ')",$value);
      }
*/
      db_query("DELETE FROM {classification_node_types}");
      $node_types = node_get_types('names');
      
      foreach ($node_types as $key => $node) {
      	$node_form = $form_state['values'][$key];
      	if($node_form['option'] != 0) {
      		db_query("INSERT INTO {classification_node_types} (type,select_option,weight,required) VALUES ('%s',%d,%d,%d)",$key,$node_form['option'],$node_form['weight'], $node_form['required']);
      	}
      }
      
		  drupal_set_message(t('Configuration options have been saved.'),'status');
      break;
  }
  
}
