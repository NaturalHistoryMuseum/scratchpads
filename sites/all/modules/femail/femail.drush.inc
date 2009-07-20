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
  $msg = array(
    'body' => fread(STDIN, 1000000)
  );
  $lines = preg_split("/[\n\r]/", $msg['body']);
  $mailparse = mailparse_msg_create();
  mailparse_msg_parse($mailparse, $msg['body']);
  $structure = mailparse_msg_get_structure($mailparse);
  foreach ($structure as $s){
    // Get the subject to use as the node/comment title
    $part = mailparse_msg_get_part($mailparse, $s);
    $part_data = mailparse_msg_get_part_data($part);
    if(isset($part_data['headers']['subject'])){
      $msg['title'] = $part_data['headers']['subject'];
    }
    // Get the From: header, which will be used to work out which user to
    // attribute this node/comment to
    if(isset($part_data['headers']['from'])){
      $from = mailparse_rfc822_parse_addresses($part_data['headers']['from']);
      $msg['from'] = $from[0]['address'];
    }
    // Get the To: header, which will be used to work out which forum this 
    // message wants to go into
    if(isset($part_data['headers']['to'])){
      $to = mailparse_rfc822_parse_addresses($part_data['headers']['to']);
      $msg['to'] = $to[0]['address'];
    }
    // Get the body for the node/comment body
    if(!$body_is_set){
      if(isset($part_data['headers']['content-type'])){
        if(strpos($part_data['headers']['content-type'], 'text/plain') !== FALSE){
          $msg['body'] = substr($msg['body'], $part_data['starting-pos-body'], $part_data['ending-pos-body'] - $part_data['starting-pos-body']);
          $body_is_set = TRUE;
        }
      }
    }
    // Get the msgid
    if(isset($part_data['headers']['message-id'])){
      $msg['msgid'] = $part_data['headers']['message-id'];
    }
    // Get the InReplyTo header for possibly putting this message as a reply to 
    // another forum post
    if(isset($part_data['headers']['in-reply-to'])){
      $msg['in-reply-to'] = $part_data['headers']['in-reply-to'];
    }
  }
  
  // If the in-reply-to is set, then we need to create a comment
  if(isset($msg['in-reply-to'])){
    
  } else {
    // else we need to create a node
    global $user;
    $account = user_load(array('mail' => $msg['from']));
    if($account){
      $user = $account;
    } else {
      $result = db_query("SELECT uid FROM {femail_user_emails} WHERE email = '%s' AND status !=0", $msg['from']);
      if($result){
        $user = user_load(array('uid' => array_pop(db_fetch_array($result))));
      } else {
        $user = user_load(array('uid' => 0));
      }
    }
    // Check we have permission to post a forum post.
    if(user_access('create forum content')){
      // Get the tid of the forum that this post will go into
      $emails = variable_get('femail_emails', array());
      $tid = array_search($msg['to'], $emails);
      $node = new stdClass();
      $node->uid = $user->uid;
      $node->body = $msg['body'];
      $node->comment = variable_get('comment_forum', COMMENT_NODE_READ_WRITE);
      $node->title = $msg['title'];
      $node->promote = 1;
      $node->type = 'forum';
      $node->taxonomy = array($tid => $tid);
      node_save($node);
      // Insert the msgid into the database for use later
      db_query("INSERT INTO {femail_msgs} (vid, msgid) VALUES (%d, '%s')", $node->vid, $msg['msgid']);
    }
  }
}
