<?php

function spm_autocomplete($taxon_name){
  $req = 'http://services.eol.org/lifedesk/service.php?function=search&search=' . urlencode($taxon_name);
  $response = drupal_http_request($req);
  $names = array();
  if($response->code == 200){
    $data = $response->data;
    $xml = simplexml_load_string($data);
    if($xml->value){
      $options = array();
      $child_count = array();
      foreach($xml->value as $values){
        $names[(string)$values->name] = (string)$values->name;
      }
    }
  }
  if(count($names)){
    print drupal_to_js($names);
  } else {
    print drupal_to_js(array("UNRECOGNIZED NAME"));
  }
  exit;
}