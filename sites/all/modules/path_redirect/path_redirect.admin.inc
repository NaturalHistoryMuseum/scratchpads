<?php
// $Id: path_redirect.admin.inc,v 1.1.2.21 2009/01/27 19:30:56 davereid Exp $

/**
 * @file
 * Administrative page callbacks for the path_redirect module.
 */

/**
 * Render a list of redirects for the main admin page.
 */
function path_redirect_admin() {
  $header = array(
    array('data' => t('From'), 'field' => 'path', 'sort' => 'asc'),
    array('data' => t('To'), 'field' => 'redirect'),
    array('data' => t('Type'), 'field' => 'type'),
    array('data' => t('Operations'), 'colspan' => '2')
  );

  $result = pager_query('SELECT rid, path, redirect, query, fragment, type FROM {path_redirect}'. tablesort_sql($header), 50);

  $rows = array();
  while ($r = db_fetch_object($result)) {
    $redirect = url($r->redirect, array('query' => $r->query, 'fragment' => $r->fragment, 'absolute' => TRUE));
    $rows[] = array(
      // @todo: Revise the following messy, confusing line.
      l($r->path, preg_replace('/\?.*/', '', $r->path), array('query' => strstr($r->path, '?') ? preg_replace('/.*\?/', '', $r->path) : NULL)),
      l($redirect, $redirect, array('external' => TRUE)),
      $r->type,
      l(t('edit'), 'admin/build/path-redirect/edit/'. $r->rid, array('query' => drupal_get_destination())),
      l(t('delete'), 'admin/build/path-redirect/delete/'. $r->rid, array('query' => drupal_get_destination())),
    );
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No redirects have been added.'), 'colspan' => '5'));
  }

  $output = theme('table', $header, $rows, array('class' => 'path-redirects'));
  $output .= '<p>'. l(t('Add redirect'), 'admin/build/path-redirect/add') .'</p>';
  $output .= theme('pager');

  return $output;
}

/**
 * Callback for add and edit pages.
 *
 * @return
 *   A form for drupal_get_form.
 */
function path_redirect_edit($form_state, $rid = FALSE) {
  if ($rid) {
    $redirect = path_redirect_load($rid);
    drupal_set_title(check_plain($redirect['path']));
    $output = path_redirect_edit_form($form_state, $redirect);
  }
  else {
    drupal_set_title(t('Add redirect'));
    $output = path_redirect_edit_form($form_state);
  }
  return $output;
}

function path_redirect_edit_form(&$form_state, $edit = array()) {
  // Merge default values.
  $edit += array(
    'path' => '',
    'redirect' => '',
    'query' => '',
    'fragment' => '',
    'type' => variable_get('path_redirect_default_status', 301),
    'rid' => NULL,
  );

  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('From'),
    '#description' => t('Enter a Drupal path or path alias to redirect. Fragment anchors <em>#foo</em> are <strong>not</strong> allowed.'),
    '#size' => 42,
    '#maxlength' => 255,
    '#default_value' => $edit['path'],
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
  );

  $form['redirect'] = array(
    '#type' => 'item',
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
    '#title' => t('To'),
    '#description' => '<div style="display:block">'. t('Enter a Drupal path, path alias, or external URL to redirect to. Use %front to redirect to the front page. Enter (optional) queries after "?" and (optional) anchor after "#". Most redirects will not contain queries or fragment anchors.', array('%front' => '<front>')) .'</div>',
  );

  $form['redirect']['redirect'] = array(
    '#type' => 'textfield',
    '#size' => 30,
    '#maxlength' => 255,
    '#default_value' => $edit['redirect'],
  );

  $form['redirect'][] = array(
    '#value' => '?',
  );

  $form['redirect']['query'] = array(
    '#type' => 'textfield',
    '#size' => 12,
    '#maxlength' => 255,
    '#default_value' => $edit['query'],
  );

  $form['redirect'][] = array(
    '#value' => '#',
  );

  $form['redirect']['fragment'] = array(
    '#type' => 'textfield',
    '#size' => 12,
    '#maxlength' => 50,
    '#default_value' => $edit['fragment'],
  );

  $form[] = array(
    '#value' => "<p> </p>", // little bit of extra space
  );

  $form['type'] = array(
    '#type' => 'fieldset',
    '#title' => t('Redirect Type'),
    '#collapsible' => TRUE,
    '#collapsed' => ($edit['type'] == variable_get('path_redirect_default_status', 301)),
  );

  foreach (path_redirect_status_codes() as $code => $info) {
    $form['type'][]['type'] = array(
      '#type' => 'radio',
      '#title' => $info['title'],
      '#description' => $info['description'],
      '#return_value' => $code,
      '#default_value' => $edit['type'],
    );
  }
  $form['type']['link'] = array(
    '#type' => 'markup',
    '#value' => '<p>'. t('You can find more information about HTTP redirect status codes at <a href="@link">@link</a>.', array('@link' => 'http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10.3')) .'</p>',
  );

  $form['rid'] = array(
    '#type' => 'hidden',
    '#value' => $edit['rid'],
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $edit['rid'] ? t('Update redirect') : t('Create new redirect'),
  );

  return $form;
}

