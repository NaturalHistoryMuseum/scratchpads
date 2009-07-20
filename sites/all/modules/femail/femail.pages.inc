<?php
function femail_user_settings($account){
  drupal_set_title(check_plain($account->name));
  drupal_add_css(drupal_get_path('module', 'openid') .'/openid.css', 'module');

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
 * Form builder; Add an OpenID identity.
 *
 * @ingroup forms
 * @see openid_user_add_validate()
 */
function femail_user_add() {
  $form['openid_identifier'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
  );
  $form['submit'] = array('#type' => 'submit', '#value' => t('Add an additional Email'));
  return $form;
}