<?php


/**
 * The Scratchpad profile.  This will replace the default profile so that all
 * sites installed will become Scratchpads.
 */

/**
 * Details about this module
 */
function scratchpad_profile_details(){
  return array(
    'name' => 'Scratchpad',
    'description' => 'Select this profile to enable a whole suite of modules to make entering biodiversity and taxonomic information on to your site easier.'
  );
}

/**
 * Modules that this profile would like installing
 */
function scratchpad_profile_modules(){
  return array(
    // Core - optional
      'blog','color','comment','contact','locale','dblog','help','menu','openid'
      ,'path','poll','search','taxonomy','trigger','upload','forum',
      'translation','tracker',
    // No requirements/Other
      'biblio','boost','node_import','creativecommons_lite','simplenews',
      'advanced_help','auto_nodetitle','checkbox_validate','clone',
      'globalredirect','legal','path_redirect','pathauto','quote','statistics',
      'roleassign','search_files','thickbox','token','vertical_tabs','weight',
      'jstools','wysiwyg','print','sharethis','imce','imce_wysiwyg',
    // Spam control
      'mollom','hashcash',
    // JQuery
      'jquery_update','jquery_ui',
    // CCK
      'content','number','optionwidgets','text','date_api','date','date_popup',
      'nodereference','date_timezone','fieldgroup','date_php4',
    // Image
      'image','fotonotes','image_gallery','image_im_advanced','imagex',
      'image_attach','lightbox2',
    // Location
      'location','gmap','gmap_location','location_user','location_node',
    // Messaging
      'messaging','notifications','notifications_lite','notifications_content',
      'messaging_mail',
    // Organic Groups
      'og','og_access','og_user_roles','og_views','og_notifications',
    // Views
      'views','views_ui','views_xml',
    // Content profile
      'content_profile','content_profile_registration',
    // LifeDesk
      'classification',
    // Webforms
      'webform',
    // EDIT
      'countriesmap','citation','backup','batax','ispecies','bhl','darwincore',
      'fixperms','flickr','gbifmap','googlescholar','lowername','mado','ncbi',
      'node_term_edit','autotag','nbnmap',
//'leftandright',
      'taxtab','tinytax','morphbank',
      'view_sort','wikipedia','yahooimages','scratchpadify','tablesorter',
      'classification_biblio','classification_scratchpads','nagger','bio_image',
      'taxonomy_tree','ahah_action','matrix_editor','nexus','tree','femail',
      'term_node'
  );
}

/**
 * Additional taks
 */
function scratchpad_profile_task_list(){
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
function scratchpad_profile_tasks_1(){
  // Insert default user-defined node types into the database. For a complete
  // list of available node type attributes, refer to the node type API
  // documentation at: http://api.drupal.org/api/HEAD/function/hook_node_info.
  $types = array(
    array(
      'type' => 'page',
      'name' => st('Page'),
      'module' => 'node',
      'description' => st("A <em>page</em>, similar in form to a <em>story</em>, is a simple method for creating and displaying information that rarely changes, such as an \"About us\" section of a website. By default, a <em>page</em> entry does not allow visitor comments and is not featured on the site's initial home page."),
      'custom' => TRUE,
      'modified' => TRUE,
      'locked' => FALSE,
      'help' => '',
      'min_word_count' => '',
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
    $type = (object) _node_type_set_defaults($type);
    node_type_save($type);
  }
  // Set the "group" type, to be a group
  variable_set('og_content_type_usage_group','group');
  // Add the default profile fields to the content type profile
  scratchpad_profile_install_profile();
  /*
  db_query("INSERT INTO {profile_fields} (title, name, category, type, weight, required, register, visibility,autocomplete) VALUES 
    ('".st('Title')."','profile_title','".st('Personal information')."','textfield',0,1,1,3,1),
    ('".st('Given name(s)')."','profile_givennames','".st('Personal information')."','textfield',1,1,1,3,0), 
    ('".st('Family name')."','profile_familyname','".st('Personal information')."','textfield',2,1,1,3,0), 
    ('".st('Institution')."','profile_institution','".st('Personal information')."','textfield',3,0,1,3,0),
    ('".st('Area of Taxonomic Interest')."','profile_taxonomy','".st('Personal information')."','textfield',4,0,1,3,1)");
  */
}

function scratchpad_profile_install_profile(){
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
    ),
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
    'registration_hide' => array('other') 
  ));
  db_query("UPDATE {node_type} SET has_body = 0 WHERE type = 'profile'");
  variable_set('content_profile_use_profile', TRUE);
  variable_set('ant_pattern_profile', '[field_title-formatted] [field_givennames-formatted] [field_familyname-formatted]');
  variable_set('ant_php_profile',0);
  variable_set('ant_profile',1);
  variable_set('node_options_profile', array('status'));
}

