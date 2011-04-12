<?php
// Increase the timelimit, as this profile is SSSSLLLLOOOOWWWWW!
// Note, this is done outside any functions, to ensure that it is executed when
// the profile is loaded.
set_time_limit(300);

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
    'emonocot'
  );
}

/**
 * Additional taks
 * 
 * Implementation of hook_profile_task_list
 */
function emonocot_profile_profile_task_list(){
  return array(
    'personal' => st('Personal information'),
    'gmapkey' => st('Google Maps API Key'),
    'clustrmap' => st('ClustrMaps HTML Code'),
    'mission' => st('Mission statement')
  );
}

/**
 * Pull out all the functions into the following numbered functions so that the
 * scratchpad_training module can reuse them.
 */
function emonocot_profile_profile_tasks_1(){
  //Set variable to say this is an eMonocot Scratchpad
  variable_set('emonocot_is_emonocot_site', TRUE);
  
  // Insert default user-defined node types into the database. For a complete
  // list of available node type attributes, refer to the node type API
  // documentation at: http://api.drupal.org/api/HEAD/function/hook_node_info.
  $types = array(
    array(
      'type' => 'page',
      'name' => st('Static Page'),
      'module' => 'node',
      'description' => st("A <em>page</em>, similar in form to a <em>story</em>, is a simple method for creating and displaying information that rarely changes, such as an \"About us\" section of a website. By default, a <em>page</em> entry does not allow visitor comments and is not featured on the site's initial home page."),
      'custom' => TRUE,
      'modified' => TRUE,
      'locked' => FALSE,
      'help' => '',
      'min_word_count' => ''
    ),
    array(
      'type' => 'group',
      'name' => st('Group'),
      'module' => 'node',
      'description' => st("A <em>group</em> is used for grouping content on your Scratchpad.  Access to content can be restricted by adding it to a group."),
      'custon' => TRUE,
      'modified' => TRUE,
      'locked' => TRUE,
      'help' => '',
      'min_word_count' => ''
    )
  );
  foreach($types as $type){
    $type = (object)_node_type_set_defaults($type);
    node_type_save($type);
  }
  // Set the "group" type, to be a group
  variable_set('og_content_type_usage_group', 'group');
  // Add the default profile fields to the content type profile
  emonocot_profile_profile_install_profile();
  /*
  db_query("INSERT INTO {profile_fields} (title, name, category, type, weight, required, register, visibility,autocomplete) VALUES 
    ('".st('Title')."','profile_title','".st('Personal information')."','textfield',0,1,1,3,1),
    ('".st('Given name(s)')."','profile_givennames','".st('Personal information')."','textfield',1,1,1,3,0), 
    ('".st('Family name')."','profile_familyname','".st('Personal information')."','textfield',2,1,1,3,0), 
    ('".st('Institution')."','profile_institution','".st('Personal information')."','textfield',3,0,1,3,0),
    ('".st('Area of Taxonomic Interest')."','profile_taxonomy','".st('Personal information')."','textfield',4,0,1,3,1)");
  */
}

/**
 * Install the profile.
 */
function emonocot_profile_profile_install_profile(){
  // Load the file for doing the stuff!
  module_load_include('inc', 'content', 'includes/content.crud');
  fieldgroup_save_group('profile', array(
    'label' => 'Personal Information',
    'group_name' => 'group_personal',
    'group_type' => 'standard'
  ));
  $fields = array(
    array(
      'label' => 'Title',
      'field_name' => 'field_title',
      'type' => 'text',
      'widget_type' => 'text_textfield',
      'type_name' => 'profile',
      'weight' => 0,
      'required' => 1
    ),
    array(
      'label' => 'Given name(s)',
      'field_name' => 'field_givennames',
      'type' => 'text',
      'widget_type' => 'text_textfield',
      'type_name' => 'profile',
      'weight' => 1,
      'required' => 1
    ),
    array(
      'label' => 'Family name',
      'field_name' => 'field_familyname',
      'type' => 'text',
      'widget_type' => 'text_textfield',
      'type_name' => 'profile',
      'weight' => 2,
      'required' => 1
    ),
    array(
      'label' => 'Institution',
      'field_name' => 'field_institution',
      'type' => 'text',
      'widget_type' => 'text_textfield',
      'type_name' => 'profile',
      'weight' => 3
    ),
    array(
      'label' => 'Area of Taxonomic Interest',
      'field_name' => 'field_taxonomicinterest',
      'type' => 'text',
      'widget_type' => 'text_textfield',
      'type_name' => 'profile',
      'weight' => 4
    )
  );
  foreach($fields as $field){
    content_field_instance_create($field);
    db_query("INSERT INTO {content_group_fields} (type_name, group_name, field_name) VALUES ('profile','group_personal','%s')", $field['field_name']);
  }
  variable_set('content_profile_profile', array(
    'weight' => 0,
    'user_display' => 'full',
    'edit_link' => 1,
    'edit_tab' => 'sub',
    'add_link' => 1,
    'registration_use' => 1,
    'admin_user_create_use' => 1,
    'registration_hide' => array(
      'other'
    )
  ));
  db_query("UPDATE {node_type} SET has_body = 0 WHERE type = 'profile'");
  variable_set('content_profile_use_profile', TRUE);
  variable_set('ant_pattern_profile', '[field_title-formatted] [field_givennames-formatted] [field_familyname-formatted]');
  variable_set('ant_php_profile', 0);
  variable_set('ant_profile', 1);
  variable_set('node_options_profile', array(
    'status'
  ));
}

