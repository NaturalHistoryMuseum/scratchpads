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
    drupal_set_message('No key is set, you can not login.  Please contact this site\'s maintainer');
    drupal_goto();
  }
  // Load the class file
  module_load_include('inc', 'uwho_site', 'uwho.cryptastic');
  $cryptastic = new cryptastic();
  $key = $cryptastic->pbkdf2($key, $key . $url, 1000, 32);
  $decrypted = $cryptastic->decrypt($_GET['uwhodata'], $key);
  if(!$decrypted){return $_GET['uwhodata'];}
  return print_r($decrypted, 1);
}