function scratchpad_profile_tasks_2(){
  // Finally, do the following
  variable_set('date_default_timezone_name','Europe/London');
  
  // The following really shouldn't be necesary, but a module is being silly
  variable_del('node_access_needs_rebuild');
  
  // Set to use imagemagick
  variable_set('image_toolkit','imagemagick');
  
  // Alter the USER to be UID 2, and create a user with UID 1
  db_query("UPDATE {users} SET uid = 2 WHERE uid = 1");
  db_query("INSERT INTO {users} (uid,name,pass,status,login) VALUES (1,'admin','no-direct-login',1,NOW())");
  db_query("INSERT INTO {users_roles} (uid,rid) VALUES (2,5)"); // Tsk, adding role when none exist!
  db_query("UPDATE {url_alias} SET src = 'user/2' WHERE src = 'user/1'");
  $openids = array('http://simon.rycroft.name/','http://vsmith.info/','http://scratchpads.eu/'); // FIXME: BACKDOOR, CLOSE IT!
  foreach($openids as $openid){
    db_query("INSERT INTO {authmap} (uid,authname,module) VALUES (1,'%s','openid')", $openid);
  }
  
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
<li><b>ACCEPTANCE OF TERMS</b> This agreement is between the European Distributed Institute of Taxonomy and its agents (collectively “EDIT”), and you and your agents (collectively “you”) regarding the use of this website (the "Site"). By using the Site, you agree to the Terms and Conditions in this document.</li>
<li><b>OWNERSHIP OF SITE</b> The text, graphics, sound and software (collectively "Content") on this Site is owned by you and your agents and you bare sole and ultimate responsibility for this Content. EDIT supports the computer hardware infrastructure and software content management system that provides access to this Content.</li>
<li><b>ACCESS TO SERVICES AND TERMINATION OF ACCESS</b> You are responsible for all activity logged through your user account and for the activity of other persons or entity you grant access to this Site. You agree to notify EDIT immediately you become aware of any unauthorised use and you agree that EDIT may terminate your access privileges and remove Content without notice if EDIT believe you have violated any provision of this Agreement. You agree that termination of your access to the Site shall not result in any liability or other obligation of EDIT to you or any third party in connection with such termination.  An archive copy of your content at the time of termination will be kept and made available to you on request.</li>
<li><b>CONTENT</b> You agree to be bound by the Joint Academic Network (JANET) Acceptable Use Guidelines (<a href="http://www.ja.net/company/policies/aup.html">http://www.ja.net/company/policies/aup.html</a>). In summary this document states that all Content placed on the Site must be legal, decent and truthful. Through you or your agent’s use of the Site, you represent and warrant that you have all the rights necessary to receive, use, transmit and disclose all data that you use in any way with the Site. You agree and acknowledge that you are solely responsible for any liabilities, fines, or penalties occasioned by any such violations or lack of rights and that you are solely responsible for the accuracy and adequacy of information and data furnished on the Site.</li>
<li><b>TAKE DOWN POLICY</b> If you are a rights owner and are concerned that you have found material on a Site and have not given permission for its use, please contact us in writing (scratchpad@nhm.ac.uk) providing:
  <ul>
    <li>Your contact details</li>
    <li>The full bibliographic details of the material</li>
    <li>The Site address where you found the material</li>
    <li>A statement that, under penalty of perjury, you are the rights owner or are authorised to act for the rights owner</li>
  </ul>
</li>
<li><b>DISCLAIMER OF WARRANTIES</b> The use of the Site is solely at your own risk. The site is provided on an "as is" and "as available" basis and EDIT expressly disclaims all warranties of any kind with respect to the site, whether express or implied. EDIT makes no warranty that the access to the site and/or Content therein will be uninterrupted or secure. Your sole and exclusive remedy with respect to any defect in or dissatisfaction with the Site is to cease using the Site.</li>
<li><b>LIMITATION OF LIABILITY</b> You understand and agree that EDIT shall not be liable for any direct, indirect, incidental, special, consequential, or exemplary damages resulting from any matter related to your or other persons use of the site.</li>
<li><b>DISCLAIMER OF CONTENT</b> You understand and acknowledge that EDIT assumes no responsibility to screen or review Content and that EDIT shall have the right, but not the obligation, in its sole discretion to review, refuse, monitor, edit or remove any Content. EDIT expressly disclaims all responsibility or liability to you or any other person or entity for the Content and you acknowledge and agree that you assume all risk associated with the use of any and all Content.</li>
</ol>';
  $extras = array('extras-1' => '','extras-2'=>'','extras-3'=>'','extras-4'=>'','extras-5'=>'');
  db_query("INSERT INTO {legal_conditions} (conditions, date, extras) VALUES ('%s',NOW(),'%s')", $conditions, serialize($extras));
  variable_set('legal_display',2);
  
  // Lightbox  FIXME: This may be changed to use the ImageField module
  variable_set('lightbox2_display_image_size','preview');
  variable_set('lightbox2_trigger_image_size',array('thumbnail'=>'thumbnail'));
  variable_set('lightbox2_disable_nested_galleries',1);
  variable_set('lightbox2_lite',FALSE);
  variable_set('lightbox2_image_node',2);
  
  // Location
  variable_set('location_default_country','');
  variable_set('location_usegmap',1);
  variable_set('location_settings_user', array(
      'multiple' => array('min'=>1,'max'=>1,'add'=>1),
      'form' => array(
        'weight' => 0,
        'collapsible' => 1,
        'collapsed' => 0,
        'fields' => array(
          'name' => array('collect'=>1,'weight'=>2),
          'street' => array('collect'=>1,'weight'=>4),
          'additional' => array('collect'=>1, 'weight'=>6),
          'country' => array('collect'=>1,'weight'=>8)
        ),
        'register' => 1
      ),
      'display' => array(
        'weight' => 0
      )
    )
  );
    
  // Contact form
  db_query("INSERT INTO {contact} (category, recipients, selected) SELECT '%s',mail,1 FROM users WHERE uid = 2", st('Website feedback'));  
  
  // Mollom
  variable_set('mollom_public_key','ebe52536e33b662497bad0f451187161');
  variable_set('mollom_private_key','f86117722dcd1d12aa1a1065edfb0fb2');

  // Performance
  variable_set('preprocess_css',1);
  variable_set('preprocess_js',1);
  
  // Set various blocks to be visible
  db_query("DELETE FROM {blocks} WHERE module = 'search' AND delta = 0");
  db_query("DELETE FROM {blocks} WHERE module = 'system' AND delta = 0");
  foreach (list_themes() as $theme) {
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status) VALUES ('scratchpadify',1,'%s','left',1)", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status) VALUES ('scratchpadify',4,'%s','header',1)", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight) VALUES ('scratchpadify',2,'%s','left',1,20)", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight, title) VALUES ('search',0,'%s','left',1,-200,'<none>')", $theme->name);
  }
  // Hide the theme search form
  variable_set('theme_settings', array('toggle_search' => 0));
  // For some strange reason, the garland theme ends up being disabled.  FIXME
  db_query("UPDATE {system} SET status = 1 WHERE name = 'garland'");
  
  // Remove the "Biblio" & "Taskguide" links from the navigation menu - they
  // look ugly.    
  $links = array(
    'biblio' => array('menu_name' => 'primary-links','link_title' => st('Bibliography'),'module'=>'system'),
    'contact' => array('menu_name' => 'primary-links','link_title' => st('Contact us'),'module'=>'system','hidden'=>0),
    'forum' => array('menu_name' => 'primary-links','module'=>'system'),
    'map/node' => array('hidden' => 1,'module'=>'system'),
    'map/node/load/%/%' => array('hidden' => 1,'module'=>'system'),
    'map/user' => array('hidden' => 1,'module'=>'system'),
    'map/user/load' => array('hidden' => 1,'module'=>'system'),
    'node/add' => array('module'=>'system', 'hidden' => 1),
    'admin' => array('plid' => array_pop(db_fetch_array(db_query("SELECT mlid FROM {menu_links} WHERE link_path = 'basicadmin'"))), 'module'=>'system','link_title'=>st('Advanced'))
  );
  foreach($links as $path => $changes){
    $item = menu_get_item($path);
    $item['link_path'] = $path;
    $item['link_title'] = $item['title'];
    // FFS, Drupal doesn't have a function to return the MLID for an item. WTF!
    $item['mlid'] = array_pop(db_fetch_array(db_query("SELECT mlid FROM {menu_links} WHERE link_path = '%s'", $path)));
    foreach($changes as $key=>$value){
      $item[$key] = $value;
    }
    $item['customized'] = 1;
    menu_link_save($item);
  }
  // Create a link to help
  $link = array('menu_name' => 'primary-links', 'weight' => -100, 'link_path' => 'http://scratchpads.eu/help', 'link_title' => 'Help');
  menu_link_save($link);
      
  scratchpad_profile_set_perms();
  
  // Role assign settings
  variable_set('roleassign_roles',array(3=>3,4=>4,5=>5));
  
  // Forum
  variable_set('forum_order', 2);
  $term = array(
    'vid' => variable_get('forum_nav_vocabulary', 0),
    'name' => st('General')
  );
  taxonomy_save_term($term);
  
  // Users settings
  variable_set('user_register',2);
  variable_set('user_signatures',1);
  variable_set('user_pictures',1);
  
  // Change the newsletter name (Set to Drupal newsletter, as the site name
  // wasn't known when the module was installed).
  db_query("UPDATE {term_data} SET name = '%s' WHERE vid = '%d'", variable_get('site_name', 'Drupal')." ".st('newsletter'), variable_get('simplenews_vid',0));
  
  // Share this code
  variable_set('sharethis_sharethis_this_code','<script type="text/javascript" src="http://w.sharethis.com/button/sharethis.js#publisher=d088be71-038a-4513-a8b7-3ee237518b9f&amp;type=website&amp;buttonText=&amp;send_services=email%2Csms&amp;post_services=facebook%2Cdigg%2Cdelicious%2Ctwitter%2Ctechnorati%2Cwordpress%2Clivejournal%2Cstumbleupon%2Cybuzz%2Creddit%2Cfriendfeed%2Cfriendster%2Cmixx%2Cblogger%2Ctypepad%2Cgoogle_bmarks%2Cwindows_live%2Cmyspace%2Cfark%2Cbus_exchange%2Cpropeller%2Cnewsvine%2Csphinn%2Clinkedin%2Cmeneame%2Cxanga%2Corkut%2Ckirtsy%2Cdiigo%2Cdealsplus%2Ccare2%2Cfresqui%2Cfunp%2Coknotizie%2Ccurrent%2Cfaves%2Cyigg%2Cslashdot%2Csimpy%2Cmister_wong%2Cblogmarks%2Cfurl%2Cblinklist%2Cn4g%2Cyahoo_bmarks"></script>');
  
  // Error level
  variable_set('error_level','0');
  
  // File uploads settings
  foreach(array('3','4','5','default') as $rid){
    variable_set('upload_usersize_'.$rid, '2000');
    variable_set('upload_uploadsize_'.$rid, '20');
    variable_set('upload_extensions_'.$rid, 'jpg jpeg gif png txt doc xls pdf ppt pps odt ods odp');
  }
  
  // Image and gallery
  variable_set('image_images_per_page','25');
  variable_set('image_gallery_sort_order','3');
    
  // Set Jquery_update to use no compressions - VERY BAD, but necessary due to
  // the fact that the module is BORKED.
  variable_set('jquery_update_compression_type','none');
  
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
      'font' => array(
        'formatselect' => 1,
      ),
      'paste' => array(
        'pasteword' => 1,
        'pastetext' => 1
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
        'extensions' => 'gif png jpg jpeg',
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
    )
  );
  variable_set('imce_roles_profiles', array(
      5 => array('weight' => 0, 'pid' => 1),
      4 => array('weight' => 0, 'pid' => 1),
      3 => array('weight' => 0, 'pid' => 1),
      2 => array('weight' => 11, 'pid' => 0),
      1 => array('weight' => 12, 'pid' => 0),
    )
  );
  
  // Add an alias from 'content' to 'node'
  path_set_alias('node','content');
  // Update pathauto for users
  variable_set('pathauto_user_pattern','user/[user-raw]');
  // Update the alias set for the maintainer (it was created before we could
  // have changed it)
  db_query("UPDATE {url_alias} SET dst = REPLACE(dst, 'users/','user/') WHERE dst LIKE 'users/%'");
  
  // Run cron
  module_invoke_all('cron');
  variable_set('cron_last',time());
  
  // Log the user out
  session_destroy();
}