function emonocot_profile_profile_tasks_2(){
  // Comment upload module - enable for forums
  variable_set('comment_upload_forum', 1);
  // Remote Issues Tab
  $feed = array(
    'title' => 'eMonocot Scratchpads Issue Queue',
    'url' => 'http://dev.scratchpads.eu/project/issues/rss/emonocot',
    'refresh' => 900
  );
  aggregator_save_feed($feed);
  // Set the variables
  // Note, $feed is not passed by reference to aggregator_save_feed (Possible 
  // bug with the Drupal code there), so we have to use db_last_insert_id.
  $fid = db_last_insert_id('aggregator_feed', 'fid');
  variable_set('remote_issue_tab_feed', $fid);
  variable_set('remote_issue_tab_uwho_client_key', '58d972f86f9d963837bebcd8d4b46d39');
  variable_set('remote_issue_tab_uwho_url', 'http://dev.scratchpads.eu/uwho');
  variable_set('remote_issue_tab_footer', '<h1>Help</h1><h3>Not sure what you are doing, try the <a href="http://scratchpads.eu/help">Scratchpad Help Page</a>.</h3>');
  variable_set('remote_issues_tab_roles', array(
    5
  ));

  // Finally, we feed the feed so the Issues tab isn't empty.
  $feed = aggregator_feed_load($fid);
  aggregator_refresh($feed);
  // Og_user_roles  
  variable_set('og_user_roles_roles_group', array(
    3 => 3,
    4 => 4
  ));
  variable_set('ogur_assign_founder_group', 1);
  variable_set('ogur_founder_value_group', 4);
  variable_set('og_user_roles_admingrouprole_value', 4);
  variable_set('og_user_roles_assign_admingrouprole', 1);
  // Finally, do the following
  variable_set('date_default_timezone_name', 'Europe/London');
  // The following really shouldn't be necesary, but a module is being silly
  variable_del('node_access_needs_rebuild');
  // Set statistics module up to count node accesses.
  variable_set('statistics_count_content_views', 1);

  // Alter the USER to be UID 2, and create a user with UID 1
  db_query("UPDATE {users} SET uid = 2 WHERE uid = 1");
  db_query("INSERT INTO {users} (uid,name,pass,status,login) VALUES (1,'Scratchpad Team','no-direct-login',1,NOW())");
  // Update the autoincrement (not sure why this breaks, silly MySQL).
  db_query('ALTER TABLE {users} AUTO_INCREMENT = 3');
  db_query("INSERT INTO {users_roles} (uid,rid) VALUES (2,5)"); // Tsk, adding role when none exist!
  db_query("UPDATE {url_alias} SET src = 'user/2' WHERE src = 'user/1'");
  // Set page cache to "Normal"
  variable_set('cache', 1);
  // Delete the filter HTML filter, and update Full to be the default
  db_query("DELETE FROM {filters} WHERE format = 1");
  db_query("DELETE FROM {filter_formats} WHERE format = 1");
  db_query("UPDATE {filters} SET format = 1 WHERE format = 2");
  db_query("UPDATE {filter_formats} SET format = 1 WHERE format = 2");
  db_query("INSERT INTO {filter_formats} (format, name, cache) VALUES (2, 'Full HTML (script allowed)', 1)");
  db_query("INSERT INTO {filters} (format,module,delta,weight) VALUES (1,'biblio',0,10),(1,'gmap',0,10),(1,'quote',0,10),(1,'scratchpadify',0,10)");
  db_query("INSERT INTO {filters} (format, module, delta, weight) VALUES (2,'gmap',0,10),(2,'quote',0,10),(2,'biblio',0,10),(2,'filter',2,0),(2,'filter',1,1),(2,'filter',3,10)");
  // Insert conditions into the Scratchpad
  $conditions = '<ol>
<li><b>ACCEPTANCE OF TERMS</b> This agreement is between eMonocot, and you and your agents (collectively “you”) regarding the use of this website (the "Site"). By using the Site, you agree to the Terms and Conditions in this document.</li>
<li><b>OWNERSHIP OF SITE</b> The text, graphics, sound and software (collectively "Content") on this Site is owned by you and your agents and you bare sole and ultimate responsibility for this Content. eMonocot supports the computer hardware infrastructure and software content management system that provides access to this Content.</li>
<li><b>ACCESS TO SERVICES AND TERMINATION OF ACCESS</b> You are responsible for all activity logged through your user account and for the activity of other persons or entity you grant access to this Site. You agree to notify eMonocot immediately if you become aware of any unauthorised use and you agree that eMonocot may terminate your access privileges and remove Content without notice if eMonocot believes you have violated any provision of this Agreement. You agree that termination of your access to the Site shall not result in any liability or other obligation of eMonocot to you or any third party in connection with such termination.  An archive copy of your content at the time of termination will be kept and made available to you on request.</li>
<li><b>CONTENT</b> You agree to be bound by the Joint Academic Network (JANET) Acceptable Use Guidelines (<a href="http://www.ja.net/company/policies/aup.html">http://www.ja.net/company/policies/aup.html</a>). In summary this document states that all Content placed on the Site must be legal, decent and truthful. Through you or your agent’s use of the Site, you represent and warrant that you have all the rights necessary to receive, use, transmit and disclose all data that you use in any way with the Site. You agree and acknowledge that you are solely responsible for any liabilities, fines, or penalties occasioned by any such violations or lack of rights and that you are solely responsible for the accuracy and adequacy of information and data furnished on the Site.</li>
<li><b>TAKE DOWN POLICY</b> If you are a rights owner and are concerned that you have found material on a Site and have not given permission for its use, please contact us in writing (scratchpad@nhm.ac.uk) providing:
  <ul>
    <li>Your contact details</li>
    <li>The full bibliographic details of the material</li>
    <li>The Site address where you found the material</li>
    <li>A statement that, under penalty of perjury, you are the rights owner or are authorised to act for the rights owner</li>
  </ul>
</li>
<li><b>DISCLAIMER OF WARRANTIES</b> The use of the Site is solely at your own risk. The site is provided on an "as is" and "as available" basis and eMonocot expressly disclaims all warranties of any kind with respect to the site, whether express or implied. eMonocot makes no warranty that the access to the site and/or Content therein will be uninterrupted or secure. Your sole and exclusive remedy with respect to any defect in or dissatisfaction with the Site is to cease using the Site.</li>
<li><b>LIMITATION OF LIABILITY</b> You understand and agree that eMonocot shall not be liable for any direct, indirect, incidental, special, consequential, or exemplary damages resulting from any matter related to your or other persons use of the site.</li>
<li><b>DISCLAIMER OF CONTENT</b> You understand and acknowledge that eMonocot assumes no responsibility to screen or review Content and that eMonocot shall have the right, but not the obligation, in its sole discretion to review, refuse, monitor, or remove any Content. ViBRANT expressly disclaims all responsibility or liability to you or any other person or entity for the Content and you acknowledge and agree that you assume all risk associated with the use of any and all Content.</li>
</ol>';
  $extras = array(
    'extras-1' => '',
    'extras-2' => '',
    'extras-3' => '',
    'extras-4' => '',
    'extras-5' => ''
  );
  db_query("INSERT INTO {legal_conditions} (conditions, date, extras) VALUES ('%s',NOW(),'%s')", $conditions, serialize($extras));
  variable_set('legal_display', 2);
  variable_set('lightbox2_display_image_size', 'preview');
  variable_set('lightbox2_trigger_image_size', array(
    'thumbnail' => 'thumbnail'
  ));
  variable_set('plupload_import_field_type', 'image:::field_imagefield');
  variable_set('lightbox2_disable_nested_galleries', 1);
  variable_set('lightbox2_lite', FALSE);
  variable_set('lightbox2_image_node', 2);
  // Messaging
  // Set the default send method.
  variable_set('messaging_default_method', 'mail');
  variable_set('notifications_default_send_interval', 0);
  variable_get('notifications_send_immediate', 1);
  // Location
  variable_set('location_default_country', '');
  variable_set('location_usegmap', 1);
  variable_set('location_settings_user', array(
    'multiple' => array(
      'min' => 1,
      'max' => 1,
      'add' => 1
    ),
    'form' => array(
      'weight' => 0,
      'collapsible' => 1,
      'collapsed' => 0,
      'fields' => array(
        'name' => array(
          'collect' => 1,
          'weight' => 2
        ),
        'street' => array(
          'collect' => 1,
          'weight' => 4
        ),
        'additional' => array(
          'collect' => 1,
          'weight' => 6
        ),
        'country' => array(
          'collect' => 1,
          'weight' => 8
        )
      ),
      'register' => 1
    ),
    'display' => array(
      'weight' => 0
    )
  ));
  // Contact form
  db_query("INSERT INTO {contact} (category, recipients, selected) SELECT '%s',mail,1 FROM users WHERE uid = 2", st('Website feedback'));
  // Mollom
  variable_set('mollom_public_key', 'ebe52536e33b662497bad0f451187161');
  variable_set('mollom_private_key', 'f86117722dcd1d12aa1a1065edfb0fb2');
  // Performance
  variable_set('preprocess_css', 1);
  variable_set('preprocess_js', 1);
  // Set various blocks to be visible
  db_query("DELETE FROM {blocks} WHERE module = 'search' AND delta = 0");
  db_query("DELETE FROM {blocks} WHERE module = 'system' AND delta = 0");
  foreach(list_themes() as $theme){
    //Changed for eMonocot profile
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'scratchpadify' AND delta = 1", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight,title) VALUES ('scratchpadify',1,'%s','left',1,-10,'Common Tasks')", $theme->name);
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'emonocot' AND delta = 0", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight) VALUES ('emonocot',0,'%s','left',1,20)", $theme->name);
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'user' AND delta = 1", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight) VALUES ('user', 1, '%s', 'left',1,-11)", $theme->name);
    //Not changed for eMonocot profile
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'scratchpadify' AND delta = 4", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status) VALUES ('scratchpadify',4,'%s','header',1)", $theme->name);
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'search' AND delta = 0", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight, title) VALUES ('search',0,'%s','left',1,-200,'<none>')", $theme->name);
    //eMonocot jiggling
    db_query("UPDATE {blocks} SET weight = -30 WHERE theme = '%s' AND module = 'scratchpadify' AND delta = 1 ", $theme->name);
  }
  //eM Play with blocks
  db_query("INSERT INTO {blocks_roles} (module, delta, rid) VALUES ('user', 1, 2)");
  db_query("UPDATE {blocks} SET title = 'Search' WHERE module = 'search' AND delta = 0");
  // Hide the theme search form
  variable_set('theme_settings', array(
    'toggle_search' => 0
  ));
  // Remove the "Biblio" & "Taskguide" links from the navigation menu - they
  // look ugly.    
  $links = array(
    'biblio' => array(
      'menu_name' => 'primary-links',
      'link_title' => st('Bibliography'),
      'module' => 'system',
      'weight' => -50
    ),
    'contact' => array(
      'menu_name' => 'primary-links',
      'link_title' => st('Contact us'),
      'module' => 'system',
      'hidden' => 0,
      'weight' => -48
    ),
    'forum' => array(
      'menu_name' => 'primary-links',
      'module' => 'system',
      'weight' => -49
    ),
    'logout' => array(
      'menu_name' => 'primary-links',
      'link_title' => st('Log Out'),
      'module' => 'system'
    ),
    'map/node' => array(
      'hidden' => 1,
      'module' => 'system'
    ),
    'map/node/load/%/%' => array(
      'hidden' => 1,
      'module' => 'system'
    ),
    'map/user' => array(
      'hidden' => 1,
      'module' => 'system'
    ),
    'map/user/load' => array(
      'hidden' => 1,
      'module' => 'system'
    ),
    'node/add' => array(
      'module' => 'system',
      'hidden' => 1
    ),
    'admin' => array(
      'plid' => array_pop(db_fetch_array(db_query("SELECT mlid FROM {menu_links} WHERE link_path = 'basicadmin'"))),
      'module' => 'system',
      'link_title' => st('Advanced')
    )
  );
  foreach($links as $path => $changes){
    $item = menu_get_item($path);
    $item['link_path'] = $path;
    $item['link_title'] = $item['title'];
    // FFS, Drupal doesn't have a function to return the MLID for an item. WTF!
    $item['mlid'] = array_pop(db_fetch_array(db_query("SELECT mlid FROM {menu_links} WHERE link_path = '%s'", $path)));
    foreach($changes as $key => $value){
      $item[$key] = $value;
    }
    $item['customized'] = 1;
    menu_link_save($item);
  }
  // Create a link to help
  $link = array(
    'menu_name' => 'primary-links',
    'weight' => -45,
    'link_path' => 'admin/advanced_help/scratchpadify_help',
    'link_title' => 'Help'
  );
  menu_link_save($link);
  //eM link
  $em_link = array(
    'menu_name' => 'primary-links',
    'link_title' => st('Log In'),
    'weight' => 40,
    'link_path' => 'user/login'
  );
  menu_link_save($em_link);
  emonocot_profile_profile_set_perms();
  // Role assign settings
  variable_set('roleassign_roles', array(
    3 => 3,
    4 => 4,
    5 => 5
  ));
  // Forum
  variable_set('forum_order', 2);
  $term = array(
    'vid' => variable_get('forum_nav_vocabulary', 0),
    'name' => st('General'),
    'description' => st('A forum for posts related, or not, to this site, and which do not fit in any other forum')
  );
  taxonomy_save_term($term);
  // Og_forum settings
  variable_set('forum_auto_public', 1);
  variable_set('forum_default_container_yn', 1);
  $term = array(
    'vid' => variable_get('forum_nav_vocabulary', 0),
    'name' => st('Groups'),
    'description' => 'Forums associated with this site\'s groups.',
    'weight' => 1000
  );
  taxonomy_save_term($term);
  $containers = variable_get('forum_containers', array());
  $containers[] = $term['tid'];
  variable_set('forum_containers', $containers);
  variable_set('forum_default_container', $term['tid']);
  // Users settings
  variable_set('user_register', 2);
  variable_set('user_signatures', 1);
  variable_set('user_pictures', 1);
  // Change the newsletter name (Set to Drupal newsletter, as the site name
  // wasn't known when the module was installed).
  db_query("UPDATE {term_data} SET name = '%s' WHERE vid = '%d'", variable_get('site_name', 'Drupal') . " " . st('newsletter'), variable_get('simplenews_vid', 0));
  // Error level
  variable_set('error_level', '0');
  // File uploads settings
  foreach(array(
    '3',
    '4',
    '5',
    'default'
  ) as $rid){
    variable_set('upload_usersize_' . $rid, '2000');
    variable_set('upload_uploadsize_' . $rid, '20');
    variable_set('upload_extensions_' . $rid, 'jpg jpeg gif png txt doc xls pdf ppt pps odt ods odp');
  }
  // Set Jquery_update to use no compressions - VERY BAD, but necessary due to
  // the fact that the module is BORKED.
  variable_set('jquery_update_compression_type', 'none');
  // Setup TinyMCE and WYSIWYG
  $tinymce_settings = array(
    'default' => 1,
    'user_choose' => 1,
    'show_toggle' => 1,
    'theme' => 'advanced',
    'language' => 'en',
    'buttons' => array(
      'default' => array(
        'bold' => 1,
        'italic' => 1,
        'underline' => 1,
        'strikethrough' => 1,
        'justifyleft' => 1,
        'justifycenter' => 1,
        'justifyfull' => 1,
        'justifyright' => 1,
        'bullist' => 1,
        'numlist' => 1,
        'outdent' => 1,
        'indent' => 1,
        'undo' => 1,
        'link' => 1,
        'unlink' => 1,
        'anchor' => 1,
        'image' => 1,
        'sup' => 1,
        'sub' => 1,
        'blockquote' => 1,
        'code' => 1,
        'hr' => 1,
        'cut' => 1,
        'copy' => 1,
        'paste' => 1
      ),
      'advimage' => array(
        'advimage' => 1
      ),
      'inlinepopups' => array(
        'inlinepopups' => 1
      ),
      'font' => array(
        'formatselect' => 1
      ),
      'paste' => array(
        'pasteword' => 1,
        'pastetext' => 1
      ),
      'table' => array(
        'tablecontrols' => 1
      ),
      'safari' => array(
        'safari' => 1
      ),
      'imce' => array(
        'imce' => 1
      ),
      'drupal' => array(
        'break' => 1
      )
    ),
    'toolbar_loc' => 'top',
    'toolbar_align' => 'left',
    'path_loc' => 'bottom',
    'resizing' => 1,
    'verify_html' => 1,
    'preformatted' => 0,
    'convert_fonts_to_spans' => 1,
    'remove_linebreaks' => 0,
    'apply_source_formatting' => 0,
    'paste_auto_cleanup_on_paste' => 1,
    'block_formats' => 'p,pre,h1,h2,h3,h4,h5,h6',
    'css_settings' => 'theme'
  );
  db_query("INSERT INTO {wysiwyg} (format, editor, settings) VALUES (2, 'tinymce', '%s'),(1, 'tinymce', '%s')", serialize($tinymce_settings), serialize($tinymce_settings));
  // Setup IMCE
  variable_set('imce_profiles', array(
    1 => array(
      'name' => st('All users IMCE'),
      'filesize' => 20,
      'quota' => 200,
      'tuquota' => 0,
      'extensions' => 'gif png jpg jpeg pdf doc xls txt',
      'dimensions' => '800x600',
      'filenum' => 1,
      'directories' => array(
        array(
          'name' => 'u%uid',
          'subnav' => 1,
          'browse' => 1,
          'upload' => 1,
          'thumb' => 1,
          'delete' => 0,
          'resize' => 0
        )
      ),
      'thumbnails' => array(
        array(
          'name' => 'Thumb',
          'dimensions' => '90x90',
          'prefix' => 'thumb_',
          'suffix' => ''
        )
      )
    )
  ));
  variable_set('imce_roles_profiles', array(
    5 => array(
      'weight' => 0,
      'pid' => 1
    ),
    4 => array(
      'weight' => 0,
      'pid' => 1
    ),
    3 => array(
      'weight' => 0,
      'pid' => 1
    ),
    2 => array(
      'weight' => 11,
      'pid' => 0
    ),
    1 => array(
      'weight' => 12,
      'pid' => 0
    )
  ));
  // Create the profile pictures directory
  file_create_path(variable_get('user_picture_path', 'pictures'));
  // Set the og node type for all content types
  $node_types = array_keys(array_map('check_plain', node_get_types('names')));
  foreach($node_types as $node_type){
    $usage = 'group_post_standard';
    if($node_type == 'group' || $node_type == 'publication'){
      $usage = 'group';
    }
    variable_set('og_content_type_usage_' . $node_type, $usage);
  }
  // Add an alias from 'content' to 'node'
  path_set_alias('node', 'content');
  // Update pathauto for users
  variable_set('pathauto_user_pattern', 'user/[user-raw]');
  // Update the alias set for the maintainer (it was created before we could
  // have changed it)
  db_query("UPDATE {url_alias} SET dst = REPLACE(dst, 'users/','user/') WHERE dst LIKE 'users/%'");
  // Run cron
  module_invoke_all('cron');
  variable_set('cron_last', time());
  // Log the user out
  session_destroy();
}


