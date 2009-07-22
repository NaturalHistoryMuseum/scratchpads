<?php
function femail_user_settings($account){
  drupal_set_title(check_plain($account->name));
  
  // ---------------- Subscribe to forums --------------------------------------
  $output = theme('box',t('Subscriptions'),drupal_get_form('femail_user_subscribe', $account));
  
  // ---------------- Add additional emails ------------------------------------
  $header = array(t('Email'), t('Status'), t('Operations'));
  $rows = array();

  $result = db_query("SELECT * FROM {femail_user_emails} WHERE uid = %d", $account->uid);
  while ($mail_bits = db_fetch_array($result)) {
    $status = $mail_bits['status'] ? t('Verified') : t('Not verified');
    $rows[] = array(check_plain($mail_bits['email']), $status, 'Remove, Make primary');
  }

  $output .= theme('box', t('Your email addresses'), theme('table', $header, $rows) . drupal_get_form('femail_user_add', $account));
  
  return $output;
}

/**
 * Form for selecting which forums to subscribe to
 */
function femail_user_subscribe($form_state, $account){
  // It's OK to use this, NO FORUM IS HUGE!
  $forum_tree = taxonomy_get_tree(variable_get('forum_nav_vocabulary',0));  
  $forum_containers = variable_get('forum_containers', array());
  //print_r($forum_containers);exit;
  $is_odd = TRUE;
  $form['femail_subscribe'] = array(
    '#tree' => TRUE,
    '#prefix' => '<table><tr><th>'.t('Forums').'</th></tr>',
    '#suffix' => '</table>'
  );
  $results = db_query("SELECT tid FROM {femail_user_subscriptions} WHERE uid = %d", $account->uid);
  $subscriptions = array();
  while($row = db_fetch_array($results)){
    $subscriptions[$row['tid']] = $row['tid'];
  }
  foreach($forum_tree as $branch){
    if($class=="odd"){
      $class="even";
    } else {
      $class="odd";
    }
    if(in_array($branch->tid, $forum_containers)){
      // Container
      $form['femail_subscribe'][$branch->tid] = array(
        '#value' => '<tr class="'.$class.'"><td style="padding-left:'.(($branch->depth*10)+5).'px;">'.check_plain($branch->name).'</td></tr>'
      );
    } else {
      // Forum
      $form['femail_subscribe'][$branch->tid] = array(
        '#type' => 'checkbox',
        '#title' => check_plain($branch->name),
        '#default_value' => isset($subscriptions[$branch->tid])? 1 : 0,
        '#prefix' => '<tr class="'.$class.'"><td style="padding-left:'.(($branch->depth*10)+5).'px">',
        '#suffix' => '</td></tr>'
      );
    }
  }
  $form['femail_uid'] = array(
    '#type' => 'hidden',
    '#value' => $account->uid
  );
  $form['femail_subscribe_all'] = array(
    '#type' => 'checkbox',
    '#title' => t('Subscribe to all'),
    '#default_value' => isset($subscriptions[0])? 1 : 0,
    '#description' => t('Note, this is not the same as checking all the boxes above.  This option will automatically subscibe you to any new forums')
  );
  $form['submit'] = array('#type' => 'submit', '#value' => t('Subscribe'));
  return $form;
}

/**
 * 
 */
function femail_user_subscribe_submit($form, $form_state){
  // Lets delete everything from the femail_user_subscriptions for this user
  db_query("DELETE FROM {femail_user_subscriptions} WHERE uid = %d", $form_state['values']['femail_uid']);
  if($form_state['values']['femail_subscribe_all']){
    db_query("INSERT INTO {femail_user_subscriptions} (uid, tid) VALUES (%d, 0)", $form_state['values']['femail_uid']);
  } else {
    foreach($form_state['values']['femail_subscribe'] as $tid =>$value){
      if($value){
        db_query("INSERT INTO {femail_user_subscriptions} (uid, tid) VALUES (%d, %d)", $form_state['values']['femail_uid'], $tid);
      }
    }
  }
  drupal_set_message("Subscription updated");
}

/**
 * Following all stollen from the OpenID module
 */
function femail_user_add($form_state, $account) {
  $form['femail_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
  );
  $form['femail_uid'] = array(
    '#type' => 'hidden',
    '#value' => $account->uid
  );
  $form['submit'] = array('#type' => 'submit', '#value' => t('Add an additional Email'));
  return $form;
}

function femail_user_add_submit($form, $form_state){
  // FIXME - Send the email out for validation
  db_query("INSERT INTO {femail_user_emails} (uid, email, status) VALUES (%d, '%s', 0)", $form_state['values']['femail_uid'], $form_state['values']['femail_email']);
  drupal_set_message($form_state['values']['femail_email']." added. Please now check your email to validate this address.");
}

function femail_user_add_validate($form, &$form_state) {
  // Check for existing entries.
  if (db_result(db_query("SELECT email FROM {femail_user_emails} WHERE email='%s' UNION SELECT mail FROM {users} WHERE mail='%s'", $form_state['values']['femail_email'], $form_state['values']['femail_email']))) {
    form_set_error('femail_email', t('That Email is already in use on this site.'));
  }
}