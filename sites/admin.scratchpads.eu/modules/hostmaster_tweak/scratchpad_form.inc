<?php

/**
 * Callback for Scratchpad form
 */
function scratchpad_application_form(){
  global $user;
  if($user->uid){
    return array(
      'logout_to_see_form' => array(
        '#value' => '<p>'.t('Please logout to see the application form').'</p>'
      )
    );
  }
  return array(
    'about_you' => array(
      '#type' => 'fieldset',
      '#title' => t('About you'),
      '#collapsed' => FALSE,
      '#collapsible' => FALSE,
      'title' => array(
        '#title' => t('Title'),
        '#required' => TRUE,
        '#type' => 'textfield',
        '#description' => t('Mr/Mrs/Dr/Professor/Vicar')
      ),
      'fullname' => array(
        '#title' => t('Full name'),
        '#description' => t('At minimum this should be your first given name and your family name'),
        '#type' => 'textfield',
        '#required' => TRUE
      ),
      'email' => array(
        '#title' => t('Email'),
        '#description' => t('Your email address. This must be a valid email address, as your password will be sent to it'),
        '#type' => 'textfield',
        '#required' => TRUE
      ),
      'institution' => array(
        '#title' => t('Institution'),
        '#description' => t('The institution with which you are most closely associated. If retired or amateur, please insert "N/A"'),
        '#type' => 'textfield',
        '#required' => TRUE
      ),
      'country' => array(
        '#title' => t('Country'),
        '#description' => t('The country that you are based in.'),
        '#type' => 'select',
        '#required' => TRUE,
        '#options' => array(
          NULL => '-- Select --',
          'Afghanistan',
          'Albania',
          'Algeria',
          'American Samoa',
          'Andorra',
          'Angola',
          'Anguilla',
          'Antigua and Barbuda',
          'Argentina',
          'Armenia',
          'Aruba',
          'Australia',
          'Austria',
          'Azerbaijan',
          'Bahamas',
          'Bahrain',
          'Bangladesh',
          'Barbados',
          'Belarus',
          'Belgium',
          'Belize',
          'Benin',
          'Bermuda',
          'Bhutan',
          'Bolivia',
          'Bosnia and Herzegovina',
          'Botswana',
          'Brazil',
          'British Indian Ocean Territory',
          'British Virgin Islands',
          'Brunei',
          'Bulgaria',
          'Burkina Faso',
          'Burundi',
          'Cambodia',
          'Cameroon',
          'Canada',
          'Cape Verde Islands',
          'Cayman Islands',
          'Central African Republic',
          'Chad',
          'Chile',
          'China',
          'Colombia',
          'Comoros',
          'Congo',
          'Cook Islands',
          'Costa Rica',
          'Côte d\'Ivoire',
          'Croatia',
          'Cuba',
          'Cyprus',
          'Czech Republic',
          'dc:title',
          'Democratic Republic of the Congo',
          'Denmark',
          'Djibouti',
          'Dominica',
          'Dominican Republic',
          'East Timor',
          'Ecuador',
          'Egypt',
          'El Salvador',
          'Equatorial Guinea',
          'Eritrea',
          'Estonia',
          'Ethiopia',
          'Falkland Islands',
          'Fiji',
          'Finland',
          'France',
          'French Guiana',
          'French Polynesia',
          'Gabon',
          'Gambia',
          'Georgia',
          'Germany',
          'Ghana',
          'Gibraltar',
          'Greece',
          'Greenland',
          'Grenada',
          'Guadeloupe',
          'Guam',
          'Guatemala',
          'Guinea',
          'Guinea-Bissau',
          'Guyana',
          'Haiti',
          'Honduras',
          'Hungary',
          'Iceland',
          'India',
          'Indonesia',
          'Iran',
          'Iraq',
          'Ireland',
          'Israel',
          'Italy',
          'Jamaica',
          'Japan',
          'Jordan',
          'Kazakhstan',
          'Kenya',
          'Kiribati',
          'Korea, North',
          'Korea, South',
          'Kuwait',
          'Kyrgyzstan',
          'Laos',
          'Latvia',
          'Lebanon',
          'Lesotho',
          'Liberia',
          'Libya',
          'Liechtenstein',
          'Lithuania',
          'Luxembourg',
          'Macedonia',
          'Madagascar',
          'Malawi',
          'Malaysia',
          'Maldives',
          'Mali',
          'Malta',
          'Marshall Islands',
          'Martinique',
          'Mauritania',
          'Mauritius',
          'Mayotte',
          'Mexico',
          'Micronesia',
          'Moldova',
          'Monaco',
          'Mongolia',
          'Montserrat',
          'Morocco',
          'Mozambique',
          'Myanmar',
          'Namibia',
          'Nauru',
          'Nepal',
          'Netherlands',
          'Netherlands Antilles',
          'New Caledonia',
          'New Zealand',
          'Nicaragua',
          'Niger',
          'Nigeria',
          'Niue',
          'Norfolk Island',
          'Northern Mariana Islands',
          'Norway',
          'Oman',
          'Pakistan',
          'Palau',
          'Palestinian West Bank and Gaza',
          'Panama',
          'Papua New Guinea',
          'Paraguay',
          'Peru',
          'Philippines',
          'Pitcairn',
          'Poland',
          'Portugal',
          'Puerto Rico',
          'Qatar',
          'Réunion',
          'Romania',
          'Russia',
          'Rwanda',
          'Saint Helena',
          'Saint Kitts and Nevis',
          'Saint Lucia',
          'Saint Pierre and Miquelon',
          'Saint Vincent and the Grenadines',
          'Samoa',
          'San Marino',
          'São Tomé e Príncipe',
          'Saudi Arabia',
          'Senegal',
          'Serbia and Montenegro',
          'Seychelles',
          'Sierra Leone',
          'Singapore',
          'Slovakia',
          'Slovenia',
          'Solomon Islands',
          'Somalia',
          'South Africa',
          'Spain',
          'Sri Lanka',
          'Sudan',
          'Suriname',
          'Swaziland',
          'Sweden',
          'Switzerland',
          'Syria',
          'Taiwan',
          'Tajikistan',
          'Tanzania',
          'Thailand',
          'Togo',
          'Tokelau',
          'Tonga',
          'Trinidad and Tobago',
          'Tunisia',
          'Turkey',
          'Turkmenistan',
          'Turks and Caicos Islands',
          'Tuvalu',
          'Uganda',
          'Ukraine',
          'United Arab Emirates',
          'United Kingdom',
          'Uruguay',
          'USA',
          'U.S. Virgin Islands',
          'Uzbekistan',
          'Vanuatu',
          'Vatican State',
          'Venezuela',
          'Viet Nam',
          'Wallis and Futuna',
          'Yemen',
          'Zambia',
          'Zimbabwe'
        )
      )
    ),
    'about_scratchpad' => array(
      '#type' => 'fieldset',
      '#title' => t('About your Scratchpad'),
      '#collapsed' => FALSE,
      '#collapsible' => FALSE,
      'sitetitle' => array(
        '#title' => t('Site title'),
        '#description' => t('The site title will appear in the header section of your Scratchpad by default but can be removed later.'),
        '#type' => 'textfield',
        '#required' => TRUE
      ),
      'taxonomicscope' => array(
        '#title' => t('Taxonomic scope'),
        '#description' => t('The taxonomic group which best describes what taxa will be featured on your site. e.g. Insecta, Aves, Phthiraptera, Dragons'),
        '#type' => 'textfield',
        '#required' => TRUE
      ),
      'subdomain' => array(
        '#title' => t('Website domain'),
        '#description' => '<h3>Your domain name should be:</h3><ul><li><em>Short</em>. You can choose up to 34 characters, but short names and single words are easier to remember. We recommend no more than 20 characters.</li><li><em>Simple</em>. The subdomain name should be your website\'s name.</li><li><em>Legal</em>. Spaces and special characters are not allowed. Remember also that domain names are NOT case sensitive.</li></ul><h3>Using your own domain</h3><p>Users are more than welcome to purchase their own domain name for use with a Scratchpad.  All that is required is that the domain name is pointed at our server quartz.nhm.ac.uk which has the IP address 157.140.2.32.  If using your own domain name, the domain purchase should be completed before the Scratchpad application.</p><p>If you would like to use your own domain name, then enter the full domain name (without "http://").</p>',
        '#type' => 'textfield',
        '#required' => TRUE,
        '#field_suffix' => '.myspecies.info'
      ),
      'missionstatement' => array(
        '#title' => t('Mission statement'),
        '#description' => t('A description of your site, including what information a user of your site is likely to find there. By default the mission statement will appear on the front page of your site (although this can be changed later).'),
        '#type' => 'textarea',
        '#resizable' => FALSE,
        '#required' => TRUE
      ),
      'googleapi' => array(
        '#title' => t('Google API key'),
        '#description' => '<p>Go to the <a href="http://code.google.com/apis/maps/signup.html" target="_blank"><strong>Google API key signup page</strong></a>, check the terms &amp; conditions, enter the URL for your new Scratchpad ("xyz.myspecies.info") and click on the \'Generate API key\' button. Sign into an existing Google Account or create a new one. If you are now back on the Sign up page you will have to reenter the data to generate the API key. You will now see a page with several versions of the API key. Copy the first version under \'Your key is:\' and paste it in the field above.</p>',
        '#type' => 'textfield',
        '#required' => TRUE
      ),
      'clustrmaphtml' => array(
        '#title' => t('ClustrMap HTML'),
        '#description' => '<p>Note, this is not required. <a href="http://clustrmaps.com/admin/register.php" target="_blank"><strong>ClustrMap signup page</strong></a></p>',
        '#type' => 'textarea',
        '#resizable' => FALSE
      )
    ),
    'cmments_fieldset' => array(
      '#type' => 'fieldset',
      '#title' => t('Miscellaneous'),
      '#collapsed' => FALSE,
      '#collapsible' => FALSE,
      'comments' => array(
        '#type' => 'textarea',
        '#resizable' => FALSE,
        '#title' => t('Any other comments')
      )
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Get my Scratchpad')
    )
  );
}

