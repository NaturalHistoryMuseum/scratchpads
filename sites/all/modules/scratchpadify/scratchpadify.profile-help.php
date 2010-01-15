<?php
function _scratchpadify_install_configure_form_submit($form, &$form_state) {
  global $user;

  variable_set('site_name', $form_state['values']['site_name']);
  variable_set('site_mail', $form_state['values']['site_mail']);
  variable_set('date_default_timezone', $form_state['values']['date_default_timezone']);

  // Enable update.module if this option was selected.
  if ($form_state['values']['update_status_module'][1]) {
    // Stop enabling the update module, it's a right royal pain in the arse.
    //drupal_install_modules(array('update'));
  }

  // Turn this off temporarily so that we can pass a password through.
  variable_set('user_email_verification', FALSE);
  $form_state['old_values'] = $form_state['values'];
  $form_state['values'] = $form_state['values']['account'];

  // We precreated user 1 with placeholder values. Let's save the real values.
  $account = user_load(1);
  $merge_data = array('init' => $form_state['values']['mail'], 'roles' => array(), 'status' => 0);
  user_save($account, array_merge($form_state['values'], $merge_data));
  // Log in the first user.
  user_authenticate($form_state['values']);
  $form_state['values'] = $form_state['old_values'];
  unset($form_state['old_values']);
  variable_set('user_email_verification', TRUE);

  if (isset($form_state['values']['clean_url'])) {
    variable_set('clean_url', $form_state['values']['clean_url']);
  }
  // The user is now logged in, but has no session ID yet, which
  // would be required later in the request, so remember it.
  $user->sid = session_id();

  // Record when this install ran.
  variable_set('install_time', time());
}