<?php
function femail_user_settings($account){
  drupal_set_title(check_plain($account->name));

  $header = array(t('Email'), t('Status'), t('Operations'));
  $rows = array();

  $result = db_query("SELECT * FROM {femail_user_emails} WHERE uid = %d", $account->uid);
  while ($mail_bits = db_fetch_array($result)) {
    $status = $mail_bits ? t('Verified') : t('Not verified');
    $rows[] = array(check_plain($mail_bits['mail']), $status, '');
  }

  $output = theme('table', $header, $rows);
  $output .= drupal_get_form('femail_user_add');
  return $output;
}

/**
 * Following all stollen from the OpenID module
 */
function femail_user_add() {
  $form['femail_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
  );
  $form['submit'] = array('#type' => 'submit', '#value' => t('Add an additional Email'));
  return $form;
}

function femail_user_add_validate($form, &$form_state) {
  // Check for existing entries.
  if (db_result(db_query("SELECT email FROM {femail_user_emails} WHERE email='%s' UNION SELECT mail FROM {users} WHERE mail='%s'", $form_state['values']['femail_email'], $form_state['values']['femail_email']))) {
    form_set_error('femail_email', t('That Email is already in use on this site.'));
  } else {
    // Sod it, lets just do the insert here.
    global $user;
    db_query("INSERT INTO {femail_user_emails} (uid, email, status) VALUES (%d, '%s', 0)", $user->uid, $form_state['values']['femail_email']);
    drupal_goto('user/'.arg(1).'/femail');
  }
}