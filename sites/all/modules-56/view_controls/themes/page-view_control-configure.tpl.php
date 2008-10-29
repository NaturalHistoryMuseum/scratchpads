<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language ?>" xml:lang="<?php print $language ?>">
<head>
  <title>Tmp</title>
  <meta http-equiv="Content-Style-Type" content="text/css" />
  <?php print $head ?>
  <?php print $styles ?>
</head>
<body>

<div id="header">
	<div id="header_inner" class="fluid">
		<div id="logo">
		  <?php if ($logo) : ?>
			<span><a href="<?php print $base_path ?>" title="<?php print t('Home') ?>"><img src="<?php print($logo) ?>" alt="<?php print t('Home') ?>" border="0" /></a>
		  <?php endif; ?>
		  <?php if ($site_name) : ?>
			<h1><a href="<?php print $base_path ?>" title="<?php print t('Home') ?>"><?php print($site_name) ?></a></h1>
		  <?php endif;?>
		  <?php if ($site_slogan) : ?>
			<h2><?php print($site_slogan) ?></h2>
		  <?php endif;?>
		</div>
		<div id="menu">			
			<?php if (is_array($primary_links)) : ?>
				<ul id="primary">
			<?php foreach ($primary_links as $link): ?>
				<li><?php print $link?></li>
			<?php endforeach; ?>
				</ul>
			<?php endif; ?>
		</div>
	</div>
</div>
<div id="main">
	<div id="main_inner" class="fluid">
		<div id="primaryContent_2columns">
			<div id="columnA_2columns">
				<?php if ($title != ""): ?>
				  <?php print $breadcrumb ?>
				  <h1 class="title"><?php print $title ?></h1>
		
				  <?php if ($tabs != ""): ?>
					<div class="tabs"><?php print $tabs ?></div>
				  <?php endif; ?>
		
				<?php endif; ?>
		
				<?php if ($help != ""): ?>
					<div id="help"><?php print $help ?></div>
				<?php endif; ?>
		
				<?php if ($messages != ""): ?>
				  <?php print $messages ?>
				<?php endif; ?>
		
			  <!-- start main content -->
			  <?php print($content) ?>
			  <!-- end main content -->
			</div>
		</div>
		<div id="secondaryContent_2columns">
			<div id="columnC_2columns">
				<?php if ($sidebar_left != ""): ?>
				  <?php print $sidebar_left ?>
				<?php endif; ?>
			</div>
		</div>
		<br class="clear" />
	</div>
</div>
<?php if ($footer_message) : ?>
<div id="footer" class="fluid">
    <p><?php print $footer_message;?></p>
</div>
<?php endif; ?>
<?php print $closure;?>
</body>
</html>