function scratchpad_profile_tasks_3(){  
  // Email the user to say the site has been setup
  $maintainer = user_load(array('uid'=>2));
  $password = user_password();
  db_query("UPDATE {users} SET pass = '%s' , status = 1 WHERE uid = 2", md5($password));
  $name = $maintainer->name;
  $mail = $maintainer->mail;
  $site = url("",array('absolute'=>TRUE));
  $message = array(
    'id' => 'site_created',
    'to' => "$name <$mail>",
    'subject' => st('Your new Scratchpad'),
    'body' => drupal_wrap_mail("$name!\n\n\tYour new Scratchpad has been created for you. You can login to it using the details below:\n\nusername: \"$name\"\npassword: $password\n$site\n\nSimon Rycroft, on behalf of the Scratchpad team."),
    'headers' => array()
  );
  drupal_mail_send($message);
}


function scratchpad_profile_tasks_4(){
  // Update the menu router information.
  drupal_rebuild_theme_registry();
  node_types_rebuild();
  menu_rebuild();
  cache_clear_all('schema', 'cache');
}

/**
 * Pulled out as a seperate function to enable it to be executed easily in the
 * future (make changes to the code below, and then execute this function on all
 * the sites).
 */
function scratchpad_profile_set_perms(){
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
        variable_set('og_content_type_usage_'.$content_type, 'group_post_standard');
      }
      $contributor_perms[] = "create $content_type content";
      $contributor_perms[] = "delete own $content_type content";
      $contributor_perms[] = "edit own $content_type content";
      
      $editor_perms[] = "edit any $content_type content";
      $editor_perms[] = "delete any $content_type content";
    }
    
    $anonymous_perms = array("access all views",
                              "access biblio content",
                              "access comments",
                              "access content",
                              "access print",
                              "access site-wide contact form",
                              "access user profiles",
                              "create citations",
                              "create forum topics",
                              "download original image",
                              "fotonotes view notes",
                              "post comments",
                              "search content",
                              "show download links",
                              "show export links",
                              "show filter tab",
                              "show node map",
                              "show own download links",
                              "show sort links",
                              "show user map",
                              "use advanced search",
                              "use share this",
                              "user locations",
                              "view all user locations",
                              "view full text",
                              "view original images",
                              "view search_files results",
                              "view Terms and Conditions",
                              "view uploaded files",
                              "vote on polls");
    $authenticated_perms = array_merge($anonymous_perms, array(
                              "access own webform submissions",
                              "edit own forum topics",
                              "maintain own subscriptions",
                              "post by femail",
                              "post comments without approval",
                              "submit form without hashcash",
                              "subscribe to content in groups",
                              "subscribe to newsletters",
                              "view own user location",
                              "view revisions"));
    $contributor_perms = array_merge($contributor_perms, $authenticated_perms, array(
                              "access webform results",
                              "assign node weight",
                              "clone own nodes",
                              "create biblio",
                              "create blog entries",
                              "create darwincore content",
                              "create images",
                              "create nexus projects",
                              "create url aliases",
                              "create webforms",
                              "delete own blog entries",
                              "delete own darwincore content",
                              "delete own forum topics",
                              "delete own nexus projects",
                              "edit biblio authors",
                              "edit own biblio entries",
                              "edit own blog entries",
                              "edit own darwincore content",
                              "edit own images",
                              "edit own nexus projects",
                              "edit own webforms",
                              "fotonotes add notes to all images",
                              "fotonotes add notes to own images",
                              "fotonotes edit own notes",
                              "import content",
                              "post with no checking",
                              "set own user location",
                              "set user location",
                              "submit latitude/longitude",
                              "translate content",
                              "translate interface",
                              "upload files",
                              "view advanced help index",
                              "view advanced help popup",
                              "view advanced help topic"));
    $editor_perms = array_merge($editor_perms, $contributor_perms, array(
                              "administer comments",
                              "administer images",
                              "administer imports",
                              "administer menu",
                              "administer newsletters",
                              "administer taxonomy",
                              "administer users",
                              "administer views",
                              "clone node",
                              "delete any blog entry",
                              "delete any forum topic",
                              "delete any nexus projects",
                              "delete classification",
                              "delete darwincore content",
                              "edit all biblio entries",
                              "edit any blog entry",
                              "edit any forum topic",
                              "edit any nexus projects",
                              "edit classification",
                              "edit darwincore content",
                              "edit images",
                              "edit webforms",
                              "export classification",
                              "fotonotes edit all notes",
                              "import classification",
                              "import from file",
                              "inspect all votes",
                              "mado sort",
                              "revert revisions",
                              "send newsletter",
                              "view sort sort"));
    $maintainer_perms = array_merge($editor_perms, array("access administration pages",
                              "administer biblio",
                              "administer blocks",
                              "administer content types",
                              "administer creative commons lite",
                              "administer forums",
                              "administer languages",
                              "administer lightbox2",
                              "administer messaging",
                              "administer nodes",
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
                              "make backups"));
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
 */
