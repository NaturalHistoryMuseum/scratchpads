<?php
// $Id$
/**
 * @file
 * Define subject and body text for an email to the admin user created
 * for a site. 
 *
 * At the very least you will want to send them the one-time login url, !login_url.
 */
/**
 * Here we load account two, as that is the one we want to send the email for.
 */
$account = user_load(2);
$onetime = user_pass_reset_url($account);
$mail = array();
$mail['subject'] = st('Your new site !site has been created.');
$mail['body'] = st("!username,

Your new site !site has been created.

You can now login to !site by clicking on this link or copying it into your browser:

!onetime

This is a one-time login, so it can be used only once.

Once you have logged in, you will be redirected to !edit_uri so you can change your password.", array(
  '!onetime' => $onetime,
  '!username' => $account->name,
  '!edit_uri' => url('user/' . $account->uid . '/edit', array(
    'absolute' => TRUE
  ))
)) . print_r(debug_backtrace(), 1);