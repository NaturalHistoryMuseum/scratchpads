<?php

/**
 * Provides a page that lists all the Scratchpads.
 */
function scratchpad_list_scratchpads($output='HTML'){
  if($output == 'HTML'){
    $sites = variable_get('scratchpad_sites_list',array());
    $items = array();
    ksort($sites);
    foreach($sites as $data){
      $items[] = l($data['site_title'] . ' (http://' . $data['site_url'] . ')', 'http://'.$data['site_url']) . '<br/><span style="font-size:80%">Users: '.$data['users'].' | Nodes: '.$data['nodes'] .' | Views: '.$data['views'].'</span>';
    }
    return theme('item_list', $items, count($sites).' sites');
  } else {
    // We'll output the list serialized
    drupal_set_header('Content-Type: text/plain');
    $scratchpad_details = variable_get('scratchpad_sites_list',array());
    foreach($scratchpad_details as $site => $site_details){
      unset($scratchpad_details[$site]['maintainer_emails']);
      unset($scratchpad_details[$site]['last_login']);
    }
    echo serialize($scratchpad_details);
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
      $data['last_reported'] = time(); 
      $sites[$data['site_url']] = $data;
      // Loop through the sites, and delete the ones that haven't called
      // in, in over a month.
      $too_old_time = time() - 2592000; // Seconds in a month
      foreach($sites as $site => $site_details){
        if($site_details['last_reported'] < $too_old_time){
          unset($sites[$site]);
        }
      }
      variable_set('scratchpad_sites_list', $sites); 
    }
  }
}