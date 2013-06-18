<?php /* $ID$ Help file for displaying forums in the scratchy theme */

function _scratchy_forum_display($forums, $topics, $parents, $tid, $sortby, $forum_per_page) {
  global $user;
  // forum list, topics list, topic browser and 'add new topic' link

  $vocabulary = taxonomy_get_vocabulary(variable_get('forum_nav_vocabulary', ''));
  $title = $vocabulary->name;

  // Breadcrumb navigation:
  $breadcrumb = array();
  if ($tid) {
    $breadcrumb[] = array('path' => 'forum', 'title' => $title);
  }

  if ($parents) {
    $parents = array_reverse($parents);
    foreach ($parents as $p) {
      if ($p->tid == $tid) {
        $title = $p->name;
      }
      else {
        $breadcrumb[] = array('path' => 'forum/'. $p->tid, 'title' => $p->name);
      }
    }
  }

  drupal_set_title(check_plain($title));

  $breadcrumb[] = array('path' => $_GET['q']);
  menu_set_location($breadcrumb);

  if (count($forums) || count($parents)) {
    $output = '<div class="node">
  <div class="boxtop"><div class="bc ctr"></div><div class="bc ctl"></div></div>
  <div class="boxcontent">
    <div class="subboxcontent"><div class="content"><div id="forum"><ul style="margin-top:0">';

    if (user_access('create forum topics')) {
      $output .= '<li>'. l(t('Post new forum topic.'), "node/add/forum/$tid") .'</li>';
    }
    else if ($user->uid) {
      $output .= '<li>'. t('You are not allowed to post a new forum topic.') .'</li>';
    }
    else {
      $output .= '<li>'. t('<a href="@login">Login</a> to post a new forum topic.', array('@login' => url('user/login', drupal_get_destination()))) .'</li>';
    }
    $output .= '</ul>';

    $output .= theme('forum_list', $forums, $parents, $tid);

    if ($tid && !in_array($tid, variable_get('forum_containers', array()))) {
      $output .= theme('forum_topic_list', $tid, $topics, $sortby, $forum_per_page);
      drupal_add_feed(url('taxonomy/term/'. $tid .'/0/feed'), 'RSS - '. $title);
    }
    $output .= '</div></div></div>
  </div>
  <div class="boxbtm">
    <div class="bc cbr"></div>
    <div class="bc cbl"></div>
  </div>
</div>';
  }
  else {
    drupal_set_title(t('No forums defined'));
    $output = '';
  }

  return $output;
}

function _scratchy_forum_topic_list($tid, $topics, $sortby, $forum_per_page) {
  global $forum_topic_list_header;
  $rows = array();
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
          array('data' => $topic->num_comments . ($topic->new_replies ? '<br />'. l(format_plural($topic->new_replies, '1 new', '@count new'), "node/$topic->nid", NULL, NULL, 'new') : ''), 'class' => 'replies'),
          array('data' => _forum_format($topic), 'class' => 'created'),
          array('data' => _forum_format(isset($topic->last_reply) ? $topic->last_reply : NULL), 'class' => 'last-reply')
        );
      }
    }
  }

  $output = theme('table', $forum_topic_list_header, $rows);
  $output .= theme('pager', NULL, $forum_per_page, 0);

  return $output;
}

function _scratchy_forum_icon($new_posts, $num_posts = 0, $comment_mode = 0, $sticky = 0) {

  if ($num_posts > variable_get('forum_hot_topic', 15)) {
    $icon = $new_posts ? 'hot-new' : 'hot';
  }
  else {
    $icon = $new_posts ? 'new' : 'default';
  }

  if ($comment_mode == COMMENT_NODE_READ_ONLY || $comment_mode == COMMENT_NODE_DISABLED) {
    $icon = 'closed';
  }

  if ($sticky == 1) {
    $icon = 'sticky';
  }

  $output = theme('image', "misc/forum-$icon.png");

  if ($new_posts) {
    $output = "<a name=\"new\">$output</a>";
  }

  return $output;
}

function _scratchy_forum_topic_navigation($node) {
  return;
  $output = '';

  // get previous and next topic
  $sql = "SELECT n.nid, n.title, n.sticky, l.comment_count, l.last_comment_timestamp FROM {node} n INNER JOIN {node_comment_statistics} l ON n.nid = l.nid INNER JOIN {term_node} r ON n.nid = r.nid AND r.tid = %d WHERE n.status = 1 AND n.type = 'forum' ORDER BY n.sticky DESC, ". _forum_get_topic_order_sql(variable_get('forum_order', 1));
  $result = db_query(db_rewrite_sql($sql), $node->tid);

  $stop = 0;
  while ($topic = db_fetch_object($result)) {
    if ($stop == 1) {
      $next = new stdClass();
      $next->nid = $topic->nid;
      $next->title = $topic->title;
      break;
    }
    if ($topic->nid == $node->nid) {
      $stop = 1;
    }
    else {
      $prev = new stdClass();
      $prev->nid = $topic->nid;
      $prev->title = $topic->title;
    }
  }

  if ($prev || $next) {
    $output .= '<div class="forum-topic-navigation clear-block">';

    if ($prev) {
      $output .= l(t('‹ ') . $prev->title, 'node/'. $prev->nid, array('class' => 'topic-previous', 'title' => t('Go to previous forum topic')));
    }
    if ($prev && $next) {
      // Word break (a is an inline element)
      $output .= ' ';
    }
    if (!empty($next)) {
      $output .= l($next->title . t(' ›'), 'node/'. $next->nid, array('class' => 'topic-next', 'title' => t('Go to next forum topic')));
    }

    $output .= '</div>';
  }

  return $output;
}

