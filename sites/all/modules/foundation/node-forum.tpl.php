<?php
// $Id: node.tpl.php,v 1.5 2007/10/11 09:51:29 goba Exp $
?>
<div id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?>">
  <div class="boxtop">
    <div class="bc ctr"></div>
    <div class="bc ctl"></div>
  </div>
  <div class="boxcontent">
    <div class="boxtitle<?php ($node->sticky && !$page) ? '-sticky' : ''?>">
      <h1><?php print $teaser ? l($node->title, "node/$node->nid") : check_plain($node->title)?></h1>
      <?php if ($submitted): ?>
      <span class="submitted"><?php print $submitted; ?></span>
      <?php endif; ?>
    </div>
    <div class="subboxcontent">
      <div class="content">
        <?php print $content ?>
      </div>
      <div class="clear-block">
        <div class="meta">
          <?php if ($taxonomy): ?>
          <div class="terms"><?php print $terms ?></div>
          <?php endif;?>
        </div>
        <?php if ($links): ?>
        <div class="links"><?php print str_replace("Add new comment","reply",$links); ?></div>
        <?php endif; ?>
      </div>
    </div>
  </div>
  <div class="boxbtm">
    <div class="bc cbr"></div>
    <div class="bc cbl"></div>
  </div>
</div>