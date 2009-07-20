<?php
function femail_user_settings($account){
  drupal_set_title(check_plain($account->name));
  drupal_add_css(drupal_get_path('module', 'openid') .'/openid.css', 'module');

  $header = array(t('OpenID'), t('Operations'));
  $rows = array();

  $result = db_query("SELECT * FROM {authmap} WHERE module='openid' AND uid=%d", $account->uid);
  while ($identity = db_fetch_object($result)) {
    $rows[] = array(check_plain($identity->authname), l(t('Delete'), 'user/'. $account->uid .'/openid/delete/'. $identity->aid));
  }

  $output = theme('table', $header, $rows);
  $output .= drupal_get_form('openid_user_add');
  return $output;
}