if(!function_exists('quoted_printable_encode')){

  function quoted_printable_encode($input, $line_max = 1000){ // Don't actually need the line_max, but can't be bothered to remove it!
    $hex = array(
      '0',
      '1',
      '2',
      '3',
      '4',
      '5',
      '6',
      '7',
      '8',
      '9',
      'A',
      'B',
      'C',
      'D',
      'E',
      'F'
    );
    $lines = preg_split("/(?:\r\n|\r|\n)/", $input);
    $linebreak = "=0D=0A=\r\n";
    /* the linebreak also counts as characters in the mime_qp_long_line
      * rule of spam-assassin */
    $line_max = $line_max - strlen($linebreak);
    $escape = "=";
    $output = "";
    $cur_conv_line = "";
    $length = 0;
    $whitespace_pos = 0;
    $addtl_chars = 0;
    // iterate lines
    for($j = 0; $j < count($lines); $j++){
      $line = $lines[$j];
      $linlen = strlen($line);
      // iterate chars
      for($i = 0; $i < $linlen; $i++){
        $c = substr($line, $i, 1);
        $dec = ord($c);
        $length++;
        if($dec == 32){
          // space occurring at end of line, need to encode
          if(($i == ($linlen - 1))){
            $c = "=20";
            $length += 2;
          }
          $addtl_chars = 0;
          $whitespace_pos = $i;
        }elseif(($dec == 61) || ($dec < 32) || ($dec > 126)){
          $h2 = floor($dec / 16);
          $h1 = floor($dec % 16);
          $c = $escape . $hex["$h2"] . $hex["$h1"];
          $length += 2;
          $addtl_chars += 2;
        }
        // length for wordwrap exceeded, get a newline into the text
        if($length >= $line_max){
          $cur_conv_line .= $c;
          // read only up to the whitespace for the current line
          $whitesp_diff = $i - $whitespace_pos + $addtl_chars;
          /* the text after the whitespace will have to be read
           * again ( + any additional characters that came into
           * existence as a result of the encoding process after the whitespace)
           *
           * Also, do not start at 0, if there was *no* whitespace in
           * the whole line */
          if(($i + $addtl_chars) > $whitesp_diff){
            $output .= substr($cur_conv_line, 0, (strlen($cur_conv_line) - $whitesp_diff)) . $linebreak;
            $i = $i - $whitesp_diff + $addtl_chars;
          }else{
            $output .= $cur_conv_line . $linebreak;
          }
          $cur_conv_line = "";
          $length = 0;
          $whitespace_pos = 0;
        }else{
          // length for wordwrap not reached, continue reading
          $cur_conv_line .= $c;
        }
      } // end of for
      $length = 0;
      $whitespace_pos = 0;
      $output .= $cur_conv_line;
      $cur_conv_line = "";
      if($j <= count($lines) - 1){
        $output .= $linebreak;
      }
    } // end for
    return trim($output);
  }
}

