<?php
if(!function_exists('bb2html')){
  function bb2html($text) {
    $bbcode = array(
                    "[strong]", "[/strong]",
                    "[b]",  "[/b]",
                    "[u]",  "[/u]",
                    "[i]",  "[/i]",
                    "[em]", "[/em]"
                  );
    $htmlcode = array(
                  "<strong>", "</strong>",
                  "<strong>", "</strong>",
                  "<u>", "</u>",
                  "<em>", "</em>",
                  "<em>", "</em>"
                );
    return str_replace($bbcode, $htmlcode, $text);
  }
}
if(!function_exists('bb_strip')){
  function bb_strip($text) {
    $bbcode = array(
                    "[strong]", "[/strong]",
                    "[b]",  "[/b]",
                    "[u]",  "[/u]",
                    "[i]",  "[/i]",
                    "[em]", "[/em]"
                  );
    return str_replace($bbcode, '', $text);
  }
}
phptemplate_comment_wrapper(NULL, $node->type); ?>

<div id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?>">

<?php print $picture ?>

<?php if ($page == 0): ?>
  <h2><a href="<?php print $node_url ?>" title="<?php print bb_strip($title) ?>"><?php print bb2html($title) ?></a></h2>
<?php endif; ?>

  <?php if ($submitted): ?>
    <span class="submitted"><?php print t('!date — !username', array('!username' => theme('username', $node), '!date' => format_date($node->changed))); ?></span>
  <?php endif; ?>

  <div class="content">
    <?php print $content ?>
  </div>

  <div class="clear-block clear">
    <div class="meta">
    <?php if ($taxonomy): ?>
      <div class="terms"><?php print $terms ?></div>
    <?php endif;?>
    </div>

    <?php if ($links): ?>
      <div class="links"><?php print $links; ?></div>
    <?php endif; ?>
  </div>

</div>