<?php

/**
 * Callback that is the core of this module.  Here we'll decode the data sent to
 * us, check it, and then create a user (if required), and log the user in.  We
 * can also redirect to a set page if required.
 */
function uwho_site_callback(){
  // First, we need to get the key that has been set, if no key is set, we 
  // panic, redirect to the front page, and display an error.
  $key = variable_get('uwho_site_key', FALSE);
  $url = url('uwho', array(
    'absolute' => TRUE
  ));
  //echo substr($_GET['q'], 5);exit;
  // If one of the essential settings hasn't been set, or we're not logged in!
  if(!$key){
    drupal_set_message('No key has been set, you can not login.  Please contact this site\'s maintainer');
    drupal_goto();
  }
  // Load the class file
  module_load_include('inc', 'uwho_site', 'uwho.cryptastic');
  $cryptastic = new cryptastic();
  $key = $cryptastic->pbkdf2($key, $key . $url, 1000, 32);
  $decrypted = $cryptastic->decrypt($_GET['uwhodata'], $key);
  if(!$decrypted){
    // We've failed to decrypt the stuff, doh!  Error, and redirect to the front
    // page.
    drupal_set_message('Failed to decrypt your data.  Please inform a maintainer', 'error');
    watchdog('uwho', 'Decryption failed');
    drupal_goto();
  }
  // FIXME - Keep a record of users created this way.
  $new_user = new stdClass();
  $new_user->name = $decrypted['user']['name'] . 'uwho';
  $new_user->pass = md5(rand());
  $new_user->mail = $decrypted['user']['mail'];
  $new_user->status = 1;
  $new_user->url = $decrypted['user']['url'];
  global $user;
  $user = user_save($new_user, (array)$new_user);
  drupal_goto(variable_get('uwho_site_redirect_path', ''));
}