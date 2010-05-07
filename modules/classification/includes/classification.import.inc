<?php

/**
 * Form to coordinate the import of names from files or recursive web service calls
 */
function classification_import_form(&$form_state){
  
  $module_path = drupal_get_path ( 'module', 'classification' ) . '/';
  drupal_add_css ( $module_path . 'css/classification.css' );
  drupal_add_js ( $module_path . 'js/classification_import.js' );
  drupal_add_js ( '$(function() { IMPORT = new classification_import(); });', 'inline' );
  
  $form = array (
    '#cache' => TRUE );
  
  $help = theme ( 'advanced_help_topic', 'classification', 'import' );
  
  $form ['status_bar'] = array (
    '#type' => 'markup', 
    '#value' => ' ', 
    '#prefix' => '<div id="message"></div>' );
  
  $message = '<div class=\'messages status\'>' . t ( 'You have three options to import your names and/or classification(s). Please see the ' . l ( t ( 'help page' ), 'classification/help' ) . ' for instruction. If you create any ' . l ( t ( 'alternate classifications' ), 'classification/manage' ) . ' to help manage names in your site\'s classification, this page will be modified to include a drop-down menu to specify the destination for your import.' ) . '</div>';
  
  $form ['title'] = array (
    '#type' => 'markup', 
    '#value' => $message );
  
  $vocabs = taxonomy_get_vocabularies ();
  
  $opt [''] = '--Choose--';
  foreach ( $vocabs as $voc ) {
    if (preg_match ( "/alternate_/i", $voc->name )) {
      $opt [$voc->vid] = str_replace ( "alternate_", "", check_plain ( $voc->name ) );
    } elseif ($voc->name == 'Taxa') {
      $opt [$voc->vid] = variable_get ( 'site_name', 'Taxa' ) . ' classification';
    } else {
    }
  }
  
  if (count ( $opt ) > 2) {
    $form ['vocab'] = array (
      '#type' => 'fieldset', 
      '#title' => t ( 'Destination for Import' ) );
    if ($_GET ["d"]) {
      $default = $_GET ["d"];
    } else {
      $default = '';
    }
    $form ['vocab'] ['vid'] = array (
      '#type' => 'select', 
      '#title' => t ( 'Destination Classification for Import' ), 
      '#required' => TRUE, 
      '#options' => $opt, 
      '#default_value' => $default );
  } else {
    $form ['vocab'] ['vid'] = array (
      '#type' => 'hidden', 
      '#value' => classification_get_vid ( 'Taxa' ) );
  }
  
  $form ['batch_import'] = array (
    '#type' => 'fieldset', 
    '#title' => t ( 'Batch Import (enter names manually)' ), 
    '#collapsible' => TRUE, 
    '#collapsed' => TRUE );
  
  $form ['batch_import'] ['name_list'] = array (
    '#type' => 'textarea', 
    '#title' => t ( 'One name per line' ), 
    '#cols' => 30 );
  
  $form ['batch_import'] ['import_list'] = array (
    '#type' => 'submit', 
    '#attributes' => array (
      'class' => 'classification-buttons import' ), 
    '#value' => t ( 'Import Names' ) );
  
  $form ['file_import'] = array (
    '#type' => 'fieldset', 
    '#title' => t ( 'Import From Text File' ), 
    '#collapsible' => TRUE, 
    '#collapsed' => TRUE );
  
  $ifopts = array (
    0 => "- FILE FORMAT -", 
    1 => "Parent Child", 
    2 => "Tabbed list", 
    3 => "Full hierarchy" );
  
  $form ['file_import'] ['import_format'] = array (
    '#type' => 'select', 
    '#title' => t ( 'Input file format' ), 
    '#options' => $ifopts, 
    '#field_suffix' => $help );
  
  $form ['file_import'] ['data'] = array (
    '#type' => 'file', 
    '#title' => t ( 'File to import' ) );
  
  $form ['file_import'] ['import'] = array (
    '#type' => 'submit', 
    '#attributes' => array (
      'class' => 'classification-buttons import' ), 
    '#value' => t ( 'Import File' ) );
  
  $form ['eol_import'] = array (
    '#type' => 'fieldset', 
    '#title' => t ( 'Import From EOL' ), 
    '#collapsible' => TRUE, 
    '#collapsed' => TRUE );
  
  $form ['eol_import'] ['taxonomic_name'] = array (
    '#type' => 'textfield', 
    '#title' => t ( 'Search for highest taxonomic name of relevance' ), 
    '#description' => t ( 'This must be a parent name for all taxa to be treated on this site.' ) );
  
  $form ['eol_import'] ['search'] = array (
    '#type' => 'button', 
    '#attributes' => array (
      'class' => 'classification-buttons search' ), 
    '#value' => t ( 'Search' ), 
    '#ahah' => array (
      'event' => 'click', 
      'path' => 'classification/import/search', 
      'wrapper' => 'search_results', 
      'method' => 'replace', 
      'effect' => 'fade', 
      'progress' => array (
        'type' => 'none', 
        'message' => '' ) ), 
    '#suffix' => '<br>' );
  
  $form ['eol_import'] ['results'] = array (
    '#type' => 'markup', 
    '#value' => ' ', 
    '#prefix' => '<div id="search_results">', 
    '#suffix' => '</div>' );
  
  $form ['eol_import'] ['total_names'] = array (
    '#type' => 'hidden' );
  
  $form ['eol_import'] ['status'] = array (
    '#type' => 'markup', 
    '#value' => ' ', 
    '#prefix' => '<div id="import_status">', 
    '#suffix' => '</div>' );
  
  $form ['#attributes'] = array (
    'enctype' => 'multipart/form-data' );
  
  /*
$form['delete'] = array( // Debug only!
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
*/
  
  return $form;
}

