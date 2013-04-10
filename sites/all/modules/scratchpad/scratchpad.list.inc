<?php

/**
 * Provides a page that lists all the Scratchpads.
 */
function scratchpad_list_scratchpads(){
  $sites = variable_get('scratchpad_sites_list', array());
  $items = array();
  ksort($sites);
  foreach($sites as $data){
    $output = l($data['site_title'] . ' (http://' . $data['site_url'] . ')', 'http://' . $data['site_url']) . '<br/><span style="font-size:80%">Users: ' . $data['users']['total'] . ' | Nodes: ' . (is_array($data['nodes']) ? array_sum($data['nodes']) : 0) . ' | Views: ' . $data['views'] . '</span>';
    if(user_access('SCRATCHPADIFY')){
      '<br/><span style="font-size:80%">Last login: ' . $data['last_login_time'] . ' | Last Edited: ' . $data['last_node_changed_time'] . '</span>';
    }
    $items[] = $output;
  }
  return theme('item_list', $items, count($sites) . ' sites');
}

function scratchpad_list_scratchpads_emails(){
  // Output the emails as a list
  $scratchpad_details = variable_get('scratchpad_sites_list', array());
  foreach($scratchpad_details as $site => $site_details){
    if(isset($site_details['maintainer_emails']) && is_array($site_details['maintainer_emails'])){
      foreach($site_details['maintainer_emails'] as $mail){
        $mails[$mail] = $mail;
      }
    }else{
      $mails['not_found@' . $site] = 'not_found@' . $site;
    }
  }
  drupal_set_title('');
  return theme('item_list', $mails, 'Emails for ' . count($mails) . ' maintainers', 'ul', array(
    'style' => 'font-size:80%; padding:50px'
  )) . '<h1>' . l('Mail all the above users', 'mailto:scratchpad@nhm.ac.uk?bcc=<' . implode(">,<", $mails) . '>', array(
    'absolute' => TRUE
  )) . '</h1>';
}

function scratchpad_list_scratchpads_serial($json = FALSE){
  // We'll output the list serialized
  drupal_set_header('Content-Type: text/plain');
  $scratchpad_details = variable_get('scratchpad_sites_list', array());
  if(!user_access('SCRATCHPADIFY')){
    foreach($scratchpad_details as $site => $site_details){
      // Sod it, lets just hide the maintainer emails.
      unset($scratchpad_details[$site]['maintainer_emails']);
    }
  }
  if($json){
    drupal_set_header('Content-Type: application/json');
    echo json_encode($scratchpad_details);
  }else{
    echo serialize($scratchpad_details);
  }
  exit();
}

function scratchpad_list_scratchpads_table(){
  $header = array(
    'Site',
    'Profile',
    'Last&nbsp;Login',
    'Last&nbsp;Edited',
    'Date reported',
    'Users',
    'Nodes',
    'Views',
    'Maintainer Emails'
  );
  $scratchpad_details = variable_get('scratchpad_sites_list', array());
  foreach($scratchpad_details as $site => $data){
    $rows[] = array(
      l('http://' . $site, 'http://' . $site, array(
        'absolute' => TRUE
      )),
      isset($data['profile']) ? $data['profile'] : 'unknown',
      '<span style="display:none;">' . $data['last_login_time'] . '</span>' . date('j-F-Y', $data['last_login_time']),
      '<span style="display:none;">' . $data['last_node_changed_time'] . '</span>' . date('j-F-Y', $data['last_node_changed_time']),
      '<span style="display:none;">' . $data['last_reported'] . '</span>' . date('j-F-Y', $data['last_reported']),
      $data['users']['total'],
      is_array($data['nodes']) ? array_sum($data['nodes']) : 0,
      $data['views'],
      @implode(", ", $data['maintainer_emails'])
    );
  }
  drupal_add_js(drupal_get_path('module', 'tablesorter') . '/tablesorter.js');
  drupal_add_css(drupal_get_path('module', 'tablesorter') . '/tablesorter.css');
  return theme('table', $header, $rows, array(
    'class' => 'views-table'
  ));
}

/**
 * Provides a callback for reporting a Scratchpad
 */
function scratchpad_report_site(){
  // First lets check that $site is a Scratchpad.  We can do this be getting the
  // front page and looking for our Google Analytics code
  $data = @unserialize($_GET['data']);
  if(is_array($data) && isset($data['site_url'])){
    $sites = variable_get('scratchpad_sites_list', array());
    $data['last_reported'] = time();
    $sites[$data['site_url']] = $data;
    // Loop through the sites, and delete the ones that haven't called
    // in, in over a month.
    $too_old_time = time() - 1209600; // Seconds in two weeks
    foreach($sites as $site => $site_details){
      if($site_details['last_reported'] < $too_old_time){
        unset($sites[$site]);
      }
    }
    variable_set('scratchpad_sites_list', $sites);
  }else{
    watchdog('scratchpad_report', 'Error with data: %data', array(
      '%data' => $_GET['data']
    ));
  }
}

