<?php //$Id: node-og.tpl.php 525 2008-06-05 12:10:19Z simor $ 
?>
<div class="node<?php if ($sticky) { print " sticky"; } ?><?php if (!$status) { print " node-unpublished"; } ?>">
  <?php if ($page == 0) { ?><h2 class="title"><a href="<?php print $node_url?>"><?php print $title?></a></h2><?php }; ?>
  <span class="taxonomy"><?php print $terms?></span>
  <div class="content"><?php print $content?></div>
</div>