/**
 * AHAH Callback for import search
 */
function classification_import_search(){
  
  $name = $_POST ['taxonomic_name'];
  
  if (! $name || $name == '') {
    $options = 'error';
  } else {
    $req = 'http://services.eol.org/lifedesk/service.php?function=search&search=' . $name;
    $response = drupal_http_request ( $req );
    if ($response->code == 200) {
      $data = $response->data;
      $xml = simplexml_load_string ( $data );
      if ($xml->value) {
        $options = array ();
        $child_count = array ();
        foreach ( $xml->value as $values ) {
          $ancestry = $values->ancestry;
          $id = $values->id;
          $children = $values->number_of_children;
          $synonyms = $values->number_of_children_synonyms;
          $total_names = $children + $synonyms;
          if ($values->metadata->url) {
            $url = ' (<a href="' . $values->metadata->url . '" target="_blank">' . $values->metadata->title . '</a>)';
          }
          $options ['' . $id . '|' . $total_names . ''] .= $ancestry . $url;
        }
      } else {
        $options = 'nothing';
      }
    } else {
      $options = 'error';
    }
  }
  
  $form_element = _classification_search_result_form ( $options );
  drupal_alter ( 'form', $form_element, array (), 'classification_import_search' );
  $form_state = array (
    'submitted' => FALSE );
  $form_build_id = $_POST ['form_build_id'];
  
  if (! $form = form_get_cache ( $form_build_id, $form_state )) {
    exit ();
  }
  $form ['eol_import'] ['results'] = $form_element;
  form_set_cache ( $form_build_id, $form, $form_state );
  $form += array (
    '#post' => $_POST, 
    '#programmed' => FALSE );
  
  $form = form_builder ( 'classification_import_form', $form, $form_state );
  
  $item_form = $form ['eol_import'] ['results'];
  unset ( $item_form ['#prefix'], $item_form ['#suffix'] );
  $output = drupal_render ( $item_form );
  
  drupal_json ( array (
    'status' => TRUE, 
    'data' => $output ) );
}

/**
 * Additional form elements with successful AHAH result for classification search
 */
function _classification_search_result_form($options){
  
  $form = array (
    '#tree' => TRUE );
  
  switch ($options) {
    case 'error' :
      $form ['message'] = array (
        '#type' => 'markup', 
        '#value' => '<div class="messages error">' . t ( 'Sorry, the search and automatic import service is currently offline. Please try later.' ) . '</div>' );
      break;
    case 'nothing' :
      $form ['message'] = array (
        '#type' => 'markup', 
        '#value' => '<div class="messages error">' . t ( 'Sorry, there was no search result. Please try again.' ) . '</div>' );
      break;
    default :
      $form ['ancestry'] = array (
        '#type' => 'radios', 
        '#title' => t ( 'Possible hierarchies under which your name was found' ), 
        '#options' => $options, 
        '#attributes' => array (
          'onclick' => 'javascript:IMPORT.get_total(this.value);' ) );
      
      $form ['ancestry_submit'] = array (
        '#type' => 'markup', 
        '#value' => '<div id="import_classification_button"><button class="classification-buttons import" onclick="javascript:IMPORT.import_classification();return false">' . t ( 'Import Classification!' ) . '</button></div>' );
  }
  
  return $form;
}

/**
 *  Import form submission
 */
function classification_import_form_submit($form, &$form_state){
  switch ($form_state ['values'] ['op']) {
    case t ( 'Import Names' ) :
      classification_import_names ( $form, $form_state );
      break;
    case t ( 'Import File' ) :
      classification_import_file ( $form, $form_state );
      break;
    case t ( 'Search!' ) :
      // Nothing here because we use AHAH framework
      break;
    case t ( 'Delete' ) :
      classification_delete_primary (); // Debug only!
      break;
    case t ( 'Import Classification!' ) :
      // Nothing here because we use a callback
      break;
    default :
      break;
  }
}