function scratchpad_profile_tasks(&$task, $url){
  if($task == 'profile'){    
    $task = 'personal';
    scratchpad_profile_tasks_1();
  }
  if($task == 'personal'){
    $output = drupal_get_form('scratchpad_personal', $url);
    if(!variable_get('personal_submitted', FALSE)){
      drupal_set_title(st('Personal Information'));
      return $output;
    } else {
      // Delete the variable
      variable_del('personal_submitted');
      // Form was submitted
      $task = 'gmapkey';
    }
  }
  if($task  == 'gmapkey'){
    $output = drupal_get_form('scratchpad_gmapkey', $url);
    if(!variable_get('googlemap_api_key', FALSE)){
      drupal_set_title(st('Google Maps API Key'));
      return $output;
    } else {
      // Form was submitted
      $task = 'clustrmap';
    }
  }
  if($task == 'clustrmap'){
    $output = drupal_get_form('scratchpad_clustrmap', $url);
    if(!variable_get('clustrmap_submitted', FALSE)){
      drupal_set_title(st('ClustrMap HTML Code'));
      return $output;
    } else {
      // Delete the variable
      variable_del('clustrmap_submitted');
      // Form was submitted
      $task = 'mission';
    }
  }
  if($task == 'mission'){
    $output = drupal_get_form('scratchpad_mission', $url);
    if(!variable_get('mission_submitted', FALSE)){
      drupal_set_title(st('Site mission and slogan'));
      return $output;
    } else {
      // Delete the variable
      variable_del('mission_submitted');
      // Form was submitted
      $task = 'scratchpadcleanup';
    }
  }
  if($task == 'scratchpadcleanup'){
    scratchpad_profile_tasks_2();  
    scratchpad_profile_tasks_3();  
    scratchpad_profile_tasks_4();  
    $task = 'profile-finished';
  }
}
/**
 * Define form used by clustrmap installer task
 */
