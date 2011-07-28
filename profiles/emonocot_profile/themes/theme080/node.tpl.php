<?php
// $Id: node.tpl.php,v 1.4 2007/08/07 08:39:36 goba Exp $
?>
<div class="node<?php if ($sticky) { print " sticky"; } ?><?php if (!$status) { print " node-unpublished"; } ?>">
	<?php if ($page == 0): ?>
    	
			<?php print $picture ?>
            <div class="bg-h2">         
                <h1 class="title"><a href="<?php print $node_url ?>"><?php print $title ?></a></h1>
                <div class="submit"><span class="submitted"><?php print $submitted ?></span></div>
            </div>
       
    <?php endif; ?>
    <div class="taxonomy"><?php print $terms ?></div>
  
    <div class="content"><?php print $content ?></div>
    <br />
    <?php if ($links): ?>
    	<div class="align-links">
        	<div class="bg-links">
                <div class="links-left">
                    <div class="links-right">
                        <div class="links"><?php print $links ?></div>
                    </div>
                </div>
            </div>
        </div>
    <?php endif; ?>
</div>