function classification_delete_primary(){
  $vid = classification_get_vid ( 'Taxa' );
  $tree = taxonomy_get_tree ( $vid );
  db_query ( 'DELETE FROM {taxonomy_enhancer_fields} WHERE vid = %d AND fid = "%s"', $vid, 'field_rank' );
  db_query ( 'DELETE FROM {taxonomy_enhancer_fields} WHERE vid = %d AND fid = "%s"', $vid, 'field_vernacular_language' );
  foreach ( $tree as $branch ) {
    db_query ( "DELETE FROM {taxonomy_enhancer_value_text} WHERE tid = %d", $branch->tid );
    $res = db_query ( "SELECT trid FROM {term_relation} WHERE tid1 = %d OR tid2 = %d", $branch->tid, $branch->tid );
    while ( $term = db_fetch_object ( $res ) ) {
      db_query ( "DELETE FROM {term_relation_has_type} WHERE trid = %d", $term->trid );
    }
  }
  taxonomy_del_vocabulary ( $vid );
  $vocab = array (
    'name' => 'Taxa', 
    'hierarchy' => 1, 
    'module' => 'taxonomy' );
  taxonomy_save_vocabulary ( $vocab );
  $vid = classification_get_vid ( 'Taxa' );
  $ranks = "Family Genus Species";
  _classification_update_lists ( 'rank', 'Rank', $ranks );
  $verns = "en es fr";
  _classification_update_lists ( 'vernacular_language', 'Vernacular Language', $verns );
}

/**
 * Classification import callback
 */
function classification_import_classification(){
  $form = array (
    '#post' => $_POST );
  
  $tid = $_REQUEST ['tid'];
  $vid = $_REQUEST ['vid'];
  $tot_count = $_REQUEST ['tot_count'];
  
  $form_state = array (
    'submitted' => FALSE );
  
  if ($tid && $vid) {
    $url = 'http://services.eol.org/lifedesk/service.php?function=details_tcs&id=' . $tid;
    classification_import_tcs_init ( $url, $vid, $tot_count );
  }
}

/**
 * Start the loop process by first getting the highest tid in the {term_data} table
 **/
function classification_import_tcs_init($url, $voc, $tot_count){
  
  global $id, $vid, $counter, $user, $temp_table;
  
  if (! $voc) {
    drupal_set_message ( t ( 'Sorry, you need to select a classification before you may import names.' ), 'error' );
    return;
  }
  
  // These commands are repeated trice in the code: in TCS, text file and batch import
  // Now that we have the $vid, this init function could also be used to insert the potential relationships and extended fields
  $result_rtid = db_query ( "SELECT max(rtid) as rtid FROM {term_relation_types}" );
  $seq_rtid = db_fetch_object ( $result_rtid );
  $rtid = $seq_rtid->rtid;
  $rtid = $rtid + 1;
  
  // The order of these cannot be changed
  $relationTypes = array (
    0 => "synonym", 
    1 => "synonym (objective = homotypic)", 
    2 => "synonym (subjective = heterotypic)", 
    3 => "lexical variant", 
    4 => "disputed synonym", 
    5 => "spelling alternative", 
    6 => "incorrect spelling", 
    7 => "incorrect authority information", 
    8 => "vernacular name", 
    9 => "usage reference" );
  
  foreach ( $relationTypes as $key => $value ) {
    $sql = db_query ( "SELECT name FROM {term_relation_types} WHERE name = '%s'", $value );
    $obj = db_fetch_object ( $sql );
    if (! $obj) {
      db_query ( "INSERT INTO {term_relation_types} (rtid,name) VALUES (%d,'%s')", $key + $rtid, $value );
    }
  }
  
  $result_tid = db_query ( "SELECT max(tid) as tid FROM {term_data}" );
  $seq_tid = db_fetch_object ( $result_tid );
  $id = $seq_tid->tid;
  $id = $id + 1;
  $vid = $voc;
  $counter = 1;
  
  /*
    * Initialize the temporary counter table.
    * Cannot use a true temporary table because the callback comes from another HTTP request
    */
  $temp_table = "classification_temp_" . $user->uid;
  db_query ( "DROP TABLE IF EXISTS {" . $temp_table . "}" );
  db_query ( "CREATE TABLE {" . $temp_table . "} (cid INT, term VARCHAR(255), curr_count INT, tot_count INT) TYPE = HEAP" );
  db_query ( "INSERT INTO {" . $temp_table . "} (cid,term,curr_count,tot_count) VALUES (%d,'%s',%d,%d)", 1, 'Starting', 1, $tot_count );
  
  // Go to sleep for 3 seconds to ensure the temp table is there
  sleep ( 3 );
  
  classification_import_tcs_loop ( $url, 'full', $id );
  classification_create_enhanced_field ( $vid, 'field_rank', 'Rank', 'options' );
  classification_create_enhanced_field ( $vid, 'field_vernacular_language', 'Vernacular Language', 'options' );
  
  // Clear the counter table after a short sleep
  sleep ( 5 );
  db_query ( "DROP TABLE {" . $temp_table . "}" );
}

/**
 * Classification import status callback
 */
function classification_import_status(){
  // Cannot use global $temp_table here because this function is a callback from another HTTP request
  global $user;
  
  $form = array (
    '#post' => $_POST );
  
  $tot_count = $_REQUEST ['tot_count'];
  
  $table = db_fetch_object ( db_query ( "SHOW TABLES LIKE 'classification_temp_" . $user->uid . "'" ) );
  if ($table) {
    $record = db_fetch_object ( db_query ( "SELECT curr_count, term FROM {classification_temp_" . $user->uid . "} WHERE cid = 1" ) );
    $name = check_plain ( $record->term );
    $curr_count = $record->curr_count;
    drupal_json ( array (
      'status' => 'success', 
      'curr_count' => $curr_count, 
      'tot_count' => $tot_count, 
      'name' => $name ) );
  } else {
    drupal_json ( array (
      'status' => 'fail' ) );
  }
}