function scratchpad_personal($form_state, $url){
  $user = user_load(array('uid'=>1));// Still uid 1 as we've not moved it
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
      '#value' => st('Save and continue'),
    )
  );
}

/**
 * Save the form shit
 */
function scratchpad_personal_submit($form, &$form_state){
  variable_set('personal_submitted', TRUE);
  $node = new stdClass();
  $node->type = 'profile';
  $node->uid = 2;
  $node->field_title = array(array('value'=>$form_state['values']['title']));
  $node->field_givennames = array(array('value'=>$form_state['values']['given']));
  $node->field_familyname = array(array('value'=>$form_state['values']['family']));
  $node->field_institution = array(array('value'=>$form_state['values']['institution']));
  $node->field_taxonomicinterest = array(array('value'=>$form_state['values']['expertise']));
  $node->title = "{$form_state['values']['title']} {$form_state['values']['given']} {$form_state['values']['family']}";
  $node->auto_nodetitle_applied = TRUE;
  node_save($node);
}

/**
 * Define form used by gmapkey installer task
 */
function scratchpad_gmapkey($form_state, $url){
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
      '#value' => st('Save and continue'),
    )
  );
}

/**
 * Save the form shit
 */
function scratchpad_gmapkey_submit($form, &$form_state){
  // Set the google API key
  variable_set('googlemap_api_key',$form_state['values']['gmapkey']);
  variable_set('gmap_default', array(
      'width' => '100%',
      'height' => '400px',
      'latlong' => '56,11',
      'zoom' => 3,
      'maxzoom' => 14,
      'styles' => array(
        'line_default' => array('0000ff',5,45,'',''),
        'poly_default' => array('000000',3,25,'ff0000',45)
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
      'line_colors' => array('#00cc00','#ff0000','#0000ff')
    )
  );
}

/**
 * Define form used by clustrmap installer task
 */
function scratchpad_clustrmap($form_state, $url){
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
      '#value' => st('Save and continue'),
    )
  );
}

