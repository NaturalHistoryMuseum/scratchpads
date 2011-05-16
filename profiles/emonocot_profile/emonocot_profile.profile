<?php
// Increase the timelimit, as this profile is SSSSLLLLOOOOWWWWW!
// Note, this is done outside any functions, to ensure that it is executed when
// the profile is loaded.
set_time_limit(300);
// Load the emonocot functions, and all the scratchpad_profile files.
require_once (dirname(__FILE__) . '/profile_functions.inc');
require_once (dirname(__FILE__) . '/../scratchpad_profile/profile_functions.inc');
require_once (dirname(__FILE__) . '/../scratchpad_profile/scratchpad_profile.profile');
require_once (dirname(__FILE__) . '/../scratchpad_profile/profile_forms.inc');

/**
 * The emonocot profile.
 * 
 * Profile name: "emonocot".
 * 
 * Note, this profile was renamed to avoid a conflict with the module of the
 * same name.
 */
/**
 * Details about this module
 * 
 * Implementation of hook_profile_details
 */
function emonocot_profile_profile_details(){
  return array(
    'name' => 'eMonocot Scratchpad',
    'description' => 'Provides a customised Scratchpad for the eMonocot project.'
  );
}

/**
 * Modules that this profile would like installing
 * 
 * Implementation of hook_profile_modules
 */
