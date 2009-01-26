<?php
function _leftandright_rebuild_tree($vid){
  // Lets load this vocabulary, firstly to make sure it exists, and secondly
  // so that we know its name
  $vocabulary = taxonomy_vocabulary_load($vid);
  if(!$vocabulary){return;}
  // Check that the vocabulary has terms, if not, just return
  $result = db_query("SELECT COUNT(tid) c FROM {term_data} WHERE vid = %d", $vid);
  $array_results = db_fetch_array($result);
  if(!$array_results['c']){
    drupal_set_message(check_plain($vocabulary->name).' has no terms');
    return;
  }
  
  // Get the DB connection shit in the same way as core does
  global $db_url;  
  $url = parse_url($db_url);
  $url['user'] = urldecode($url['user']);
  if(isset($url['pass'])) {
    $url['pass'] = urldecode($url['pass']);
  } else {
    $url['pass'] = '';}
  $url['host'] = urldecode($url['host']);
  $url['path'] = urldecode($url['path']);
  $url['port'] = $url['port'] ? $url['port'] : 3306;
  
  // Calculate the tid to start from (creating a parent if the vocabulary
  // has multiple roots (forest not tree).
  $results = db_query("SELECT t.tid FROM {term_data} t, {term_hierarchy} h WHERE parent = 0 AND vid = %d AND t.tid = h.tid;", $vid);
  // Following query added as db_num_rows no longer exists GAH!
  $results_count = db_query("SELECT COUNT(t.tid) FROM {term_data} t, {term_hierarchy} h WHERE parent = 0 AND vid = %d AND t.tid = h.tid;", $vid);
  $num_rows = array_pop(db_fetch_array($results_count));
  $tid;
  $temporary_root_created = false;
    
  if($num_rows==1){
    // Single tree, woot
    $tid = array_pop(db_fetch_array($results));
  } else {
    $temporary_root_created = true;
    // Either no results (Farp), Or we have a forest.  Lets tree it!
    // Create new term to be the base of the tree.
    // Because the term is temporary, we don't use taxonomy_ functions, but instead
    // use trusty SQL.
    // Create term
    db_query("INSERT INTO {term_data} (vid, name) VALUES (%d , 'TEMPORARY TERM')", $vid);
    $tid = db_last_insert_id('term_data','tid');
    db_query("INSERT INTO {term_hierarchy} (tid, parent) VALUES (%d , 0)", $tid);
    // Set parent of current roots as this term
    $result = db_query("SELECT t.tid FROM {term_hierarchy} h, {term_data} t WHERE t.tid = h.tid AND parent =0 AND vid = %d AND t.tid != %d",$vid, $tid);
    $tids = array();
    while($row=db_fetch_array($result)){
      $tids[] = $row['tid'];
    }
    db_query("UPDATE {term_hierarchy} SET parent = %d WHERE tid IN (".implode(",",$tids).")",$tid,$tid);
  }
  
  // Create the string which will become the PHP file to execute
  /**
   * FIXME - The following doesn't use Drupal's {table_name} shenanigans
   */  
  $tmp_php_file_string = '<?php
mysql_connect(\''.$url['host'].'\',\''.$url['user'].'\',\''.$url['pass'].'\','.$url['port'].');
mysql_select_db(\''.substr($url['path'], 1).'\');
mysql_query("'.db_prefix_tables("LOCK TABLES {term_data} READ, {term_hierarchy} READ, {leftandright} WRITE;").'");
function rebuild_tree($tid, $left, $depth) {
  $right = $left+1;
  $results = mysql_query("SELECT h.tid FROM '.db_prefix_tables('{term_hierarchy}').' h,'.db_prefix_tables('{term_data}').' t  WHERE parent = $tid AND t.tid=h.tid ORDER BY name");
  while ($row = mysql_fetch_array($results)){
    $right = rebuild_tree($row[0], $right, $depth+1);
  }  
  echo "($tid , '.$vid.' , $left , $right, $depth)";
  if($left != 1){
    echo ",";
  }
  return $right+1;
}
mysql_query("UNLOCK TABLES;"); 
echo "'.db_prefix_tables("LOCK TABLES {term_data} READ, {term_hierarchy} READ, {leftandright} WRITE;").'";
echo "DELETE FROM '.db_prefix_tables('{leftandright}').' WHERE vid = '.$vid.'; INSERT INTO '.db_prefix_tables('{leftandright}').' (tid, vid, lft, rgt, depth) VALUES ";
rebuild_tree('.$tid.',1, ';
  if($temporary_root_created){
    $tmp_php_file_string .= '-1';
  } else {
    $tmp_php_file_string .= '0';    
  }
  $tmp_php_file_string .= ');
echo ";";';
  
  // Add the deletion of the temporary term, if required, to the PHP file
  if($temporary_root_created){
    $tmp_php_file_string .= '
echo "DELETE FROM '.db_prefix_tables('{leftandright}').' WHERE tid = '.$tid.';DELETE FROM '.db_prefix_tables('{term_data}').' WHERE tid = '.$tid.';DELETE FROM '.db_prefix_tables('{term_hierarchy}').' WHERE tid = '.$tid.';";
echo "UPDATE '.db_prefix_tables('{term_hierarchy}').' SET parent = 0 WHERE parent = '.$tid.';";';
  }
  $tmp_php_file_string .= '
echo "UNLOCK TABLES;";';
  // Write this out to a temporary file which we'll EXEC!
  $filepath = file_create_filename('leftandright.php', file_directory_temp());
  file_save_data($tmp_php_file_string, $filepath, FILE_EXISTS_REPLACE);
  
  // FIXME - TEST FOR WINDOWS HERE, IF WINDOWS, EXECUTE BAT FILE INSTEAD
  // Lets try to execute the file
  if($windoze){
    
  } else {
    $file_to_execute = $_SERVER['DOCUMENT_ROOT'].base_path().drupal_get_path('module','leftandright').'/landr';
    // FIXME - Needs port
    $command = $file_to_execute.' '.$filepath.' '.$url['host'].' '.$url['user'].' '.$url['pass'].' '.substr($url['path'], 1).' '.$vid;
    $pid = shell_exec("nohup $command > /dev/null & echo $!");
    variable_set('leftandright_rebuild_pids',array_merge(variable_get('leftandright_rebuild_pids',array()),array($pid=>$pid)));
  }
  
  // Finally delete the file
  sleep(2); // Not sure if this sleep is necesary, but it seems to be
  file_delete($filepath);
  
  // Set message to say that we're doing the rebuild
  drupal_set_message(t('Rebuilding the tree for vocabulary:').' '.check_plain($vocabulary->name));
}