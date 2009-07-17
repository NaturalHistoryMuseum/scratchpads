<?php

/**
 * femail_link theme
 */
function theme_femail_link($tid){
  $emails = variable_get('femail_emails', array());
  if(isset($emails[$tid])){
    return '<p>'.t('You may post new topics to this forum by email:') .' '. l($emails[$tid], 'mailto:'.$emails[$tid], array('absolute'=> TRUE)) . '</p>';
  } else {
    // This should be set, we need to warn the admin
    watchdog('femail', 'Femail address not set %term_id', array('%term_id', $tid));
  }
}