<?php

/**
 * Form to coordinate the import of names from files or recursive web service calls
 */
function classification_import_form(&$form_state,$vid='') {
  global $relations, $conf;

  //get the rank, relations, and vernacular vocabs for later reuse
  $relations = array();
  $query = db_query("SELECT rtid,name,vid FROM {term_relation_types}");
  while($result = db_fetch_array($query)) {
    $relations[$result['name']] = $result; 	
  }

  $module_path = drupal_get_path('module', 'classification') .'/';
  drupal_add_css($module_path .'css/classification.css');
  drupal_add_js($module_path .'js/jquery.scrollTo.js');
  drupal_add_js($module_path . 'js/classification_import.js');

  $form = array(
    '#cache' => TRUE,
  );

  $help = theme('advanced_help_topic', 'classification' , 'import');

  $form['status_bar'] = array(
    '#type' => 'markup',
    '#value' => ' ',
    '#prefix' => '<div id="message"></div>',
  );

  if(variable_get('classification_status',1)==0) {
	  $message = '<div class=\'messages warning\'>' . t('Sorry, your classification is currently being imported or edited. Please wait a moment and try again.') .  '</div>';

	  $form['title'] = array(
	    '#type' => 'markup',
	    '#value' => $message,
	   );
	
	return $form;	
  }
  else {
	  $message = '<div class=\'messages status\'>' . t('You have three options to import your names and/or classification(s). Please see the ' . l(t('help site'), 'http://help.' . $conf['_lifedesks_domain'] . '/classification/import', array('absolute' => TRUE)) . ' for instruction. If you create any ' . l(t('alternate classifications'),'admin/content/taxonomy') . ' to help manage your taxonomic names, this page will include a drop-down menu. The interaction between content forms and your classification (<em>e.g.</em> multi-tagging or tree selector) may be adjusted') . ' ' . l(t('here'),'admin/content/taxonomy') . '.</div>';

	  $form['title'] = array(
	    '#type' => 'markup',
	    '#value' => $message,
	   );	
  }

  $vocabs = taxonomy_get_vocabularies();
  $classification_settings = variable_get('classification_settings',array());

  $opt[''] = '--Choose--';
  foreach($vocabs as $vocab) {
    if(array_key_exists($vocab->vid, $classification_settings) or ($vocab->vid == classification_get_vid('Taxa'))) {
	  if($vocab->name == 'Taxa') 
	  {
	    $opt[$vocab->vid] = variable_get('site_name', 'Taxa') . ' classification';	
	  }
	  elseif ($vocab->name != 'Deleted Names') 
	  {
	    $opt[$vocab->vid] = $vocab->name;
	  }
    }	
  }

  if(count($opt) > 2) {
    $form['vocab'] = array(
      '#type' => 'fieldset',
      '#title' => t('Destination for Import'),
    );

    $form['vocab']['vid'] = array(
      '#type' => 'select',
      '#title' => t('Destination Classification for Import'),
      '#required' => TRUE,
      '#options' => $opt,
      '#default_value' => arg(4),
    );
  }
  else {
  	$form['vocab']['vid'] = array(
  		'#type' => 'hidden',
  		'#value' => classification_get_vid('Taxa'),
  		);
  }
 
  $form['batch_import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Batch Import (enter names manually)'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['batch_import']['name_list'] = array(
    '#type' => 'textarea',
    '#title' => t('One name per line'),
    '#cols' => 30,
  );

  $form['batch_import']['import_list'] = array(
    '#type' => 'submit', 
    '#value' => t('Import Names'),
  );
  
  $form['file_import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import From Text File'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $ifopts = array(0=>"- FILE FORMAT -", 3=>"Full hierarchy", 1=>"Child/Parent");
  
  $form['file_import']['import_format'] = array(
    '#type' => 'select',
    '#title' => t('Input file format'),
    '#options' => $ifopts,
    '#field_suffix' => $help,
    '#description' => t('<b>Full hierarchy</b> has header row like "<tt>taxon taxon rank description synonyms</tt>" or "<tt>genus species description synonyms</tt>"<br> 
    <b>Child/Parent</b> has no header, rows are like "<tt>taxon parent description synonym|_vernacular_</tt>". Columns are tab-delimited.'),
  );
  
  $form['file_import']['data'] = array(
    '#type' => 'file',
    '#title' => t('File to import'),
  );

  $form['file_import']['import'] = array(
    '#type' => 'submit',
    '#value' => t('Import File'),
  );

  $form['eol_import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import From EOL'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
      
  $form['eol_import']['taxonomic_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Search for highest taxonomic name of relevance'),
    '#description' => t('This must be a parent name for all taxa to be treated on this site.'),
  );
  
  $form['eol_import']['search'] = array(
    '#type' => 'submit', 
    '#value' => t('Search'),
    '#suffix' => '<div id="search_results"></div><div id="import_status"></div>',
  );

  $form['eol_import']['total_names'] = array(
     '#type' => 'hidden',
  );
  
  $form['#attributes'] = array('enctype' => 'multipart/form-data');

/*
$form['delete'] = array( // Debug only!
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
*/

return $form;
}

function classification_services_proxy() {
	require_once("xml2json.php");
	
	unset($_GET['q']);
	$querystring = http_build_query($_GET,'','&');
	$response = drupal_http_request("http://services.eol.org/lifedesk/service.php?" . $querystring);

	$str = '{ "status" : ';
	if($response->code == 200) {
		$status = '"ok"';
		$json = xml2json::transformXmlStringToJson($response->data);
		$data = $json ? $json : '""';
		if($json) {
		  switch($_GET['function']) {
		  	case 'search':
		  	  $message = '"' . t('Choose from the following classification(s). Ranks above your search are shown to help you choose.') . '"';
			break;
			case 'details_tcs':
			  $message = '""';
			break;
			default:
		  }
	   }
	   else {
	     $message = '"' . t('Sorry, there was nothing found by that name') . '"';	
	   }
	}
	else {
		$status = '"failed"';
		$message = '"' . t('Sorry, this service is currently unavailable') . '"';
		$data = '""';
	}
	
	$str .= $status . ', "message" : ' . $message . ', "data" : ' . $data;
	$str .= " }";
	
	echo $str;
}

/**
 *  Import form submission
 */
function classification_import_form_submit($form, &$form_state) {
  switch ($form_state['values']['op']) {
    case t('Import Names'):
      variable_set('classification_status',0);
      classification_import_names($form, $form_state);
      variable_set('classification_status',1);
    break;
    case t('Import File'):
      variable_set('classification_status',0);
      classification_import_file($form, $form_state);
      variable_set('classification_status',1);
    break;
    case t('Search!'):
      // Nothing here because we use AHAH framework
    break;
    case t('Delete'):
      classification_delete_primary(); // Debug only!
    break;
    case t('Import Classification!'):
    // Nothing here because we use a callback
    break;
    default:
    break;
  }
}

function classification_delete_primary() {
	
  $vid = classification_get_vid('Taxa');
  $tree = taxonomy_get_tree($vid);
  foreach ($tree as $branch) {
    $res = db_query("SELECT trid FROM {term_relation} WHERE tid1 = %d", $branch->tid);
    while ($term = db_fetch_object($res)) { 
	  db_query("DELETE FROM {term_relation_has_type} WHERE trid = %d", $term->trid); 
	}
  }

  variable_del('classification_viewer_names');
  variable_del('classification_viewer_canonicals');
  variable_del('classification_autocomplete_size');

  //make the new primary classification
  $vocab = array(
	 'name' => 'Taxa',
	 'hierarchy' => 1,
	 'module' => 'taxonomy',
	 'description' => t('The primary classification'),
	 'tags' => 1,
	 'weight' => -10
  );
  taxonomy_save_vocabulary($vocab);

  //update the classification_settings variable
  $classification_settings = variable_get('classification_settings',array());
  $settings = $classification_settings[$vid]['settings'];
  $classification_settings[$vocab['vid']] = array(
    'settings' => $settings,
  );
  variable_set('classification_settings',$classification_settings);

  //now delete the original vocabulary
  taxonomy_del_vocabulary($vid);
}

/**
 * Classification import callback
 */
function classification_import_classification() {
	$form = array(
    '#post' => $_POST,
  );
  
  $tid = $_REQUEST['tid'];
  $vid = $_REQUEST['vid'];
  $tot_count = $_REQUEST['tot_count'];
  
  $form_state = array('submitted' => FALSE);
  
  if($tid && $vid) {
    $url = 'http://services.eol.org/lifedesk/service.php?function=details_tcs&id=' . $tid;
    classification_import_tcs_init($url,$vid,$tot_count);
  }
}
 
/**
 * Start the loop process by first getting the highest tid in the {term_data} table
**/
function classification_import_tcs_init($url, $voc, $tot_count) {

   global $id, $vid, $counter, $user, $relations, $syn_tid, $vern_tid;

   if(!$voc) {
     drupal_set_message(t('Sorry, you need to select a classification before you may import names.'),'error');
     return;
   }

   //get the rank, relations, and vernacular vocabs for later reuse (again, because this was a callback)
   $relations = array();
   $query = db_query("SELECT rtid,name,vid FROM {term_relation_types}");
   while($result = db_fetch_array($query)) {
     $relations[$result['name']] = $result; 	
   }

   //get the tid for the term 'synonym' for later reuse and if not found, then add it
   $syn_tid = db_result(db_query("SELECT tid FROM {term_data} WHERE vid=%d and LOWER(name)='%s'",$relations['taxonomic relationships']['vid'],'synonym'));
   if(!$syn_tid) {
     $syn_term = array(
	   'name' => 'synonym',
	   'vid' => $relations['taxonomic relationships']['vid'],
	   'parent' => 0,
	 );	
	 taxonomy_save_term($syn_term);
	 $syn_tid = $syn_term['tid'];
   }

   $vern_tid = db_result(db_query("SELECT tid FROM {term_data} WHERE vid=%d AND name='vernacular name'",$relations['taxonomic relationships']['vid']));

   variable_set('classification_status',0);

   $vid = $voc;
   $counter = 1;

   $cache_data = array(
	    'curr_count' => "$counter",
	    'term' => "" . t('Getting started...') . ""
	  );
   cache_set('classification_name_' . $user->uid . $user->login,$cache_data);

   classification_import_tcs_loop($url,'full');

   if(module_exists('nestedset')){
	module_load_include("rebuild.inc", "nestedset");
	nestedset_rebuild_tree($vid);
	}
	
   cache_clear_all('classification_name_' . $user->uid . $user->login,'cache');

   variable_set('classification_status',1);

}

/**
 * Classification import status callback
 */
function classification_import_status($tot_count='') {
	header('Content-Type: text/plain; charset=utf-8');
	header('Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0, max-age=0');
	header('Pragma: no-cache');
	header('Expires: Thu, 01 Jan 1970 00:00:00 GMT');
	
	global $user;
  
    $data = cache_get('classification_name_' . $user->uid . $user->login);
	$name = check_plain($data->data['term']);
	$curr_count = $data->data['curr_count'];
    
    drupal_json(array('status' => 'success', 'curr_count' => $curr_count ,'tot_count' => $tot_count, 'name' => $name));
}

/**
 *  Recursion function for import of names from a TCS web service
 *  There is no appropriate error checking and that should be implemented 
 */
function classification_import_tcs_loop($url, $type, $tid='', $pid=false) {

   global $id,$user,$vid,$counter,$temp_table,$relations,$syn_tid,$vern_tid;

   $response = drupal_http_request($url);
   if(!$response->code == 200) {
      drupal_set_message(t('Sorry, that URL is unreachable. If an import ended prematurely, you may want to delete the classification and start the process again.'), 'error');
      return;
   }
  
   $id++;

   if(!$pid) {
     $pid = 0;	
   }

   $data = $response->data;
   $xml = simplexml_load_string($data);

   $TaxonNames = $xml->TaxonNames;
   $TaxonConcept = $xml->TaxonConcepts->TaxonConcept;
   $TaxonRelationships = $TaxonConcept->TaxonRelationships;
   $ProviderSpecificData = $TaxonNames->TaxonName->ProviderSpecificData->NameSources;

   $simple_name = $TaxonNames->TaxonName->Simple;
   $canonical_name = $TaxonNames->TaxonName->CanonicalName->Simple;
   $rank = strtolower($TaxonNames->TaxonName->Rank);

  switch ($type) {
    case 'full':
      $term = array(
        'name' => $simple_name,
        'parent' => $pid,
        'vid' => $vid,
      );
      taxonomy_save_term($term);

      if(trim($rank) != "") {
	      //insert the rank relationship & if not recognized, then add it
	      $rank_db = db_result(db_query("SELECT td.tid FROM {term_data} td WHERE td.vid=%d AND LOWER(td.name)='%s'",$relations['taxonomic ranks']['vid'],$rank));
	      if(!$rank_db) {
		    $new_rank = array(
			  'name' => $rank,
			  'vid' => $relations['taxonomic ranks']['vid'],
			  'parent' => 0,
			);
			taxonomy_save_term($new_rank);
			$rank_db = $new_rank['tid'];
	      }

	      db_query("INSERT INTO {term_relation} (tid1,tid2) VALUES (%d,%d)",$term['tid'],$rank_db);
	      $trid = db_last_insert_id('term_relation','trid');

	      db_query("INSERT INTO {term_relation_has_type} (trid,rtid) VALUES (%d,%d)",$trid,$relations['taxonomic ranks']['rtid']);
      }
   
      $cache_data = array(
	    'curr_count' => "$counter",
	    'term' => "$simple_name"
	  );
      cache_set('classification_name_' . $user->uid . $user->login, $cache_data);
    break;
    case 'syn':
      $syn = array(
	    'name' => $simple_name,
	    'vid' => $vid,
	    'parent' => $pid,
	  );
	  taxonomy_save_term($syn);

      db_query("INSERT INTO {term_relation} (tid1,tid2) VALUES (%d,%d)",$syn['tid'],$syn_tid);
      $trid = db_last_insert_id('term_relation','trid');

      db_query("INSERT INTO {term_relation_has_type} (trid,rtid) VALUES (%d,%d)",$trid,$relations['taxonomic relationships']['rtid']);

      $cache_data = array(
	    'curr_count' => "$counter",
	    'term' => "$simple_name"
	  );
      cache_set('classification_name_' . $user->uid . $user->login,$cache_data);
    break;
    case 'vern':
      $simple_name = $TaxonConcept->Name;
      $language = $TaxonConcept->Name->attributes()->language;

      $vern = array(
	    'name' => $simple_name,
	    'vid' => $vid,
	    'parent' => $pid,
	  );
	  taxonomy_save_term($vern);
	
	  //we need the vern tid for the taxonomic relationship
	  db_query("INSERT INTO {term_relation} (tid1,tid2) VALUES (%d,%d)",$vern['tid'],$vern_tid);
	  $trid = db_last_insert_id('term_relation','trid');
	
	  db_query("INSERT INTO {term_relation_has_type} (trid,rtid) VALUES (%d,%d)",$trid,$relations['taxonomic relationships']['rtid']);
	
	  //now we do the same for the ISO language code but only if we have a match
	  if($language) {
		$lang_tid = db_result(db_query("SELECT tid FROM {term_data} WHERE vid=%d AND RIGHT(name,2)='%s'",$relations['names to vernacular codes']['vid'],$language));
		if($lang_tid) {
		  db_query("INSERT INTO {term_relation} (tid1,tid2) VALUES (%d,%d)",$vern['tid'],$lang_tid);
		  $trid = db_last_insert_id('term_relation','trid');

		  db_query("INSERT INTO {term_relation_has_type} (trid,rtid) VALUES (%d,%d)",$trid,$relations['names to vernacular codes']['rtid']);	
		}	
	  }

      $cache_data = array(
	    'curr_count' => "$counter",
	    'term' => "$simple_name"
	  );
      cache_set('classification_name_' . $user->uid . $user->login,$cache_data);
    break;
  }

  //Slow down the recursion a hundredth of a second
  usleep(10000);
  $counter++;
  
  if($type == 'full') { 
    foreach ($TaxonRelationships->TaxonRelationship as $relationships) {
  	 switch ($relationships->attributes()->type) {
  	   case "is child taxon of":
  	     break;
  	   case "is parent taxon of":
  	     $url = $relationships->ToTaxonConcept->attributes()->ref;
  	     classification_import_tcs_loop($url,'full',$id,$term['tid']);
  	     break;
  	   case "has synonym":
  	     $url = $relationships->ToTaxonConcept->attributes()->ref;
  	     classification_import_tcs_loop($url,'syn',$id,$term['tid']);
         break;
       case "has vernacular":
         $url = $relationships->ToTaxonConcept->attributes()->ref;
         classification_import_tcs_loop($url,'vern',$id,$term['tid']);
         break;
  	 }
    }	     
  }
}

/**
 * Batch import function
 */
function classification_import_names($form,$form_state) {

  $vid = $form_state['values']['vid'];

  $parent = $form_state['values']['classification']['name_tags'][$vid] ? $form_state['values']['classification']['name_tags'][$vid] : 0;
  
  $whole = trim($form_state['values']['name_list']);
  $temp = explode("\n",$whole);
  foreach ($temp as $a) {
    $taxon[$a]['name'] = trim($a);
    $taxon[$a]['parent'] = $parent;
    $taxon[$a]['vid'] = $vid;
	taxonomy_save_term($taxon[$a]);
  }

   if(module_exists('nestedset')){
	module_load_include("rebuild.inc", "nestedset");
	nestedset_rebuild_tree($vid);
	}

  $link = l(t('View and Edit'),'admin/classification/biological/edit');
  drupal_set_message(t('Your names were successfully imported. ' . $link . ' your classification'));
}

/**
* File import function
*/
function classification_import_file($form, &$form_state) {
  set_time_limit(180);
  if ($form_state['values']['vid'] == 0)
  {
    form_set_error('vid',t('Please select a vocabulary for classification'));
  }
  if ($form_state['values']['url']=="")
  {
	if($form_state['values']['import_format'] == 0)
	{
		form_set_error('import_format',t('Please select a File Format'));
	}
	if ($file = file_save_upload('data'))
	{
		if($file->filemime !== 'text/plain' || $file->filesize < 1) {
			file_delete($file->filepath);
			form_set_error('data', t('Sorry, your uploaded file must be a tab-delimited text file and must have some content'));
		}
		else {
		  $fd = fopen($file->filepath, "rb");
		  $info = fstat($fd);
		  $len = $info["size"];
		  $text_init = fread($fd, $len);
		  if (!drupal_validate_utf8($text_init))
			{
			$text = utf8_encode($text_init);
			}
		  else
			{
			$text = removeBOM($text_init);
			}
                  fclose($fd);
		  if(!$fd)
		  {
			form_set_error('data', t('Classification import failed: file %filename cannot be read.', array('%filename' => $file->filename)));      
		  }
		  else
		  {
			classification_import_parse($text, $form_state['values']['vid'], $form_state['values']['import_format']);
			$currname = date("Ymd_Gi_").'classificationimport.txt.gz';
			file_save_data(gzencode($text), 'classification/temp/'.$currname, FILE_EXISTS_REPLACE);
		  }
		}

	}
	else
	{
		form_set_error('data', t('Classification import failed: file was not uploaded.'));
	}
  }
}

// Parse the file, creating the correct SQL as we go.
// Each line will have an entry in the term_data table, and term_hierarchy table 
function classification_import_parse($text, $vid, $import_format) {

	//classification_import_set_relation_types(); get them!!!
	$key_terms = array();
	$key_terms_query = db_query("SELECT tid, td.name AS name FROM {term_data} td LEFT JOIN {vocabulary} vb ON td.vid = vb.vid 
		WHERE vb.name = 'Taxonomic Relationships' OR vb.name = 'Vernacular Language Codes';");
	while($result = db_fetch_array($key_terms_query)) 
		{
		$key_terms[$result["tid"]] = $result["name"];
		}
	$is_synonym_term_tid = array_search("synonym", $key_terms);
	$is_vernacular_term_tid = array_search("vernacular name", $key_terms);
	$vernacular_default_language = array_search("English,en", $key_terms);
	
	$relation_types = array();
	$relation_types_query = db_query("SELECT rtid, name FROM {term_relation_types};");
	while($result = db_fetch_array($relation_types_query)) 
		{
		$relation_types[$result["rtid"]] = $result["name"];
		}
	$synonym_relationship_rtid = array_search("taxonomic relationships", $relation_types);
	$vernacular_relationship_rtid = array_search("names to vernacular codes", $relation_types);
	$rank_relationship_rtid = array_search("taxonomic ranks", $relation_types);
		
	$available_ranks = array();
	$ranks_query = db_query("SELECT tid, td.name AS name FROM {term_data} td LEFT JOIN {vocabulary} vb ON td.vid = vb.vid WHERE vb.name = 'Taxonomic Ranks';");
	while($result = db_fetch_array($ranks_query)) 
		{
		$available_ranks[$result["tid"]] = $result["name"];
		}
		
	$ranks_vid = classification_get_vid('Taxonomic Ranks');
	
	// Let's first get the lowest value for "tid", upon which we will build all the other values	
	$max_query = db_query("SELECT MAX(`tid`) AS tid FROM {term_data};");
	$sequence = db_fetch_object($max_query)->tid + 1;

	$term_data = "";
	$term_hierarchy = "";
	$lines = preg_split("/[\n\r]+/", $text);
	
	if (count($lines) > 15000)
		{
		drupal_set_message(t('Sorry, there is a limit of 15,000 names. Please shrink your text file or choose another classification'), $type= 'error');
		return;
		}		

	$synonym_term_data = $synonym_relation = $synonym_hierarchy = $synonym_tids = $rank_relations = $rank_tids = $new_ranks = "";
	$vern_tids = array();

    	if($import_format == 1){
  		// File is in the format:
  		/*
			kingdom1	any	descr1	syn1	kingdom
			phylum1	kingdom1
			phylum2	kingdom1			phylum
			class1	phylum1			class
			order1	class1		syn2|syn3	order
			class2	phylum1
  		*/
     
  		// Each line of the taxonomy constitues an entry into both of the tables.
  		// Fill out the term_data first, filling out an array name => parent_name
 		// name => number
  		// which can then be converted into numbers.
  		$parents = array();
  		$term_id = array();
 		foreach($lines as $line)
			{
   			if (trim($line)=="") break;
			$line_parts = split("\t", trim($line));
			// Remove '"' if at start or end of part
			for($i = 0; $i < count($line_parts); $i++)
				{
				$line_parts[$i] = trim($line_parts[$i]);
				if(substr($line_parts[$i], 0, 1) == '"') $line_parts[$i] = substr($line_parts[$i], 1, strlen($line_parts[$i]) - 2);
				}
        
			$name = $line_parts[0];
			$parent = $line_parts[1];
			$description = (isset($line_parts[2])) ? $line_parts[2] : '';
			$field_rank = (isset($line_parts[4])) ? $line_parts[4] : '';
					
			$synonyms = (isset($line_parts[3]) and $line_parts[3] != '') ? split("\|", $line_parts[3]) : '';
			$sequence_p = $sequence;
			if($synonyms != '') foreach($synonyms as $synonym)
				{
				if(empty($synonym)) break;
				$synonym_name = trim($synonym, "\"\t\n\r .");
				$vern_name = trim($synonym_name, "_");
				if ($sequence == $sequence_p) $sequence++;
				if ($synonym_name != $vern_name) 
					{
					$synonym_name = $vern_name;
					$vern_tids[] = $sequence;
					}
				if (($synonym_term_data != "")&&($synonym_name != "")) $synonym_term_data .= ",";
				if ($synonym_name != "") $synonym_term_data .= "(" . $sequence . "," . $vid.",'" . addslashes($synonym_name) . "','',0)";
				if ($synonym_relation != "") $synonym_relation .= ",";
				if (in_array($sequence, $vern_tids))
					{
					$synonym_relation .= '('.$sequence.','.($is_vernacular_term_tid).')';
					$synonym_relation .= ',('.$sequence.','.($vernacular_default_language).')';
					}
				else
					{
					$synonym_relation .= '('.$sequence.','.($is_synonym_term_tid).')';
					}
				if ($synonym_hierarchy != "") $synonym_hierarchy .= ",";
				$synonym_hierarchy .= '(' . $sequence . ',' . ($sequence_p) . ')';
				if ($synonym_tids != "") $synonym_tids .= " OR tid1=";
				$synonym_tids .= $sequence;
				$sequence++;
				}
      	
			// field_rank_data, let synonyms do not have rank
			if ($field_rank != "")
				{
				$rank_enhanced = strtolower(trim(preg_replace("/[ -]/", "", $field_rank)));
				$rank_tid = array_search($rank_enhanced, $available_ranks);
				if ($rank_tid == "")
					{
					$sequence++;
					$rank_tid = $sequence;
					if ($new_ranks != "") $new_ranks .= ",";
					$new_ranks .= "(" . $rank_tid . "," . $ranks_vid . ",'" . addslashes($rank_enhanced) . "','',0)";
					}
				if ($rank_relations != "") $rank_relations .= ",";
				$rank_relations .= '(' . $sequence_p . ',' . ($rank_tid) . ')';
				if ($rank_tids != "") $rank_tids .= " OR tid1=";
				$rank_tids .= $sequence_p;
				}
			if ($term_data !="") $term_data .=",";
			$term_data .= "(" . $sequence_p . "," . $vid . ",'" . addslashes($name) . "','" . addslashes($description) . "',0)";
			$parents[$name]=$parent;
			$term_id[$name]=$sequence_p;
			$sequence++;
			}
  		// We should now have all the term_data SQL created, and know the IDs for all of the names, we can now create the hierachy SQL.
		$names = array_keys($parents);
		foreach ($names as $name){
        	// Convert the parent name into an ID
        	if ($term_hierarchy!="")
          	$term_hierarchy .=",";
        	$parent_id = $term_id[$parents[$name]];
        	if ($parent_id == "")
          	$parent_id = 0;
        	$term_hierarchy .= '('.$term_id[$name].','.$parent_id.')';
		}
    }
    elseif($import_format == 3)
	{
		//File is in the format:
		/*
			(1) rank-column
			===
				taxon	taxon	taxon	taxon	rank	description	synonyms
				kingdom1				kingdom
				kingdom1	phylum1			phylum
				kingdom1	phylum1	class1		classis	descr0
				kingdom1	phylum1	class1	order1	order	descr1	syn1|syn2
				kingdom1	phylum1	class1	order2	order		syn3|syn4
				kingdom1	phylum1	class2	order3	order	descr3	syn5
			===
			OR
			(2) rank-row
			===
				kingdom	phylum	class	order	description	synonyms
				kingdom1				
				kingdom1	phylum1			
				kingdom1	phylum1	class1		descr0
				kingdom1	phylum1	class1	order1	descr1	syn1|syn2
				kingdom1	phylum1	class1	order2		syn3|syn4
				kingdom1	phylum1	class2	order3	descr3	syn5
			===
		*/
	
      	$first_line = array_shift($lines);
      	$first_line_parts = split("\t", trim($first_line));
      	$number_of_ranks = 0;
      	$line_parts_index = array();
      	$first_line_parts_cleaned = array();
      	$i=0;
      	foreach($first_line_parts as $part){
		$p = strtolower(trim($part));
		if ($p == 'synonym') $p = 'synonyms';
		if ($p == 'descriptions') $p = 'descriptions'; // !!! check for other names here !!!
        	if ($p != 'rank' and $p != 'description' and $p != 'synonyms')
			{
			$number_of_ranks++;
			} 
			else 
			{
			$line_parts_index[$p] = $i;
			}
			$first_line_parts_cleaned[] = $p;
        	$i++;
      	}
      	// checking sub-format: rank row or rank column
      	if (!in_array('rank', $first_line_parts_cleaned))
		{
		$rank_row = TRUE;
		}
      	else
		{
		$rank_row = FALSE;
		}
      	$names = array();
      	$hierarchy = array();
      	$vern_tids = array();
      	foreach ($lines as $line)
		{
        	if (trim($line)!="")
	  		{
          		$line_parts = split("\t", trim($line));
          		$taxon_parts = array();
          		for($i = 0; $i < $number_of_ranks; $i++)
				{
				// !in_array() results in saving the first occurence of name
				if(!in_array(trim($line_parts[$i]), $names) && trim($line_parts[$i] != ""))
					{
					$name = trim($line_parts[$i]);
					$names[$sequence] = $name;
					$sequence_p = $sequence;
					if ($line_parts[$i+1] == "" or $i == $number_of_ranks-1)
					{
						if ($rank_row)
							{
							$field_rank = $first_line_parts_cleaned[$i];
							}
						else
							{
							$field_rank = $line_parts[$line_parts_index['rank']];
							}
						$reference = $line_parts[$line_parts_index['description']];
						$synonyms = split("\|", $line_parts[$line_parts_index['synonyms']]);
						if (!empty($synonyms[0])) foreach($synonyms as $synonym)
							{
							if(empty($synonym)) break;
							$synonym_name = trim($synonym, "\"\t\n\r .");
							$vern_name = trim($synonym_name, "_");
							if ($sequence == $sequence_p) $sequence++;
							if ($synonym_name != $vern_name) 
								{
								$synonym_name = $vern_name;
								$vern_tids[] = $sequence;
								}
							if ($synonym_term_data != "") $synonym_term_data .=",";
							if ($synonym_name != "") $synonym_term_data .= "(".$sequence.",".$vid.",'".addslashes($synonym_name)."','',0)";
							if ($synonym_relation != "") $synonym_relation .= ",";
							if (in_array($sequence, $vern_tids))
								{
								$synonym_relation .= '('.$sequence.','.($is_vernacular_term_tid).')';
								$synonym_relation .= ',('.$sequence.','.($vernacular_default_language).')';
								}
							else
								{
								$synonym_relation .= '('.$sequence.','.($is_synonym_term_tid).')';
								}
							if ($synonym_hierarchy != "") $synonym_hierarchy .= ",";
							$synonym_hierarchy .= '(' . $sequence . ',' . ($sequence_p) . ')';
							if ($synonym_tids != "") $synonym_tids .=" OR tid1=";
							$synonym_tids .= $sequence;
							$sequence++;
							}
					}
					else
					{
						if ($i < $number_of_ranks - 1 and $rank_row)
							{
							$field_rank = $first_line_parts_cleaned[$i];
							}
						else
							{
							$field_rank = "";
							}
					}
				
					// term_data
					if ($term_data !="") $term_data .=",";
					$term_data .= "(".$sequence_p.",".$vid.",'".addslashes($name)."','".addslashes(trim($reference))."',0)";

					// rank data
					if ($field_rank != "")
						{
						$rank_enhanced = strtolower(trim(preg_replace("/[ -]/", "", $field_rank)));
						$rank_tid = array_search($rank_enhanced, $available_ranks);
						if ($rank_tid == "")
							{
							$sequence++;
							$rank_tid = $sequence;
							if ($new_ranks != "") $new_ranks .= ",";
							$new_ranks .= "(" . $rank_tid . "," . $ranks_vid . ",'" . addslashes($rank_enhanced) . "','',0)";
							}
						if ($rank_relations != "") $rank_relations .= ",";
						$rank_relations .= '(' . $sequence_p . ',' . ($rank_tid) . ')';
						if ($rank_tids != "") $rank_tids .= " OR tid1=";
						$rank_tids .= $sequence_p;
						}
					
					// term_hierarchy
					if ($term_hierarchy !="") $term_hierarchy .=",";
					if ($i > 0)
						{
						// Add to hierarchy, allows to skip one (only one!) parent
						$parent = trim($line_parts[$i - 1]);
						if ($parent != "")
							{
							$parent_id = array_search($parent, $names);
							}
						else
							{
							$parent_up = trim($line_parts[$i - 2]);
							$parent_id = array_search($parent_up, $names);
							}
						$term_hierarchy .= '('.$sequence_p.','.$parent_id.')';
						}
						else 
						{
							$root_tid = 0;
							$term_hierarchy .= '(' . $sequence_p . ',' . $root_tid . ')';
						}
						$sequence++;
					}
				}
			}
		}
	}

// ====	

$term_hierarchy_query = "INSERT INTO {term_hierarchy} VALUES ".$term_hierarchy;
$term_data_query = "INSERT INTO {term_data} VALUES ".$term_data;

$import_error = 'There were error(s) in your text file, it was not (fully) imported.';
$data_success = db_query($term_data_query) or die(drupal_set_message(t($import_error), 'error'));
$hierarchy_success = db_query($term_hierarchy_query) or die(drupal_set_message(t($import_error), 'error'));

if ($synonym_term_data != "")
	{
      	$synonym_term_data_query = "INSERT INTO {term_data} VALUES ".$synonym_term_data;
      	$synonym_hierarchy_query = "INSERT INTO {term_hierarchy} (`tid`,`parent`) VALUES ".$synonym_hierarchy;
      	$synonym_relation_query = "INSERT INTO {term_relation} (`tid1`,`tid2`) VALUES ".$synonym_relation;
      	$synonym_tids_query = "SELECT trid, tid1, tid2 FROM {term_relation} WHERE tid1=".$synonym_tids;
	}

if ($synonym_term_data_query != "" and $data_success and $hierarchy_success)
	{
	db_query($synonym_term_data_query) or die(drupal_set_message(t($import_error), 'error'));
	db_query($synonym_hierarchy_query) or die(drupal_set_message(t($import_error), 'error'));
	db_query($synonym_relation_query) or die(drupal_set_message(t($import_error), 'error'));
	// After relation import, update term_relation_has_type table
	$trids = db_query($synonym_tids_query);
	$synonym_trids = "";
	while ($row = db_fetch_object($trids)) 
		{
		if ($synonym_trids != "") $synonym_trids .= ",";
		if ($row->tid2 == $vernacular_default_language)
			{
			$synonym_trids .= '(' . $row->trid . ',' . $vernacular_relationship_rtid . ')';
			}
		else
			{
			$synonym_trids .= '(' . $row->trid . ',' . $synonym_relationship_rtid . ')';
			}
		}
	$synonym_trids_query = "INSERT INTO {term_relation_has_type} VALUES " . $synonym_trids;
	db_query($synonym_trids_query) or die(drupal_set_message(t($import_error), 'error'));
	}
		
if ($new_ranks != "")
	{
	$new_ranks_query = "INSERT INTO {term_data} VALUES ".$new_ranks;
	db_query($new_ranks_query) or die(drupal_set_message(t($import_error), 'error'));
	}

if ($rank_relations != "")
	{
	$rank_relations_query = "INSERT INTO {term_relation} (`tid1`,`tid2`) VALUES ".$rank_relations;
	$rank_tids_query = "SELECT trid FROM {term_relation} WHERE tid1=".$rank_tids;
	}

if ($rank_relations_query != "" and $data_success and $hierarchy_success)
	{
	db_query($rank_relations_query) or die(drupal_set_message(t($import_error), 'error'));
	// After relations import, update term_relation_has_type table
	$trids = db_query($rank_tids_query);
	$rank_trids = "";
	while ($row = db_fetch_object($trids)) 
		{
		if ($rank_trids != "") $rank_trids .= ",";
		$rank_trids .= '(' . $row->trid . ',' . $rank_relationship_rtid . ')';
		}
	$rank_trids_query = "INSERT INTO {term_relation_has_type} VALUES " . $rank_trids;
	db_query($rank_trids_query) or die(drupal_set_message(t($import_error), 'error'));
	}
    
if(module_exists('nestedset'))
	{
	module_load_include("rebuild.inc", "nestedset");
	nestedset_rebuild_tree($vid);
	}

$link = l(t('View and Edit'),'admin/classification/biological/edit');
drupal_set_message(t('Your text file was successfully imported. ' . $link . ' your classification'));	
}

function theme_classification_import_message(){
  $vid = $_REQUEST['vid'];
  $tot_count = $_REQUEST['tot_count'];
  $status = $_REQUEST['status'];
    
  $json = array(
    'message' => '<div class="messages status" id="import_message">',
  );

  switch ($status){
    case 'fail':
      $json['message'] .= t('Import complete.');
      break;
  	case 'success':
  	  $json['message'] .= t('Import completed (or is <i>almost</i> done).');
  	  break;
  }
  $json['message'] .= ' ';
  $json['message'] .= l('View and Edit', 'admin/classification/biological/edit');
  $json['message'] .= ' your classification.</div>';

  return drupal_json($json);
}

function removeBOM($str = "")
{
if(substr($str, 0,3) == pack("CCC",0xef,0xbb,0xbf)) 
	{
        $str=substr($str, 3);
        }
return $str; 
}
