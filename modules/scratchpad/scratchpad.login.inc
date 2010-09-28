<?php

function scratchpad_logins(){
  $result = db_query('SELECT uid, name FROM {users} WHERE uid > 0 AND status = 1 AND ORDER BY uid ASC');
  while($row = db_fetch_array($result)){
    $users[$row['uid']] = $row['name'] . " [" . $row['uid'] . "]";
  }
  $lowest_maintainer_uid = db_result(db_query('SELECT MIN(uid) FROM {users_roles} WHERE rid = 5 AND uid > 1'));
  return array(
    'openid' => array(
      '#title' => 'OpenID',
      '#type' => 'select',
      '#options' => array(
        'http://scratchpads.eu/' => 'http://scratchpads.eu/',
        'http://vsmith.info' => 'http://vsmith.info'
      )
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
  openid_begin($_POST['openid'], url('logins/return/'.$_POST['uid'], array('absolute' => TRUE)));
}

function scratchpad_do_openid_login($uid){
  $result = openid_complete();
  if($result['status']){
    global $user;
    $user = user_load(array('uid' => $uid));
  } else {
    drupal_set_message('Login failed', 'error');
  }
  drupal_goto();
}