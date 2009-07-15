<?php

function femail_drush_command() {
  return array(
    'femail' => array(
      'callback' => 'femail_drush_callback',
      'description' => 'Drush command for importing mail messages into a forum'
    )
  );
}

function femail_drush_help($section) {
  switch ($section) {
    case 'drush:femail':
      return dt("Imports a mail message into a site's Forum.  Usually called by Procmail or similar");
  }
}

function femail_drush_callback(){
  // Read in the email and save it to a new node
  $node = new stdClass();
  $node->body = fread(STDIN, 1000000);
  $node->title = substr($node->body, 0,100);
  $node->uid = 1;
  $node->type = 'page';
  node_save($node);
}