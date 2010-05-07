<?php

function foundation_forum_topic_list($tid, $topics, $sortby, $forum_per_page) {
  drupal_add_css(drupal_get_path('module','foundation').'/foundation.css');
  global $forum_topic_list_header;
  $rows = array();
  drupal_set_title('');
  if ($topics) {
    foreach ($topics as $topic) {
      // folder is new if topic is new or there are new comments since last visit
      if ($topic->tid != $tid) {
        $rows[] = array(
          array('data' => theme('forum_icon', $topic->new, $topic->num_comments, $topic->comment_mode, $topic->sticky), 'class' => 'icon'),
          array('data' => check_plain($topic->title), 'class' => 'title'),
          array('data' => l(t('This topic has been moved'), "forum/$topic->tid"), 'colspan' => '3')
        );
      }
      else {
        $rows[] = array(
          array('data' => theme('forum_icon', $topic->new, $topic->num_comments, $topic->comment_mode, $topic->sticky), 'class' => 'icon'),
          array('data' => l($topic->title, "node/$topic->nid"), 'class' => 'topic'),
          array('data' => $topic->num_comments . ($topic->new_replies ? '<br />'. l(format_plural($topic->new_replies, '1 new', '@count new'), "node/$topic->nid", array('class'=>'new')) : ''), 'class' => 'replies'),
          array('data' => _forum_format($topic), 'class' => 'created'),
          array('data' => _forum_format(isset($topic->last_reply) ? $topic->last_reply : NULL), 'class' => 'last-reply')
        );
      }
    }
  }
  $output = theme('table', $forum_topic_list_header, $rows);
  $output .= theme('pager', NULL, $forum_per_page, 0);
  return '<div class="forum-container"><div class="content forum">'.$output.'</div></div>';
}