function emonocot_profile_profile_tasks_4(){
  // Set garland as installed WTF???  
  db_query("UPDATE {system} SET status = 1 WHERE name = 'emonocot_sp'");
  variable_set('theme_default', 'emonocot_sp');
  // Ensure access is properly set (why the fuck wouldn't it be I hear you ask,
  // well, I don't bloody know).  
  node_access_rebuild();
  // Update the menu router information.
  drupal_rebuild_theme_registry();
  node_types_rebuild();
  menu_rebuild();
  // Clear all caches
  cache_clear_all('*', 'cache', TRUE);
  cache_clear_all('*', 'cache_block', TRUE);
  cache_clear_all('*', 'cache_content', TRUE);
  cache_clear_all('*', 'cache_filter', TRUE);
  cache_clear_all('*', 'cache_form', TRUE);
  cache_clear_all('*', 'cache_location', TRUE);
  cache_clear_all('*', 'cache_menu', TRUE);
  cache_clear_all('*', 'cache_mollom', TRUE);
  cache_clear_all('*', 'cache_page', TRUE);
  cache_clear_all('*', 'cache_term', TRUE);
  cache_clear_all('*', 'cache_views', TRUE);
  cache_clear_all('*', 'cache_views_data', TRUE);
}

/**
 * Pulled out as a seperate function to enable it to be executed easily in the
 * future (make changes to the code below, and then execute this function on all
 * the sites).
 */
