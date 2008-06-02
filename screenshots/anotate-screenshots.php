<?php
$convert_string = "convert -background '#0008' -fill white -gravity west -size 1024x500 caption:\"%s\" +size %s +swap -gravity south -composite %s";
// Get a list of all the files in the directory, and filter HARD!
$files = scandir('.');
$databasedetails = parse_ini_file('/var/www/drupal_db_passwords',true);
foreach($files as $file){
  if(strpos($file,'.jpg')>0){
    // $file is the start need to connect to a database now, and do some stuff
    $domain = str_replace('-','',array_shift(explode(".", $file)));
    if($domain =="pad"){
      $domain = explode(".",$file);
      $domain = str_replace('-','',$domain[1]);
    }
    mysql_connect("localhost", $databasedetails[$domain]['user'], $databasedetails[$domain]['password'], TRUE, 2);
    mysql_select_db($domain);
    $nodes = array_pop(mysql_fetch_array(mysql_query("SELECT COUNT(nid) AS nodes FROM node;")));
    $users = array_pop(mysql_fetch_array(mysql_query("SELECT COUNT(uid) AS users FROM users")));
    $views = array_pop(mysql_fetch_array(mysql_query("SELECT SUM(totalcount) AS totalcount FROM node_counter;")));
    $caption = ' Pages: '.$nodes.'\n Users: '.$users.'\n Views: '.$views;
    $file_parts = explode('.',$file);
    array_pop($file_parts);
    array_push($file_parts,'annotated.jpg');
    $filenew = implode('.',$file_parts);
    echo sprintf($convert_string,$caption,$file,$filenew)."\n";
  }
}
