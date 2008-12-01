<?php

require_once("classification_functions.class");
global $names;
$names = new Names;

/**
 * Callback for rendering tree
 */
 function classification_js_tree($vid,$tid='') {
 	   drupal_set_header('Content-Type: text/plain; charset=utf-8');   
     $matches = array();
 	  
     if(!$tid) {
        $tid = 0;
     }
     else {
        $tid = str_replace("node_", "",$tid);
     }
 
     $names_options = variable_get('classification_viewer_names',2);
     $canonicals = variable_get('classification_viewer_canonicals',1);
     
     switch ($names_options) {
     	 // Show all names
     	 case 0:
         $result = db_query(db_rewrite_sql("
         SELECT 
           d.tid as tid, 
           d.name as name, 
           a.name as relation, 
           a.rtid as relation_type 
         FROM 
           {term_relation_types} a 
         INNER JOIN 
           {term_relation_has_type} b ON a.rtid = b.rtid 
         INNER JOIN 
           {term_relation} c ON b.trid = c.trid 
         RIGHT JOIN 
           {term_data} d ON c.tid1 = d.tid 
         INNER JOIN 
           {term_hierarchy} e ON d.tid = e.tid 
         WHERE 
           d.vid = %d AND e.parent = %d 
         ORDER BY 
           a.rtid, d.name"), $vid,$tid);
         break;
       // Hide vernaculars
       case 1:
         $result = db_query(db_rewrite_sql("
         SELECT 
           d.tid as tid, 
           d.name as name, 
           a.name as relation, 
           a.rtid as relation_type 
         FROM 
           {term_relation_types} a 
         INNER JOIN 
           {term_relation_has_type} b ON a.rtid = b.rtid 
         INNER JOIN 
           {term_relation} c ON b.trid = c.trid 
         RIGHT JOIN 
           {term_data} d ON c.tid1 = d.tid 
         INNER JOIN 
           {term_hierarchy} e ON d.tid = e.tid 
         WHERE 
           d.vid = %d AND e.parent = %d AND ( a.rtid <> 9 OR a.rtid IS NULL ) 
         ORDER BY 
           a.rtid, d.name"), $vid,$tid);
         break;
       // Hide all except valid
       case 2:
         $result = db_query(db_rewrite_sql("
         SELECT 
           d.tid as tid, 
           d.name as name, 
           a.name as relation, 
           a.rtid as relation_type 
         FROM {term_relation_types} a 
         INNER JOIN 
           {term_relation_has_type} b ON a.rtid = b.rtid 
         INNER JOIN 
           {term_relation} c ON b.trid = c.trid 
         RIGHT JOIN 
           {term_data} d ON c.tid1 = d.tid 
         INNER JOIN 
           {term_hierarchy} e ON d.tid = e.tid 
         WHERE 
           d.vid = %d AND e.parent = %d AND a.rtid IS NULL 
         ORDER BY 
           a.rtid, d.name"), $vid,$tid); 
         break;
       // Show all names
       default:
         $result = db_query(db_rewrite_sql("
         SELECT 
           d.tid as tid, 
           d.name as name, 
           a.name as relation, 
           a.rtid as relation_type 
         FROM 
           {term_relation_types} a 
         INNER JOIN 
           {term_relation_has_type} b ON a.rtid = b.rtid 
         INNER JOIN 
           {term_relation} c ON b.trid = c.trid 
         RIGHT JOIN 
           {term_data} d ON c.tid1 = d.tid 
         INNER JOIN 
           {term_hierarchy} e ON d.tid = e.tid 
         WHERE 
           d.vid = %d AND e.parent = %d 
         ORDER BY 
           a.rtid, d.name"), $vid,$tid);         
     }

     global $names;

     while ($object = db_fetch_object($result)) {
       $tid = $object->tid;
       $patterns = array('/name/', '/ = homotypic/', '/ = heterotypic/');
       $relation = trim(preg_replace($patterns,'',$object->relation));
       // Use the canonical function in Patrick's class for the eventual browsing tree
       // But, only use if the classification is called 'Taxa'
       $vocab = classification_get_vocabulary($vid);
       if($canonicals == 1 && $relation !== 'vernacular' && $vocab == 'Taxa') {
          $name = $names->canonical_form($object->name);
       }
       else {
       	  $name = $object->name;
       }
       $matches[] = array('name' => check_plain($name), 'tid' => $tid, 'relation' => $relation);
     }

     echo classification_to_js($vid,$matches);
}

/**
 * Build some json for the tree renderer
 */
function classification_to_js($vid,$var) {
   if(!$var) return;
   $str = '';
   if(count($var) > 1) {
      $str .= "[" . "\n";
   }
   foreach($var as $parent) {
      $str .= "{" . "\n";
      $str .= "\"attributes\" : { \"id\" : \"node_" . $parent['tid'] . "\"";
      if($parent['relation']) {
      	$str .= ", \"class\" : \"" . $parent['relation'] . "\"";
      }
      $str .= " }," . "\n";
      $str .= "\"data\" : \"" . $parent['name'] . "\"";

      $children = classification_count_children($vid,$parent['tid']);
      if($children > 0){
         $str .= "," . "\n" . "\"state\" : \"closed\"" . "\n";
      }
      else {
      	 $str .= "\n";
      }
      
      $str .= "}";
      $counter++;
      if($counter !== count($var)){
         $str .= ","  . "\n";
      }
   }
   if(count($var) > 1) {
      $str .= "\n" . "]" . "\n";
   }

   return $str;
}

/**
 * Helper function to determine if term has children
 */
function classification_count_children($vid,$tid) {
   
   $names_options = variable_get('classification_viewer_names',2);
   
   switch ($names_options) {
   	 // Show all names
   	 case 0:
       $result = db_query(db_rewrite_sql("
       SELECT 
         COUNT(*) AS count 
       FROM 
         {term_data} a 
       INNER JOIN 
         {term_hierarchy} b ON a.tid = b.tid 
       WHERE 
         a.vid = %d AND b.parent = %d"), $vid,$tid);
       break;
     // Hide vernaculars
     case 1:
       $result = db_query(db_rewrite_sql("
       SELECT 
         COUNT(*) AS count 
       FROM 
         {term_relation_types} a 
       INNER JOIN 
         {term_relation_has_type} b ON a.rtid = b.rtid 
       INNER JOIN 
         {term_relation} c ON b.trid = c.trid 
       RIGHT JOIN 
         {term_data} d ON c.tid1 = d.tid 
       INNER JOIN 
         {term_hierarchy} e ON d.tid = e.tid 
       WHERE 
         d.vid = %d AND e.parent = %d AND ( a.rtid <> 9 OR a.rtid IS NULL )"), $vid,$tid);
       break;
     // Hide all except valid
     case 2:
       $result = db_query(db_rewrite_sql("
       SELECT 
         COUNT(*) AS count 
       FROM 
         {term_relation_types} a 
       INNER JOIN 
         {term_relation_has_type} b ON a.rtid = b.rtid 
       INNER JOIN 
         {term_relation} c ON b.trid = c.trid 
       RIGHT JOIN 
         {term_data} d ON c.tid1 = d.tid 
       INNER JOIN 
         {term_hierarchy} e ON d.tid = e.tid 
       WHERE 
         d.vid = %d AND e.parent = %d AND a.rtid IS NULL"), $vid,$tid);
       break;
     // Show all names by default
     default:
       $result = db_query(db_rewrite_sql("
       SELECT 
         COUNT(*) AS count 
       FROM 
         {term_data} a 
       INNER JOIN 
         {term_hierarchy} b ON a.tid = b.tid 
       WHERE 
         a.vid = %d AND b.parent = %d"), $vid,$tid);
   }
   
   $object = db_fetch_object($result);
   return $object->count;
}