//Create the enhanced fields
function classification_create_enhanced_field($vid, $fid, $title, $field_type){
  
  $sql = db_query ( "SELECT distinct(value) FROM {taxonomy_enhancer_value_text} WHERE fid = '%s' AND value != ''", $fid );
  while ( $obj = db_fetch_object ( $sql ) ) {
    $opt [] = $obj->value;
  }
  
  $field = array (
    'vid' => $vid, 
    'fid' => $fid, 
    'title' => $title, 
    'module' => 'classification', 
    'type' => $field_type, 
    'weight' => 0, 
    'settings' => array () );
  $field ['settings'] ['allowed_values'] = $opt;
  $field ['settings'] ['options_type'] = 'select';
  if (count ( $opt ) > 0) {
    db_query ( "UPDATE {taxonomy_enhancer_fields} SET settings = '%s' WHERE vid = %d AND fid = '%s'", serialize ( $field ['settings'], $vid, $fid ) );
  }
}

/**
 *  Recursion function for import of names from a TCS web service
 *  There is no appropriate error checking and that should be implemented 
 */
function classification_import_tcs_loop($url, $type, $tid, $pid = false){
  
  global $id, $vid, $counter, $temp_table;
  
  $response = drupal_http_request ( $url );
  if (! $response->code == 200) {
    drupal_set_message ( t ( 'Sorry, that URL is unreachable. If an import ended prematurely, you may want to delete the classification and start the process again.' ), 'error' );
    return;
  }
  
  $id ++;
  
  if (! $pid) {
    $pid = 0;
  }
  
  $data = $response->data;
  $xml = simplexml_load_string ( $data );
  
  $TaxonNames = $xml->TaxonNames;
  $TaxonConcept = $xml->TaxonConcepts->TaxonConcept;
  $TaxonRelationships = $TaxonConcept->TaxonRelationships;
  $ProviderSpecificData = $TaxonNames->TaxonName->ProviderSpecificData->NameSources;
  
  $simple_name = $TaxonNames->TaxonName->Simple;
  $canonical_name = $TaxonNames->TaxonName->CanonicalName->Simple;
  $rank = $TaxonNames->TaxonName->Rank;
  
  db_query ( "INSERT INTO {term_hierarchy} (tid,parent) VALUES (%d,%d)", $tid, $pid );
  db_query ( "INSERT INTO {term_data} (tid,vid,name) VALUES (%d,%d,'%s')", $tid, $vid, $simple_name );
  if (trim ( $canonical_name ) != '') {
    db_query ( "INSERT INTO {term_synonym} (tid, name) VALUES (%d, '%s')", $tid, $canonical_name );
  }
  switch ($type) {
    case 'full' :
      db_query ( "INSERT INTO {taxonomy_enhancer_value_text} (tid,fid,value) VALUES (%d,'field_rank','%s')", $tid, $rank );
      break;
    case 'syn' :
      db_query ( "INSERT INTO {term_relation} (tid1,tid2) VALUES (%d,%d)", $tid, $pid );
      $res = db_fetch_object ( db_query ( "SELECT trid FROM {term_relation} where tid1 = %d AND tid2 = %d", $tid, $pid ) );
      db_query ( "INSERT INTO {term_relation_has_type} (trid,rtid) VALUES (%d,1)", $res->trid );
      break;
    case 'vern' :
      $simple_name = $TaxonConcept->Name;
      $language = $TaxonConcept->Name->attributes ()->language;
      db_query ( "INSERT INTO {taxonomy_enhancer_value_text} (tid,fid,value) VALUES (%d,'field_vernacular_language','%s')", $tid, $language );
      db_query ( "INSERT INTO {term_relation} (tid1,tid2) VALUES (%d,%d)", $tid, $pid );
      $res = db_fetch_object ( db_query ( "SELECT trid FROM {term_relation} where tid1 = %d AND tid2 = %d", $tid, $pid ) );
      db_query ( "INSERT INTO {term_relation_has_type} (trid,rtid) VALUES (%d,9)", $res->trid );
      break;
  }
  db_query ( "UPDATE {" . $temp_table . "} SET curr_count = %d, term = '%s' WHERE cid = 1", $counter, $simple_name );
  
  //Slow down the recursion a hundredth of a second
  usleep ( 10000 );
  $counter ++;
  
  if ($type == 'full') {
    foreach ( $TaxonRelationships->TaxonRelationship as $relationships ) {
      switch ($relationships->attributes ()->type) {
        case "is child taxon of" :
          break;
        case "is parent taxon of" :
          $url = $relationships->ToTaxonConcept->attributes ()->ref;
          classification_import_tcs_loop ( $url, 'full', $id, $tid );
          break;
        case "has synonym" :
          $url = $relationships->ToTaxonConcept->attributes ()->ref;
          classification_import_tcs_loop ( $url, 'syn', $id, $tid );
          break;
        case "has vernacular" :
          $url = $relationships->ToTaxonConcept->attributes ()->ref;
          classification_import_tcs_loop ( $url, 'vern', $id, $tid );
          break;
      }
    }
  }
}

/**
 * Batch import function
 */
