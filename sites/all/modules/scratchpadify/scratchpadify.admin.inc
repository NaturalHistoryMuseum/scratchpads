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
  $output = '<h1 style="padding:20px; border: solid 1px black">Click the links on your left to add new content to your site.</h1>';
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