<?php

/**
 * Provides a page that lists all the Scratchpads.
 */
function scratchpadify_list_scratchpads($output='HTML'){
  if($output == 'HTML'){
    return print_r(variable_get('scratchpadify_sites_list',array()), TRUE);
  } else {
    // We'll output the list serialized
    echo serialize(variable_get('scratchpadify_sites_list',array()));
    exit;
  }
}

/**
 * Provides a callback for reporting a Scratchpad
 */
function scratchpadify_add_scratchpad_to_list($site){
  // First lets check that $site is a Scratchpad.  We can do this be getting the
  // front page and looking for our Google Analytics code
  $front_page = file_get_contents('http://'.$site);
  if(strpos($front_page,'UA-2428547-2')){
    $sites = variable_get('scratchpadify_sites_list', array());  
    $sites[$site] = $site;
  }
}