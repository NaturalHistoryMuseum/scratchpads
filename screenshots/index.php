<?php echo '<?xml version="1.0" encoding="UTF-8"?>';?>
<!DOCTYPE html 
     PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">
<head>
<title>Screenshots</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
</head>
<body>
<h1>All The Screenshots</h1>
<?php

// All the small images
$files = scandir('.');
foreach($files as $file){
  if(substr($file,-9)=='small.jpg' || substr($file,-19)=='small.annotated.jpg'){
    ?><img src="/screenshots/<?php echo $file; ?>"/><?php
  }
}

?>
</body>
</html>