function _scratchy_forum_list($forums, $parents, $tid) {
  global $user;

  if ($forums) {

    $header = array(t('Forum'), t('Topics'), t('Posts'), t('Last post'));

    foreach ($forums as $forum) {
      if ($forum->container) {
        $description  = '<div style="margin-left: '. ($forum->depth * 30) ."px;\">\n";
        $description .= ' <div class="name">'. l($forum->name, "forum/$forum->tid") ."</div>\n";

        if ($forum->description) {
          $description .= ' <div class="description">'. filter_xss_admin($forum->description) ."</div>\n";
        }
        $description .= "</div>\n";

        $rows[] = array(array('data' => $description, 'class' => 'container', 'colspan' => '4'));
      }
      else {
        $new_topics = _forum_topics_unread($forum->tid, $user->uid);
        $forum->old_topics = $forum->num_topics - $new_topics;
        if (!$user->uid) {
          $new_topics = 0;
        }

        $description  = '<div style="margin-left: '. ($forum->depth * 30) ."px;\">\n";
        $description .= ' <div class="name">'. l($forum->name, "forum/$forum->tid") ."</div>\n";

        if ($forum->description) {
          $description .= ' <div class="description">'. filter_xss_admin($forum->description) ."</div>\n";
        }
        $description .= "</div>\n";

        $rows[] = array(
          array('data' => $description, 'class' => 'forum'),
          array('data' => $forum->num_topics . ($new_topics ? '<br />'. l(format_plural($new_topics, '1 new', '@count new'), "forum/$forum->tid", NULL, NULL, 'new') : ''), 'class' => 'topics'),
          array('data' => $forum->num_posts, 'class' => 'posts'),
          array('data' => _forum_format($forum->last_post), 'class' => 'last-reply'));
      }
    }
    
    //return print_r($rows,true);

    return '<div class="node"><div class="content forum">'.theme('table', $header, $rows).'</div></div>
  ';

  }

}

// No longer in D6, so here instead!
function _forum_format($topic) {
  if ($topic && $topic->timestamp) {
    return t('@time ago<br />by !author', array('@time' => format_interval(time() - $topic->timestamp), '!author' => theme('username', $topic)));
  }
  else {
    return t('n/a');
  }
}


function _scratchy_theme_forum($node, $teaser, $page){
  
  $parents = taxonomy_get_parents_all($node->tid);  
  $breadcrumb = array();    
  $breadcrumb[] = l('Home','');
  $breadcrumb[] = l('Forums','forum');
  if ($parents) {
    $parents = array_reverse($parents);
    foreach ($parents as $p) {
      if ($p->tid == $tid) {
        $title = $p->name;
      }
      else {
        $breadcrumb[] = l($p->name,'forum/'.$p->tid);
      }
    }
  }
  drupal_set_breadcrumb($breadcrumb);
  $output = '<div class="node'. ((!$node->status) ? ' unpublished' : '') . (($node->sticky && !$page) ? ' sticky' : '') .'">
  <div class="boxtop">
    <div class="bc ctr"></div>
    <div class="bc ctl"></div>
  </div>
  <div class="boxcontent">
    <div class="boxtitle'.(($node->sticky && !$page) ? '-sticky' : '').'">
      <h1>'. ($teaser ? l($node->title, "node/$node->nid") : check_plain($node->title)) .'</h1>
    </div>
    <div class="subboxcontent">';
  
  // Removed for now.  I SHALL RETURN!
  /*if ($tabs = theme('menu_local_tasks')) {
    $output .= $tabs;
  }*/
  
  $output .='<div class="content">';
  if ($teaser && $node->teaser) {
    $output .= $node->teaser;
  }
  else {
    $output .= $node->body;
  }
  $output .='</div>';

  if (theme_get_setting("toggle_node_info_$node->type")) {
    $submitted['node_submitted'] = array(
      'title' => t("By !author at @date", array('!author' => theme('username', $node), '@date' => format_date($node->created, 'small'))),
      'html' => TRUE,
    );
  }
  else {
    $submitted['node_submitted'] = array();
  }

  $terms = array();
  if (module_exists('taxonomy')) {
    $terms = taxonomy_link("taxonomy terms", $node);
  }

  $links = array_merge($submitted, $terms);
  if ($node->links) {
    $links = array_merge($links, $node->links);
  }
  if (count($links)) {
    $output .= '<div class="links">'. theme('links', $links, array('class' => 'links inline')) ."</div>\n";
  }
  
  $output .='</div>
  </div>
  <div class="boxbtm">
    <div class="bc cbr"></div>
    <div class="bc cbl"></div>
  </div>
</div>';
  return $output;
}