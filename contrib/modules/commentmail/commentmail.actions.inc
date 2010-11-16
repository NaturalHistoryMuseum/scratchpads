<?php

/**
 * Allow quick approval directly from an email.
 *
 * @param $cid
 *   Comment ID number of the comment to act upon.
 * @param $op
 *   The operation to be performed.
 */
function commentmail_quick_approve($cid = NULL, $op = FALSE) {
  global $user;
  if (user_access('administer comments')) {

      $comment = db_fetch_object(db_query('SELECT * FROM {comments} WHERE cid = %d', $cid));
      if ($comment) {
        if ($op == 'approve') {
         if (isset($_GET['token']) && drupal_valid_token($_GET['token'], $comment->timestamp)) {
          $comment->status = 0;
          if (comment_save((array)$comment)) {
            // Link to comment on page.
            drupal_goto('node/'. $comment->nid, NULL, 'comment-'. $comment->cid);
          }
          else {
            drupal_set_message(t('Comment not saved'), 'error');
          }
         }
         else {
           return drupal_access_denied();
         }
        }
        $comment->comment = check_markup($comment->comment, $comment->format, FALSE);
        $node = node_load($comment->nid);
        $output = "<h2>". t('Comment on <em>%title</em>', array('%title' => $node->title)) ."</h2>";
        $confirm_delete = "return confirm('". t('Are you sure you want to delete this comment?') ."');";
        $confirm_deleteban = "return confirm('". t('Are you sure you want to delete this comment and ban the computer that posted it?') ."');";
        if ($comment->status) {
          $links[] = array('title' => t('approve'), 'href' => 'comment/qa/'. $cid .'/approve', 'query' => 'token='. drupal_get_token($comment->timestamp));
        }
        else {
          $links[] = array('title' => t('approved'));
        }
        $links[] = array('title' => t('edit'), 'href' => 'comment/edit/'. $cid);
        $links[] = array('title' => t('delete'), 'href' => 'comment/quickdelete/'. $cid, 'query' => 'token='. drupal_get_token($comment->timestamp), 'attributes' => array('onclick' => $confirm_delete));
        $links[] = array('title' => t('delete and ban ip'), 'href' => 'comment/quickdeleteban/'. $cid, 'query' => 'token='. drupal_get_token($comment->timestamp), 'attributes' => array('onclick' => $confirm_deleteban));
        $output .= theme('comment', $comment, $node ,$links);
      }
      else {
        $output = t("Comment not found.");
      }
    }
    elseif ($user->uid > 0) {
      return drupal_access_denied();
    }
    else {
      // User is not logged in so go to the user login page,
      // then come back here.
      drupal_goto('user/login', 'destination=comment/qa/'. $cid);
    }
  return $output;
}

/**
 * Allow quick deletion of a comment directly from an email.
 *
 * @param $cid
 *   Comment ID of comment being acted upon.
 */
function commentmail_quick_delete($cid = NULL) {
  if (is_numeric($cid) && $comment = db_fetch_object(db_query('SELECT c.*, u.name AS registered_name, u.uid FROM {comments} c INNER JOIN {users} u ON u.uid = c.uid WHERE c.cid = %d', $cid))) {

   if (isset($_GET['token']) && drupal_valid_token($_GET['token'], $comment->timestamp)) {
    commentmail_delete($comment);

    drupal_goto("node/$comment->nid");
   }
   else {
     return drupal_access_denied();
   }
  }
  else {
    return drupal_not_found();
  }
}

/**
 * Allow quick deletion and banning of a comment directly from an email.
 *
 * @param $cid
 *   Comment ID of comment being acted upon.
 */
function commentmail_quick_deleteban($cid = NULL) {
    if (is_numeric($cid) && $comment = db_fetch_object(db_query('SELECT * FROM {comments} WHERE cid = %d', $cid))) {

   if (isset($_GET['token']) && drupal_valid_token($_GET['token'], $comment->timestamp)) {
    commentmail_delete($comment);

    $aid = db_last_insert_id('access','aid');
    db_query("INSERT INTO {access} (aid, mask, type, status) VALUES ('%s', '%s', 'host', 0)", $aid, $comment->hostname);

    drupal_set_message(t('The address <em>%ip</em> has been banned.', array('%ip' => $comment->hostname)));

    drupal_goto("node/$comment->nid");
   }
   else {
     return drupal_access_denied();
   }
  }
  else {
    return drupal_not_found();
  }
}

