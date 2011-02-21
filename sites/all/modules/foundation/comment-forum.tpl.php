<div class="comment<?php print ($comment->new) ? ' comment-new' : ''; print ' '. $status; print ' '. $zebra; ?>">
  <div class="boxtop">
    <div class="bc ctr"></div>
    <div class="bc ctl"></div>
  </div>
  <div class="boxcontent">
    <div class="boxtitle-none boxtitle">
      <h1><?php print $title; ?></h1>
      <?php if ($submitted): ?>
      <span class="submitted"><?php print $submitted; ?></span>
      <?php endif; ?>
    </div>
    <div class="subboxcontent">
      <div class="content"><?php print $content; ?></div>
      <?php if ($signature): ?>
      <div class="clear-block">
        <div>--</div>
        <?php print $signature ?>
      </div>
      <?php endif; ?>
      <?php if ($links): ?>
      <div class="links"><?php print $links ?></div>
      <?php endif; ?>
    </div>
  </div>
  <div class="boxbtm">
    <div class="bc cbr"></div>
    <div class="bc cbl"></div>
  </div>
</div>