function emonocot_profile_profile_modules(){
<<<<<<< HEAD
  return array(
    //eM indicates modules that are ordinarily part of the Scratchpads that are disabled under eMonocot
    // Core - optional
    //eM 'blog',
    //eM 'color',
    'comment',
    'contact',
    'locale',
    'dblog',
    'help',
    'menu',
    'openid',
    'path',
    //'poll',
    'search',
    'taxonomy',
    'trigger',
    'upload',
    'forum',
    'translation',
    'tracker',
    // No requirements/Other
    'biblio',
    'boost',
    'node_import',
    'creativecommons_lite',
    //eM 'simplenews',
    'advanced_help',
    'auto_nodetitle',
    'checkbox_validate',
    'clone',
    'modalframe',
    'globalredirect',
    'legal',
    'path_redirect',
    'pathauto',
    'quote',
    'statistics',
    'roleassign',
    'search_files',
    'thickbox',
    'token',
    'vertical_tabs',
    'weight',
    'jstools',
    'wysiwyg',
    'print',
    'imce',
    'imce_wysiwyg',
    'ajax',
    // Spam control
    'mollom',
    //'hashcash', - re-enable later
    // JQuery
    'jquery_update',
    'jquery_ui',
    // CCK
    'content',
    'number',
    'optionwidgets',
    'text',
    'date_api',
    'date',
    'date_popup',
    'nodereference',
    'date_timezone',
    'fieldgroup',
    'date_php4',
    'nodereferrer',
    'noderelationships',
    // Image
    'imagecache',
    'imageapi',
    'imageapi_imagemagick',
    'filefield',
    'imagefield',
    'lightbox2',
    'plupload',
    // Location
    'location',
    'gmap',
    'gmap_location',
    'location_user',
    'location_node',
    // Messaging
    'messaging',
    'notifications',
    'notifications_lite',
    'notifications_content',
    'notifications_autosubscribe',
    'messaging_mail',
    // Organic Groups
    'og',
    'og_access',
    'og_user_roles',
    'og_views',
    'og_notifications',
    'og_forum',
    // Views
    'views',
    'views_ui',
    'views_xml',
    // Content profile
    'content_profile',
    'content_profile_registration',
    // Webforms
    //eM 'webform',
    // EDIT
    'countriesmap',
    'citation',
    'backup',
    'batax',
    'ispecies',
    'bhl',
    'darwincore',
    'fixperms',
    'flickr',
    'gbifmap',
    'googlescholar',
    'lowername',
    'mado',
    'ncbi',
    'node_term_edit',
    'autotag',
    'nbnmap',
    'foundation',
    'content_taxonomy',
    'content_taxonomy_autocomplete',
    //'leftandright',
    'taxtab',
    'tinytax',
    'morphbank',
    'view_sort',
    'wikipedia',
    'yahooimages',
    'scratchpadify',
    'scratchpadify_help',
    'tablesorter',
    'nagger',
    //'bio_image',  DISABLED. REMOVE SOON
    'itis_term',
    'term_node',
    'spm',
    'taxonomy_tree',
    'ahah_action',
    'matrix_editor',
    //'nexus',  //Character project
    //'tree',
    'femail',
    'term_node',
    'scratchpad',
    'tui',
    'classification_import',
    'csv_import',
    'tcs_import',
    'eol_import',
    //eM 'publication',
    'nodereferencethumb',
    'aggregator',
    'uwho_client',
    'remote_issue_tab',
    'comment_upload',
    'commentmail',
    //eMonocot
    'emonocot',
=======
  $modules_to_disable = array(
    'blog',
    'color',
    'simplenews',
    'poll',
    'webform',
    'nexus',
    'tree',
    'publication',
    'beautytips',
    'slickgrid',
    'views_batch_page',
    'ajax_load',
    'editor_views',
    'scratchpad_slickgrid'
  );
  $emonocot_modules = array(
    'emonocot'
>>>>>>> f85baf57b5e8331a9dad5bbe1bfa06709116c6ca
  );
  $scratchpad_modules = scratchpad_profile_profile_modules();
  foreach($scratchpad_modules as $module){
    if(!in_array($module, $modules_to_disable)){
      $emonocot_modules[] = $module;
    }
  }
  return $emonocot_modules;
}

/**
 * Implementation of hook_profile_task_list
 */
function emonocot_profile_profile_task_list(){
  return scratchpad_profile_profile_task_list();
}

/**
 * Pull out all the functions into the following numbered functions so that the
 * scratchpad_training module can reuse them.
 */
function emonocot_profile_profile_tasks_1(){
  //Set variable to say this is an eMonocot Scratchpad
  variable_set('emonocot_is_emonocot_site', TRUE);
  scratchpad_profile_profile_tasks_1();
  // Rename "Page" content type to "Static Page"
  db_query("UPDATE {node_type} SET name = 'Static Page' WHERE type = 'page'");
}

function emonocot_profile_profile_tasks_2(){
  scratchpad_profile_set_settings();
  emonocot_profile_set_conditions();
  scratchpad_profile_set_issue_feed('eMonocot Scratchpads Issue Queue', 'http://dev.scratchpads.eu/project/issues/rss/emonocot');
  scratchpad_profile_profile_set_perms();
  emonocot_profile_set_blocks();
  emonocot_profile_tweak_menu();
}
/**
 * Code for the tasks
 * 
 * Implementation of hook_profile_tasks
 */
function emonocot_profile_profile_tasks(&$task, $url){
  if($task == 'profile'){
    $task = 'personal';
    emonocot_profile_profile_tasks_1();
  }
  $data = array();
  $output = scratchpad_profile_do_tasks($task, $url, $data);
  if($output){
    return $output;
  }
  if($task == 'scratchpadcleanup'){
    emonocot_profile_profile_tasks_2();
    // The above function moves the UID 1 to UID 2.  Now we can update that user
    // if we're installing user Aegir.  This means the next funciton will email
    // the right person! WOOT!
    if(is_array($data) && count($data)){
      db_query("UPDATE {users} SET mail = '%s', name = '%s' WHERE uid = 2", $data['client_email'], $data['fullname']);
    }
    scratchpad_profile_set_theme('emonocot_sp');
    $task = 'profile-finished';
  }
<<<<<<< HEAD
}

/**
 * Define form used by clustrmap installer task
 */
function emonocot_personal($form_state, $url){
  $user = user_load(array(
    'uid' => 1
  )); // Still uid 1 as we've not moved it
  $name_parts = explode(" ", $user->name);
  $familyname = array_pop($name_parts);
  $givenname = implode(" ", $name_parts);
  return array(
    '#action' => $url,
    '#redirect' => FALSE,
    'title' => array(
      '#required' => TRUE,
      '#type' => 'textfield',
      '#title' => st('Title')
    ),
    '#action' => $url,
    '#redirect' => FALSE,
    'given' => array(
      '#required' => TRUE,
      '#type' => 'textfield',
      '#title' => st('Given Name(s)'),
      '#default_value' => $givenname
    ),
    'family' => array(
      '#required' => TRUE,
      '#type' => 'textfield',
      '#title' => st('Family Name'),
      '#default_value' => $familyname
    ),
    'institution' => array(
      '#required' => FALSE,
      '#type' => 'textfield',
      '#title' => st('Institution')
    ),
    'expertise' => array(
      '#required' => FALSE,
      '#type' => 'textfield',
      '#title' => st('Area of Taxonomic expertise')
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => st('Save and continue')
    )
  );
}

/**
 * Save the form shit
 */
function emonocot_personal_submit($form, &$form_state){
  variable_set('personal_submitted', TRUE);
  $node = new stdClass();
  $node->type = 'profile';
  $node->uid = 2;
  $node->field_title = array(
    array(
      'value' => $form_state['values']['title']
    )
  );
  $node->field_givennames = array(
    array(
      'value' => $form_state['values']['given']
    )
  );
  $node->field_familyname = array(
    array(
      'value' => $form_state['values']['family']
    )
  );
  $node->field_institution = array(
    array(
      'value' => $form_state['values']['institution']
    )
  );
  $node->field_taxonomicinterest = array(
    array(
      'value' => $form_state['values']['expertise']
    )
  );
  $node->title = "{$form_state['values']['title']} {$form_state['values']['given']} {$form_state['values']['family']}";
  $node->auto_nodetitle_applied = TRUE;
  node_save($node);
}

/**
 * Define form used by gmapkey installer task
 */
function emonocot_gmapkey($form_state, $url){
  return array(
    '#action' => $url,
    '#redirect' => FALSE,
    'gmapkey' => array(
      '#type' => 'textfield',
      '#title' => st('Google Maps API Key'),
      '#default_value' => '',
      '#required' => TRUE,
      '#description' => st('A Google maps API key is required for your Scratchpad to function properly.  One can be obtained from <a href="http://code.google.com/apis/maps/signup.html">http://code.google.com/apis/maps/signup.html</a>.')
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => st('Save and continue')
    )
  );
}

/**
 * Save the form shit
 */
function emonocot_gmapkey_submit($form, &$form_state){
  // Set the google API key
  variable_set('googlemap_api_key', $form_state['values']['gmapkey']);
  variable_set('gmap_default', array(
    'width' => '100%',
    'height' => '400px',
    'latlong' => '56,11',
    'zoom' => 3,
    'maxzoom' => 14,
    'styles' => array(
      'line_default' => array(
        '0000ff',
        5,
        45,
        '',
        ''
      ),
      'poly_default' => array(
        '000000',
        3,
        25,
        'ff0000',
        45
      )
    ),
    'controltype' => 'Large',
    'mtc' => 'standard',
    'maptype' => 'Hybrid',
    'baselayers' => array(
      'Map' => 1,
      'Satellite' => 0,
      'Hybrid' => 1,
      'Physical' => 1
    ),
    'behavior' => array(
      'locpick' => '',
      'nodrag' => 0,
      'nokeyboard' => 1,
      'nomousezoom' => 1,
      'nocontzoom' => 0,
      'autozoom' => 1,
      'dynmarkers' => 1,
      'overview' => 0,
      'collapsehack' => 1,
      'scale' => 0
    ),
    'markermode' => 0,
    'line_colors' => array(
      '#00cc00',
      '#ff0000',
      '#0000ff'
    )
  ));
}

/**
 * Define form used by clustrmap installer task
 */
function emonocot_clustrmap($form_state, $url){
  return array(
    '#action' => $url,
    '#redirect' => FALSE,
    'clustrmap' => array(
      '#type' => 'textarea',
      '#title' => st('ClustrMap HTML Code'),
      '#description' => st('A ClustrMap for your site can be obtained from <a href="http://clustrmaps.com/">http://clustrmaps.com/</a>.')
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => st('Save and continue')
    )
  );
}

/**
 * Save the form shit
 */
function emonocot_clustrmap_submit($form, &$form_state){
  // Create a block with the required code in it.
  if(trim($form_state['values']['clustrmap']) != ''){
    $box = array(
      'body' => $form_state['values']['clustrmap'],
      'info' => 'ClustrMap',
      'title' => ''
    );
    emonocot_block_add($box);
  }
  variable_set('clustrmap_submitted', TRUE);
}

/**
 * Define form used by clustrmap installer task
 */
function emonocot_mission($form_state, $url){
  return array(
    '#action' => $url,
    '#redirect' => FALSE,
    'slogan' => array(
      '#type' => 'textfield',
      '#title' => st('Slogan'),
      '#description' => st('Displayed along side your site title')
    ),
    'mission' => array(
      '#type' => 'textarea',
      '#title' => st('Mission statement'),
      '#description' => st('Your site\'s mission statement.  This should be used to describe your site to naive users')
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => st('Save and continue')
    )
  );
}

/**
 * Save the form shit
 */
function emonocot_mission_submit($form, &$form_state){
  // Create a block with the required code in it.
  if(trim($form_state['values']['mission']) != ''){
    variable_set('site_mission', $form_state['values']['mission']);
  }
  if(trim($form_state['values']['slogan']) != ''){
    variable_set('site_slogan', $form_state['values']['slogan']);
  }
  variable_set('mission_submitted', TRUE);
}

/**
 * Add a box/block and display it
 */
function emonocot_block_add($box){
  db_query("INSERT INTO {boxes} (body, info, format) VALUES ('%s', '%s', 1)", $box['body'], $box['info']);
  $delta = db_last_insert_id('boxes', 'bid');
  foreach(list_themes() as $theme){
    db_query("INSERT INTO {blocks} (module,delta,theme,status,region,cache,title) VALUES ('block', %d, '%s', 1, 'left', %d, '%s')", $delta, $theme->name, BLOCK_NO_CACHE, $box['title']);
  }
  return;
=======
>>>>>>> f85baf57b5e8331a9dad5bbe1bfa06709116c6ca
}