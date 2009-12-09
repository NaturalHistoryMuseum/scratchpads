<?php

/**
 * Provides a page that lists all the Scratchpads.
 */
function scratchpad_list_scratchpads($output='HTML'){
  if($output == 'HTML'){
    $sites = variable_get('scratchpad_sites_list',array());
    $items = array();
    uksort($sites);
    foreach($sites as $data){
      $items[] = l($data['site_title'] . ' (http://' . $data['site_url'] . ')', 'http://'.$data['site_url']);
    }
    return theme('item_list', $items, count($sites).' sites');
  } else {
    // We'll output the list serialized
    drupal_set_header('Content-Type: text/plain');
    echo serialize(variable_get('scratchpad_sites_list',array()));
    exit;
  }
}

/**
 * Provides a callback for reporting a Scratchpad
 */
function scratchpad_report_site(){
  // First lets check that $site is a Scratchpad.  We can do this be getting the
  // front page and looking for our Google Analytics code
  $data = @unserialize($_GET['data']);
  if(is_array($data) && isset($data['site_url'])){
    // Check for presence of the Scratchpad Google analytics code on the front
    // page of the reporting site, if it is there, then it's a Scratchpad.
    $front_page = file_get_contents('http://'.$data['site_url']);
    if(strpos($front_page,'UA-2428547-2')){
      $sites = variable_get('scratchpad_sites_list', array());  
      $sites[$data['site_url']] = $data;
      variable_set('scratchpad_sites_list', $sites); 
    }
  }
}