function path_redirect_edit_validate($form, &$form_state) {
  if (trim($form_state['values']['path']) == '') {
    form_set_error('path', t('You must enter a <strong>from</strong> path.'));
  }
  else {
    $path_error = '';

    // The "from" path should not conflict with another redirect
    $result = path_redirect_load(NULL, $form_state['values']['path']);
    if ($result && (!$form_state['values']['rid'] || ($form_state['values']['rid'] !== $result['rid']))) {
      $path_error .= ' '. t('The <strong>from</strong> path you entered is already redirected. You can <a href="@edit-page">edit this redirect</a> instead.', array('@edit-page' => url('admin/build/path-redirect/edit/'. $result['rid'])));
    }
    // Check that the "from" path is valid and contains no # fragment
    if (strstr($form_state['values']['path'], '#')) {
      $path_error .= ' '. t('You cannot redirect from a fragment anchor.');
    }
    // Make sure "from" has the form of a local Drupal path
    if (!valid_url($form_state['values']['path'])) {
      $path_error .= ' '. t('The redirect <strong>from</strong> path does not appear valid. This must be a local Drupal path.');
    }

    if (!empty($path_error)) {
      form_set_error('path', $path_error);
    }
  }

  if (!valid_url($form_state['values']['redirect']) && !valid_url($form_state['values']['redirect'], TRUE) && $form_state['values']['redirect'] != '<front>') {
    form_set_error('redirect', t('The redirect <strong>to</strong> path does not appear valid.'));
  }

  if ($form_state['values']['redirect'] == '<front>') {
    $form_state['values']['redirect'] = variable_get('site_frontpage', 'node');
  }

  // check that there there are no redirect loops
  if ($form_state['values']['path'] === $form_state['values']['redirect']) {
    form_set_error('redirect', t('You are attempting to redirect the page to itself. This will result in an infinite loop.'));
  }

  // Allow spaces in "from" path
  $form_state['values']['path'] = str_replace("+", " ", $form_state['values']['path']);
}

function path_redirect_edit_submit($form, &$form_state) {
  path_redirect_save($form_state['values']);
  drupal_set_message(t('Redirect has been saved.'));
  drupal_goto('admin/build/path-redirect');
}

function path_redirect_delete_confirm($form_state, $rid) {
  $form['rid'] = array(
    '#type' => 'value',
    '#value' => $rid,
  );
  $redirect = path_redirect_load($rid);
  return confirm_form($form,
    t('Are you sure you want to delete the redirect from %path to %redirect?', array('%path' => $redirect['path'], '%redirect' => $redirect['redirect'])),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/build/path-redirect');
}

function path_redirect_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    path_redirect_delete(NULL, NULL, $form_state['values']['rid']);
    drupal_set_message(t('The redirect has been deleted.'));
  }
}

function path_redirect_settings() {
  $form['path_redirect_redirect_warning'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display a warning message to users when they are redirected.'),
    '#default_value' => variable_get('path_redirect_redirect_warning', 0),
  );

  $form['path_redirect_allow_bypass'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow users to bypass redirects by adding %code to the URL.', array('%code' => variable_get('clean_url', 0) ? '?redirect=no' : '&redirect=no')),
    '#default_value' => variable_get('path_redirect_allow_bypass', 0),
  );

  $form['path_redirect_default_status'] = array(
    '#type' => 'select',
    '#title' => t('Default redirect status'),
    '#description' => t('You can find more information about HTTP redirect status codes at <a href="@link">@link</a>.', array('@link' => 'http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10.3')),
    '#default_value' => variable_get('path_redirect_default_status', 301),
    '#options' => array(),
  );
  foreach (path_redirect_status_codes() as $code => $details) {
    $form['path_redirect_default_status']['#options'][$code] = $details['title'];
  }

  return system_settings_form($form);
}

/**
 * Return an array of 300-range status codes.
 */
function path_redirect_status_codes() {
  return array(
    300 => array(
      'title' => t('300 Multiple Choices'),
      'description' => t('The request is ambiguous and needs clarification as to which resource was requested.'),
    ),
    301 => array(
      'title' => t('301 Moved Permanently'),
      'description' => t('Moved Permanently. The resource has permanently moved elsewhere, the response indicates where it has gone to. <strong>Recommended.</strong>'),
    ),
    302 => array(
      'title' => t('302 Found'),
      'description' => t("The resource has temporarily moved elsewhere, the response indicates where it is at present. <strong>This is Drupal's default redirect type.</strong>"),
    ),
    303 => array(
      'title' => t('303 See Other'),
      'description' => t('See Other/Redirect. A preferred alternative source should be used at present.'),
    ),
    304 => array(
      'title' => t('304 Not Modified'),
      'description' => t("The server has identified from the request information that the client's copy of the information is up-to-date and the requested information does not need to be sent again."),
    ),
    305 => array(
      'title' => t('305 Use Proxy'),
      'description' => t('The request must be sent through the indicated proxy server.'),
    ),
    307 => array(
      'title' => t('307 Temporary Redirect'),
      'description' => t('The resource has temporarily moved elsewhere, the response indicates where it is at present. Client should still use this URL.'),
    ),
  );
}
