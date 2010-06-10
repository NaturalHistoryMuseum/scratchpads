<?php
/**
 * @file
 * 
 * THE FOLLOWING IS COPIED PRETTY MUCH VERBATIM FROM THE "regally" MODULE.
 * OTHER PARTS ARE DEFINITELY INSPIRED BY THE "regally" MODULE.
 * 
 * Regally views integration.
 *
 * A gallery is made up of two views:
 * - a view of nodes (the images)
 * - a view of terms (the child galleries)
 */

/**
 * Implementation of hook_views_data_alter().
 * Add fields for image gallery (ie vocabulary terms) to the term_data table.
 */
function regally_views_data_alter(&$data) {
  // ----------------------------------------------------------------------
  // Relationships for cover node.
  //
  // These allow any node field to be used to create the gallery cover node.
  // The limitation, however, is that either consider the immediate gallery,
  // or a flat pool of the gallery and descendants.
  // Note that a bug in Views -- http://drupal.org/node/380560 -- will cause
  // an error message when adding node fields on these relationships.
  $data['term_data']['regally_cover_latest'] = array(
    'group' => t('Image gallery'),    
    'relationship' => array(
      'title' => t('Latest image'),
      'label'  => t('Cover image, latest'),
      'help' => t('Relate an image gallery to its most recently updated node (does not consider child galleries).'),
      'handler' => 'regally_handler_relationship_gallery_cover',
      'base'   => 'node',
      'field'  => 'nid',
      'correlated field' => 'tid',
      'subquery order' => ' gallery_cover_node.created DESC ',
    ),
  );
  $data['term_data']['regally_cover_oldest'] = array(
    'group' => t('Image gallery'),
    'relationship' => array(
      'title' => t('Oldest image'),
      'label'  => t('Cover image, oldest'),
      'help' => t('Relate an image gallery to its oldest node (does not consider child galleries).'),
      'handler' => 'regally_handler_relationship_gallery_cover',
      'base'   => 'node',
      'field'  => 'nid',
      'correlated field' => 'tid',
      'subquery order' => ' gallery_cover_node.created ASC ',
    ),
  );
  $data['term_data']['regally_cover_node_title'] = array(
    'group' => t('Image gallery'),
    'relationship' => array(
      'title' => t('First image by title'),
      'label'  => t('Cover image, first by title'),
      'help' => t('Relate an image gallery to its first node when sorted by title (does not consider child galleries).'),
      'handler' => 'regally_handler_relationship_gallery_cover',
      'base'   => 'node',
      'field'  => 'nid',
      'correlated field' => 'tid',
      'subquery order' => ' gallery_cover_node.title ASC ',
    ),
  );

  // ----------------------------------------------------------------------
  // Simple fields.

  // Gallery count.
  $data['term_data']['regally_count'] = array(
    'group' => t('Image gallery'),
    'field' => array(
      'title' => t('Count'),
      'help' => t('Count of items in a gallery.'),
      'handler' => 'regally_handler_field_gallery_count',
    ),
  );
  
  // ----------------------------------------------------------------------
  // Fields for cover image.
  //
  // These use a combination of code and separate queries to get a cover node.
  // This makes them more powerful that using the relationship cover node,
  // as we can consider child galleries recursively rather than just
  // flattening all descendant galleries.
  // We can also do complex things such as grab the top-most node from the
  // gallery according to how the view for that gallery sorts them.
  // The downside however is that without a relationship, the fields here are
  // all you've got.
  // To add more fields, define them on term_data and optionally add handlers.
  // See regally_handler_field_gallery_cover for more information.
  $data['term_data']['regally_latest_thumbnail'] = array(
    'group' => t('Image gallery'),    
    'field' => array(
      'title' => t('Latest image'),
      'help' => t('The most recently posted image in the gallery or its child galleries.'),
      'handler' => 'regally_handler_field_gallery_cover_thumbnail',
      'order clause' => 'n.sticky DESC, n.created DESC',
    ),
  );
  $data['term_data']['regally_latest_time'] = array(
    'group' => t('Image gallery'),    
    'field' => array(
      'title' => t('Last updated time'),
      'help' => t('The time of the most recently posted image in the gallery or its child galleries.'),
      'handler' => 'regally_handler_field_gallery_cover_latest_time',
      'order clause' => 'n.sticky DESC, n.created DESC',
    ),
  );
  $data['term_data']['regally_first_title'] = array(
    'group' => t('Image gallery'),
    'field' => array(
      'title' => t('First image by title'),
      'help' => t('The first posted image in the gallery or its child galleries.'),
      'handler' => 'regally_handler_field_gallery_cover_thumbnail',
      'order clause' => 'n.sticky DESC, n.title ASC',
    ),
  );
  $data['term_data']['regally_oldest_thumbnail'] = array(
    'group' => t('Image gallery'),
    'field' => array(
      'title' => t('Oldest image'),
      'help' => t('The first posted image in the gallery or its child galleries.'),
      'handler' => 'regally_handler_field_gallery_cover_thumbnail',
      'order clause' => 'n.sticky DESC, n.created ASC',
    ),
  );
}

/**
 * Implementation of hook_views_handlers().
 */
function regally_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'regally') . '/views',
    ),
    'handlers' => array(
      'regally_handler_field_gallery_count' => array(
        'parent' => 'views_handler_field_taxonomy',
      ),
      'regally_handler_field_gallery_cover' => array(
        'parent' => 'views_handler_field_taxonomy',
     ),
      'regally_handler_field_gallery_cover_thumbnail' => array(
        'parent' => 'regally_handler_field_gallery_cover',
     ),
      'regally_handler_field_gallery_cover_latest_time' => array(
        'parent' => 'regally_handler_field_gallery_cover',
      ),
      'regally_handler_relationship_gallery_cover' => array(
        'parent' => 'views_handler_relationship',
      ),
      'regally_plugin_display_regally' => array(
         'parent' => 'views_plugin_display_page',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_plugins().
 */
function regally_views_plugins() {
  return array(
    'display' => array(
      /**
       * The image gallery display: like a page, but with a subgallery
       * list embedded above it.
       * Uses a handler rather than a tpl file.
       */
      'regally' => array(
        'title' => t('Gallery page'),
        'help' => t('Display the view as a gallery of images, with a URL and menu links.'),
        'parent' => 'page',
        'handler' => 'regally_plugin_display_regally',
        'theme' => 'views_view',
        'uses hook menu' => TRUE,
        'use ajax' => TRUE,
        'use pager' => TRUE,
        'accept attachments' => TRUE,
        'admin' => t('Page'),
        'help topic' => 'display-page', // TODO ??
        'path' => drupal_get_path('module', 'regally') . '/views',
      ),
    ),
    'style' => array(
      /**
       * A list style for term lists.
       */
      'regally_terms' => array(
        'title'  => 'Subgallery list',
        'help' => t('Displays subgalleries in a formatted list.'),
        'parent' => 'list',
        'handler' => 'views_plugin_style_list',
        'theme path' => drupal_get_path('module', 'regally') . '/views/theme',
        'theme file' => 'theme.inc',
        'theme' => 'regally_view_regally_terms',
        'uses row plugin' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'help topic' => 'style-list',
      ),
    ),
  );
}