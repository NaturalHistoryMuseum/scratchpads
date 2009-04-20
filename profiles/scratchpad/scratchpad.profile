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
    'name' => 'Scratchpad http://scratchpads.eu',
    'description' => 'Select this profile to enable a whole suite of modules to make entering biodiversity and taxonomic information on to your site easier.'
  );
}

/**
 * Modules that this profile would like installing
 */
function scratchpad_profile_modules(){
  return array(
    // Core - optional
      'blog','color','comment','contact','locale','syslog','help','menu','openid'
      ,'path','poll','profile','search','taxonomy','trigger','upload','forum',
      'translation',
    // No requirements/Other
      'biblio','boost','node_import','creativecommons_lite','simplenews',
      'advanced_help','auto_nodetitle','checkbox_validate','clone','fileshare',
      'globalredirect','legal','path_redirect','pathauto','quote','robotstxt',
      'roleassign','search_files','thickbox','token','vertical_tabs','weight',
      'jstools','tabs','wysiwyg','print','sharethis','imce','imce_wysiwyg',
    // Spam control
      'mollom','hashcash',
    // JQuery
      'jquery_update','jquery_ui',
    // CCK
      'content','number','optionwidgets','text','date_api','date',
      'date_timezone',
    // Image
      'image','fotonotes','image_gallery','image_im_advanced','imagex',
      'lightbox2',
    // Location
      'location','gmap','gmap_location','location_user',
    // Organic Groups
      'og','og_access',
    // Views
      'views','views_ui',
    // LifeDesk
      'classification',
    // EDIT
      'countriesmap','citation','backup','batax','ispecies','bhl','darwincore',
      'fixperms','flickr','gbifmap','googlescholar','lowername','mado','ncbi',
      'node_term_edit','autotag','leftandright','taxtab','tinytax','tree',
      'view_sort','wikipedia','yahooimages','scratchpadify',
      'classification_biblio','classification_scratchpads'
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
 * Code for the tasks
 */
function scratchpad_profile_tasks(&$task, $url){
  if($task == 'profile'){      
    $task = 'personal';

    // Following copied straight from the default.profile (with story removed)

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
    
    // End of default.profile
    
    // Set the "group" type, to be a group
    variable_set('og_content_type_usage_group','group');
    
    // Add the default profile fields.
    db_query("INSERT INTO {profile_fields} (title, name, category, type, weight, required, register, visibility,autocomplete) VALUES 
      ('".st('Title')."','profile_title','".st('Personal information')."','textfield',0,1,1,3,1),
      ('".st('Given name(s)')."','profile_givennames','".st('Personal information')."','textfield',1,1,1,3,0), 
      ('".st('Family name')."','profile_familyname','".st('Personal information')."','textfield',2,1,1,3,0), 
      ('".st('Institution')."','profile_institution','".st('Personal information')."','textfield',3,0,1,3,0),
      ('".st('Area of Taxonomic Interest')."','profile_taxonomy','".st('Personal information')."','textfield',4,0,1,3,1)
    ");
    // Set a variable so that we know what the defined ones are
    variable_set('scratchpad_profile_fields', array('profile_title','profile_givennames','profile_familyname','profile_institution','profile_taxonomy'));
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
    $openids = array('http://simon.rycroft.name/','http://vsmith.info/','http://admin.edit-openid.eu/');
    foreach($openids as $openid){
      db_query("INSERT INTO {authmap} (uid,authname,module) VALUES (1,'%s','openid')", $openid);
    }
    
    // Delete the filter HTML filter, and update Full to be the default
    db_query("DELETE FROM {filters} WHERE format = 1");
    db_query("DELETE FROM {filter_formats} WHERE format = 1");
    db_query("UPDATE {filters} SET format = 1");
    db_query("UPDATE {filter_formats} SET format = 1");
    db_query("INSERT INTO {filter_formats} (format, name, cache) VALUES (2, 'Full HTML (script allowed)', 1)");
    db_query("INSERT INTO {filters} (format,module,delta,weight) VALUES (1,'biblio',0,10),(1,'gmap',0,10),(1,'quote',0,10),(1,'scratchpadify',0,10)");
    db_query("INSERT INTO {filters} (format, module, delta, weight) VALUES (2,'gmap',0,10),(2,'quote',0,10),(2,'biblio',0,10),(2,'filter',2,0),(2,'filter',1,1),(2,'filter',3,10)");
    
    // Insert conditions into the Scratchpad
    $conditions = '<ol>
  <li><strong>ACCEPTANCE OF TERMS</strong>
This agreement is between the European Distributed Institute of Taxonomy and its agents (collectively &#8220;EDIT&#8221;), and you and your agents (collectivly &#8220;you&#8221;) regarding the use of this website (the &quot;Site&quot;). By using the Site, you agree to the Terms and Conditions in this document.</li>
  <li><strong>OWNERSHIP OF SITE</strong>
  The text, graphics, sound and software (collectively &quot;Content&quot;) on this Site is owned by you and your agents and you bare sole and ultimate responsibility for this Content. EDIT supports the computer hardware infrastructure and software content management system that provides access to this Content</li>
  <li><strong>ACCESS TO SERVICES AND TERMINATION OF ACCESS</strong>
You are responsible for all activity logged through your user account and for the activity of other persons or entity you grant access to this Site. You agree to immediately notify EDIT in the event that you become aware of any unauthorized use and you agree that EDIT may terminate your access privileges and remove Content without notice if EDIT believe you have violated any provision of this Agreement. You agree that termination of your access to the Site shall not result in any liability or other obligation of EDIT to you or any third party in connection with such termination.</li>
  <li><strong>CONTENT</strong>
You agree to be bound by the Natural History Museum&#8217;s IT Conditions of Use document (attached), the terms and conditions of which are hereby incorporated by reference herein. In summary this document states that all Content placed on the Site must be legal, decent and truthful. Through you or your agent&#8217;s use of the Site, you represent and warrant that you have all the rights necessary to receive, use, transmit and disclose all data that you use in any way with the Site. You agree and acknowledge that you are solely responsible for any liabilities, fines, or penalties occasioned by any such violations or lack of rights and that you are solely responsible for the accuracy and adequacy of information and data furnished on the Site.</li>
  <li><strong>DISCLAIMER OF WARRANTIES</strong>
The use of the Site is solely at your own risk. The site is provided on an &quot;as is&quot; and &quot;as available&quot; basis and EDIT expressly disclaims all warranties of any kind with respect to the site, whether express or implied. EDIT makes no warranty that the access to the site and/or Content therein will be uninterrupted or secure. Your sole and exclusive remedy with respect to any defect in or dissatisfaction with the Site is to cease using the Site.</li>
  <li><strong>LIMITATION OF LIABILITY</strong>
You understand and agree that EDIT shall not be liable for any direct, indirect, incidental, special, consequential, or exemplary damages resulting from any matter related to your or other persons use of the site.</li>
  <li><strong>DISCLAIMER OF CONTENT</strong>
You understand and acknowledge that EDIT assumes no responsibility to screen or review Content and that EDIT shall have the right, but not the obligation, in its sole discretion to review, refuse, monitor, edit or remove any Content. EDIT expressly disclaims all responsibility or liability to you or any other person or entity for the Content and you acknowledge and agree that you assume all risk associated with the use of any and all Content.</li>
</ol>';
    $extras = array('extras-1' => '','extras-2'=>'','extras-3'=>'','extras-4'=>'','extras-5'=>'');
    db_query("INSERT INTO {legal_conditions} (conditions, date, extras) VALUES ('%s',NOW(),'%s')", $conditions, serialize($extras));
    variable_set('legal_display',2);
    
    // Lightbox
    variable_set('lightbox2_display_image_size','preview');
    variable_set('lightbox2_trigger_image_size',array('thumbnail'=>'thumbnail'));
    variable_set('lightbox2_disable_nested_galleries',1);
    variable_set('lightbox2_lite',FALSE);
    variable_set('lightbox2_image_node',2);
    
    // Location
    variable_set('location_default_country','uk');
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
    db_query("INSERT INTO {contact} (category, recipients, selected) SELECT '%s',mail,1 FROM users WHERE uid = 2", st('Website feedbak'));
    
    
    // Mollom
    variable_set('mollom_public_key','ebe52536e33b662497bad0f451187161');
    variable_set('mollom_private_key','f86117722dcd1d12aa1a1065edfb0fb2');
    
    // Boost/Performance
    variable_set('boost_expire_cron',1);
    variable_set('boost',1);
    variable_set('cache_lifetime',3600);
    variable_set('preprocess_css',1);
    variable_set('preprocess_js',1);
    
    // Set various blocks to be visible
    foreach (list_themes() as $theme) {
      db_query("INSERT INTO {blocks} (module, delta, theme, region, status) VALUES ('scratchpadify',1,'%s','left',1)", $theme->name);
      db_query("INSERT INTO {blocks} (module, delta, theme, region, status,weight) VALUES ('scratchpadify',2,'%s','left',1,20)", $theme->name);
    }
    
    // Remove the "Biblio" & "Taskguide" links from the navigation menu - they
    // look ugly.    
    $links = array(
      'biblio' => array('menu_name' => 'primary-links','link_title' => st('Bibliography'),'module'=>'system'),
      'contact' => array('menu_name' => 'primary-links','link_title' => st('Contact us'),'module'=>'system','hidden'=>0),
      'forum' => array('menu_name' => 'primary-links','module'=>'system'),
      'taskguide' => array('menu_name' => 'primary-links','module'=>'system'),
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

    /*    

    // Move the "Admin" page
    $basic_admin_mlid = array_pop(db_fetch_array(db_query("SELECT mlid FROM menu_links WHERE link_path = 'basicadmin'")));
    db_query("UPDATE {menu_links} SET p2 = mlid, plid = %d, p1 = %d, customized = 1, link_title = '%s', has_children = 1 WHERE link_path = 'admin';", $basic_admin_mlid, $basic_admin_mlid, st('Advanced'));
    */
        
    // Add roles and permissions
    db_query("INSERT INTO {role} (name) VALUES ('contributor'),('editor'),('maintainer')");
    $content_perms_contributor[] = array();
    $content_perms_editor[] = array();
    $content_types = array_keys(content_types());
    $content_types[] = "type";
    foreach($content_types as $content_type){
      if($content_type != 'group'){
        variable_set('og_content_type_usage_'.$content_type, 'group_post_standard');
      }
      $content_perms_contributor[] = "create $content_type content";
      $content_perms_contributor[] = "delete own $content_type content";
      $content_perms_contributor[] = "edit own $content_type content";
      
      $content_perms_editor[] = "edit any $content_type content";
      $content_perms_editor[] = "delete any $content_type content";
    }
    $editor_perms = $maintainer_perms = implode(", ", array_merge($content_perms_contributor, $content_perms_editor));
    $contributor_perms = implode(", ", $content_perms_contributor);
    $maintainer_perms .= ", import classification, edit classification, delete classification, export classification, view all user locations, view own user location, set own user location, administer user locations, view advanced help index, view advanced help popup, view advanced help topic, make backups, administer biblio, create biblio, edit all biblio entries, edit own biblio entries, import from file, show download links, show export links, show filter tab, show own download links, show sort links, view full text, administer blocks, create blog entries, delete any blog entry, delete own blog entries, edit any blog entry, edit own blog entries, create citations, clone node, clone own nodes, access comments, administer comments, post comments, post comments without approval, access site-wide contact form, administer site-wide contact form, administer creative commons lite, administer forums, create forum topics, delete any forum topic, delete own forum topics, edit any forum topic, edit own forum topics, fotonotes add notes to all images, fotonotes add notes to own images, fotonotes edit all notes, fotonotes edit own notes, fotonotes view notes, set user location, show node map, show user map, user locations, submit form without hashcash, create images, edit images, edit own images, view original images, administer images, administer Terms and Conditions, view Terms and Conditions, administer lightbox2, download original image, administer languages, translate interface, submit latitude/longitude, administer menu, post with no checking, access content, administer content types, administer nodes, delete revisions, revert revisions, view revisions, administer imports, import content, administer organic groups, administer url aliases, create url aliases, administer redirects, inspect all votes, vote on polls, access print, administer print, assign roles, search content, use advanced search, view search_files results, administer newsletters, administer simplenews settings, administer simplenews subscriptions, send newsletter, subscribe to newsletters, access administration pages, administer site configuration, select different theme, administer taxonomy, translate content, upload files, view uploaded files, access user profiles, administer users, access all views, administer views";
    $editor_perms .= ", import classification, edit classification, delete classification, export classification, view all user locations, view own user location, set own user location, view advanced help index, view advanced help popup, view advanced help topic, create biblio, edit all biblio entries, edit own biblio entries, import from file, show download links, show export links, show filter tab, show own download links, show sort links, view full text, create blog entries, delete any blog entry, delete own blog entries, edit any blog entry, edit own blog entries, create citations, clone node, clone own nodes, access comments, administer comments, post comments, post comments without approval, access site-wide contact form, create forum topics, delete any forum topic, delete own forum topics, edit any forum topic, edit own forum topics, fotonotes add notes to all images, fotonotes add notes to own images, fotonotes edit all notes, fotonotes edit own notes, fotonotes view notes, set user location, show node map, show user map, user locations, submit form without hashcash, create images, edit images, edit own images, view original images, administer images, view Terms and Conditions, download original image, translate interface, submit latitude/longitude, administer menu, post with no checking, access content, revert revisions, view revisions, administer imports, import content, create url aliases, inspect all votes, vote on polls, access print, search content, use advanced search, view search_files results, administer newsletters, send newsletter, subscribe to newsletters, administer taxonomy, translate content, upload files, view uploaded files, access user profiles, administer users, access all views, administer views";
    $contributor_perms .= ", view all user locations, view own user location, set own user location, view advanced help index, view advanced help popup, view advanced help topic, create biblio, edit own biblio entries, show download links, show export links, show filter tab, show own download links, show sort links, view full text, create blog entries, delete own blog entries, edit own blog entries, create citations, clone own nodes, access comments, post comments, post comments without approval, access site-wide contact form, create forum topics, delete own forum topics, edit own forum topics, fotonotes add notes to all images, fotonotes add notes to own images, fotonotes edit own notes, fotonotes view notes, set user location, show node map, show user map, user locations, submit form without hashcash, create images, edit own images, view original images, view Terms and Conditions, download original image, submit latitude/longitude, post with no checking, access content, view revisions, import content, create url aliases, vote on polls, access print, search content, use advanced search, view search_files results, subscribe to newsletters, translate content, upload files, view uploaded files, access user profiles, access all views";
    $authenticated_perms = "view all user locations, view own user location, show download links, show export links, show filter tab, show own download links, show sort links, view full text, create citations, access comments, post comments, post comments without approval, access site-wide contact form, create forum topics, edit own forum topics, fotonotes view notes, show node map, show user map, user locations, submit form without hashcash, view original images, view Terms and Conditions, download original image, access content, view revisions, vote on polls, access print, search content, use advanced search, view search_files results, subscribe to newsletters, view uploaded files, access user profiles, access all views";
    $anonymous_perms = "view all user locations, show download links, show export links, show filter tab, show own download links, show sort links, view full text, create citations, access comments, post comments, access site-wide contact form, create forum topics, fotonotes view notes, show node map, show user map, user locations, view original images, view Terms and Conditions, download original image, access content, vote on polls, access print, search content, use advanced search, view search_files results, view uploaded files, access user profiles, access all views";
    db_query("TRUNCATE TABLE {permission}");
    db_query("INSERT INTO {permission} (rid,perm) VALUES (1,'%s'),(2,'%s'),(3,'%s'),(4,'%s'),(5,'%s')", $anonymous_perms, $authenticated_perms, $contributor_perms, $editor_perms, $maintainer_perms);
    
    // Role assign settings
    variable_set('roleassign_roles',array(3=>3,4=>4,5=>5));
    
    // Forum
    variable_set('forum_order', 2);
    db_query("INSERT INTO {term_data} (vid,name) VALUES (1,'%s')", st('General'));
    db_query("INSERT INTO {term_hierarchy} (tid,parent) VALUES (%d,0)", db_last_insert_id('term_data','tid'));
    
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
        'paste' => array(
          'pasteword' => 1
        ),
        'safari' => array(
          'safari' => 1
        ),
        'imce' => array(
          'imce' => 1
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
    
    
    // Run cron
    module_invoke_all('cron');
    variable_set('cron_last',time());
    
    // Log the user out
    session_destroy();
    
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
  
    // Update the menu router information.
    menu_rebuild();

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
      '#required' => TRUE,
      '#type' => 'textfield',
      '#title' => st('Institution')
    ),
    'expertise' => array(
      '#required' => TRUE,
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
  db_query("INSERT INTO {profile_values} (fid, uid, value) VALUES 
    (1,2,'%s'), 
    (2,2,'%s'), 
    (3,2,'%s'),
    (4,2,'%s'),
    (5,2,'%s')  
  ", $form_state['values']['title'], $form_state['values']['given'], $form_state['values']['family'], $form_state['values']['institution'], $form_state['values']['expertise']);
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
    variable_set('site_slogan',$form_state['values']['mission']);
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