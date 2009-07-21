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

  // User needed for both comments, and nodes, so load first.
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
  // If the in-reply-to is set, then we need to create a comment else we need to
  // create a node, or creating a comment has failed as the msgid could not be
  // found.
  if(isset($msg['in-reply-to'])){
    // First we'll make sure the in-reply-to is in the msgid table, if not, we
    // will have to add this as a new thread.
    $result = db_query("SELECT cid, nid FROM {femail_msgs} WHERE msgid = '%s'", $msg['in-reply-to']);
    $row = db_fetch_array($result);
    if($row){
      // Check we have permission to post a forum post.
      if(user_access('post comments')){
        // Get the tid of the forum that this post will go into
        $edit = array(
          'author' => $user->name,
          'subject' => $msg['title'],
          'comment' => $msg['body'],
          'pid' => $row['cid'],
          'nid' => $row['nid'],
          'uid' => $user->uid,
          'op' => 'Save',
          'date' => 'now',
          'name' => $user->name
        );
        $cid = comment_save($edit);
        // Insert the msgid into the database for use later
        if($msg['msgid']){
          db_query("INSERT INTO {femail_msgs} (nid, msgid, cid) VALUES (%d, '%s', %d)", $node->vid, $msg['msgid'], $cid);
        }
      }
      return;
    }
  }
  // else we need to create a node, or creating a comment has failed as the 
  // msgid could not be found.
  // Check we have permission to post a forum post.
  if(user_access('create forum topics')){
    // Get the tid of the forum that this post will go into
    $emails = variable_get('femail_emails', array());
    $tid = array_search($msg['to'], $emails);
    $term = taxonomy_get_term($tid);
    $node = new stdClass();
    $node->uid = $user->uid;
    $node->body = $msg['body'];
    $node->comment = variable_get('comment_forum', COMMENT_NODE_READ_WRITE);
    $node->title = $msg['title'];
    $node->type = 'forum';
    $node->taxonomy = array($term->vid => $tid);
    // Insert the msgid into the database for use later
    if($msg['msgid']){
      db_query("INSERT INTO {femail_msgs} (nid, msgid, cid) VALUES (%d, '%s', 0)", $node->vid, $msg['msgid']);
    }
    node_save($node);
  }
}
