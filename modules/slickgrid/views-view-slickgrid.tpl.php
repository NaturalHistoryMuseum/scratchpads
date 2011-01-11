<!-- <div class="<?php
print $class;
?>-wrapper">
  <div id="<?php
  print $id;
  ?>" class="<?php
  print $class;
  ?>-wrapper" style="<?php
  print $style;
  ?>"></div>
  <?php
  if(isset($controls)):
    ?>
    <div id="<?php
    print $id;
    ?>-controls" class="<?php
    print $class;
    ?>-controls"></div>
  
  
  <?php endif;
  ?>
</div> -->

<?php
if(isset($title)):
  ?>
<div class="grid-header"><h5><?php print $title; ?></h5></div>

<?php endif;

print $slickgrid;
?>