/**
 * Delete a comment.
 *
 * @param $comment
 *   Full comment object to be deleted.
 */
function commentmail_delete($comment) {
  $comment->name = $comment->uid ? $comment->registered_name : $comment->name;

    drupal_set_message(t('The comment and all its replies have been deleted.'));
    module_load_include('inc', 'comment', 'comment.admin');
    // Delete comment and its replies.
    _comment_delete_thread($comment);

    _comment_update_node_statistics($comment->nid);

    // Clear the cache so an anonymous user sees that the comment was deleted.
    cache_clear_all();
}

/**
 * Approve a comment.
 *
 * @param $cid
 *   Comment ID of comment to be approved.
 */
function commentmail_approve($form, $cid) {
  if ($comment = _comment_load($cid)) {
    if ($comment->status == COMMENT_NOT_PUBLISHED) {
      return confirm_form(
        array('cid' => array('#type' => 'value', '#value' => $comment->cid)),
        t('Are you sure you want to approve the comment %title?', array('%title' => $comment->subject)),
        array('path' => 'node/'. $comment->nid, 'fragment' => 'comment-'. $comment->cid),
        t('The comment will be visible to all users.'),
        t('Approve'),
        t('Cancel')
      );
    }
    else {
      drupal_set_message(t('The comment is already published.'));
      drupal_goto('node/'. $comment->nid, NULL, 'comment-'. $comment->cid);
    }
  }
  else {
    drupal_set_message(t('The comment no longer exists.'));
  }
}

/**
 * Submission handler for comment approval.
 */
function commentmail_approve_submit($form, &$form_state) {
  $comment = _comment_load($form_state['values']['cid']);
  $comment->status = 0;

  if (comment_save((array)$comment)) {
    // Link to comment on page.
    drupal_set_message(t('The comment has been approved.'));
    drupal_goto('node/'. $comment->nid, NULL, 'comment-'. $comment->cid);
  }
  else {
    drupal_set_message(t('There was an error during the comment approving process.'), 'error');
  }
}

/**
 * Delete a comment and ban the author.
 *
 * @param $cid
 *   Comment ID of comment to be deleted.
 */
function commentmail_deleteban($form, $cid) {

  if ($comment = _comment_load($cid)) {
    return confirm_form(
        array('cid' => array('#type' => 'value', '#value' => $comment->cid)),
        t('Are you sure you want to delete the comment %title and ban its author?', array('%title' => $comment->subject)),
        array('path' => 'node/'. $comment->nid, 'fragment' => 'comment-'. $comment->cid),
        t('Any replies to this comment will be lost. This action cannot be undone. In addition, the author of the comment is no longer allowed to post comments on your site.'),
        t('Delete and ban'),
        t('Cancel')
      );
  }
  else {
    drupal_set_message(t('The comment no longer exists.'));
  }
}

/**
 * Submission handler for comment delete/ban.
 */
function commentmail_deleteban_submit($form, &$form_state) {
  $comment = _comment_load($form_state['values']['cid']);

  module_load_include('inc', 'comment', 'comment.admin');

  // Delete comment and its replies.
  _comment_delete_thread($comment);
  _comment_update_node_statistics($comment->nid);

  drupal_set_message(t('The comment and all its replies have been deleted.'));

  // Clear the cache so an anonymous user sees that his comment was deleted.
  cache_clear_all();

  // Now, ban the user.
  $aid = db_last_insert_id('access','aid');
  db_query("INSERT INTO {access} (aid, mask, type, status) VALUES ('%s', '%s', 'host', 0)", $aid, $comment->hostname);

  drupal_set_message(t('The host %host has been banned.', array('%host' => $comment->hostname)));

  drupal_goto('node/'. $comment->nid);
}
