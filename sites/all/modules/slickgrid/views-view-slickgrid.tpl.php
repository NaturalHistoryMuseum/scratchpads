<!-- <div class="clear-block <?php
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
  if(isset($slickgrid)):
  ?>
<div class="slickgrid-wrapper clear-block">
<?php
print $slickgrid;
?>
</div>
  <?php endif;
  ?>
