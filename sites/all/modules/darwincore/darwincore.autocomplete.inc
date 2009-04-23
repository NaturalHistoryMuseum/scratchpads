<?php
/**
 * Function for the autocomplete
 */
function darwincore_autocomplete($field, $string=''){
  switch ($field){
    case 'taxonomicname':
      $results = db_query_range("SELECT DISTINCT name AS string FROM {term_data} WHERE LOWER(name) LIKE LOWER('%s%%')", $string, 0, 10);
      break;
    default:
      $results = db_query_range("SELECT DISTINCT %s AS string FROM {darwincore} WHERE LOWER(%s) LIKE LOWER('%s%%')", $field, $field, $string, 0, 10);
  }
  while ($result = db_fetch_object($results)){
    $matches[$result->string] = check_plain($result->string);
  }
  print drupal_to_js($matches);
  exit();
}