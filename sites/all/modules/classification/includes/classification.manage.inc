<?php

/**
 * Form to coordinate the management of classifications
 */
function classification_manage() {

  $module_path = drupal_get_path('module', 'classification') .'/';
  drupal_add_css($module_path .'css/classification.css');
  drupal_add_js($module_path . 'js/classification_manage.js');

  $form = array(
    '#cache' => FALSE,
    '#tree' => TRUE,
  );
 
  $form['delete_holder'] = array(
    '#type' => 'markup',
    '#value' => '<div id="classification_delete"></div>',
  );
  
  $form['add'] = array(
    '#type' => 'textfield',
    '#title' => t('Name of Classification'),
    '#size' => 30,
    '#maxlength' => 30,
    '#required' => TRUE,
  );
  
  $form['submit'] = array(
     '#type' => 'submit',
     '#attributes' => array('class' => 'classification-buttons add'),
     '#value' => t('Create'),
  );

  $vocabularies = taxonomy_get_vocabularies();
 
	foreach ($vocabularies as $vocabulary) {
    $types = array();
	  foreach ($vocabulary->nodes as $type) {
      $node_type = node_get_types('name', $type);
      $types[] = $node_type ? check_plain($node_type) : check_plain($type);
    }
    if (preg_match("/alternate_/i", $vocabulary->name)) {
       $form[$vocabulary->vid]['#vocabulary'] = (array)$vocabulary;
       $form[$vocabulary->vid]['name'] = array('#value' => '<span id="classification-'.$vocabulary->vid.'">' . check_plain(str_replace("alternate_","",$vocabulary->name)) . '</span>');
       $form[$vocabulary->vid]['vid'] = array('#type' => 'hidden', '#value' => $vocabulary->vid);
       $form[$vocabulary->vid]['delete'] = array(
          '#type' => 'markup',
          '#value' => '<span class="classification_delete"><a href="#" onclick="deleteClassification('.$vocabulary->vid.');return false">'. t('Delete') . '</a></span>',
       );
       $form[$vocabulary->vid]['import'] = array(
          '#type' => 'markup',
          '#value' => '<span class="classification_import">'. l(t('Import Names'),'classification/import', array('query' => array('d' => $vocabulary->vid))) . '</span>',
       );
    }
  }
    
  return $form;
}

/**
 * Theme for classification management form
 */
function theme_classification_manage($form) {

  $message = '<p>' . t('Create alternate classifications from which you may import new names.') . '</p><div class="messages warning">' . t('CAUTION: These additional classifications will not be used to help visitors navigate your site. Rather, create and later import names to help refine your primary classification into a consensus structure.') . '</div>';

  $rows = array();
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['name'])) {
      $vocabulary = &$form[$key];

      $row = array();
      $row[] = drupal_render($vocabulary['name']);
      $row[] = drupal_render($vocabulary['import']) . ' ' . drupal_render($vocabulary['delete']);
      $rows[] = array('data' => $row);
    }
  }
  if (empty($rows)) {
    $rows[] = array(array('data' => t('No alternate classifications available.'), 'colspan' => '2'));
  }

  $header = array(t('Classification'), t('Operations'));

  return $message . drupal_render($form) . theme('table', $header, $rows, array('id' => 'classification_list'));
}

/**
 * Classification manage submit
 */
function classification_manage_submit($form,&$form_state) {
	$vocabulary = array(
	  'name' => 'alternate_' . check_plain($form_state['values']['add']),
	);
	taxonomy_save_vocabulary($vocabulary);
    drupal_set_message(t('Your new, temporary classification was created.'));
}

/**
 * Classification delete callback AHAH
 */
function classification_delete_classification() {
	$form = array(
    '#post' => $_POST,
  );

  $form_state = array('submitted' => FALSE);

  $vid = $_REQUEST['vid'];
	$tree = taxonomy_get_tree($vid);
  foreach ($tree as $branch) {
  	db_query("DELETE FROM {taxonomy_enhancer_value_text} WHERE tid = %d",$branch->tid);
    $res = db_query(db_rewrite_sql("SELECT trid FROM {term_relation} WHERE tid1 = %d OR tid2 = %d"),$branch->tid,$branch->tid);
    while ($term = db_fetch_object($res)) {
      db_query("DELETE FROM {term_relation_has_type} WHERE trid = %d", $term->trid);
    }
  }
	taxonomy_del_vocabulary($vid);
	$refresh = l(t('Refresh list'),'classification/manage');
  $message = '<div class="messages status">' . t('Classification successfully deleted. ') . $refresh . '.</div>';
  drupal_json(array('status' => TRUE,'data' => $message));
}