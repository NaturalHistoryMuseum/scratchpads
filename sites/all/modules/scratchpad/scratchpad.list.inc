<?php

/**
 * Provides a page that lists all the Scratchpads.
 */
function scratchpad_list_scratchpads(){
  $sites = variable_get('scratchpad_sites_list',array());
  $items = array();
  ksort($sites);
  foreach($sites as $data){
    $output = l($data['site_title'] . ' (http://' . $data['site_url'] . ')', 'http://'.$data['site_url']) . '<br/><span style="font-size:80%">Users: '.$data['users']['total'].' | Nodes: '.$data['nodes'] .' | Views: '.$data['views'].'</span>';
    if(user_access('SCRATCHPADIFY')){
      '<br/><span style="font-size:80%">Last login: '.$data['last_login_time'].' | Last Edited: '.$data['last_node_changed_time'].'</span>';
    }
    $items[] = $output;
  }
  return theme('item_list', $items, count($sites).' sites');
}

function scratchpad_list_scratchpads_emails(){
  // Output the emails as a list
  $scratchpad_details = variable_get('scratchpad_sites_list', array());
  foreach($scratchpad_details as $site => $site_details){
    if(isset($site_details['maintainer_emails']) && is_array($site_details['maintainer_emails'])){
      foreach($site_details['maintainer_emails'] as $mail){
        $mails[$mail] = $mail;
      }
    } else {
      $mails['not_found@'.$site] = 'not_found@'.$site;
    }
  }
  drupal_set_title('');
  return theme('item_list', $mails, 'Emails for '.count($mails).' maintainers', 'ul', array('style' => 'font-size:80%; padding:50px')) . 
    '<h1>'.l('Mail all the above users','mailto:scratchpad@nhm.ac.uk?bcc=<'.implode(">,<",$mails).'>', array('absolute' => TRUE)).'</h1>';
}

function scratchpad_list_scratchpads_serial(){
  // We'll output the list serialized
  drupal_set_header('Content-Type: text/plain');
  $scratchpad_details = variable_get('scratchpad_sites_list',array());
  if(!user_access('SCRATCHPADIFY')){
    foreach($scratchpad_details as $site => $site_details){
      unset($scratchpad_details[$site]['maintainer_emails']);
      unset($scratchpad_details[$site]['last_login_time']);
      unset($scratchpad_details[$site]['last_node_changed_time']);
      unset($scratchpad_details[$site]['users']['week']);
      unset($scratchpad_details[$site]['users']['month']);
    }
  }
  echo serialize($scratchpad_details);
  exit;
}

function scratchpad_list_scratchpads_table(){
  $header = array('Site', 'Last&nbsp;Login','Last&nbsp;Edited','Users','Nodes','Views','Maintainer Emails');
  $scratchpad_details = variable_get('scratchpad_sites_list',array());
  foreach($scratchpad_details as $site=>$data){
    $rows[] = array(
      l('http://'.$site, 'http://'. $site, array('absolute' => TRUE)),
      '<span style="display:none;">'.$data['last_login_time'].'</span>'.date('j-F-Y', $data['last_login_time']),
      '<span style="display:none;">'.$data['last_node_changed_time'].'</span>'.date('j-F-Y', $data['last_node_changed_time']),
      $data['users']['total'],
      $data['nodes'],
      $data['views'],
      @implode(", ", $data['maintainer_emails'])
    );
  }
  drupal_add_js (drupal_get_path('module', 'tablesorter') . '/tablesorter.js');
  drupal_add_css(drupal_get_path('module', 'tablesorter') . '/tablesorter.css');
  return theme('table', $header, $rows, array('class' => 'views-table'));
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

function scratchpad_list_scratchpads_stats(){
  $scratchpad_statistics = variable_get('scratchpad_sites_list', array());
  $total_users = 0;
  $total_login_users = 0;
  $total_users_week = 0;
  $total_users_month = 0;
  $total_sites_week = 0;
  $total_sites_month = 0;
  $total_nodes = array('totaltotaltotal' => 0);
  foreach($scratchpad_statistics as $site => $site_statistics){
    $total_users += $site_statistics['users']['total'];
    $total_login_users += $site_statistics['users']['login'];
    $total_users_week += $site_statistics['users']['week'];
    $total_users_month += $site_statistics['users']['month'];
    if($site_statistics['last_login_time'] > time() - 604800){
      $total_sites_week ++;
    }
    if($site_statistics['last_login_time'] > time() - 2592000){
      $total_sites_month ++;
    }
    $total_nodes['totaltotaltotal'] += array_sum($site_statistics['nodes']);
    if(is_array($site_statistics['nodes'])){
      foreach($site_statistics['nodes'] as $type => $num){
        $total_nodes[$type] += $num;
      }
    } else {
      $sites_with_no_nodes[] = $site;
    }
  }
  $output = '<pre>
Users: '.$total_users.' (Total activated)
Users: '.$total_login_users.' (Active and have logged in)
Users: '.$total_users_week.' (Logged in, in last 7 days)
Users: '.$total_users_month.' (Logged in, in last 30 days)

Sites: '.count($scratchpad_statistics).'
Sites: '.$total_sites_week.' (With users logged in, in last 7 days)
Sites: '.$total_sites_month.' (With users logged in, in last 30 days)

Nodes: '.$total_nodes['totaltotaltotal'].'

';
  unset($total_nodes['totaltotaltotal']);
  arsort($total_nodes);
  foreach($total_nodes as $type => $count){
    $output .= ' '.str_pad("$type:", 40, " ", STR_PAD_RIGHT)."$count\n";
  }
  return $output .'
  
Sites with no nodes: '. implode(", ", $sites_with_no_nodes).'</pre>';
}