function classification_import_names($form, $form_state){
  
  // These commands are repeated trice in the code: in TCS, text file and batch import
  // Now that we have the $vid, this init function could also be used to insert the potential relationships and extended fields
  $result_rtid = db_query ( "SELECT max(rtid) as rtid FROM {term_relation_types}" );
  $seq_rtid = db_fetch_object ( $result_rtid );
  $rtid = $seq_rtid->rtid;
  $rtid = $rtid + 1;
  
  // The order of these cannot be changed
  $relationTypes = array (
    0 => "synonym", 
    1 => "synonym (objective = homotypic)", 
    2 => "synonym (subjective = heterotypic)", 
    3 => "lexical variant", 
    4 => "disputed synonym", 
    5 => "spelling alternative", 
    6 => "incorrect spelling", 
    7 => "incorrect authority information", 
    8 => "vernacular name", 
    9 => "usage reference" );
  
  foreach ( $relationTypes as $key => $value ) {
    $sql = db_query ( "SELECT name FROM {term_relation_types} WHERE name = '%s'", $value );
    $obj = db_fetch_object ( $sql );
    if (! $obj) {
      db_query ( "INSERT INTO {term_relation_types} (rtid,name) VALUES (%d,'%s')", $key + $rtid, $value );
    }
  }
  
  $vid = $form_state ['values'] ['vid'];
  $whole = trim ( $form_state ['values'] ['name_list'] );
  $temp = explode ( "\n", $whole );
  foreach ( $temp as $a ) {
    $taxon [$a] ['name'] = check_plain ( trim ( $a ) );
    $taxon [$a] ['parent'] = 0;
    $taxon [$a] ['vid'] = $vid;
    taxonomy_save_term ( $taxon [$a] );
  }
  
  classification_create_enhanced_field ( $vid, 'field_rank', 'Rank', 'options' );
  classification_create_enhanced_field ( $vid, 'field_vernacular_language', 'Vernacular Language', 'options' );
  
  $link = l ( t ( 'View and Edit' ), 'classification/edit' );
  drupal_set_message ( t ( 'Success! Your names were imported. ' . $link . ' your classification' ) );
}

/**
 * File import function
 */
function classification_import_file($form, &$form_state){
  set_time_limit ( 180 );
  if ($form_state ['values'] ['vid'] == 0) {
    form_set_error ( 'vid', t ( 'Please select a vocabulary for classification' ) );
  }
  if ($form_state ['values'] ['url'] == "") {
    if ($form_state ['values'] ['import_format'] == 0) {
      form_set_error ( 'import_format', t ( 'Please select a File Format' ) );
    }
    ;
    if ($file = file_save_upload ( 'data' )) {
      $fd = fopen ( $file->filepath, "rb" );
      $info = fstat ( $fd );
      $len = $info ["size"];
      $text = fread ( $fd, $len );
      fclose ( $fd );
      if (! $fd) {
        form_set_error ( 'data', t ( 'Classification import failed: file %filename cannot be read.', array (
          '%filename' => $file->filename ) ) );
      } else {
        classification_import_parse ( $text, $form_state ['values'] ['vid'], $form_state ['values'] ['import_format'] );
        $currname = date ( "Ymd_Gi_" ) . 'classificationimport.txt.gz';
        file_save_data ( gzencode ( $text ), 'classification/temp/' . $currname, FILE_EXISTS_REPLACE );
      }
    } else {
      form_set_error ( 'data', t ( 'Classification import failed: file was not uploaded.' ) );
    }
  }
}

