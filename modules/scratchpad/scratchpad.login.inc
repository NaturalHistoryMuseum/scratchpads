<?php

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
    $boost_warning = t("Note, this page will be cached by Boost.  If you've recently added a new maintainer and they're not in the list below, press Ctrl-F5 (<a href=\"http://en.wikipedia.org/wiki/Wikipedia:Bypass_your_cache#Instructions_for_various_browsers\">or something else</a>)");
  }
  return array(
    'boost_warning' => array(
      '#value' => $boost_warning
    ),
    'openid' => array(
      '#title' => 'OpenID',
      '#type' => 'select',
      '#options' => variable_get('scratchpad_administrator_openids', array(
        'http://scratchpads.eu/' => 'http://scratchpads.eu/',
        'http://vsmith.info' => 'Vincent Smith (http://vsmith.info)',
        'http://edwbaker.myopenid.com' => 'Edward Baker (http://edwbaker.myopenid.com)',
        'https://me.yahoo.com/milichia#2260b' => 'Irina Brake (https://me.yahoo.com/milichia#2260b)'
      ))
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
  openid_begin($_POST['openid'], url('logins/return/' . $_POST['uid'], array(
    'absolute' => TRUE
  )));
}

function scratchpad_do_openid_login($uid){
  $result = openid_complete();
  if($result['status']){
    global $user;
    $user = user_load(array(
      'uid' => $uid
    ));
    if(function_exists('boost_init')){
      boost_init();
    }
  }else{
    drupal_set_message('Login failed', 'error');
  }
  drupal_goto();
}