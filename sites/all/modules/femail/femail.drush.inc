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
  $lines = preg_split("/[\n\r]/", $node->body);
  $mailparse = mailparse_msg_create();
  mailparse_msg_parse($mailparse, $node->body);
  $structure = mailparse_msg_get_structure($mailparse);
  foreach ($structure as $s){
    // Get the subject to use as the node/comment title
    $part = mailparse_msg_get_part($mailparse, $s);
    $part_data = mailparse_msg_get_part_data($part);
    if(isset($part_data['headers']['subject'])){
      $node->title = $part_data['headers']['subject'];
    }
    // Get the body for the node/comment body
    if(isset($part_data['headers']['content-type'])){
      if(strpos($part_data['headers']['content-type'], 'text/plain') !== FALSE){
        $node->body = substr($node->body, $part_data['starting-pos-body'], $part_data['ending-pos-body'] - $part_data['starting-pos-body']);
        break;
      }
    }
    // Get the From: header, which will be used to work out which user to
    // attribute this node/comment to
    
  }
  $node->uid = 2;
  $node->promote = 1;
  $node->type = 'page';
  node_save($node);
}
