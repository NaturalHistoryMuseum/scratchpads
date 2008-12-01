<?php

/**
 * Form to coordinate the import of names from files or recursive web service calls
 */
function classification_import_form(&$form_state) {

  $module_path = drupal_get_path('module', 'classification') .'/';
  drupal_add_css($module_path .'css/classification.css');
  drupal_add_js($module_path . 'js/classification_editing.js');
  drupal_add_js('$(function() { EDIT = new classification_edit(); });','inline');

  $form = array(
    '#cache' => TRUE,
  );

  $help = theme('advanced_help_topic', 'classification' , 'import');

  $form['status_bar'] = array(
    '#type' => 'markup',
    '#value' => ' ',
    '#prefix' => '<div id="status_bar"><div id="message"></div><div class="help_icon">' . $help . '</div></div>',
    '#suffix' => '<div class="clear"></div>',
  );

  $form['title'] = array(
    '#type' => 'markup',
    '#value' => '<h3>' . t('Import Classification') . '</h3>',
   );

  $vocabs = taxonomy_get_vocabularies();
  
  foreach($vocabs as $voc) {
  	if($voc->name !== 'Taxon Description Chapters') {
  		$opt[$voc->vid] = $voc->name;
  	}
  }

  $form['vocab'] = array(
    '#type' => 'fieldset',
    '#title' => t('Destination for Import'),
    '#attributes' => array('style' => '  background-color: #ccffcc;border: 1px solid #008800;'),
  );

  $form['vocab']['vid'] = array(
    '#type' => 'select',
    '#title' => t('Destination Classification for Import'),
    '#required' => TRUE,
    '#options' => $opt,
    '#default_value' => classification_get_vid('Taxa'),
    '#description' => t('"Taxa" is your primary classification and is used for your site\'s navigation and organization of content.'),
  );
  
  $form['file_import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import From Text File'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $ifopts = array(0=>"- FILE FORMAT -",1=>"Parent Child",2=>"Tabbed list",3=>"Full hierarchy");
  
  $form['file_import']['import_format'] = array(
    '#type' => 'select',
    '#title' => t('Input file format'),
    '#options' => $ifopts,
    '#field_suffix' => $help,
  );
  
  $form['file_import']['data'] = array(
    '#type' => 'file',
    '#title' => t('File to import'),
  );

  $form['file_import']['import'] = array(
    '#type' => 'submit',
    '#attributes' => array('class' => 'classification-buttons import'), 
    '#value' => t('Import File'),
  );

  $form['eol_import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search and Import From EOL'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
      
  $form['eol_import']['taxonomic_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Search for highest taxonomic name of relevance'),
    '#description' => t('This must be a parent name for all taxa to be treated on this site.'),
  );
  
  $form['eol_import']['search'] = array(
    '#type' => 'button',
    '#attributes' => array('class' => 'classification-buttons search'), 
    '#value' => t('Search!'),
    '#ahah' => array(
         'event' => 'click',
         'path' => 'admin/classification/import/search',
         'wrapper' => 'search_results',
         'method' => 'replace',
         'effect' => 'fade',
         'progress' => array('type' => 'none', 'message' => ''),
     ),
     '#suffix' => '<br>',
  );
    
  $form['eol_import']['results'] = array(
     '#type' => 'markup',
     '#value' => ' ',
     '#prefix' => '<div id="search_results">',
     '#suffix' => '</div>',
  );

  $form['eol_import']['status'] = array(
     '#type' => 'markup',
     '#value' => ' ',
     '#prefix' => '<div id="import_status">',
     '#suffix' => '</div>',
  );
  
  $form['#attributes'] = array('enctype' => 'multipart/form-data');

  return $form;
}

/**
 * AHAH Callback for import search
 */
function classification_import_search() {
	   
	   $name = $_POST['taxonomic_name'];

	   if(!$name || $name == '') {
	   	 $options = 'error';
	   }
	   else {
	   	 $req = 'http://services.eol.org/lifedesk/service.php?function=search&search=' . $name;
       $response = drupal_http_request($req);
         if($response->code == 200) {
            $data = $response->data;    
            $xml = simplexml_load_string($data);
              if($xml->value) {
        	       $options = array();
        	       $child_count = array();
                 foreach($xml->value as $values) {
                    $ancestry = $values->ancestry;
                    $id = $values->id;
                    $children = $values->number_of_children;
                    $options['' . $id . '|' . $children . ''] .= $ancestry . ' (<a href="' . $values->metadata->url . '" target="_blank">' . $values->metadata->title . '</a>)';
                 }
              }
              else {
              	 $options = 'nothing';
              }
         }
         else {
         	  $options = 'error';
         }
	    }
	   
	   	$form_element = _classification_search_result_form($options);
      drupal_alter('form', $form_element, array(), 'classification_import_search');
      $form_state = array('submitted' => FALSE);
      $form_build_id = $_POST['form_build_id'];
          
      if (!$form = form_get_cache($form_build_id, $form_state)) {
         exit();
      }
      $form['eol_import']['results'] = $form_element;
      form_set_cache($form_build_id, $form, $form_state);
      $form += array(
         '#post' => $_POST,
         '#programmed' => FALSE,
      );

      $form = form_builder('classification_import_form', $form, $form_state);

      $item_form = $form['eol_import']['results'];
      unset($item_form['#prefix'], $item_form['#suffix']);
      $output = drupal_render($item_form);
          
      drupal_json(array('status' => TRUE,'data' => $output));
}

/**
 * Additional form elements with successful AHAH result for classification search
 */
function _classification_search_result_form($options) {
	
	$form = array(
    '#tree' => TRUE,
  );
  
  switch ($options) {
  	case 'error':
       $form['message'] = array(
          '#type' => 'markup',
          '#value' => '<div class="messages error">' . t('Sorry, you must type a name before you may conduct a search.') . '</div>',
       );
       break;
    case 'nothing':
       $form['message'] = array(
          '#type' => 'markup',
          '#value' => '<div class="messages error">' . t('Sorry, there was no search result.') . '</div>',
       );
       break;
    default:
      $form['ancestry'] = array(
        '#type' => 'radios',
        '#title' => t('Possible hierarchies under which your name was found'),
        '#options' => $options,
      );
      
      $form['ancestry_submit'] = array(
        '#type' => 'markup',
        '#value' => '<div id="import_classification"><a href="#" onclick="javascript:EDIT.import_classification();return false">' . t('Import Classification!') . '</a></div>',
      );
  }
  
  return $form;
}

/**
 *  Import form submission
 */
function classification_import_form_submit($form, &$form_state) {
  switch ($form_state['values']['op']) {
    case t('Import File'):
      classification_import_file($form, $form_state);
    break;
    case t('Search!'):
      // Nothing here because we use AHAH framework
    break;
    case t('Import Classification!'):
       $url = 'http://services.eol.org/lifedesk/service.php?function=details_tcs&id=' . $form_state['values']['results']['ancestry'];
       $voc = $form_state['values']['vid'];
       classification_import_tcs_init($url,$voc);
    break;
    default:
    break;
  }
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
  
  $form_state = array('submitted' => FALSE);
  
  if($tid && $vid) {
    $url = 'http://services.eol.org/lifedesk/service.php?function=details_tcs&id=' . $tid;
    drupal_json(array('status' => 'success', 'message' => t('Import is underway...')));
    classification_import_tcs_init($url,$vid);
  }
  else {
  	drupal_json(array('status' => 'fail', 'message' => t('Sorry, import failed')));
  }
}

/**
 * Classification import status callback
 */
function classification_import_status() {
	  $record = db_fetch_object(db_query(db_rewrite_sql("SELECT count,term FROM {classification_temp} WHERE cid = 1")));
	  $name = $record->term;
	  $count = $record->count;
	  
	  drupal_json(array('status' => 'success', 'message' => array('count' => $count, 'name' => $name)));
}
 
/**
 * Start the loop process by first getting the highest tid in the {term_data} table
**/
function classification_import_tcs_init($url,$voc) {

   if(!$voc) {
     drupal_set_message(t('Sorry, you need to select a classification before you may import names.'),'error');
     return;
   }

   // Now that we have the $vid, this init function could also be used to insert the potential relationships and extended fields

   $result_rtid = db_query("SELECT max(rtid) as rtid FROM {term_relation_types}");
   $seq_rtid = db_fetch_object($result_rtid);
   $rtid = $seq_rtid->rtid;
   $rtid = $rtid+1;

   // The order of these cannot be changed
   $relationTypes = array(
     0 => "synonym",
     1 => "synonym (objective = homotypic)",
     2 => "synonym (subjective = heterotypic)",
     3 => "lexical variant",
     4 => "disputed synonym",
     5 => "spelling alternative",
     6 => "incorrect spelling",
     7 => "incorrect authority information",
     8 => "vernacular name",
     9 => "usage reference",
   );

   foreach ($relationTypes as $key => $value) {
      $sql = db_query("SELECT name FROM {term_relation_types} WHERE name = '%s'", $value);
      $obj = db_fetch_object($sql);
      if(!$obj) {
         db_query("INSERT INTO {term_relation_types} (rtid,name) VALUES (%d,'%s')",$key+$rtid,$value);
      }
   }    

   $result_tid = db_query("SELECT max(tid) as tid FROM {term_data}");
   $seq_tid = db_fetch_object($result_tid);
   global $id;
   $id = $seq_tid->tid;
   $id = $id+1;
   
   global $vid;
   $vid = $voc;
   
   global $counter;
   $counter = 0;
   
   // Initialize the counter table
   db_query("DELETE FROM {classification_temp}");
   db_query("INSERT INTO {classification_temp} (cid,count,term) VALUES (%d,%d,'%s')",1,0,'Starting');

   classification_import_tcs_loop($url,'full',$id);
   classification_create_enhanced_field($vid,'field_rank','Rank');
   classification_create_enhanced_field($vid,'field_vernacular_language','Vernacular Language');
   drupal_set_message(t('Congratulations, all names were successfully imported!'));

   // Clear the counter table
   db_query("DELETE FROM {classification_temp}");
}

function classification_create_enhanced_field($vid,$fid,$title){
   $opt = "";
   
   db_query("DELETE FROM {taxonomy_enhancer_fields} WHERE fid = '%s'", $fid);
   $sql = db_query("SELECT distinct(value) FROM {taxonomy_enhancer_value_text} WHERE fid = '%s'", $fid);
      while ($obj = db_fetch_object($sql)) {
	  		$opt = $opt."\n".$obj->value;
	  		if($fid == 'field_rank') {
	  		   $opt_arr['' . $obj->value . ''] = $obj->value;
	  		}
	  }
	  
	  $len = strlen($opt);
	  if ($fid == 'field_rank') {
	  	  $vars = variable_get('classification_ranks',0);
	  	  if ($vars == 0) {
	  	  	variable_set('classification_ranks',$opt_arr);
	  	  }
	  }

   // this is silly and needs to use the serialize function
	  
   $settings = 'a:5:{s:8:"multiple";i:0;s:4:"rows";N;s:6:"format";N;s:14:"allowed_values";s:'.$len.':"'.$opt.'";s:12:"options_type";s:6:"select";}';
   
   db_query("INSERT INTO {taxonomy_enhancer_fields} (vid,fid,title,module,type,settings) VALUES (%d,'%s','%s','%s','%s','%s')",$vid,$fid,$title, 'taxonomy_enhancer_text','options',$settings);
}

/**
 *  Recursion function for import of names from a TCS web service
 *  There is no appropriate error checking and that should be implemented 
 */
function classification_import_tcs_loop($url,$type,$tid,$pid=false) {

   global $vid;

   $response = drupal_http_request($url);
   if(!$response->code == 200) {
      drupal_set_message(t('Sorry, that URL is unreachable. If an import ended prematurely, you may want to delete the classification and start the process again.'), 'error');
      return;
   }
  
   global $id;
   $id++;
   
   global $counter;
   $counter++;

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
   $rank = $TaxonNames->TaxonName->Rank;

  switch ($type) {
    case 'full':
      db_query("INSERT INTO {term_data} (tid,vid,name) VALUES (%d,%d,'%s')",$tid,$vid,$simple_name);
      db_query("INSERT INTO {term_hierarchy} (tid,parent) VALUES (%d,%d)",$tid,$pid);
      db_query("INSERT INTO {taxonomy_enhancer_value_text} (tid,fid,value) VALUES (%d,'field_rank','%s')",$tid,$rank);
      db_query("UPDATE {classification_temp} SET count = %d, term = '%s' WHERE cid = 1",$counter,$simple_name);
    break;
    case 'syn':
      db_query("INSERT INTO {term_data} (tid,vid,name) VALUES (%d,%d,'%s')",$tid,$vid,$simple_name);
      db_query("INSERT INTO {term_hierarchy} (tid,parent) VALUES (%d,%d)",$tid,$pid);
      db_query("INSERT INTO {term_relation} (tid1,tid2) VALUES (%d,%d)",$tid,$pid);
      $res = db_fetch_object(db_query("SELECT trid FROM {term_relation} where tid1 = %d AND tid2 = %d",$tid,$pid));
      db_query("INSERT INTO {term_relation_has_type} (trid,rtid) VALUES (%d,1)",$res->trid);
      db_query("UPDATE {classification_temp} SET count = %d, term = '%s' WHERE cid = 1",$counter,$simple_name);
    break;
    case 'vern':
      $simple_name = $TaxonConcept->Name;
      $language = $TaxonConcept->Name->attributes()->language;
      db_query("INSERT INTO {term_data} (tid,vid,name) VALUES (%d,%d,'%s')",$tid,$vid,$simple_name);
      db_query("INSERT INTO {term_hierarchy} (tid,parent) VALUES (%d,%d)",$tid,$pid);
      db_query("INSERT INTO {taxonomy_enhancer_value_text} (tid,fid,value) VALUES (%d,'field_vernacular_language','%s')",$tid,$language);
      db_query("INSERT INTO {term_relation} (tid1,tid2) VALUES (%d,%d)",$tid,$pid);
      $res = db_fetch_object(db_query("SELECT trid FROM {term_relation} where tid1 = %d AND tid2 = %d",$tid,$pid));
      db_query("INSERT INTO {term_relation_has_type} (trid,rtid) VALUES (%d,9)",$res->trid);
      db_query("UPDATE {classification_temp} SET count = %d, term = '%s' WHERE cid = 1",$counter,$simple_name);
    break;
  }
  
  if($type == 'full') { 
    foreach ($TaxonRelationships->TaxonRelationship as $relationships) {
  	 switch ($relationships->attributes()->type) {
  	   case "is child taxon of":
  	     break;
  	   case "is parent taxon of":
  	     $url = $relationships->ToTaxonConcept->attributes()->ref;
  	     classification_import_tcs_loop($url,'full',$id,$tid);
  	     break;
  	   case "has synonym":
  	     $url = $relationships->ToTaxonConcept->attributes()->ref;
  	     classification_import_tcs_loop($url,'syn',$id,$tid);
         break;
       case "has vernacular":
         $url = $relationships->ToTaxonConcept->attributes()->ref;
         classification_import_tcs_loop($url,'vern',$id,$tid);
         break;
  	 }
    }	     
  }
}

/**
* File import
*/

function classification_import_file($form, &$form_state) {
  if ($form_state['values']['vid'] == 0)
  {
    form_set_error('vid',t('Please select a Vocabulary'));
  }
  if ($form_state['values']['url']=="")
  {
	if($form_state['values']['import_format'] == 0)
	{
		form_set_error('import_format',t('Please select a File Format'));
	};
	if ($file = file_save_upload('data'))
	{
		$fd = fopen($file->filepath, "rb");
		$info = fstat($fd);
		$len = $info["size"];
		$text = fread($fd, $len);
		fclose($fd);
		if(!$fd)
		{
			form_set_error('data', t('Vocabulary import failed: file %filename cannot be read.', array('%filename' => $file->filename)));      
		}
		else
		{
			classification_import_parse($text, $form_state['values']['vid'], $form_state['values']['import_format']);
		}
	}
	else
	{
		form_set_error('data', t('Vocabulary import failed: file was not uploaded.'));
	}
  }
}

function classification_import_parse($text, $vid, $import_format) {
// Parse the data sent in the file, and shove it in to the Database.

// Parse the file, creating the correct SQL as we go.
// Each line will have an entry in the term_data table, and term_hierarchy table 
// (roots having a parent of 0)

   $result_rtid = db_query("SELECT max(rtid) as rtid FROM {term_relation_types}");
   $seq_rtid = db_fetch_object($result_rtid);
   $rtid = $seq_rtid->rtid;
   $rtid = $rtid+1;

   // The order of these cannot be changed
   $relationTypes = array(
     0 => "synonym",
     1 => "synonym (objective = homotypic)",
     2 => "synonym (subjective = heterotypic)",
     3 => "lexical variant",
     4 => "disputed synonym",
     5 => "spelling alternative",
     6 => "incorrect spelling",
     7 => "incorrect authority information",
     8 => "vernacular name",
     9 => "usage reference",
   );

   foreach ($relationTypes as $key => $value) {
      $sql = db_query("SELECT name FROM {term_relation_types} WHERE name = '%s'", $value);
      $obj = db_fetch_object($sql);
      if(!$obj) {
         db_query("INSERT INTO {term_relation_types} (rtid,name) VALUES (%d,'%s')",$key+$rtid,$value);
      }
   }    

classification_create_enhanced_field($vid,'field_rank','Rank');

// Let's first get the lowest value for "tid", upon which we will build all the
// other values.
$sql = "SELECT MAX(`tid`) FROM {term_data};";
$result = db_query($sql);
$sequence = db_fetch_array($result);
$sequence = max($sequence)+1;

// Values are:
// tid vid name description weight
// ($sequence, $vid, $parsedData, $parsedData, 0)
// tid parent
// $sequence $parent
$term_data = "";
$term_hierarchy = "";
$synonym_data = "";
$lines = preg_split("/[\n\r]+/", $text);

	// Here starts the term_relation addition
	$synonym_term_data = "";
	$synonym_relation = "";
	$synonym_tids = "";
	// Here ends the term_relation addition

if($import_format==0){
    // Here we try and guess the import format, and set it accordingly.
    // Files are defined as below.  We only test the first 10 lines.
    // if first five chars = '&lt;?xml' format is XML
    // if all have the same number of tabs
    //   then they're 1 or 3
    //     if two or three tabs
    //       then 2
    //       else more 3
    //   else 2
    if (substr($lines[0],0,5)=="<?xml"){
      $import_format=4;
    }
    else {
      $numbers_of_tabs = array();
      for($i=0;$i<10;$i++){
        if (trim($lines[$i])==""){
          break;
        }
        $numbers_of_tabs[]=count(split("\t",$lines[$i]))-1;
      }
      // Check if they're all the same (add together, divide by number of them, if equal
      // to all of them, then they're the same.
      if (array_sum($numbers_of_tabs)/count($numbers_of_tabs)==$numbers_of_tabs[$i]){
        $import_format=2;
        if ((array_sum($numbers_of_tabs)/count($numbers_of_tabs))<4)
          $import_format=1;
      }
      if ($import_format!=2) {
        $import_format=3;
      }
    }
  }
  if($import_format==4){
    drupal_set_message(t('Import of local XML file does not yet work'), 'error');
  }
  else {
    if($import_format==1){
      // File is in the format:
      /*
kingdom1	any	descr1	syn1
phylum1	kingdom1
phylum2	kingdom1
class1	phylum1
order1	class1		syn2,syn3
class2	phylum1
     */
      // Each line of the taxonomy constitues an entry into both of the tables.
      // Fill out the term_data first, filling out an array name=>parent_name
      // name=>number
      // which can then be converted into numbers.
      drupal_set_message('Imported using Parent-child format');
      $parents = array();
      $term_id = array();
      foreach($lines as $line){
        if (trim($line)=="")
          break;
        $line_parts = split("\t",$line);
        // Remove '"' if at start or end of part
        for($i=0;$i<count($line_parts);$i++){
          $line_parts[$i] = trim($line_parts[$i]);
          if(substr($line_parts[$i],0,1)=='"')
            $line_parts[$i]=substr($line_parts[$i],1,strlen($line_parts[$i])-2);
        }
        
        $name = $line_parts[0];
        $parent = $line_parts[1];
        $description = (isset($line_parts[2])) ? $line_parts[2] : '';

	$synonyms = (isset($line_parts[3])) ? split(",",$line_parts[3]) : $synonyms=array();
      	$sequence_p = $sequence;
	foreach($synonyms as $synonym){
		if ($synonym_data!="") $synonym_data.=",";
		$synonym_name = trim($synonym);
		$synonym_data .= '('.$sequence_p.',\''.addslashes($synonym_name).'\')';
	// Here starts the term_relation addition
		if ($sequence == $sequence_p) $sequence++;
		if ($synonym_term_data != "") $synonym_term_data .=",";
		if ($synonym_name != "") $synonym_term_data .= "(".$sequence.",".$vid.",'".addslashes($synonym_name)."','',0)";
		if ($synonym_relation != "") $synonym_relation .=",";
		$synonym_relation .= '('.$sequence.','.($sequence_p).')';
		if ($synonym_tids != "") $synonym_tids .=" OR tid1=";
		$synonym_tids .= $sequence;
		$sequence++;
	// Here ends the term_relation addition
      	}
	
      	if ($term_data !="") $term_data .=",";
      	$term_data .= "(".$sequence_p.",".$vid.",'".addslashes($name)."','".addslashes($description)."',0)";
      	$parents[$name]=$parent;
      	$term_id[$name]=$sequence_p;
      	$sequence++;
      }
      // We should now have all the term_data SQL created, and know the IDs for all
      // of the names, we can now create the hierachy SQL.
      $names = array_keys($parents);
      foreach ($names as $name){
        // Convert the parent name into an ID
        if ($term_hierarchy!="")
          $term_hierarchy .=",";
        $parent_id = $term_id[$parents[$name]];
        if ($parent_id=="")
          $parent_id="0";
        $term_hierarchy .= '('.$term_id[$name].','.$parent_id.')';
      }
    }
    elseif($import_format==2){
      drupal_set_message('Imported using tabbed hierarchy format');
      // File is in the format:
      /*
kingdom1;descr1
	phylum1;descr2;syn1
		class1
			order1;;syn2,syn3
		class2
			order2
			order3
*/
      foreach ($lines as $line){
      	if (trim($line)=="")
      		break;
      	$line_parts = split(";",$line);
      	$name = $line_parts[0];
        $reference = (isset($line_parts[1])) ? $line_parts[1] : '';
        $synonyms = (isset($line_parts[2])) ? split(",",$line_parts[2]) : array();
      	// Count the number of tabs before the name.  There should be no tabs within the name, whilst
      	// all tabs after the name are removed.
      	$name = rtrim($name);
      	$count = count_chars($name, 1);
      	$rank = 0;
      	if (isset($count[ord("\t")])) $rank = $count[ord("\t")];
      	
      	// Add the SQL for this item to the term_data stuff
      	
      	$sequence_p = $sequence;
	foreach($synonyms as $synonym){
		if ($synonym_data!="") $synonym_data.=",";
		$synonym_name = trim($synonym, "\"\t\n\r .");
		$synonym_data .= '('.$sequence_p.',\''.addslashes($synonym_name).'\')';
	// Here starts the term_relation addition
		if ($sequence == $sequence_p) $sequence++;
		if ($synonym_term_data != "") $synonym_term_data .=",";
		if ($synonym_name != "") $synonym_term_data .= "(".$sequence.",".$vid.",'".addslashes($synonym_name)."','',0)";
		if ($synonym_relation != "") $synonym_relation .=",";
		$synonym_relation .= '('.$sequence.','.($sequence_p).')';
		if ($synonym_tids != "") $synonym_tids .=" OR tid1=";
		$synonym_tids .= $sequence;
		$sequence++;
	// Here ends the term_relation addition
      	}
      	
      	if ($term_data !="") $term_data .=",";
      	$term_data .= '('.$sequence_p.','.$vid.",'".addslashes(trim($name, "\"\t\n\r ."))."','".addslashes(trim($reference, "\"\t\n\r ."))."',0)";

	// Set this as a parent item, wipe all parent items with numbers *LESS* than this (easiest way is to just
      	// copy over the ones below to a new array)
      	if ($rank>0){		
      		$temp_array=array();
      		for($i=0;$i<$rank;$i++){
      			if(isset($parents[$i]))
      				$temp_array[$i]=$parents[$i];
      		}
      		$parents=$temp_array;
      		if ($term_hierarchy !="")
      			$term_hierarchy .=",";
      		$term_hierarchy .= "(".$sequence_p.",".array_pop($temp_array).")";
      		$parents[$rank]=$sequence_p;
      	}
      	else{
      		$parents = array(0 => $sequence_p);
      		if ($term_hierarchy !="")
      			$term_hierarchy .=",";
      		$term_hierarchy .= "(".$sequence_p.",0)";
      	}
      	$sequence ++;
      }
    }
    elseif($import_format==3){
      // File is in the format:
/*
      taxon	taxon	taxon	taxon	rank	description	synonyms
kingdom1				kingdom
kingdom1	phylum1			phylum
kingdom1	phylum1	class1		classis	descr0
kingdom1	phylum1	class1	order1	order	descr1	syn1,syn2
kingdom1	phylum1	class1	order2	order		syn3,syn4
kingdom1	phylum1	class2	order3	order	descr3	syn5
*/
      drupal_set_message('Imported using full hierarchy format');
      $first_line = array_shift($lines);
      $first_line_parts = split("\t",$first_line);
      $number_of_ranks = 0;
      $line_parts_index = array();
      $i=0;
      foreach($first_line_parts as $part){        
        if(strtolower(trim($part))=='taxon'){
          $number_of_ranks++;
        } else {
          $line_parts_index[strtolower(trim($part))] = $i;
        }
        $i++;
      }
    	$names = array();
    	$hierarchy = array();
    	foreach ($lines as $line){
        if (trim($line)!="")
	  {
          $line_parts = split("\t",$line);
          $taxon_parts = array();
          for($i=0;$i<$number_of_ranks;$i++){
            if(!in_array(trim($line_parts[$i]),$names) && trim($line_parts[$i]!="")){
		$name = trim($line_parts[$i]);
		$names[$sequence] = $name;
		$sequence_p = $sequence;
		if ($line_parts[$i+1] == "" or $i == $number_of_ranks-1)
			{
			$field_rank = $line_parts[$line_parts_index['rank']];
			$reference = $line_parts[$line_parts_index['description']];
			$synonyms = split(",",$line_parts[$line_parts_index['synonyms']]);
			if (!empty($synonyms[0])) foreach($synonyms as $synonym)
				{
				if ($synonym_data != "") $synonym_data .=",";
				$synonym_name = trim($synonym);
				if ($synonym_name != "") $synonym_data .= '('.$sequence_p.',\''.addslashes($synonym_name).'\')';
				// Here starts the term_relation addition
				if ($sequence == $sequence_p) $sequence++;
				if ($synonym_term_data != "") $synonym_term_data .=",";
				if ($synonym_name != "") $synonym_term_data .= "(".$sequence.",".$vid.",'".addslashes($synonym_name)."','',0)";
				if ($synonym_relation != "") $synonym_relation .=",";
				$synonym_relation .= '('.$sequence.','.($sequence_p).')';
				if ($synonym_tids != "") $synonym_tids .=" OR tid1=";
				$synonym_tids .= $sequence;
				$sequence++;
				// Here ends the term_relation addition
				}
			}
		//term_data
		if ($term_data !="") $term_data .=",";
		$term_data .= "(".$sequence_p.",".$vid.",'".addslashes($name)."','".addslashes(trim($reference))."',0)";
		
		//field_rank_data, let synonyms do not have rank
		if ($field_rank_data !="") $field_rank_data .=",";
		$field_rank_data .= "(".$sequence_p.",'field_rank','".addslashes(trim($field_rank))."')";
		
		// term_hierarchy
		if ($term_hierarchy !="") $term_hierarchy .=",";
          	if ($i>0)
		{
          		// Add to hierarchy
          		$parent = trim($line_parts[$i-1]);
          		$parent_id = array_search($parent, $names);
			$term_hierarchy .= '('.$sequence_p.','.$parent_id.')';
          	}
          	else 
		{
			$term_hierarchy .= '('.$sequence_p.',0)';
          	}
              $sequence++;
            }
          }
        }
    	}
    }
    
    $term_hierarchy = "INSERT INTO {term_hierarchy} VALUES ".$term_hierarchy;
    $term_data = "INSERT INTO {term_data} VALUES ".$term_data;
    if ($synonym_data !=""){
      $synonym_data = "INSERT INTO {term_synonym} (`tid`,`name`) VALUES ".$synonym_data;
      // Here starts the term_relation addition
      $synonym_term_data = "INSERT INTO {term_data} VALUES ".$synonym_term_data;
      $synonym_hierarchy = "INSERT INTO {term_hierarchy} (`tid`,`parent`) VALUES ".$synonym_relation;
      $synonym_relation = "INSERT INTO {term_relation} (`tid1`,`tid2`) VALUES ".$synonym_relation;
      $synonym_tids = "SELECT trid FROM {term_relation} WHERE tid1=".$synonym_tids;
      // Here ends the term_relation addition
    }

    if ($field_rank_data !="")
    {
	$field_rank_data = "INSERT INTO {taxonomy_enhancer_value_text} (`tid`,`fid`,`value`) VALUES ".$field_rank_data;
    }

	db_query($term_data);
	db_query($term_hierarchy);
	if ($synonym_data !="")
	{
		db_query($synonym_data);
		// Here starts the term_relation addition
		db_query($synonym_term_data);
		db_query($synonym_hierarchy);
		db_query($synonym_relation);
		// After relation import, update term_relation_has_type table
		$trids = db_query($synonym_tids);
		$synonym_trids = "";
		while ($row = db_fetch_object($trids)) 
		{
			if ($synonym_trids != "") $synonym_trids .=",";
			$synonym_trids .= '('.$row->trid.',1)'; // "1" because they are "synonyms"
		}
		$synonym_trids = "INSERT INTO {term_relation_has_type} VALUES ".$synonym_trids;
		db_query($synonym_trids);
		// Here ends the term_relation addition
	}
	if ($field_rank_data !="")
	{
		$field_rank_data = db_query($field_rank_data);
	}
    }
    return true;
  }
