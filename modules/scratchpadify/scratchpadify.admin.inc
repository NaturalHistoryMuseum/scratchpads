<?php
/**
 * Menu callback; Sets whether the admin menu is in compact mode or not.
 *
 * @param $mode
 *   Valid values are 'on' and 'off'.
 */
function scratchpadify_compact_page($mode = 'off') {
  global $user;
  user_save($user, array('admin_compact_mode' => ($mode == 'on')));
  drupal_goto(referer_uri());
}

/**
 * Callback to set the cookie to hide the message
 */
function scratchpadify_set_cookie(){
  $hash = variable_get('scratchpad_notification_block_message_id', 'nohash');
  setcookie('scratchpadnotification', $hash, time()+60*60*24*365);
  drupal_goto($_GET['destination']);
}

/**
 * Admin form for setting the notification message.
 */
function scratchpadify_notification_message_form(){
  $form['scratchpadify_notification_message'] = array(
    '#type' => 'textfield',
    '#title' => t('Scratchpad notification.'),
    '#default_value' => variable_get('scratchpadify_notification_message', ''),
    '#description' => t('Enter the message to be displayed on the top of all of the Scratchpads.')
  );
  return system_settings_form($form);
}

/**
 * Admin form for disabling uservoice
 */
function scratchpadify_disable_uservoice_form(){
  $form['uservoice_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Uservoice enabled'),
    '#default_value' => variable_get('uservoice_enabled', 1),
    '#description' => t('Uncheck the box to disable uservoice')
  );
  return system_settings_form($form);
}

function scratchpadify_notification_message(){
  drupal_set_header('Content-type: text/html');
  echo variable_get('scratchpadify_notification_message', '');
  exit;
}

/**
 * Taskguide function
 */