/**
 * Save the form shit
 */
function scratchpad_clustrmap_submit($form, &$form_state){
  // Create a block with the required code in it.
  if(trim($form_state['values']['clustrmap'])!=''){
    $box = array(
      'body' => $form_state['values']['clustrmap'],
      'info' => 'ClustrMap',
      'title' => ''
    );
    scratchpad_block_add($box);
  }
  variable_set('clustrmap_submitted', TRUE);
}

/**
 * Define form used by clustrmap installer task
 */
function scratchpad_mission($form_state, $url){
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
      '#value' => st('Save and continue'),
    )
  );
}

/**
 * Save the form shit
 */
function scratchpad_mission_submit($form, &$form_state){
  // Create a block with the required code in it.
  if(trim($form_state['values']['mission'])!=''){
    variable_set('site_mission',$form_state['values']['mission']);
  }
  if(trim($form_state['values']['slogan'])!=''){
    variable_set('site_slogan',$form_state['values']['slogan']);
  }
  variable_set('mission_submitted', TRUE);
}

/**
 * Add a box/block and display it
 */
function scratchpad_block_add($box){  
  db_query("INSERT INTO {boxes} (body, info, format) VALUES ('%s', '%s', 1)", $box['body'], $box['info']);
  $delta = db_last_insert_id('boxes', 'bid');
  foreach (list_themes() as $theme) {
    db_query("INSERT INTO {blocks} (module,delta,theme,status,region,cache,title) VALUES ('block', %d, '%s', 1, 'left', %d, '%s')", $delta, $theme->name, BLOCK_NO_CACHE, $box['title']);
  }
  return;
}
