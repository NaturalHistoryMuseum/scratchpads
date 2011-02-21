<?php

/**
 * Callback for admin page
 */
function remote_issue_tab_admin_page(){
  // Clear the cache here - not ideal really should be clearing on submit, but
  // hey, what do I care?
  cache_clear_all('*', 'cache', TRUE);
  $feeds = array(
    t('-- Not Set --')
  );
  $roles = user_roles(TRUE);
  $results = db_query('SELECT fid, title, url FROM {aggregator_feed}');
  while($row = db_fetch_array($results)){
    $feeds[$row['fid']] = check_plain($row['title'] . ' (' . $row['url'] . ')');
  }
  return system_settings_form(array(
    'remote_issue_tab_feed' => array(
      '#title' => t('Please select the feed to be used by the Remote Issue Tab'),
      '#type' => 'select',
      '#options' => $feeds,
      '#default_value' => variable_get('remote_issue_tab_feed', 0),
      '#description' => t('Select the RSS Feed (A new RSS feed can be added using the <a href="!agg_url">Aggregator interface</a>, after which you will be redirected here.)', array(
        '!agg_url' => url('admin/content/aggregator/add/feed', array(
          'query' => array(
            'destination' => 'admin/settings/remote-issue-tab'
          )
        ))
      ))
    ),
    'remote_issue_tab_tab_title' => array(
      '#title' => t('Tab title'),
      '#type' => 'textfield',
      '#default_value' => variable_get('remote_issue_tab_tab_title', t('Issues') . '...'),
      '#description' => t('Note, the length of this field is restricted by the size of the tab.')
    ),
    'remote_issue_tab_uwho_client_key' => array(
      '#title' => t('UWho Client Key'),
      '#description' => t('Enter the key from the site you\'d like to enable your users to login to.'),
      '#type' => 'textfield',
      '#default_value' => variable_get('remote_issue_tab_uwho_client_key', ''),
      '#required' => TRUE
    ),
    'remote_issue_tab_uwho_url' => array(
      '#title' => t('UWho Client URL'),
      '#description' => t('Enter the URL of the site you\'d like to enable your users to login to.  This must be EXACTLY as provided by the other site.'),
      '#type' => 'textfield',
      '#default_value' => variable_get('remote_issue_tab_uwho_url', ''),
      '#required' => TRUE
    ),
    'remote_issue_tab_redirect_path' => array(
      '#title' => t('UWho Redirect PATH'),
      '#description' => t('Enter the path of the page on the remote site that you\'d like users to be redirected to (leave blank to use the default)'),
      '#type' => 'textfield',
      '#default_value' => variable_get('remote_issue_tab_redirect_path', '')
    ),
    'remote_issue_tab_footer' => array(
      '#title' => t('Footer'),
      '#type' => 'textarea',
      '#default_value' => variable_get('remote_issue_tab_footer', ''),
      '#description' => t('Enter text to be displayed below the Recent Issues.')
    ),
    'remote_issues_tab_cache_time' => array(
      '#title' => t('Cache time'),
      '#type' => 'select',
      '#options' => array(
        900 => '15 minutes',
        1800 => '30 minutes',
        3600 => '1 hour',
        7200 => '2 hours',
        21600 => '6 hours',
        86400 => '1 day'
      ),
      '#default_value' => variable_get('remote_issues_tab_cache_time', 900),
      '#description' => t('Select the length of time for which the Remote Issues Tab should be cached.')
    ),
    // Note, we use our own settings interface here, as we want users to be able
    // to edit this setting, and not just admins.
    'remote_issues_tab_roles' => array(
      '#title' => t('Roles'),
      '#type' => 'checkboxes',
      '#options' => $roles,
      '#description' => t('Select which roles should be able to view the Issues Tab.'),
      '#default_value' => variable_get('remote_issues_tab_roles', array_keys($roles))
    )
  ));
}