function scratchpad_list_scratchpads_thumbs(){
  $scratchpad_statistics = variable_get('scratchpad_sites_list', array());
  $items = array();
  $total_sites = count($scratchpad_statistics);
  // Sort the array alphabetically first, so that we can save the alpha sort 
  // order  
  ksort($scratchpad_statistics);
  $i = 0;
  foreach($scratchpad_statistics as $key => $value){
    $scratchpad_statistics[$key]['domain_sort'] = $total_sites - $i;
    $i++;
  }
  $i = 0;
  // Ensure the array is sorted by pages, cos it looks kewl!
  uasort($scratchpad_statistics, '_scratchpad_list_scratchpads_thumbs_sort');
  foreach($scratchpad_statistics as $site => $site_statistics){
    // If we're on the same server as screenshots.scratchpads.eu, we can 
    // check to see if the thumbnails exist, and if they don't skip this entry.
    if(gethostbyname($_SERVER['SERVER_NAME']) == gethostbyname('screenshots.scratchpads.eu')){
      // We're on the same server, lets check for the files
      if(!file_exists('/var/www/screenshots.scratchpads.eu/' . $site . '.small.drop.png') || !file_exists('/var/www/screenshots.scratchpads.eu/' . $site . '.small.drop.annotated.png')){
        continue;
      }
    }
    $items[] = l('<img src="http://screenshots.scratchpads.eu/' . $site . '.small.drop.png" width="209px" height="158px"/>', 'http://' . $site, array(
      'absolute' => TRUE,
      'html' => 1,
      'attributes' => array(
        'nodes' => is_array($site_statistics['nodes']) ? array_sum($site_statistics['nodes']) . ".$i" : "1.$i",
        'domain' => $site_statistics['domain_sort'],
        'views' => $site_statistics['views'] . ".$i",
        'users' => $site_statistics['users']['total'] . ".$i"
      )
    ));
    $i++;
  }
  $output = '<div class="scratchpad_thumbs_header"><p>Sort by: <a href="#" id="domain">Domain</a> | <a href="#" id="nodes">Pages</a> | <a href="#" id="views">Views</a> | <a href="#" id="users">Users</a></p></div>';
  $output .= theme('item_list', $items, NULL, 'ul', array(
    'class' => 'scratchpad_thumbs'
  ));
  drupal_add_css(drupal_get_path('module', 'scratchpad') . '/scratchpad.thumbs.css');
  drupal_add_js(drupal_get_path('module', 'scratchpad') . '/scratchpad.thumbs.js');
  print theme('page', $output, FALSE);
}

function _scratchpad_list_scratchpads_thumbs_sort($first, $second){
  return (is_array($second['nodes']) ? array_sum($second['nodes']) : 1) / (is_array($first['nodes']) && array_sum($first['nodes']) ? array_sum($first['nodes']) : 1);
}

function scratchpad_list_scratchpads_stats(){
  $scratchpad_statistics = variable_get('scratchpad_sites_list', array());
  $total_users = 0;
  $total_login_users = 0;
  $total_users_week = 0;
  $total_users_month = 0;
  $total_sites_week = 0;
  $total_sites_month = 0;
  $total_nodes = array(
    'totaltotaltotal' => 0
  );
  $sites_with_no_nodes = array();
  foreach($scratchpad_statistics as $site => $site_statistics){
    $total_users += $site_statistics['users']['total'];
    $total_login_users += $site_statistics['users']['login'];
    $total_users_week += $site_statistics['users']['week'];
    $total_users_month += $site_statistics['users']['month'];
    if($site_statistics['last_login_time'] > time() - 604800){
      $total_sites_week++;
    }
    if($site_statistics['last_login_time'] > time() - 2592000){
      $total_sites_month++;
    }
    if(is_array($site_statistics['nodes'])){
      $total_nodes['totaltotaltotal'] += array_sum($site_statistics['nodes']);
      foreach($site_statistics['nodes'] as $type => $num){
        $total_nodes[$type] += $num;
      }
    }else{
      $sites_with_no_nodes[] = '<a href="http://' . $site . '">http://' . $site . '</a>';
    }
  }
  // Sort the no nodes array
  asort($sites_with_no_nodes);
  $output = '<pre style="font-family: mono, monospace">
Users: ' . $total_users . ' (Total activated)
Users: ' . $total_login_users . ' (Active and have logged in)
Users: ' . $total_users_week . ' (Logged in, in last 7 days)
Users: ' . $total_users_month . ' (Logged in, in last 30 days)

Sites: ' . count($scratchpad_statistics) . '
Sites: ' . $total_sites_week . ' (With users logged in, in last 7 days)
Sites: ' . $total_sites_month . ' (With users logged in, in last 30 days)

Nodes: ' . $total_nodes['totaltotaltotal'] . '

';
  unset($total_nodes['totaltotaltotal']);
  arsort($total_nodes);
  foreach($total_nodes as $type => $count){
    $output .= ' ' . str_pad("$type:", 40, " ", STR_PAD_RIGHT) . "$count\n";
  }
  return $output . '
  
Sites with no nodes:
' . implode("\n\n", $sites_with_no_nodes) . '</pre>';
}