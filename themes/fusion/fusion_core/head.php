<?php
// $Id: head.php,v 1.1.2.1 2009/09/29 04:18:15 sociotech Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language->language; ?>" xml:lang="<?php print $language->language; ?>">

<head>
<title><?php print $head_title; ?></title>
<?php print $head; ?>
<?php print $styles; ?>
<?php print $setting_styles; ?>
<!--[if IE 8]>
<?php print $ie8_styles; ?>
<![endif]-->
<!--[if IE 7]>
<?php print $ie7_styles; ?>
<![endif]-->
<!--[if lte IE 6]>
<?php print $ie6_styles; ?>
<![endif]-->
<?php if ($local_styles): ?>
<?php print $local_styles; ?>
<?php endif; ?>
<?php print $scripts; ?>
</head>
