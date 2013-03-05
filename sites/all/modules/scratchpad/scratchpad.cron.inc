<?php

function _scratchpad_cron(){
  // Don't report if we're a "dev" or D6 upgrade site.
  if(substr($_SERVER['HTTP_HOST'], 0, 3) != 's1.' && substr($_SERVER['HTTP_HOST'], 0, 4) != 'dev.'){
    // Once a day we'll report that we exist to a central repository which can
    // then build a list of all Scratchpads.
    if(variable_get('scratchpad_last_reported', 0) < time() - 86400){ // Seconds in a day
      // Send the update to our central server
      $central_servers = variable_get('scratchpad_central_servers', array(
        'http://stats.scratchpads.eu/'
      ));
      $result = db_query('SELECT COUNT(*) AS count, type FROM node GROUP BY type');
      while($row = db_fetch_array($result)){
        $nodes[$row['type']] = $row['count'];
      }
      $users = db_result(db_query("SELECT COUNT(uid) AS users FROM users WHERE status = 1 AND uid > 1")); //Total users
      $users_login = db_result(db_query("SELECT COUNT(uid) AS users FROM users WHERE login != 0")); //Total users logged in
      $users_week = db_result(db_query("SELECT COUNT(uid) AS users FROM users WHERE login > UNIX_TIMESTAMP()-604800")); //Week users
      $users_month = db_result(db_query("SELECT COUNT(uid) AS users FROM users WHERE login > UNIX_TIMESTAMP()-2592000")); //Month users
      $views = db_fetch_array(db_query("SELECT SUM(totalcount) AS totalcount FROM node_counter;"));
      $results = db_query("SELECT mail FROM {users} u, {users_roles} ur WHERE u.uid = ur.uid AND rid = 5");
      while($mail = db_result($results)){
        $mails[] = $mail;
      }
      $report_data = array(
        'site_url' => $_SERVER['HTTP_HOST'],
        'site_title' => variable_get('site_name', 'Scratchpad'),
        'site_mission' => variable_get('site_mission', ''),
        'nodes' => $nodes,
        'users' => array(
          'login' => $users_login,
          'total' => $users,
          'week' => $users_week,
          'month' => $users_month
        ),
        'views' => $views['totalcount'],
        'maintainer_emails' => $mails,
        'last_login_time' => db_result(db_query('SELECT access FROM {users} WHERE uid > 1 ORDER BY access DESC LIMIT 1;')),
        'last_node_changed_time' => db_result(db_query('SELECT created FROM {node} ORDER BY created DESC LIMIT 1')),
        'profile' => variable_get('install_profile', 'scratchpad_profile')
      );
      foreach($central_servers as $central_server){
        exec('wget "' . $central_server . 'report_scratchpad?data=' . urlencode(serialize($report_data)) . '" -O /dev/null'); // Could run into GET limits here. Ho hum!
      }
      variable_set('scratchpad_last_reported', time());
    }
    if(variable_get('scratchpad_last_updated', 0) < time() - 86400 && !in_array("http://" . $_SERVER['HTTP_HOST'] . "/", variable_get('scratchpad_central_servers', array(
      'http://stats.scratchpads.eu/'
    )))){ // 86400 = seconds in a day
      // Also update the sites list for this site
      $central_servers = variable_get('scratchpad_central_servers', array(
        'http://stats.scratchpads.eu/'
      ));
      $sites = array();
      foreach($central_servers as $central_server){
        $content = file_get_contents($central_server . 'scratchpads/serial');
        if($content){
          $sites = array_merge($sites, unserialize($content));
        }
      }
      variable_set('scratchpad_sites_list', $sites);
      variable_set('scratchpad_last_updated', time());
    }
  }
}