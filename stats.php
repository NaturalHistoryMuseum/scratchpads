<?php
header("Content-Type: text/plain");
// List the total users
$array = parse_ini_file('/var/www/drupal_db_passwords',true);
mysql_connect('localhost',$array['root']['user'],$array['root']['password']);
$sql = "SELECT CONCAT( table_schema,'.', table_name) AS tablename FROM information_schema.tables WHERE table_name = 'users' AND table_schema != 'lepindex_data';";
$result = mysql_query($sql);
$tables = array();
while($row = mysql_fetch_array($result)){
  $tables[] = 'SELECT * FROM '.$row['tablename'].' WHERE '.$row['tablename'].'.status=1 ';
}
$sql = 'SELECT COUNT(DISTINCT mail) FROM ('.implode(' UNION ',$tables).') AS bollocksingmysql;';
$result = mysql_query($sql);
echo "Users: ".array_pop(mysql_fetch_array($result))."\n";
// Now calculate total nodes
$sql = "SELECT CONCAT( table_schema,'.', table_name) AS tablename FROM information_schema.tables WHERE table_name = 'node';";
$result = mysql_query($sql);
$tables = array();
while($row = mysql_fetch_array($result)){
  $tables[] = 'SELECT nid,created,type FROM '.$row['tablename'];
}
$sql = 'SELECT COUNT(*) FROM ('.implode(' UNION ',$tables).') AS bollocksingmysql;';
$result = mysql_query($sql);
echo "Nodes: (Total ".array_pop(mysql_fetch_array($result)).")\n";
$sql = 'SELECT COUNT(nid) AS count,type FROM ('.implode(' UNION ',$tables).') AS bollocksingmysql GROUP BY type ORDER BY count DESC;';
$result = mysql_query($sql);
while ($row=mysql_fetch_array($result)){
  if ($row[1]!=''){
    echo " ".str_pad($row[1],30).$row[0]."\n";
  }
}

/**
// List the emails and hopefully institutions of users, along with the site.
$sql = "SELECT table_schema AS site FROM information_schema.tables WHERE table_name = 'users' AND table_schema != 'lepindex_data';";
$result = mysql_query($sql);
echo "Mails:\n";
while($row = mysql_fetch_array($result)){
  echo " ".$row['site']."\n";
  $sql = 'SELECT mail,value FROM '.$row['site'].'.users LEFT JOIN '.$row['site'].'.profile_values ON users.uid = profile_values.uid WHERE (fid=3 OR fid IS NULL) AND status=1';
  $result2 = mysql_query($sql);
  while($row2 = mysql_fetch_array($result2)){
    echo "  ".$row2['mail']."\t".$row2['value']."\n";
  }
}

// Administrators
$sql = "SELECT table_schema AS site FROM information_schema.tables WHERE table_name = 'users' AND table_schema != 'lepindex_data';";
$result = mysql_query($sql);
echo "Administrators:\n";
while($row = mysql_fetch_array($result)){
  echo " ".$row['site']."\n";
  $sql = 'SELECT mail FROM '.$row['site'].'.users INNER JOIN '.$row['site'].'.users_roles ON users_roles.uid = users.uid WHERE rid=6 OR rid=5';
  $result2 = mysql_query($sql);
  while($row2 = mysql_fetch_array($result2)){
    echo "  ".$row2['mail']."\n";
  }
}
*/
