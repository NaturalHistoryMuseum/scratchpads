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
 * Taskguide function
 */
function scratchpadify_taskguide(){
  $output = '<style>
table.tasksguide {
width: 90%;
border: 0;
}
tdeighty {
width: 80px;
}
tdfoureighty {
width: 480px;
}
tr.tasksguideeven td{
font-weight: 900;
font-size: 120%;
}
tr.tasksguideodd td{
padding-left: 20px;
}
table.tasksguide td sup{
padding: 0 5px 0 5px;
}
.greyed{
color: #aaa;
background-color: #f8f8f8;
}
</style>
<h3>Content</h3>
<div align="center">
<table border="0" cellspacing="2" cellpadding="2" class="tasksguide">
  <tbody>
    <tr class="tasksguideeven">
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/simplePage.png" alt="" />
      </td>
      <td class="tdfoureighty">Simple Page<sup>1</sup><a href="http://www.editwebrevisions.info/help_page_short" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/simpleMaps.png" alt="" />
      </td>
      <td class="tdfoureighty">Simple Map<sup>1</sup><a href="http://www.editwebrevisions.info/help_map" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
    </tr>
    <tr class="tasksguideodd">
      <td class="tdfoureighty">Create a <a href="/node/add/page">basic page</a></td>
      <td class="tdfoureighty">Create a <a href="/map/macro">basic map</a></td>
    </tr>
    <tr class="tasksguideeven">
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/forum.png" alt="" />
      </td>
      <td class="tdfoureighty">Forum<sup>1</sup><a href="http://www.editwebrevisions.info/help_forum" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/filestore.png" alt="" />
      </td>
      <td class="tdfoureighty">Filestore<sup>1</sup><a href="http://www.editwebrevisions.info/help_filestore" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
    </tr>
    <tr class="tasksguideodd">
      <td class="tdfoureighty">Post a new <a href="/node/add/forum">forum topic</a>, or <a href="/forum">read the forums</a></td>
      <td class="tdfoureighty"><a href="/node/add/fileshare">Upload</a> a public or private file to the filestore</td>
    </tr>
    <tr class="tasksguideeven">
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/taxonomy.png" alt="" />
      </td>
      <td class="tdfoureighty">Taxonomy<sup>2</sup><a href="http://www.editwebrevisions.info/help_taxonomy" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/specimens.png" alt="" />
      </td>
      <td class="tdfoureighty">Specimens<sup>2</sup><!--<a href="http://www.editwebrevisions.info/help_specimen" target="_blank">--><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /><!--</a>--></td>
    </tr>
    <tr class="tasksguideodd">
      <td class="tdfoureighty"><a href="/admin/content/taxonomy">Create</a> or <a href="/admin/content/taxonomy/import">upload</a> a taxonomic hierarchy</td>
      <td class="tdfoureighty greyed">Create and <!--<a href="/admin/content/node_import">-->upload a list<!--</a>--> or add a <!--<a href="/node/add/specimen">-->single entry<!--</a>--></td>
    </tr>
    <tr class="tasksguideeven">
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/literature.png" alt="" />
      </td>
      <td class="tdfoureighty">Literature<sup>2</sup><a href="http://www.editwebrevisions.info/help_biblio" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/images.png" alt="" />
      </td>
      <td class="tdfoureighty">Images<sup>2</sup><a href="http://www.editwebrevisions.info/help_images" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
    </tr>
    <tr class="tasksguideodd">
      <td class="tdfoureighty">Upload a <a href="/biblio/import/form">reference list</a> or add a <a href="/node/add/biblio">single entry</a>.</td>
      <td class="tdfoureighty"><a href="/node/754">Upload single or multiple images</a> for annotation.</td>
    </tr>
    <tr class="tasksguideeven">
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/syndication.png" alt="" />
      </td>
      <td class="tdfoureighty">Syndication (RSS)<sup>2</sup><a href="http://www.editwebrevisions.info/help_syndication" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/customData.png" alt="" />
      </td>
      <td class="tdfoureighty">Custom Data<sup>3</sup><a href="http://www.editwebrevisions.info/help_cck" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
    </tr>
    <tr class="tasksguideodd">
      <td class="tdfoureighty"><a href="/admin/content/aggregator">Add a news feed</a> from another website</td>
      <td class="tdfoureighty"><a href="/admin/content/types/add">Create</a>, <a href="/admin/content/node_import">upload</a> and <a href="/admin/build/views/add">view</a> custom datasets.</td>
    </tr>
    <tr class="tasksguideeven">
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/dna.png" alt="" />
      </td>
      <td class="tdfoureighty">DNA Sequences<sup>2</sup><!--<a href="http://www.editwebrevisions.info/help_dna" target="_blank">--><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /><!--</a>--></td>
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/genes.png" alt="" />
      </td>
      <td class="tdfoureighty">Genes<sup>2</sup><!--<a href="http://www.editwebrevisions.info/help_gene" target="_blank">--><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /><!--</a>--></td>
    </tr>
    <tr class="tasksguideodd">
      <td class="tdfoureighty greyed">Upload DNA sequences or add a single entry</td>
      <td class="tdfoureighty greyed">Add new genes to label DNA sequences</td>
    </tr>
    <tr class="tasksguideeven">
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/view.png" alt="" />
      </td>
      <td class="tdfoureighty">Template Views<sup>2</sup><a href="http://www.editwebrevisions.info/help_cck_view" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/customView.png" alt="" />
      </td>
      <td class="tdfoureighty">Custom Views<sup>3</sup><a href="http://www.editwebrevisions.info/help_cck_view" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
    </tr>
    <tr class="tasksguideodd">
      <td class="tdfoureighty greyed">Integrate data on a new page with one of our templates</td>
      <td class="tdfoureighty greyed">Create a custom view of your data by creating a new template</td>
    </tr>
  </tbody>
</table>
</div>
<h3>Administration</h3>
<div align="center">
<table border="0" cellspacing="2" cellpadding="2" class="tasksguide">
  <tbody>
    <tr class="tasksguideeven">
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/information.png" alt="" />
      </td>
      <td class="tdfoureighty">Site Information<sup>1</sup><a href="http://www.editwebrevisions.info/help_siteinfo" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/profile.png" alt="" />
      </td>
      <td class="tdfoureighty">Your Profile<sup>1</sup><a href="http://www.editwebrevisions.info/help_users" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
    </tr>
    <tr class="tasksguideodd">
      <td class="tdfoureighty">Change the <a href="/admin/settings/site-information">site title, mission statement</a> and <a href="/admin/build/themes">logo</a></td>
      <td class="tdfoureighty"><a href="/user/';
      //dynamically set the ID number of the user logged in 
  global $user;
  $output .= $user->uid .'/edit">Edit your personal information</a>, including <a href="/user/'.$user->uid.'/edit/gmap_user">where you are</a></td>
    </tr>
    <tr class="tasksguideeven">
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/users.png" alt="" />
      </td>
      <td class="tdfoureighty">Users<sup>1</sup><a href="http://www.editwebrevisions.info/help_users" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/navigation.png" alt="" />
      </td>
      <td class="tdfoureighty">Site Navigation<sup>2</sup><a href="http://www.editwebrevisions.info/help_navigation" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
    </tr>
    <tr class="tasksguideodd">
      <td class="tdfoureighty"><a href="/admin/user/user/create">Add</a>, <a href="/admin/user/user">delete and edit</a> registered users.</td>
      <td class="tdfoureighty">Customize <a href="/admin/build/menu">menus</a>, <a href="/admin/build/block">blocks</a> and <a href="/admin/content/node">all site content</a>.</td>
    </tr>
    <tr class="tasksguideeven">
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/feedback.png" alt="" />
      </td>
      <td class="tdfoureighty">Feedback<sup>2</sup><a href="http://www.editwebrevisions.info/help_feedback" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/theme.png" alt="" />
      </td>
      <td class="tdfoureighty">Visual Theme<sup>2</sup><a href="http://www.editwebrevisions.info/help_themes" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
    </tr>
    <tr class="tasksguideodd">
      <td class="tdfoureighty"><a href="/admin/content/comment/list/approval">Moderate</a> comments</td>
      <td class="tdfoureighty">Change the <a href="/admin/build/themes">visual theme</a> of your site.</td>
    </tr>
    <tr class="tasksguideeven">
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/backup.png" alt="" />
      </td>
      <td class="tdfoureighty">Backup<sup>1</sup><a href="http://www.editwebrevisions.info/help_backup" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
      <td class="tdeighty" rowspan="2">
      <img src="'.drupal_get_path('module','scratchpadify').'/images/logs.png" alt="" />
      </td>
      <td class="tdfoureighty">Logs<sup>1</sup><a href="http://www.editwebrevisions.info/help_logs" target="_blank"><img src="'.drupal_get_path('module','scratchpadify').'/images/help.png" alt="" /></a></td>
    </tr>
    <tr class="tasksguideodd">
      <td class="tdfoureighty">
      <script type="text/javascript">
      hiddenmail  = \'<a hr\';
      hiddenmail += \'ef="m\';
      hiddenmail += \'ailto\';
      hiddenmail += \':scra\';
      hiddenmail += \'tchpa\';
      hiddenmail += \'d@nhm\';
      hiddenmail += \'.ac.u\';
      hiddenmail += \'k">Co\';
      hiddenmail += \'ntact\';
      hiddenmail += \' us</\';
      hiddenmail += \'a>\';
      document.write(hiddenmail);
      </script> to restore your site from backup.</td>
      <td class="tdfoureighty"><a href="/admin/logs/visitors">Who</a>, <a href="/admin/logs/pages">what</a> and <a href="/admin/logs/search">how</a> users access the site.</td>
    </tr>
  </tbody>
</table>
</div>
<!--break-->
<h3>
User Experience Key
</h3>
<ol>
  <li>None - no experience required</li>
  <li>Limited - basic experience needed</li>
  <li>Advanced - suitable for an experienced user</li>
</ol>';
  return $output;
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
            'description' => t('List the pages/nodes of content on your Scratchpad, and perform bulk operartions'),
            'title' => t('List')
          ),
          array(
            'localized_options' => array(),
            'href' => 'node/add',
            'description' => t('Add new content to your Scratchpad'),
            'title' => t('Add')
          )
        )
      )
    ),
    array(
      'title' => t('Comments'),
      'position' => 'left',
      'content' => theme('admin_block_content', array(
          array(
            'localized_options' => array(),
            'href' => 'admin/content/comment',
            'description' => t("List and edit Scratchpad comments and the comment moderation queue"),
            'title' => t('List')
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
            'href' => 'admin/user/user',
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