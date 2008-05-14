<?php

/**
 * The following script will export the data from a SID database
 * package it up so that it can be imported into a Scratchpad,
 * and even move the files for you (I think).
 */

// Connect to the SID database in question.  This is going to be a 
// completely manual process.
$sql = '
SELECT
  id,
  name,
  filename,
  original_filename,
  notes,
  GROUP_CONCAT(keywords.keyword) AS keywords
FROM image 
LEFT JOIN image_keyword ON image.id = image_keyword.image 
INNER JOIN keywords ON keywords.keyword_id = image_keyword.keyword 
LEFT JOIN copyright ON image.copyright = copyright.copyright_id 
LEFT JOIN image_type ON image_type_id = image.image_type 
LEFT JOIN taxon ON image.taxon = taxon_id 
GROUP BY image;';
$database_details = parse_ini_file('/var/www/drupal_db_passwords',true);
mysql_connect('localhost', 'root', $database_details['root']['password'], TRUE, 2);
mysql_select_db('milichiidae_sid');
$results = mysql_query($sql);

// Once the SID stuff is done, connect to Drupal
require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

while($row = mysql_fetch_assoc($results)){
  // First step, move the files to the import folder. Once the files are moved,
  // they should be imported.
  if (isset($_GET['files'])){
    // Simply mv the files to the tempdir for import
    $oldfile = "/var/www/html/sid/upload/user_uploads/milichiidae/".$row['filename'];
    $newfile = "/var/www/html/sid_import/".$row['id'].'-'.$row['original_filename'];
    rename($oldfile,$newfile);
  } else {
    
  // Once files are imported, they can be annotated with SID annotations
  
    // Get the new Drupal Node that this SID row represents
    $node_title = array_shift(explode('.',$row['id'])).'-'.array_shift(explode('.',$row['original_filename']));
    $node = node_load(array('title'=>$node_title));
    if($node){
      // This is the node we're looking for.
      
      // Lets not output anything from this section, so if we get no output it has hopefully worked,
      // whilst if we do, it should be error messages (below)
      
      // Name
      $name = explode(' ',$row['name']);
      array_pop($name);
      $name = implode(' ',$name);
      $term_name = taxonomy_get_term_by_name($name);
      
      // Keywords
      $keywords_to_add = explode(',',$row['keywords']);
      $keywords = array();
      foreach($keywords_to_add as $keyword){
        $keyword = ucfirst($keyword);
        $keyword_terms = taxonomy_get_term_by_name($keyword);
        $keyword_added = false;
        foreach($keyword_terms as $keyword_term){
          if($keyword_term->vid = 34){
            $keywords[] = $keyword_term;
            $keyword_added = true;
          }
        }
        if(!$keyword_added){
          $form_values = array('name'=>$keyword,'description'=>'','vid'=>34,'weight'=>0);
          taxonomy_save_term($form_values);
          $keywords[] = taxonomy_get_term_by_name($keyword);
        }        
      }
      
      // Notes -> Body
      $node->body = $row['notes'];
      node_save($node);
      
      $terms = taxonomy_node_get_terms($node->nid);
      $terms = array_merge($terms, $keywords, $term_name);
      taxonomy_node_save($node->nid, $terms);
      
    }else{
      // Some error here, output the row from SID!
      /*
      $results = mysql_query("SELECT * FROM node WHERE title = '$node_title';");
      while($row=mysql_fetch_array($results)){
        print_r($row);
      }
      */
      ;
    }
  }
}