function emonocot_profile_profile_set_perms(){
  // Add roles and permissions
  db_query("DELETE FROM {role} WHERE name IN ('contributor','editor','maintainer')");
  db_query("INSERT INTO {role} (rid, name) VALUES (3, 'contributor'),(4, 'editor'),(5, 'maintainer')");
  $contributor_perms = array();
  $editor_perms = array();
  if(function_exists('content_types')){
    $content_types = array_keys(content_types());
    $content_types[] = "type";
    foreach($content_types as $content_type){
      if($content_type != 'group'){
        variable_set('og_content_type_usage_' . $content_type, 'group_post_standard');
      }
      $contributor_perms[] = "create $content_type content";
      $contributor_perms[] = "delete own $content_type content";
      $contributor_perms[] = "edit own $content_type content";
      $editor_perms[] = "edit any $content_type content";
      $editor_perms[] = "delete any $content_type content";
    }
    $anonymous_perms = array(
      "access all views",
      "access biblio content",
      "access comments",
      "access content",
      "access news feeds",
      "access print",
      "access site-wide contact form",
      "access user profiles",
      "create citations",
      "download original image",
      "fotonotes view notes",
      "search content",
      "show download links",
      "show export links",
      "show node map",
      "show own download links",
      "show sort links",
      "show user map",
      "use advanced search",
      "use share this",
      "user locations",
      "view advanced help index",
      "view advanced help popup",
      "view advanced help topic",
      "view all user locations",
      "view full text",
      "view original images",
      "view post access counter",
      "view search_files results",
      "view Terms and Conditions",
      "view uploaded files",
      "vote on polls"
    );
    $authenticated_perms = array_merge($anonymous_perms, array(
      "access own webform submissions",
      "create forum topics",
      "edit own forum topics",
      "maintain own subscriptions",
      "post by femail",
      "post comments",
      "post comments without approval",
      "show filter tab",
      "submit form without hashcash",
      "subscribe to content in groups",
      "subscribe to newsletters",
      "upload files",
      "upload files to comments",
      "view own user location",
      "view revisions"
    ));
    $contributor_perms = array_merge($contributor_perms, $authenticated_perms, array(
      "access webform results",
      "assign node weight",
      "attach images",
      "clone own nodes",
      "create biblio",
      "create blog entries",
      "create darwincore content",
      "create images",
      "create nexus projects",
      "create spm content",
      "create url aliases",
      "create webforms",
      "delete own blog entries",
      "delete own darwincore content",
      "delete own forum topics",
      "delete own images",
      "delete own nexus projects",
      "delete own spm content",
      "edit biblio authors",
      "edit own biblio entries",
      "edit own blog entries",
      "edit own darwincore content",
      "edit own images",
      "edit own nexus projects",
      "edit own spm content",
      "edit own webforms",
      "fotonotes add notes to all images",
      "fotonotes add notes to own images",
      "fotonotes edit own notes",
      "import classification", // TEMPORARY UNTIL CLASSIFICATION MODULE IS REMOVED
      "import content",
      "post with no checking",
      "set own user location",
      "set user location",
      "submit latitude/longitude",
      "translate content",
      "translate interface"
    ));
    $editor_perms = array_merge($editor_perms, $contributor_perms, array(
      "administer comments",
      "administer images",
      "administer imports",
      "administer menu",
      "administer nodes",
      "administer newsletters",
      "administer taxonomy",
      "administer users",
      "administer views",
      "clone node",
      "configure member roles",
      "delete any blog entry",
      "delete any forum topic",
      "delete any images",
      "delete any nexus projects",
      "delete any spm content",
      "delete darwincore content",
      "edit all biblio entries",
      "edit any blog entry",
      "edit any forum topic",
      "edit any images",
      "edit any nexus projects",
      "edit any spm content",
      "edit darwincore content",
      "edit images",
      "edit webforms",
      "fotonotes edit all notes",
      "import from file",
      "inspect all votes",
      "mado sort",
      "revert revisions",
      "send newsletter",
      "view sort sort"
    ));
    $maintainer_perms = array_merge($editor_perms, array(
      "access administration pages",
      "access statistics",
      "administer biblio",
      "administer blocks",
      "administer content types",
      "administer creative commons lite",
      "administer forums",
      "administer languages",
      "administer lightbox2",
      "administer messaging",
      "administer news feeds",
      "administer notifications",
      "administer organic groups",
      "administer print",
      "administer redirects",
      "administer simplenews settings",
      "administer simplenews subscriptions",
      "administer site configuration",
      "administer site-wide contact form",
      "administer Terms and Conditions",
      "administer url aliases",
      "administer user locations",
      "assign roles",
      "clear webform results",
      "delete revisions",
      "edit webform submissions",
      "make backups"
    ));
    $anonymous_perms = implode(", ", $anonymous_perms);
    $authenticated_perms = implode(", ", $authenticated_perms);
    $contributor_perms = implode(", ", $contributor_perms);
    $editor_perms = implode(", ", $editor_perms);
    $maintainer_perms = implode(", ", $maintainer_perms);
    db_query("TRUNCATE TABLE {permission}");
    db_query("INSERT INTO {permission} (rid,perm) VALUES ((SELECT rid FROM {role} WHERE name = 'anonymous user'),'%s'),((SELECT rid FROM {role} WHERE name = 'authenticated user'),'%s'),((SELECT rid FROM {role} WHERE name = 'contributor'),'%s'),((SELECT rid FROM {role} WHERE name = 'editor'),'%s'),((SELECT rid FROM {role} WHERE name = 'maintainer'),'%s')", $anonymous_perms, $authenticated_perms, $contributor_perms, $editor_perms, $maintainer_perms);
  }
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
  // Attempt to load the data from the Scratchpad Application form, if we fail,
  // we'll continue as normal, if we succeed, then we'll skip the following
  // steps
  // First we try with Drush
  $data = array();
  if(function_exists('drush_get_option')){
    $site_title = drush_get_option('site_title', FALSE);
    if($site_title){
      $data = array('sitetitle' => $site_title,'title' => drush_get_option('client_title', ''),'fullname' => drush_get_option('fullname', ''),'institution' => drush_get_option('institution', ''),'taxonomicscope' => drush_get_option('taxonomic_scope', ''),'googleapi' => drush_get_option('googleapi', ''),'clustrmaphtml' => drush_get_option('clustrmaphtml', ''),'missionstatement' => drush_get_option('mission_statement', ''),'sitetitle' => drush_get_option('site_title', ''),'client_email' => drush_get_option('client_email', ''));
    }
  }
  if(count($data)){
    $names = explode(" ", $data['fullname']);
    $familyname = array_pop($names);
    $givennames = implode(" ", $names);
    // Submit the forms
    $form_state = array('values' => array('title' => $data['title'],'given' => $givennames,'family' => $familyname,'institution' => $data['institution'],'expertise' => $data['taxonomicscope'],'gmapkey' => $data['googleapi'],'clustrmap' => $data['clustrmaphtml'],'mission' => $data['missionstatement']));
    emonocot_personal_submit(NULL, $form_state);
    emonocot_gmapkey_submit(NULL, $form_state);
    emonocot_clustrmap_submit(NULL, $form_state);
    emonocot_mission_submit(NULL, $form_state);
    // Delete variables
    variable_del('personal_submitted');
    variable_del('clustrmap_submitted');
    variable_del('mission_submitted');
    $task = 'scratchpadcleanup';
    // Add the site title and email address
    variable_set('site_name', $data['sitetitle']);
    variable_set('site_mail', $data['client_email']);
     // Update the user.
  }
  if($task == 'personal'){
    $output = drupal_get_form('emonocot_personal', $url);
    if(!variable_get('personal_submitted', FALSE)){
      drupal_set_title(st('Personal Information'));
      return $output;
    }else{
      // Delete the variable
      variable_del('personal_submitted');
      // Form was submitted
      $task = 'gmapkey';
    }
  }
  if($task == 'gmapkey'){
    $output = drupal_get_form('emonocot_gmapkey', $url);
    if(!variable_get('googlemap_api_key', FALSE)){
      drupal_set_title(st('Google Maps API Key'));
      return $output;
    }else{
      // Form was submitted
      $task = 'clustrmap';
    }
  }
  if($task == 'clustrmap'){
    $output = drupal_get_form('emonocot_clustrmap', $url);
    if(!variable_get('clustrmap_submitted', FALSE)){
      drupal_set_title(st('ClustrMap HTML Code'));
      return $output;
    }else{
      // Delete the variable
      variable_del('clustrmap_submitted');
      // Form was submitted
      $task = 'mission';
    }
  }
  if($task == 'mission'){
    $output = drupal_get_form('emonocot_mission', $url);
    if(!variable_get('mission_submitted', FALSE)){
      drupal_set_title(st('Site mission and slogan'));
      return $output;
    }else{
      // Delete the variable
      variable_del('mission_submitted');
      // Form was submitted
      $task = 'emonocotcleanup';
    }
  }
  if($task == 'emonocotcleanup'){
    emonocot_profile_profile_tasks_2();
    // The above function moves the UID 1 to UID 2.  Now we can update that user
    // if we're installing user Aegir.  This means the next funciton will email
    // the right person! WOOT!
    if(is_array($data) && count($data)){
      db_query("UPDATE {users} SET mail = '%s', name = '%s' WHERE uid = 2", $data['email'], $data['fullname']);
    }
    emonocot_profile_profile_tasks_4();
    $task = 'profile-finished';
  }
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
}
