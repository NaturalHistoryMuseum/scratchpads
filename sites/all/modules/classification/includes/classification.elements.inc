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
        $tid = str_replace("n", "",$tid);
     }
 
     $names_options = variable_get('classification_elements_names',1);
     $canonicals = variable_get('classification_elements_canonicals',0);
     
     switch ($names_options) {
     	 // Show all names
     	 case 0:
         $result = db_query(db_rewrite_sql("
         SELECT 
           td.tid as tid, 
           td.name as name, 
           trt.name as relation, 
           trt.rtid as relation_type
         FROM
           {term_data} td
         JOIN 
           {term_hierarchy} th on (td.tid=th.tid) 
         LEFT JOIN 
         	 ({term_relation} tr 
         	    JOIN {term_relation_has_type} trht ON (tr.trid=trht.trid) 
         	    JOIN {term_relation_types} trt ON (trht.rtid=trt.rtid)
         	  ) ON td.tid=tr.tid1 
		     WHERE 
		       td.vid = %d AND th.parent = %d 
		     ORDER BY 
		       trt.rtid, td.name"),$vid,$tid);
         break;
       // Hide vernaculars
       case 1:
         $result = db_query(db_rewrite_sql("
         SELECT 
           td.tid as tid, 
           td.name as name, 
           trt.name as relation, 
           trt.rtid as relation_type
         FROM
           {term_data} td
         JOIN 
           {term_hierarchy} th on (td.tid=th.tid) 
         LEFT JOIN 
         	 ( {term_relation} tr 
			       JOIN {term_relation_has_type} trht on (tr.trid=trht.trid) 
			       JOIN {term_relation_types} trt on (trht.rtid=trt.rtid)
		       )  ON td.tid=tr.tid1 
		     WHERE 
		       td.vid = %d AND th.parent = %d AND ( trt.rtid <> 9 OR trt.rtid IS NULL ) 
		     ORDER BY td.name"),$vid,$tid);
         break;
       // Hide all except valid
       case 2:
         $result = db_query(db_rewrite_sql("
         SELECT 
           td.tid as tid, 
           td.name as name, 
           trt.name as relation, 
           trt.rtid as relation_type
         FROM
           {term_data} td
         JOIN 
           {term_hierarchy} th on (td.tid=th.tid) 
         LEFT JOIN 
         	 ( {term_relation} tr 
			       JOIN {term_relation_has_type} trht on (tr.trid=trht.trid) 
			       JOIN {term_relation_types} trt on (trht.rtid=trt.rtid)
		       ) ON td.tid=tr.tid1 
		     WHERE 
		       td.vid = %d AND th.parent = %d AND trt.rtid IS NULL 
		     ORDER BY 
		       td.name"),$vid,$tid);
         break;
       // Show all names
       default:
         $result = db_query(db_rewrite_sql("
         SELECT 
           td.tid as tid, 
           td.name as name, 
           trt.name as relation, 
           trt.rtid as relation_type
         FROM
           {term_data} td
         JOIN 
           {term_hierarchy} th on (td.tid=th.tid) 
         LEFT JOIN 
         	 ( {term_relation} tr 
			       JOIN {term_relation_has_type} trht on (tr.trid=trht.trid) 
			       JOIN {term_relation_types} trt on (trht.rtid=trt.rtid)
		       ) ON td.tid=tr.tid1 
		     WHERE 
		       td.vid = %d AND th.parent = %d 
		     ORDER BY 
		       trt.rtid, td.name"),$vid,$tid);         
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
      $str .= "\"attributes\" : { \"id\" : \"n" . $parent['tid'] . "\"";
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
   
   $names_options = variable_get('classification_elements_names',1);
   
   switch ($names_options) {
   	 // Show all names
   	 case 0:
       $result = db_query(db_rewrite_sql("
       SELECT 
         COUNT(*) AS count 
       FROM 
         {term_data} td 
       JOIN 
         {term_hierarchy} th ON (td.tid = th.tid) 
       WHERE 
         td.vid = %d AND th.parent = %d"), $vid,$tid);
       break;
     // Hide vernaculars
     case 1:
       $result = db_query(db_rewrite_sql("
         SELECT 
           COUNT(*) as count
         FROM
           {term_data} td
         JOIN 
           {term_hierarchy} th on (td.tid=th.tid) 
         LEFT JOIN 
         	 ({term_relation} tr 
         	    JOIN {term_relation_has_type} trht ON (tr.trid=trht.trid) 
         	    JOIN {term_relation_types} trt ON (trht.rtid=trt.rtid)
         	  ) ON td.tid=tr.tid1 
		     WHERE 
		       td.vid = %d AND th.parent = %d AND ( trt.rtid <> 9 OR trt.rtid IS NULL )"),$vid,$tid);
       break;
     // Hide all except valid
     case 2:
       $result = db_query(db_rewrite_sql("
       SELECT 
         COUNT(*) AS count
       FROM
         {term_data} td
       JOIN 
         {term_hierarchy} th on (td.tid=th.tid) 
       LEFT JOIN 
         ( {term_relation} tr 
			     JOIN {term_relation_has_type} trht on (tr.trid=trht.trid) 
			     JOIN {term_relation_types} trt on (trht.rtid=trt.rtid)
		     ) ON td.tid=tr.tid1 
       WHERE 
         td.vid = %d AND th.parent = %d AND trt.rtid IS NULL"), $vid,$tid);
       break;
     // Show all names by default
     default:
       $result = db_query(db_rewrite_sql("
       SELECT 
         COUNT(*) AS count 
       FROM 
         {term_data} td 
       JOIN 
         {term_hierarchy} th ON (td.tid=th.tid) 
       WHERE 
         td.vid = %d AND th.parent = %d"), $vid,$tid);
   }
   
   $object = db_fetch_object($result);
   return $object->count;
}

/**
 * Find what nodes in the tree should have checked checkboxes
 */
function classification_js_tree_checked($vid,$nid) {
	drupal_set_header('Content-Type: text/plain; charset=utf-8');
	$matches = array();
	
	$result = db_query(db_rewrite_sql("
	SELECT tid 
	FROM
	  {classification_node} 
	WHERE
	  nid = %d"),$nid);
	
	while ($object = db_fetch_object($result)) {
		$matches[] = $object->tid;
	}

	asort($matches);
	
	echo classification_checked_to_js($vid,$matches);
}

function classification_checked_to_js($vid,$matches) {
	$str = '';
	if(count($matches) > 0) {
      $str .= "[" . "\n";
    }
	foreach ($matches as $tid) {
		$str .= "{\"id\" : \"n" . $tid . "\"}";
		$counter++;
        if($counter !== count($matches)){
          $str .= ","  . "\n";
        }
	}
	if(count($matches) > 0) {
      $str .= "\n" . "]" . "\n";
    }
	return $str;
}