<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language ?>" xml:lang="<?php print $language ?>">

<head>
  <title><?php print $head_title ?></title>
  <?php print $head ?>
  <?php print $styles ?>
  <?php print $scripts ?>
  <script type="text/javascript"><?php /* Needed to avoid Flash of Unstyle Content in IE */ ?> </script>
  <script type="text/javascript">var printable = 1;</script>
</head>

<body class="printable">



<table border="0" cellpadding="0" cellspacing="0" id="content">
  <tr>
    <td valign="top">
      <div id="main">
				<h1><?php print $site_name ?></h1>
        <h2><?php print $title ?></h2>
				<h3><?php print format_date(time(), 'long'); ?></h2>
        <div class="tabs"><?php print $tabs ?></div>
        <?php print $help ?>
        <?php print $messages ?>
        <?php print $content; ?>
        <?php print $feed_icons; ?>
      </div>
    </td>
  </tr>
</table>


<?php print $closure ?>
</body>
</html>
