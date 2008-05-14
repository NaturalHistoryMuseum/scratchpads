<html>
<head>
<title>Update All</title>
</head>
<body style="border: solid 1px black;padding:10px;margin:10px;font: 10px Verdana;">
<h1>Updated the following sites:</h1>
<?php
// Firstly scan the directory for folders, these will form the basis of the 
// list
$files = scandir("/var/www/html/sites");
$domains = array();
for ($i=2;$i<count($files);$i++){
  if (substr($files[$i],0,3)!="www"
    && $files[$i]!="all"
    && strpos($files[$i],"default") === false
    && is_dir("/var/www/html/sites/".$files[$i])
  )
    $domains[]=$files[$i];
}
$domains[]="quartz.nhm.ac.uk"; // Add the default

foreach($domains as $domain){
  file_get_contents("http://".$domain."/updateone.php?op=selection");
  echo '<p style="margin: 2px; padding:2px;"><a href="http://'.$domain.'">http://'.$domain.'</a></p>';
}
?>
</body>
</html>