/**
 * Theme for the above form
 */
function theme_scratchpad_application_form($form){
  if(isset($form['logout_to_see_form'])){
    return drupal_render_form($form['#id'], $form);
  }
  drupal_add_css(drupal_get_path('module', 'hostmaster_tweak') . '/hostmaster_tweak.css');
  $options = array();
  foreach(array(
    'about_you',
    'about_scratchpad'
  ) as $fieldset){
    foreach($form[$fieldset] as $key => $form_part){
      if(is_array($form_part) && isset($form_part['#description'])){
        $options[$key] = array(
          'cssSelect' => '#edit-' . $key,
          'text' => $form_part['#description'],
          'trigger' => array(
            'focus'
          ),
          'shadow' => TRUE,
          'width' => 350,
          'positions' => 'top'
        );
      }
    }
  }
  /*$options['bt_drupal_help_page'] = array(
    'cssSelect' => '#edit-sitetitle',
    'text' => 'Some shit here',
    'trigger' => array(
      'mouseover',
      'mouseout'
    ),
    'width' => 350
  );*/
  beautytips_add_beautytips($options);
  return drupal_render_form($form['#id'], $form);
}

/**
 * Validate function for the above form
 */
function scratchpad_application_form_validate(&$form, &$form_state){
  // Next check the Google Map API key is the correct length (or roughly)
  $gapi_length = strlen(trim($form_state['values']['googleapi']));
  if($gapi_length < 80 || $gapi_length > 90){
    form_set_error('googleapi', 'Your Google Map API key does not look to be valid, please ensure it is correct.');
  }
  // Next, check the domain name resolves to Quartz
  // If we have dots, then it is not a subdomain of myspecies.info - we need to check it.
  if(strpos($form_state['values']['subdomain'], '.')){
    // We have a new domain, lets check it's pointing at Quartz
    if(gethostbyname($form_state['values']['submitted_tree']['aboutscratchpad']['subdomain']) !== gethostbyname('quartz.nhm.ac.uk')){
      form_set_error('subdomain', 'The domain name you have entered is not pointing at our server.');
    }
  }else{
    $form_state['values']['subdomain'] .= '.myspecies.info';
  }
  // Also check that the domain name is unique.
  if(db_result(db_query("SELECT COUNT(*) FROM {node} n, {hosting_site} h WHERE n.nid = h.nid AND title = '%s' AND type = 'site' AND h.status != -2", $form_state['values']['subdomain']))){
    form_set_error('subdomain', t('That domain name is already taken'));
  }
}

