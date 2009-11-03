<?php

/**
 * Provides a page that lists all the Scratchpads.
 */
function scratchpadify_list_scratchpads($output='HTML'){
  if($output == 'HTML'){
    $sites = variable_get('scratchpadify_sites_list',array());
    uasort($sites, 'strnatcasecmp');
    $items = array();
    foreach($sites as $domain => $title){
      $items[] = l($title . ' (http://' . $domain . ')', 'http://'.$domain);
    }
    return theme('item_list', $items);
  } else {
    // We'll output the list serialized
    echo serialize(variable_get('scratchpadify_sites_list',array()));
    exit;
  }
}

/**
 * Provides a callback for reporting a Scratchpad
 */
function scratchpadify_add_scratchpad_to_list($domain, $title){
  // First lets check that $site is a Scratchpad.  We can do this be getting the
  // front page and looking for our Google Analytics code
  $front_page = file_get_contents('http://'.$domain);
  if(strpos($front_page,'UA-2428547-2')){
    $sites = variable_get('scratchpadify_sites_list', array());  
    $sites[$domain] = $title;
    variable_set('scratchpadify_sites_list', $sites); 
  }
}