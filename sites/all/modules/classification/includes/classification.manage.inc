<?php

/**
 * Form to coordinate the management of classifications
 */
function classification_manage() {

  $module_path = drupal_get_path('module', 'classification') .'/';
  drupal_add_css($module_path .'css/classification.css');

  $form = array(
    '#cache' => FALSE,
    '#tree' => TRUE,
  );
 
  $form['delete_holder'] = array(
    '#type' => 'markup',
    '#value' => '<div id="classification_delete"></div>',
  );
  
  $form['title'] = array(
    '#type' => 'markup',
    '#value' => '<h3>' . t('Manage Classification(s)') . '</h3>',
  );
  
  $form['add'] = array(
    '#type' => 'textfield',
    '#title' => t('Add a New Classification'),
    '#size' => 30,
    '#maxlength' => 30,
    '#required' => TRUE,
  );
  
  $form['submit'] = array(
     '#type' => 'submit',
     '#attributes' => array('class' => 'classification-buttons add'),
     '#value' => t('Create!'),
  );

  $vocabularies = taxonomy_get_vocabularies();
 
	foreach ($vocabularies as $vocabulary) {
    $types = array();
	  foreach ($vocabulary->nodes as $type) {
      $node_type = node_get_types('name', $type);
      $types[] = $node_type ? check_plain($node_type) : check_plain($type);
    }
    if( check_plain($vocabulary->name) !== 'Taxon Description Chapters' ) {
       $form[$vocabulary->vid]['#vocabulary'] = (array)$vocabulary;
       $form[$vocabulary->vid]['name'] = array('#value' => check_plain($vocabulary->name));
       $form[$vocabulary->vid]['vid'] = array('#type' => 'hidden', '#value' => $vocabulary->vid);
       if(check_plain($vocabulary->name) !== 'Taxa') {
          $form[$vocabulary->vid]['delete'] = array(
             '#value' => t('Delete'),
             '#type' => 'button',
             '#attributes' => array('class' => 'classification-buttons delete'),
             '#ahah' => array(
                'event' => 'click',
                'path' => 'admin/classification/delete_classification/' . $vocabulary->vid,
                'wrapper' => 'classification_delete',
                'method' => 'replace',
                'effect' => 'fade',
                'progress' => array('type' => 'none', 'message' => ''),
             ),
          );
       }
       else {
          $form[$vocabulary->vid]['delete'] = array(
             '#value' => t('Delete'),
             '#type' => 'button',
             '#attributes' => array('class' => 'classification-buttons delete'),
             '#disabled' => TRUE,
          );
       }	
    }
  }
    
  return $form;
}

/**
 * Theme for classification management form
 */
function theme_classification_manage($form) {
	$output = '';
  $rows = array();
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['name'])) {
      $vocabulary = &$form[$key];

      $row = array();
      $row[] = drupal_render($vocabulary['name']);
      $row[] = drupal_render($vocabulary['delete']);
      $rows[] = array('data' => $row);
    }
  }
  if (empty($rows)) {
    $rows[] = array(array('data' => t('No vocabularies available.'), 'colspan' => '2'));
  }

  $header = array(t('Name'), t('Operation'));

  return drupal_render($form) . theme('table', $header, $rows, array('id' => 'classification_list'));
}

/**
 * Classification manage submit
 */
function classification_manage_submit($form,&$form_state) {
	$vocabulary = array(
	  'name' => check_plain($form_state['values']['add']),
	);
	taxonomy_save_vocabulary($vocabulary);
  drupal_set_message(t('Your new classification was created.'));
}

/**
 * Classification delete callback AHAH
 */
function classification_delete_classification($vid) {
	$form_state = array('submitted' => FALSE);
	$tree = taxonomy_get_tree($vid);
  foreach ($tree as $branch) {
  	db_query("DELETE FROM {taxonomy_enhancer_value_text} WHERE tid = %d",$branch->tid);
    $res = db_query(db_rewrite_sql("SELECT trid FROM {term_relation} WHERE tid1 = %d OR tid2 = %d"),$branch->tid,$branch->tid);
    while ($term = db_fetch_object($res)) {
      db_query("DELETE FROM {term_relation_has_type} WHERE trid = %d", $term->trid);
    }
  }
	taxonomy_del_vocabulary($vid);
	$refresh = l(t('Refresh list'),'admin/classification/manage');
  $message = '<div class="messages status">' . t('Name metadata successfully updated. ') . $refresh . '.</div>';
  drupal_json(array('status' => TRUE,'data' => $message));
}