function scratchpadify_taskguide(){
  return '<div class="mission" style="text-align: justify">
  <h1 style="text-decoration:underline;padding-bottom:20px">Some tips to get you started:</h1>
<p>Set up your Scratchpad by going through the list below. It can be useful to attend a '.l('training course','http://scratchpads.eu/scratchpad-training-courses').' or go through the '.l('manual of the basic training course','http://scratchpads.eu/sites/scratchpads.eu/files/ScratchpadTrainingBasic.doc').' on your own.
  <div>
    <div style="width:15%; float: left; text-align: right; clear: both;"><img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/information.png" style="padding:10px;"/></div>
    <div style="width:75%; float: left;">
      <h2>Site Information</h2><p>Change basic '.l('site information','admin/settings/site-information').', such as the site name, slogan, e-mail address, and mission. To present a static front page, specify the page address at the bottom of this page (after creating the respective page). Otherwise snippets of new content will appear on the front page as it is added to your site.*</p>
    </div>
  </div>
  <div>
    <div style="width:15%; float: left; text-align: right; clear: both;"><img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/theme.png" style="padding:10px;"/></div>
    <div style="width:75%; float: left;">
      <h2>Site Theme and Logo</h2><p>Change the site colour scheme, add a '.l('custom logo and change other settings','admin/build/themes/settings').' to make your site unique.*</p>
    </div>
  </div>
  <div>
    <div style="width:15%; float: left; text-align: right; clear: both;"><img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/layout.png" style="padding:10px;"/></div>
    <div style="width:75%; float: left;">
      <h2>Layout</h2><p>Change the layout of your site by changing the '.l('block','admin/build/block').' and '.l('menu','admin/build/menu').' items.*</p>
    </div>
  </div>
<div>
    <div style="width:15%; float: left; text-align: right; clear: both;"><img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/taxonomy.png" style="padding:10px;"/></div>
    <div style="width:75%; float: left;">
      <h2>Taxonomy</h2><p>Add a '.l('biological taxonomy','admin/content/taxonomy/add/vocabulary').' (vocabulary), and then '.l('import a classification','admin/content/taxonomy/classification/import').' into your new vocabulary. The classification is the backbone of your site and allows you to aggregate species pages with information from the web and information you have uploaded into your site. With a taxonomy you can automatically integrate and navigate content across your site.</p>
    </div>
  </div>
    <div>
    <div style="width:15%; float: left; text-align: right; clear: both;"><img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/users.png" style="padding:10px;"/></div>
    <div style="width:75%; float: left;">
      <h2>Users</h2><p>'.l('Add','admin/user/user/create').' or '.l('list','admin/user/user').' site users in different \'roles\'. Contributors can add and edit their own content, editors can also edit other peoples content, and maintainers can also access advanced administrative features. The '.l('groups','node/add/group').' feature allows users to create private areas that can be selectively shared.</p>
    </div>
  </div>
  <div>
    <div style="width:15%; float: left; text-align: right; clear: both;"><img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/content.png" style="padding:10px;"/></div>
    <div style="width:75%; float: left;">
      <h2>Content</h2><p>Use the Content menu in the sidebar to view (click on content type link), add (hover over content type link and click on the \'Add...\' icon which appears to the right) or import (hover over content type link and click on the \'Import...\' icon which appears to the right) data of this type. Use the '.l('list content','admin/content/node').' link to find and manage all content (nodes) within your site. Everything added to a site can be found from there. A useful sequence of adding data is starting with the bibliography followed by localities, specimens and images.</p>
    </div>
  </div>
     <div>
    <div style="width:15%; float: left; text-align: right; clear: both;"><img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/content.png" style="padding:10px;"/></div>
    <div style="width:75%; float: left;">
      <h2>Custom Content Types and Views</h2><p>'.l('Create new content types','admin/content/types/add').' tailored to your needs and '.l('add a view','admin/build/views/add').' to show data to their best advantage. Check the '.l('Manual for the advanved training course','http://scratchpads.eu/sites/scratchpads.eu/files/ScratchpadTrainingAdvanced.doc').' for instructions.*</p>
    </div>
  </div>
 <div>
    <div style="width:15%; float: left; text-align: right; clear: both;"><img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/language.png" style="padding:10px;"/></div>
    <div style="width:75%; float: left;">
      <h2>Languages</h2><p>Make your site multilingual by '.l('adding languages','admin/settings/language/add').', allowing users to provide translations of site content.*</p>
    </div>
  </div>  
    <div style="clear:both;">
    <p style="text-align:right">*Only site maintainers can change these settings.</p>
    <p>For more help check out the Scratchpad resources ('.l('Help','http://scratchpads.eu/help').', '.l('UserVoice','http://biodiversityinformatics.uservoice.com/forums/25435-scratchpads').', and the '.l('sandbox','http://sandbox.scratchpads.eu/').') on the '.l('Scratchpad project website','http://scratchpads.eu').'. If you are still stuck, '.l('contact us','http://scratchpads.eu/contact').' and we will get back to you as soon as possible.</p>
  </div>
</div>';
}


/**
 * Admin page
 *
 * @return unknown
 */
function scratchpadify_admin_page(){
  module_load_include('admin.inc','system');
  $blocks = array(
    array(
      'title' => t('Content/Data'),
      'description' => t("Your Scratchpad's data"),
      'position' => 'left',
      'content' => theme('admin_block_content', array(
          array(
            'localized_options' => array(),
            'href' => 'admin/content/node',
            'description' => t('List the pages/nodes of content on your Scratchpad, and perform bulk operations'),
            'title' => t('List')
          ),
          array(
            'localized_options' => array(),
            'href' => 'node/add',
            'description' => t('Add new content to your Scratchpad'),
            'title' => t('Add')
          ),
          array(
            'localized_options' => array(),
            'href' => 'admin/content/comment',
            'description' => t("List and edit Scratchpad comments and the comment moderation queue"),
            'title' => t('Comments')
          )
        )
      )
    ),
    array(
      'title' => t('User management'),
      'position' => 'left',
      'description' => t(" Manage your Scratchpad's users, groups and access to features"),
      'content' => theme('admin_block_content', array(
          array(
            'localized_options' => array(),
            'href' => 'admin/user/user',
            'description' => t('List users, and perform bulk operations'),
            'title' => t('List')
          ),
          array(
            'localized_options' => array(),
            'href' => 'admin/user/user/create',
            'description' => t('Add a new user'),
            'title' => t('Add')
          ),
          array(
            'localized_options' => array(),
            'href' => 'admin/user/settings',
            'description' => t('Configure default behavior of users, including registration requirements, e-mails, and user pictures'),
            'title' => t('User settings')
          )
        )
      )
    ),
    array(
      'title' => t('Taxonomy'),
      'position' => 'right',
      'description' => t("Manage the Taxonomies and terms which are on your Scratchpad"),
      'content' => theme('admin_block_content', array(
          array(
            'localized_options' => array(),
            'href' => 'admin/content/taxonomy',
            'description' => t("List the vocabularies on your Scratchpad"),
            'title' => t('List')
          ),
          array(
            'localized_options' => array(),
            'href' => 'admin/content/taxonomy/add/vocabulary',
            'description' => t("Add a new Vocabulary to your Scratchpad"),
            'title' => t('Add')
          ),
          array(
            'localized_options' => array(),
            'href' => 'admin/content/taxonomy/classification/import',
            'description' => t("Import a classification into a vocabulary in your Scratchpad"),
            'title' => t('Import')
          )
        )
      )
    ),
    array(
      'title' => t('Views'),
      'position' => 'right',
      'description' => t('Views are customized lists of content on your system; they are highly configurable and give you control over how lists of content are presented.'),
      'content' => theme('admin_block_content', array(
          array(
            'localized_options' => array(),
            'href' => 'admin/build/views',
            'title' => t('List'),
            'description' => t('List the different Views on your Scratchpad')
          ),
          array(
            'localized_options' => array(),
            'href' => 'admin/build/views/add',
            'title' => t('Add'),
            'description' => t('Add a new view to your Scratchpad')
          )
        )
      )
    ),
    array(
      'title' => t('Layout'),
      'position' => 'right',
      'description' => t("Change the layout settings on your Scratchpad"),
      'content' => theme('admin_block_content', array(
          array(
            'localized_options' => array(),
            'href' => 'admin/build/block',
            'description' => t("Configure what block content appears in your Scratchpad's sidebars and other regions"),
            'title' => t('Blocks')
          ),
          array(
            'localized_options' => array(),
            'href' => 'admin/build/menu',
            'description' => t("Control your Scratchpad's navigation menu, primary links and secondary links. as well as rename and reorganize menu items"),
            'title' => t('Menus')
          )
        )
      )
    )
  );
  return theme('admin_page', $blocks);
}