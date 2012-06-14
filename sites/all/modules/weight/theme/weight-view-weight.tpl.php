<?php

/**
 * @file
 * Views template for Weight Changer table.
 */
?>

<table class="<?php print $class; ?>" id="<?php print $id; ?>">
  <thead>
    <tr>
      <?php if (count($header)): ?>
	      <?php foreach ($header as $field => $label): ?>
          <th class="views-field views-field-<?php print $field; ?>">
            <?php print $label; ?>
          </th>
        <?php endforeach; ?>
      <?php endif; ?>
    </tr>
  </thead>
  <tbody>
	  <?php  if (count($rows)): ?>
      <?php  foreach ($rows as $count => $row): ?>
        <tr class="<?php print (($count + 1) % 2 == 0) ? 'even' : 'odd';?> draggable">
          <?php foreach ($row as $field => $content): ?>
            <td class="views-field views-field-<?php print isset($field) ? $field : '' ?>">
              <?php print $content; ?>
            </td>
          <?php endforeach; ?>
        </tr>
      <?php endforeach;  ?>
    <?php endif; ?>
  </tbody>
</table>
<?php print $submit; ?>
