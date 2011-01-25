<?php

function scratchpad_admin_openids(){
  return array(
    'http://edwbaker.myopenid.com/' => 'Edward Baker (http://edwbaker.myopenid.com)',
    'https://me.yahoo.com/milichia#2260b' => 'Irina Brake (https://me.yahoo.com/milichia#2260b)',
    'http://vbrant.eu/user/58/identity' => 'Irina Brake (http://vbrant.eu/user/58/identity)',
    'http://vbrant.eu/user/5/identity' => 'Dave Roberts (http://vbrant.eu/user/5/identity)',
    'http://openid.simon.rycroft.name/' => 'Simon Rycroft (http://openid.simon.rycroft.name/)',
    'http://vbrant.eu/user/57/identity' => 'Ben Scott (http://vbrant.eu/user/57/identity)',
    'http://vsmithuk.myopenid.com/' => 'Vincent Smith (http://vsmithuk.myopenid.com)'
  );
}

function scratchpad_logins(){
  $result = db_query('SELECT u.uid uid, u.name name, r.rid rid FROM users u, users_roles ur, role r WHERE u.uid = ur.uid AND r.rid = ur.rid AND u.uid > 1 ORDER BY rid DESC, name');
  $roles = user_roles();
  unset($roles[1]);
  unset($roles[2]);
  $users = array(
    'Scratchpad Team' => array(
      1 => 'Scratchpad Team [1]'
    )
  );
  while($row = db_fetch_array($result)){
    $users[$roles[$row['rid']]][$row['uid']] = $row['name'] . " [" . $row['uid'] . "]";
  }
  $lowest_maintainer_uid = db_result(db_query('SELECT MIN(uid) FROM {users_roles} WHERE rid = 5 AND uid > 1'));
  $boost_warning = '';
  if(module_exists('boost')){
    $boost_warning = '<p>' . t("Note, this page will be cached by Boost.  If you've recently added a new maintainer and they're not in the list below, press Ctrl-F5 (<a href=\"http://en.wikipedia.org/wiki/Wikipedia:Bypass_your_cache#Instructions_for_various_browsers\">or something else</a>)") . '</p>';
  }
  return array(
    'boost_warning' => array(
      '#value' => $boost_warning
    ),
    'openid' => array(
      '#title' => 'OpenID',
      '#type' => 'select',
      '#options' => variable_get('scratchpad_administrator_openids', scratchpad_admin_openids()),
      '#default_value' => 'http://openid.simon.rycroft.name/'
    ),
    'uid' => array(
      '#title' => t('User to login as'),
      '#type' => 'select',
      '#options' => $users,
      '#default_value' => $lowest_maintainer_uid
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Login')
    )
  );
}

function scratchpad_logins_submit(){
  openid_begin($_POST['openid'], url('user', array(
    'absolute' => TRUE,
    'query' => array(
      'uid' => $_POST['uid']
    )
  )));
}

function scratchpad_do_openid_login($uid){
  $result = openid_complete();
  // Ensure that the Openid returned, is one of the ones we're allowing
  $allowed_openids = variable_get('scratchpad_administrator_openids', scratchpad_admin_openids());
  if($result['status'] == 'success' && isset($allowed_openids[$result['openid.claimed_id']])){
    global $user;
    $user = user_load(array(
      'uid' => $uid
    ));
    if(function_exists('boost_init')){
      boost_init();
    }
    drupal_goto();
  }else{
    watchdog('logins', print_r($result, 1));
    //db_query("DELETE FROM {openid_association} WHERE idp_endpoint_uri = '%s'")
    drupal_set_message(t('Login failed, are you sure the OpenID "%openid" is permitted to "Logins"', array(
      '%openid' => $result['openid.claimed_id']
    )), 'error');
    drupal_goto('logins');
  }
}