function classification_import_parse($text, $vid, $import_format){
  // Parse the data sent in the file, and shove it in to the Database.
  

  // Parse the file, creating the correct SQL as we go.
  // Each line will have an entry in the term_data table, and term_hierarchy table 
  // (roots having a parent of 0)
  

  // These commands are repeated trice in the code: in TCS, text file and batch import
  // Now that we have the $vid, this init function could also be used to insert the potential relationships and extended fields
  $result_rtid = db_query ( "SELECT max(rtid) as rtid FROM {term_relation_types}" );
  $seq_rtid = db_fetch_object ( $result_rtid );
  $rtid = $seq_rtid->rtid;
  $rtid = $rtid + 1;
  
  // The order of these cannot be changed
  $relationTypes = array (
    0 => "synonym", 
    1 => "synonym (objective = homotypic)", 
    2 => "synonym (subjective = heterotypic)", 
    3 => "lexical variant", 
    4 => "disputed synonym", 
    5 => "spelling alternative", 
    6 => "incorrect spelling", 
    7 => "incorrect authority information", 
    8 => "vernacular name", 
    9 => "usage reference" );
  
  foreach ( $relationTypes as $key => $value ) {
    $sql = db_query ( "SELECT name FROM {term_relation_types} WHERE name = '%s'", $value );
    $obj = db_fetch_object ( $sql );
    if (! $obj) {
      db_query ( "INSERT INTO {term_relation_types} (rtid,name) VALUES (%d,'%s')", $key + $rtid, $value );
    }
  }
  
  // Let's first get the lowest value for "tid", upon which we will build all the
  // other values.
  $sql = "SELECT MAX(`tid`) FROM {term_data};";
  $result = db_query ( $sql );
  $sequence = db_fetch_array ( $result );
  $sequence = max ( $sequence ) + 1;
  
  // Values are:
  // tid vid name description weight
  // ($sequence, $vid, $parsedData, $parsedData, 0)
  // tid parent
  // $sequence $parent
  $term_data = "";
  $term_hierarchy = "";
  $synonym_data = "";
  $lines = preg_split ( "/[\n\r]+/", $text );
  
  // Here starts the term_relation addition
  $synonym_term_data = "";
  $synonym_relation = "";
  $synonym_tids = "";
  // Here ends the term_relation addition
  

  if ($import_format == 0) {
    // Here we try and guess the import format, and set it accordingly.
    // Files are defined as below.  We only test the first 10 lines.
    // if first five chars = '&lt;?xml' format is XML
    // if all have the same number of tabs
    //   then they're 1 or 3
    //     if two or three tabs
    //       then 2
    //       else more 3
    //   else 2
    if (substr ( $lines [0], 0, 5 ) == "<?xml") {
      $import_format = 4;
    } else {
      $numbers_of_tabs = array ();
      for($i = 0; $i < 10; $i ++) {
        if (trim ( $lines [$i] ) == "") {
          break;
        }
        $numbers_of_tabs [] = count ( split ( "\t", $lines [$i] ) ) - 1;
      }
      // Check if they're all the same (add together, divide by number of them, if equal
      // to all of them, then they're the same.
      if (array_sum ( $numbers_of_tabs ) / count ( $numbers_of_tabs ) == $numbers_of_tabs [$i]) {
        $import_format = 2;
        if ((array_sum ( $numbers_of_tabs ) / count ( $numbers_of_tabs )) < 4)
          $import_format = 1;
      }
      if ($import_format != 2) {
        $import_format = 3;
      }
    }
  }
  if ($import_format == 4) {
    drupal_set_message ( t ( 'Import of local XML file does not yet work' ), 'error' );
  } else {
    if ($import_format == 1) {
      // File is in the format:
      /*
kingdom1	any	descr1	syn1
phylum1	kingdom1
phylum2	kingdom1
class1	phylum1
order1	class1		syn2|syn3
class2	phylum1
     */
      // Each line of the taxonomy constitues an entry into both of the tables.
      // Fill out the term_data first, filling out an array name=>parent_name
      // name=>number
      // which can then be converted into numbers.
      $import_message = t ( "Imported using Parent Child format" );
      $parents = array ();
      $term_id = array ();
      foreach ( $lines as $line ) {
        if (trim ( $line ) == "")
          break;
        $line_parts = split ( "\t", trim ( $line ) );
        // Remove '"' if at start or end of part
        for($i = 0; $i < count ( $line_parts ); $i ++) {
          $line_parts [$i] = trim ( $line_parts [$i] );
          if (substr ( $line_parts [$i], 0, 1 ) == '"')
            $line_parts [$i] = substr ( $line_parts [$i], 1, strlen ( $line_parts [$i] ) - 2 );
        }
        
        $name = $line_parts [0];
        $parent = $line_parts [1];
        $description = (isset ( $line_parts [2] )) ? $line_parts [2] : '';
        
        $synonyms = (isset ( $line_parts [3] )) ? split ( "\|", $line_parts [3] ) : $synonyms = array ();
        $sequence_p = $sequence;
        foreach ( $synonyms as $synonym ) {
          if ($synonym_data != "")
            $synonym_data .= ",";
          $synonym_name = trim ( $synonym, "\"\t\n\r ." );
          $synonym_data .= '(' . $sequence_p . ',\'' . addslashes ( $synonym_name ) . '\')';
          // Here starts the term_relation addition
          if ($sequence == $sequence_p)
            $sequence ++;
          if ($synonym_term_data != "")
            $synonym_term_data .= ",";
          if ($synonym_name != "")
            $synonym_term_data .= "(" . $sequence . "," . $vid . ",'" . addslashes ( $synonym_name ) . "','',0)";
          if ($synonym_relation != "")
            $synonym_relation .= ",";
          $synonym_relation .= '(' . $sequence . ',' . ($sequence_p) . ')';
          if ($synonym_tids != "")
            $synonym_tids .= " OR tid1=";
          $synonym_tids .= $sequence;
          $sequence ++;
          // Here ends the term_relation addition
        }
        
        if ($term_data != "")
          $term_data .= ",";
        $term_data .= "(" . $sequence_p . "," . $vid . ",'" . addslashes ( $name ) . "','" . addslashes ( $description ) . "',0)";
        $parents [$name] = $parent;
        $term_id [$name] = $sequence_p;
        $sequence ++;
      }
      // We should now have all the term_data SQL created, and know the IDs for all
      // of the names, we can now create the hierachy SQL.
      $names = array_keys ( $parents );
      foreach ( $names as $name ) {
        // Convert the parent name into an ID
        if ($term_hierarchy != "")
          $term_hierarchy .= ",";
        $parent_id = $term_id [$parents [$name]];
        if ($parent_id == "")
          $parent_id = "0";
        $term_hierarchy .= '(' . $term_id [$name] . ',' . $parent_id . ')';
      }
    } elseif ($import_format == 2) {
      $import_message = t ( "Imported using Tabbed List format" );
      // File is in the format:
      /*
kingdom1;descr1
	phylum1;descr2;syn1
		class1
			order1;;syn2|syn3
		class2
			order2
			order3
*/
      foreach ( $lines as $line ) {
        if (trim ( $line ) == "")
          break;
        $line_parts = split ( ";", rtrim ( $line ) );
        $name = $line_parts [0];
        $reference = (isset ( $line_parts [1] )) ? $line_parts [1] : '';
        $synonyms = (isset ( $line_parts [2] )) ? split ( "\|", $line_parts [2] ) : array ();
        // Count the number of tabs before the name.  There should be no tabs within the name, whilst
        // all tabs after the name are removed.
        $name = rtrim ( $name );
        $count = count_chars ( $name, 1 );
        $rank = 0;
        if (isset ( $count [ord ( "\t" )] ))
          $rank = $count [ord ( "\t" )];
          
        // Add the SQL for this item to the term_data stuff
        

        $sequence_p = $sequence;
        foreach ( $synonyms as $synonym ) {
          if ($synonym_data != "")
            $synonym_data .= ",";
          $synonym_name = trim ( $synonym, "\"\t\n\r ." );
          $synonym_data .= '(' . $sequence_p . ',\'' . addslashes ( $synonym_name ) . '\')';
          // Here starts the term_relation addition
          if ($sequence == $sequence_p)
            $sequence ++;
          if ($synonym_term_data != "")
            $synonym_term_data .= ",";
          if ($synonym_name != "")
            $synonym_term_data .= "(" . $sequence . "," . $vid . ",'" . addslashes ( $synonym_name ) . "','',0)";
          if ($synonym_relation != "")
            $synonym_relation .= ",";
          $synonym_relation .= '(' . $sequence . ',' . ($sequence_p) . ')';
          if ($synonym_tids != "")
            $synonym_tids .= " OR tid1=";
          $synonym_tids .= $sequence;
          $sequence ++;
          // Here ends the term_relation addition
        }
        
        if ($term_data != "")
          $term_data .= ",";
        $term_data .= '(' . $sequence_p . ',' . $vid . ",'" . addslashes ( trim ( $name, "\"\t\n\r ." ) ) . "','" . addslashes ( trim ( $reference, "\"\t\n\r ." ) ) . "',0)";
        
        // Set this as a parent item, wipe all parent items with numbers *LESS* than this (easiest way is to just
        // copy over the ones below to a new array)
        if ($rank > 0) {
          $temp_array = array ();
          for($i = 0; $i < $rank; $i ++) {
            if (isset ( $parents [$i] ))
              $temp_array [$i] = $parents [$i];
          }
          $parents = $temp_array;
          if ($term_hierarchy != "")
            $term_hierarchy .= ",";
          $term_hierarchy .= "(" . $sequence_p . "," . array_pop ( $temp_array ) . ")";
          $parents [$rank] = $sequence_p;
        } else {
          $parents = array (
            0 => $sequence_p );
          if ($term_hierarchy != "")
            $term_hierarchy .= ",";
          $term_hierarchy .= "(" . $sequence_p . ",0)";
        }
        $sequence ++;
      }
    } elseif ($import_format == 3) {
      // File is in the format:
      /*
      taxon	taxon	taxon	taxon	rank	description	synonyms
kingdom1				kingdom
kingdom1	phylum1			phylum
kingdom1	phylum1	class1		classis	descr0
kingdom1	phylum1	class1	order1	order	descr1	syn1|syn2
kingdom1	phylum1	class1	order2	order		syn3|syn4
kingdom1	phylum1	class2	order3	order	descr3	syn5
*/
      $import_message = t ( "Imported using Full Hierarchy format" );
      $first_line = array_shift ( $lines );
      $first_line_parts = split ( "\t", trim ( $first_line ) );
      $number_of_ranks = 0;
      $line_parts_index = array ();
      $i = 0;
      foreach ( $first_line_parts as $part ) {
        if (strtolower ( trim ( $part ) ) == 'taxon') {
          $number_of_ranks ++;
        } else {
          $line_parts_index [strtolower ( trim ( $part ) )] = $i;
        }
        $i ++;
      }
      $names = array ();
      $hierarchy = array ();
      foreach ( $lines as $line ) {
        if (trim ( $line ) != "") {
          $line_parts = split ( "\t", trim ( $line ) );
          $taxon_parts = array ();
          for($i = 0; $i < $number_of_ranks; $i ++) {
            if (! in_array ( trim ( $line_parts [$i] ), $names ) && trim ( $line_parts [$i] != "" )) {
              $name = trim ( $line_parts [$i] );
              $names [$sequence] = $name;
              $sequence_p = $sequence;
              if ($line_parts [$i + 1] == "" or $i == $number_of_ranks - 1) {
                $field_rank = $line_parts [$line_parts_index ['rank']];
                $reference = $line_parts [$line_parts_index ['description']];
                $synonyms = split ( "\|", $line_parts [$line_parts_index ['synonyms']] );
                if (! empty ( $synonyms [0] ))
                  foreach ( $synonyms as $synonym ) {
                    if ($synonym_data != "")
                      $synonym_data .= ",";
                    $synonym_name = trim ( $synonym, "\"\t\n\r ." );
                    if ($synonym_name != "")
                      $synonym_data .= '(' . $sequence_p . ',\'' . addslashes ( $synonym_name ) . '\')';
                      // Here starts the term_relation addition
                    if ($sequence == $sequence_p)
                      $sequence ++;
                    if ($synonym_term_data != "")
                      $synonym_term_data .= ",";
                    if ($synonym_name != "")
                      $synonym_term_data .= "(" . $sequence . "," . $vid . ",'" . addslashes ( $synonym_name ) . "','',0)";
                    if ($synonym_relation != "")
                      $synonym_relation .= ",";
                    $synonym_relation .= '(' . $sequence . ',' . ($sequence_p) . ')';
                    if ($synonym_tids != "")
                      $synonym_tids .= " OR tid1=";
                    $synonym_tids .= $sequence;
                    $sequence ++;
                    // Here ends the term_relation addition
                  }
              }
              //term_data
              if ($term_data != "")
                $term_data .= ",";
              $term_data .= "(" . $sequence_p . "," . $vid . ",'" . addslashes ( $name ) . "','" . addslashes ( trim ( $reference ) ) . "',0)";
              
              //field_rank_data, let synonyms do not have rank
              if ($field_rank_data != "")
                $field_rank_data .= ",";
              $field_rank_data .= "(" . $sequence_p . ",'field_rank','" . addslashes ( trim ( $field_rank ) ) . "')";
              
              // term_hierarchy
              if ($term_hierarchy != "")
                $term_hierarchy .= ",";
              if ($i > 0) {
                // Add to hierarchy, allows to skip one (only one!) parent
                $parent = trim ( $line_parts [$i - 1] );
                if ($parent != "") {
                  $parent_id = array_search ( $parent, $names );
                } else {
                  $parent_up = trim ( $line_parts [$i - 2] );
                  $parent_id = array_search ( $parent_up, $names );
                }
                $term_hierarchy .= '(' . $sequence_p . ',' . $parent_id . ')';
              } else {
                $term_hierarchy .= '(' . $sequence_p . ',0)';
              }
              $sequence ++;
            }
          }
        }
      }
    }
    
    $term_hierarchy = "INSERT INTO {term_hierarchy} VALUES " . $term_hierarchy;
    $term_data = "INSERT INTO {term_data} VALUES " . $term_data;
    if ($synonym_data != "") {
      $synonym_data = "INSERT INTO {term_synonym} (`tid`,`name`) VALUES " . $synonym_data;
      // Here starts the term_relation addition
      $synonym_term_data = "INSERT INTO {term_data} VALUES " . $synonym_term_data;
      $synonym_hierarchy = "INSERT INTO {term_hierarchy} (`tid`,`parent`) VALUES " . $synonym_relation;
      $synonym_relation = "INSERT INTO {term_relation} (`tid1`,`tid2`) VALUES " . $synonym_relation;
      $synonym_tids = "SELECT trid FROM {term_relation} WHERE tid1=" . $synonym_tids;
      // Here ends the term_relation addition
    }
    
    if ($field_rank_data != "") {
      $field_rank_data = "INSERT INTO {taxonomy_enhancer_value_text} (`tid`,`fid`,`value`) VALUES " . $field_rank_data;
    }
    
    $data_success = db_query ( $term_data );
    $hierarchy_success = db_query ( $term_hierarchy );
    if ($synonym_data != "" and $data_success and $hierarchy_success) {
      //db_query($synonym_data);  // We stop pupulating the term_synonym table
      // Here starts the term_relation addition
      db_query ( $synonym_term_data );
      db_query ( $synonym_hierarchy );
      db_query ( $synonym_relation );
      // After relation import, update term_relation_has_type table
      $trids = db_query ( $synonym_tids );
      $synonym_trids = "";
      while ( $row = db_fetch_object ( $trids ) ) {
        if ($synonym_trids != "")
          $synonym_trids .= ",";
        $synonym_trids .= '(' . $row->trid . ',1)'; // "1" because they are "synonyms"
      }
      $synonym_trids = "INSERT INTO {term_relation_has_type} VALUES " . $synonym_trids;
      db_query ( $synonym_trids );
      // Here ends the term_relation addition
    }
    if ($field_rank_data != "" and $data_success and $hierarchy_success) {
      $field_rank_data = db_query ( $field_rank_data );
    }
  }
  classification_create_enhanced_field ( $vid, 'field_rank', 'Rank', 'options' );
  drupal_set_message ( $import_message );
  return true;
}

function theme_classification_import_message(){
  $vid = $_REQUEST ['vid'];
  $tot_count = $_REQUEST ['tot_count'];
  $status = $_REQUEST ['status'];
  
  $json = array (
    'message' => '<div class="messages status">' );
  
  switch ($status) {
    case 'fail' :
      $json ['message'] .= t ( 'Import complete.' );
      break;
    case 'success' :
      $json ['message'] .= t ( 'Import completed in a flash (or is <i>almost</i> done).' );
      break;
  }
  $json ['message'] .= ' ';
  $json ['message'] .= l ( 'View and Edit', 'classification/edit' );
  $json ['message'] .= ' your classification.</div>';
  
  return drupal_json ( $json );
}
