<?php 

function foundation_forum_icon($new_posts, $num_posts = 0, $comment_mode = 0, $sticky = 0) {

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