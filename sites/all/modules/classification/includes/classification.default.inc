<?php

require_once("classification_functions.class");
global $names;
$names = new Names;

/**
 * Function to build menu page and classification landing page
 */
function classification_menu_block_page() {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  if (system_admin_compact_mode()) {
  	$output = '<h2>Manage Classification</h2>';
    $output .= '<ul class="menu">';
    foreach ($content as $item) {
      $output .= '<li class="leaf">'. l($item['title'], $item['href'], $item['localized_options']) .'</li>';
    }
    $output .= '</ul>';
  }
  else {
  	$output = '<h2>Manage Classification</h2>';
    $output .= '<dl class="admin-list">';
    foreach ($content as $item) {
      $output .= '<dt>'. l($item['title'], $item['href'], $item['localized_options']) .'</dt>';
      $output .= '<dd>'. $item['description'] .'</dd>';
    }
    $output .= '</dl>';
  }
  
  return $output;
}

/**
 * Autocomplete callback for node types that require it
 */
function classification_autocomplete_single($vid,$string='') {
  $regexp = '%(?:^|,\ *)("(?>[^"]*)(?>""[^"]* )*"|(?: [^",]*))%x';
  preg_match_all($regexp, $string, $matches);
  $array = $matches[1];

  $last_string = trim(array_pop($array));
  $matches = array();
  $classification_autocomplete = variable_get('classification_autocomplete_size',3);
  if (strlen($last_string) >= $classification_autocomplete ) {
    $result = db_query_range(db_rewrite_sql("SELECT t.name FROM {term_data} t WHERE t.vid = %d AND LOWER(t.name) LIKE LOWER('%%%s%%')", 't', 'tid'), $vid, $last_string, 0, 10);

    while ($tag = db_fetch_object($result)) {
      $n = $tag->name;
      $matches[$n] = check_plain($tag->name);
    }
  }
  classification_to_json($matches);
} 

function classification_autocomplete_multiple($vid,$string='') {
	// The user enters a pipe-separated list of tags. We only autocomplete the last tag.
	$array = classification_explode_tags($string);
  
   // Fetch last tag
  $last_string = trim(array_pop($array));
  $matches = array();
  if ($last_string != '') {
    $classification_autocomplete = variable_get('classification_autocomplete_size',3);
    if (strlen($last_string) >= $classification_autocomplete ) {
      $result = db_query_range(db_rewrite_sql("SELECT t.name FROM {term_data} t WHERE t.vid = %d AND LOWER(t.name) LIKE LOWER('%%%s%%')", 't', 'tid'), $vid, $last_string, 0, 10);
    
      $prefix = count($array) ? implode(' | ', $array) .' | ' : '';
    
      while ($tag = db_fetch_object($result)) {
        $n = $tag->name;
        $matches[$prefix . $n] = check_plain($tag->name);
      }
    }
  }
  classification_to_json($matches);
}

function classification_to_json($var = NULL) {
  drupal_set_header('Content-Type: text/javascript; charset=utf-8');
  if (isset($var)) {
    echo classification_to_js($var);
  }
  else {
  	drupal_json(array('status' => TRUE,'data' => ''));
  }
}

function classification_to_js($var) {
  switch (gettype($var)) {
    case 'boolean':
      return $var ? 'true' : 'false'; // Lowercase necessary!
    case 'integer':
    case 'double':
      return $var;
    case 'resource':
    case 'string':
      return '"'. str_replace(array("\r", "\n", "<", ">", "&"),
                              array('\r', '\n', '\x3c', '\x3e', '\x26'),
                              addslashes($var)) .'"';
    case 'array':
      // Arrays in JSON can't be associative. If the array is empty or if it
      // has sequential whole number keys starting with 0, it's not associative
      // so we can go ahead and convert it as an array.
      if (empty ($var) || array_keys($var) === range(0, sizeof($var) - 1)) {
        $output = array();
        foreach ($var as $v) {
          $output[] = classification_to_js($v);
        }
        return '[ '. implode(', ', $output) .' ]';
      }
      // Otherwise, fall through to convert the array as an object.
    case 'object':
      $output = array();
      foreach ($var as $k => $v) {
        $output[] = classification_to_js(strval($k)) .': '. classification_to_js($v);
      }
      return '{ '. implode(', ', $output) .' }';
    default:
      return 'null';
  }
}

/**
 * Callback to check if term actually exists
 */
 function classification_checkterm_single($vid){
 	global $names;
 	
  $form = array(
    '#post' => $_POST,
  );
  $name = $_REQUEST['classification']['name_tags'][$vid];

  $result = db_fetch_object(db_query("SELECT tid FROM {term_data} WHERE vid = %d AND name = '%s'", $vid, $name));
  if(!$result->tid){
  	$bad_name = $names->italicized_form($name);
    if (user_access('administer classification')) {
      $output = "<div  class='error'>" . $bad_name . "  does not exist in this site's classification.</div>";
    }
    else {
      $output = "<div  class='error'>" . $bad_name . " does not exist in this site's classification. Please contact your LifeDesk administrator.</div>";
    }
  }
  else {
  	$output = '';
  }
  
  drupal_json(array('status' => TRUE,'data' => $output));
}

/**
 * Callback to check if newly added term in multiple autocomplete actually exists
 */
 function classification_checkterm_multiple($vid){
 	global $names;
 	
  $form = array(
    '#post' => $_POST,
  ); 
  
  $string = $_REQUEST['classification']['name_tags'][$vid];
  
  // We need to check all names in the string, not just one
  $array = classification_explode_tags($string);
  foreach ($array as $name) {
  	$result = db_fetch_object(db_query("SELECT tid FROM {term_data} WHERE vid = %d AND name = '%s'", $vid, trim($name)));
    if(!$result->tid){
    	$bad_name[] = $names->italicized_form(trim($name));
    }
  }
  if (count($bad_name) > 0) {
    $output = "<div  class='error'>" . implode(', ',$bad_name) . " does not exist in this site's classification.</div>";
  }
  else {
  	$output = '';
  }

  drupal_json(array('status' => TRUE,'data' => $output));
}