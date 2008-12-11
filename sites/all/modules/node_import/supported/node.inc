<?php
/* $Id: node.inc,v 1.11.2.1.2.8 2008/10/29 21:13:20 robrechtj Exp $ */

/**
 * Implementation of hook_node_import_types().
 */
function node_node_import_types() {
  $types = node_get_types();
    
  $import_types = array();
  foreach ($types as $type => $info) {
    $import_types[$type] = $info->name;
  }

  return $import_types;
}

/**
 * Implementation of hook_node_import_fields().
 */
function node_node_import_fields($type) {
  $fields = array();
  if (user_access('administer nodes')) {
    $fields = array(
      'name' => t('Node: Authored by'),
      'date' => t('Node: Authored on'),
      'updated' => t('Node: Last updated on'),
      'status' => t('Node: Published'),
      'moderate' => t('Node: In moderation queue'),
      'promote' => t('Node: Promoted to front page'),
      'sticky' => t('Node: Sticky at top of lists'),
      'revision' => t('Node: Create new revision'),
    );
  }

  $type_info = node_get_types('type', $type);

  if ($type_info->has_title) {
    $fields['title'] = $type_info->title_label;
  }
  if ($type_info->has_body) {
    $fields['body'] = $type_info->body_label;
  }

  return $fields;
}

/**
 * Implementation of hook_node_import_prepare().
 */
function node_node_import_prepare(&$node, $preview = FALSE) {
  global $user;

  $type_info = node_get_types('type', $node->type);

  $globals = $node->node_import_node;
  //unset($node->node_import_node);

  $author = $globals['author'];
  $options = $globals['options'];
  $unique_title = $globals['unique_title'];

  $errors = array();

  // 1. User name and uid.

  if (isset($node->name)) {
    // We have a mapped username.
    if (empty($node->name)) {
      unset($node->name);
    }
    else if (($uid = node_import_userreference($node->name))) {
      $account = user_load(array('uid' => $uid));
      $node->name = $account->name;
      $node->uid = $account->uid;
    }
    else {
      $errors[] = t('The username %name does not exist.', array('%name' => $node->name));
      unset($node->name);
    }
  }

  if (!isset($node->name) && isset($author)) {
    // We don't have a name yet (not mapped or error above), but we
    // do have some global options.
    if ($author['name'] == '') {
      $node->name = '';
      $node->uid = 0;
    }
    else if (($uid = node_import_userreference($author['name']))) {
      $account = user_load(array('uid' => $uid));
      $node->name = $account->name;
      $node->uid = $account->uid;
    }
    else {
      $errors[] = t('The username %name does not exist.', array('%name' => $account->name));
      unset($node->name);
    }
  }

  if (!isset($node->name)) {
    // We still don't have a name yet, use the current user.
    $node->name = $user->name;
    $node->uid = $user->uid;
  }

  // 2. Creation date.

  if (isset($node->date)) {
    // We have a mapped date.
    if (empty($node->date)) {
      unset($node->date);
    }
    else if (($date = node_import_valid_date($node->date)) > -1) {
      $node->date = $date;
    }
    else {
      $errors[] = t('The date %date is not a valid date.', array('%date' => $node->date));
      unset($node->date);
    }
  }

  if (!isset($node->date) && isset($author)) {
    // We don't have a date yet (not mapped or error above), but we
    // do have some global options.
    if ($author['date'] == '') {
      $node->date = time();
    }
    else if (($date = node_import_valid_date($author['date'])) > -1) {
      $node->date = $date;
    }
    else {
      $errors[] = t('The date %date is not a valid date.', array('%date' => $author['date']));
    }
  }

  if (!isset($node->date)) {
    // We still don't have a date yet, use the current time.
    $node->date = time();
  }

  $node->created = $node->date;
  $node->date = format_date($node->created, 'custom', 'Y-m-d H:i:s O');

  // 3. Last updated

  if (isset($node->updated)) {
    // We have a mapped updated date.
    if (empty($node->updated)) {
      unset($node->updated);
    }
    else if (($date = node_import_valid_date($node->updated)) > -1) {
      $node->updated = $date;
    }
    else {
      $errors[] = t('The date %date is not a valid date.', array('%date' => $node->updated));
      unset($node->updated);
    }
  }

  if (!isset($node->updated)) {
    // We still don't have a date yet, use the current time.
    $node->updated = time();
  }

  // 4. Options (published, promoted, sticky, moderated, new revision).

  if (!isset($options)) {
    $options = array();
    foreach ((array)variable_get('node_options_'. $node->type, array('status', 'promote')) as $option) {
      $options[$option] = 1;
    }
  }
  $all_options = array('status', 'moderate', 'promote', 'sticky', 'revision');

  foreach ($all_options as $key) {
    if (isset($node->$key) && strlen($node->$key) > 0) {
      // If the field was mapped, use that value.
      $node->$key = ($node->$key ? 1 : 0);
    }
    else {
      // If not, use the global option.
      $node->$key = (isset($options[$key]) ? $options[$key] : 0);
    }
  }

  // 5. Title.

  if ($type_info->has_title && (!isset($node->title) || empty($node->title))) {
    $errors[] = t('You need to provide a non-empty title.');
  }
  else if ($unique_title) {
    $count = db_fetch_object(db_query("SELECT count(*) AS cnt FROM {node} WHERE title = '%s' AND type = '%s'", $node->title, $node->type));
    if ($count->cnt > 0) {
      $errors[] = t('The node title %title is not unique for this node type.', array('%title' => $node->title));
    }
  }

  return $errors;
}

/**
 * Implementation of hook_node_import_global().
 */
function node_node_import_global($type, $global_values) {
  global $user;
  $globals = $global_values['node_import_node'];

  if (isset($globals['options'])) {
    $options = array();
    foreach ($globals['options'] as $option => $value) {
      if ($value) {
        $options[] = $option;
      }
    }
    $globals['options'] = $options;
  }
  else {
    $defaults = array('status', 'promote');
    $globals['options'] = variable_get('node_options_'. $type, $defaults);
  }

  if (!isset($globals['author'])) {
    $globals['author'] = array(
      'name' => $user->name,
      'date' => '',
    );
  }

  if (!isset($globals['unique_title'])) {
    $globals['unique_title'] = 0;
  }

  // Create a dummy node and get the node form.
  $node = array(
    'type' => $type,
    'date' => date('r'),
    'name' => $user->name,
    'uid' => $user->uid,
  );
  $node = array_merge($node, $globals['author']);
  foreach ((array)$globals['options'] as $option) {
    $node[$option] = 1;
  }
  $node = (object)$node;
  $node_form = node_form($node);

  // We only care about a part of this node_form.
  $form = array();
  $form['node_import_node'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node options'),
    '#description' => t('Select the options you want to assign to all imported nodes unless specifically set otherwise in the CSV file'),
    '#tree' => TRUE,
  );
  foreach (array('author', 'options') as $fieldset) {
    if (isset($node_form[$fieldset])) {
      $form['node_import_node'][$fieldset] = $node_form[$fieldset];
      if ($fieldset == 'options') {
        // The workflow options are overwritten in node_form_array()
        // because $node->nid is not set.
        foreach (element_children($form['node_import_node']['options']) as $key) {
          $form['node_import_node']['options'][$key]['#default_value'] = $node->$key;
        }
      }
    }
  }

  // Unique titles?
  $form['node_import_node']['unique_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Titles must be unique for this node type.'),
    '#description' => t('Check this box if you do not want to import nodes if there is already a node with the same title.'),
    '#default_value' => $globals['unique_title'],
  );

  return $form;
}
