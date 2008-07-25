<html>
<head>
<title>Execute PHP on ALL sites</title>
</head>
<body style="border: solid 1px black;padding:10px;margin:10px;font: 10px Verdana;">
<h1>Executed the PHP on the following sites:</h1>
<?php

// Set the timeout so that if the update is slow, this doesn't fail
ini_set('default_socket_timeout',    120);    
// Scan the directory for folders, these will form the basis of the 
// list
$files = scandir("/var/www/html/sites");
//print_r($files);
$domains = array();
$num_files = count($files);
for ($i=2;$i<$num_files;$i++){
  if (substr($files[$i],0,3)!="www"
    && $files[$i]!="all"
    && $files[$i]!="default"
    && $files[$i]!="sandbox.editwebrevisions.info"
    && is_dir("/var/www/html/sites/".$files[$i])
  )
    $domains[]=$files[$i];
}
$domains[] = 'quartz.nhm.ac.uk';
header("ContentType: text/plain");
foreach ($domains as $domain){
  $contents = @file_get_contents("http://".$domain."/phptoexecute.php");
  if (strlen($contents)){
    echo '<p style="margin: 2px; padding:2px;"><a href="http://'.$domain.'">http://'.$domain.'</a></p><xmp>'.$contents.'</xmp>';
  } else {
    echo '<p style="margin: 2px; padding:2px;">Nothing from <a href="http://'.$domain.'">http://'.$domain.'</a></p>';
  }
}
?>
</body>
</html>