<h2><?php echo $title ?></h2>
<p class="description"><?php echo $taxa ?></p>

<?php if($description): ?>
<div class="body"> 
<?php echo $description ?>
</div>
<?php endif; ?>

<?php if($edit || $delete): ?>
<p class="controls"><span><?php echo $edit ?></span><span><?php echo $delete ?></span><p>
<?php endif; ?>  
 

