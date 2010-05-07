<?php
/**
 * Function for the autocomplete
 */
function darwincore_autocomplete($field, $string=''){
  // Sanity check required to ensure that NO CRACKING can be done!
  if(in_array($field, array('institutioncode','collectioncode','taxonomicname','identificationqualifier','identifiedby','collector','lifestage'))){
    switch ($field){
      case 'taxonomicname':
        $results = db_query_range("SELECT DISTINCT name AS string FROM {term_data} WHERE LOWER(name) LIKE LOWER('%s%%')", $string, 0, 10);
        break;
      case 'identifiedby':
      case 'collector':
        $results = db_query_range("SELECT DISTINCT string FROM (SELECT identifiedby AS string FROM {darwincore} UNION SELECT collector AS string FROM {darwincore}) AS something WHERE string LIKE '%s%%' ORDER BY string", $string, 0, 10);
        break;
      default:
        $results = db_query_range("SELECT DISTINCT %s AS string FROM {darwincore} WHERE LOWER(%s) LIKE LOWER('%s%%')", $field, $field, $string, 0, 10);
    }
    while ($result = db_fetch_object($results)){
      $matches[$result->string] = check_plain($result->string);
    }
  } else if (in_array($field, array('stateprovince','islandgroup','island','county','geodeticdatum','verbatimcoordinatesystem','georeferenceprotocol'))){
    $results = db_query_range("SELECT DISTINCT %s AS string FROM {darwincorelocation} WHERE LOWER(%s) LIKE LOWER('%s%%')", $field, $field, $string, 0, 10);
  }
  print drupal_to_js($matches);
  exit();
}