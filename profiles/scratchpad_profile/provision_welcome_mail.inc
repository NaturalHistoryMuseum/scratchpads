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
 * Adjust the UID 1.
 */
$account = user_load(1);
$account->name = 'Scratchpad Team';
$account->pass = 'no-direct-login';
$account->mail = 'scratchpad@nhm.ac.uk';
user_save($account);
/**
 * Here we load account two, as that is the one we want to send the email for.
 */
$account = user_load(2);
$onetime = user_pass_reset_url($account);
$mail = array();
$mail['subject'] = st('Your new site !site has been created.');
//$mail['bcc'] = 'Scratchpad Team <scratchpad@nhm.ac.uk>';
$mail['body'] = st("Dear !username,

!uri

Your new Scratchpad \"!site\" has been created for you

You can now login to !site by clicking on this link or copying it into your browser:

!onetime

This is a one-time login, so it can be used only once.

Once you have logged in, you will be redirected to !edit_uri so you can change your password.

Further help is available on the Scratchpad website [1], or if you are still having difficulty, simply reply to this message, and one of the us will get back to you.

The Scratchpad Team

[1] http://scratchpads.eu/help

--
http://scratchpads.eu
scratchpad@nhm.ac.uk", array(
  '!onetime' => $onetime,
  '!username' => $account->name,
  '!edit_uri' => url('user/' . $account->uid . '/edit', array(
    'absolute' => TRUE
  ))
));