/**
 * Submit function for the above form
 */
function scratchpad_application_form_submit(&$form, &$form_state){
  // Lets simply save the values to the table - easy!
  // If we have don't have dots, then we need to add .myspecies.info
  if(!strpos($form_state['values']['subdomain'], '.')){
    $form_state['values']['subdomain'] .= '.myspecies.info';
  }
  db_query("INSERT INTO {hostmaster_tweak} VALUES ('%s', '%s')", $form_state['values']['subdomain'], serialize($form_state['values']));
  // We submit the site-node-form!
  $node_form_state = array(
    'values' => array(
      'title' => $form_state['values']['subdomain'],
      'profile' => 691, // Scratchpad profile
      'platform' => 5, // Quartz Drupal 6 platform
      'op' => t('Save'),
      'uid' => 1
    )
  );
  module_load_include('pages.inc', 'node');
  $node = array(
    'type' => 'site'
  );
  // Submit as UID 1 (bad idea, but hey, only way).
  global $user;
  $user = user_load(array(
    'uid' => 1
  ));
  drupal_execute('site_node_form', $node_form_state, (object)$node);
  $user = drupal_anonymous_user();
  // Clear messages before forwarding.
  drupal_get_messages('error', TRUE);
  drupal_get_messages('status', TRUE);
  drupal_goto('apply/success');
}

/**
 * Callback for confirmation
 */
function scratchapd_application_success(){
  return '<div>
 <div class="boxtop">
  <div class="bc ctr"></div>
  <div class="bc ctl"></div>
 </div>
 <div class="boxcontent">
  <div class="boxtitle">
   <h1>Success</h1>
  </div>
  <div class="subboxcontent">
   <div style="padding:20px;"><h1>Thanks very much</h1><p>We have received your application.  You should receive an email from us soon.</p><p><a href="http://scratchpads.eu/">Scratchpads home</a></p></div>
 	</div>
 </div>
 <div class="boxbtm">
  <div class="bc cbr"></div>
  <div class="bc cbl"></div>
 </div>
</div>';
}

/**
 * Callback for the profile so that it can get the results
 */
function scratchapd_application_results($domain){
  $data = unserialize(db_result(db_query("SELECT data FROM {hostmaster_tweak} WHERE domain LIKE '%s'", $domain)));
  echo json_encode($data);
  exit();
}
