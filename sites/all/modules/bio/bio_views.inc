<?php
// $Id: bio_views.inc,v 1.1.2.5 2008/01/25 07:33:54 webchick Exp $

/**
 * @file
 * Views integration for Bio module.
 * 
 * The Bio module integrates with Views by re-exposing the Bio node for an
 * author as part of any Views query.
 *
 * For example, it is possible to perform queries like:
 * - Find all blog posts by authors who have Bio nodes tagged with the term
 *   "kayaking."
 * - Find all the Bio nodes by authors who have a first name (where "first
 *   name" is a field) like "joe."
 * - Find all the forum posts by the most popular authors, ranked by most
 *   viewed Bio pages.
 * 
 * This file also provides two filters: 
 * - Node: Type is Bio node:
 *   This is useful for shipping premade views that rely on Bio, but are not
 *   tied to a particular type of node acting as the Bio node (e.g. on one site
 *   it might be "Biography," and on another "profile").
 * - Bio: Author has Bio node:
 *   This is useful for only returning results where the author has created a
 *   Bio node. That way, if you are exposing Bio fields (e.g. the "interests"
 *   taxonomy), you can be sure that a Bio node exists for every other node
 *   returned in the query.
 *
 * See the README for more information on creating Views with Bio information.
 */

/**
 * Real implementation of hook_views_table_alter().
 */
function _bio_views_tables_alter(&$tables) {
  // Amass some data to use later.
  $bio_type = bio_get_type();
  $bio_name = node_get_types('name', $bio_type);
  
  if (module_exists('taxonomy')) {
    $vocabs = array_keys(taxonomy_get_vocabularies($bio_type));
  }
  
  // Fetch some CCK info if CCK is installed.
  if (module_exists('content')) {
    $content_type_info = content_types($bio_type);
    if (is_array($content_type_info['fields'])) {
      $bio_fields = array_keys($content_type_info['fields']);
    }
  }

  // The first step is to copy the node table and alias it as the "node" table.
  // This allows us to access all node table data. Later, we can point other
  // modules at the "bio" table and it will behave just like the node table.
  $old_node = $tables['node'];
  // TODO: Not sure what this does, remove it.
  unset($old_node['provider']);
  $old_node['join'] = array(
    // TODO: Default option is left. I think inner is probably better?
    //'type' => 'inner',
    'left' => array(
      'table' => 'node',
      'field' => 'uid'
    ), 
    'right' => array(
      'field' => 'uid'
    ),
    'extra' => array(
      // TODO: Will I need to specify bio.type instead of type?
      'type' => $bio_type,
    ),
  );
  
  // Remove some fields, filters, and sorts that don't make sense for a joined
  // Bio table.

  // Node: Type.
  unset($old_node['fields']['type']);
  // Node: Type.
  unset($old_node['filters']['type']);
  // Node: Author is anonymous.
  unset($old_node['filters']['anon']);
  // Node: Author is current user.
  unset($old_node['filters']['currentuid']);
  // Node: Current user authored or commented.
  unset($old_node['filters']['currentuidtouched']);
  // Node: Type.
  unset($old_node['sorts']['type']);
  // Node: Random.
  unset($old_node['sorts']['random']);
  
  // For bio_node, nid is a real column.
  unset($old_node['fields']['nid']['notafield']);

  // Prefix descriptions with "Bio:" to differentiate them from standard stuff.
  _bio_rename_table_labels($old_node, $bio_name);

  // Save our worked up node table back in the array.
  $tables['bio'] = $old_node;
  
  // Bio copies all other tables and renames them bio_foo. This allows us to
  // re-expose all the data of the Bio node in a forward compatible,
  // transparent manner. We assume all tables connect to node (via
  // intermediate tables). For now, a safe assumption.

  // Without this copy, Apache would spin off and die a horrible death.
  $table_copy = $tables;
  foreach ($table_copy as $name => $data) {

    // We can skip the node and bio tables. Other tables can be skipped, too.
    // TODO: Is there a use case for including comments? Comment stats are
    // pretty useful, but do we really need: "show me the comments on the
    // profiles of authors who wrote a blog post about foo?"
    if (in_array($name, array('node', 'bio', 'users', 'users_roles', 'comments', 'book', 'book_parent_node', 'temp_search_results'))) {
      continue;
    }
    
    // skip anything that joins against UID, we don't need that (e.g. profile_xxx)
    if ($data['join']['left']['table'] == 'node' && $data['join']['left']['field'] == 'uid') {
      continue;
    }
    
    // we should skip tables that don't relate to bio node types specifically
    // e.g. taxonomy vocabs that don't apply, cck fields that aren't in the bio node type, etc.
    
    // Skip unneeded taxonomy vocabularies.
    if (strpos($name, 'term_node_') === 0) {
      $vid = substr($name, 10);
      if (!in_array($vid, $vocabs)) {
        continue;
      }
    }
    // Skip fields not assigned to the Bio node type.
    elseif (strpos($name, 'node_data_') === 0) {
      $field = substr($name, 10);
      if (!in_array($field, $bio_fields)) {
        continue;
      }
    }

    // If the table joins directly to node, make it's new join point "bio."
    // If it joins against something else, assume that we'll create a "bio_X"
    // for it at some point.
    if ($data['join']['left']['table'] == 'node') {
      $data['join']['left']['table'] = 'bio';
    }
    else {
      $data['join']['left']['table'] = 'bio_'. $data['join']['left']['table'];
    }
    _bio_rename_table_labels($data, $bio_name);
    $tables["bio_$name"] = $data;
  }

  // Now that all the bio_* tables are ready, add some other useful filters to
  // the mix.

  // Node: Type is Bio node -- cribbed from views_node.inc.
  $tables['node']['filters']['biotype'] = array(
    'field' => 'type',
    'name' => t('Node: Type is Bio node'),
    'operator' => array('=' => t('Is'), '!=' => t('Is not')),
    'value' => _bio_provide_hidden_form($bio_type, $bio_name),
    'help' => t('This allows you to filter by whether or not the node is a bio node. Select "Is" to limit to bio nodes, select "is not" to limit to all other content types'),
  );

  // Bio: Author has a Bio -- limits the results (e.g. blog posts) to those
  // where the author has a Bio. Relies on http://drupal.org/node/142504.
  $tables['bio']['filters']['author_has_bio'] = array(
    'field' => 'uid',
    'name' => t('Bio: Author has a Bio'),
    'operator' => array('IS NOT' => t('Does'), 'IS' => t('Does not')),
    'value' => _bio_provide_hidden_form(NULL, t('Have a bio node')),
    'help' => t('Filter in only nodes for which the author has or does not have a bio node.'),
    'handler' => 'views_handler_filter_null',
  );

  // Override the default handler on the "is new" filter, as it needs some very
  // specific table names.
  $tables['bio_history']['filters']['timestamp']['handler'] = 'bio_handler_filter_isnew';
}

/**
 * Helper function to rename Node: Title => Bio: Node: Title, etc.
 */
function _bio_rename_table_labels(&$table, $bio_name) {
  foreach (array('fields', 'sorts', 'filters') as $section) {
    if (is_array($table[$section])) {
      foreach ($table[$section] as $id => $object) {
        $table[$section][$id]['name'] = $bio_name .': '. $object['name'];
      }
    }
  }
}

/**
 * Used to force the value of certain form fields by converting to markup.
 */
function _bio_provide_hidden_form($value, $markup) {
  $form['text'] = array(
    '#type' => 'markup',
    '#value' => $markup,
  );
  $form['value'] = array(
    '#type' => 'value',
    '#value' => $value,
  );

  return $form;
}

/**
 * Custom filter for new content. Copied directly from views_node.inc and modified to use bio_* tables.
 */
function _bio_handler_filter_isnew($op, $filter, $filterinfo, &$query) {
  global $user;
  if (!$user || !$user->uid) {
    return;
  }

  // Hey, Drupal kills old history, so nodes that haven't been updated
  // since NODE_NEW_LIMIT are bzzzzzzzt outta here!

  $limit = time() - NODE_NEW_LIMIT;

  $query->ensure_table('bio_history');
  if (module_exists('comment')) {
    $query->ensure_table('bio_node_comment_statistics');
    $clause = ("OR bio_node_comment_statistics.last_comment_timestamp > (***CURRENT_TIME*** - $limit)");
    $clause2 = "OR bio_history.timestamp < bio_node_comment_statistics.last_comment_timestamp";
  }

  // NULL means a history record doesn't exist. That's clearly new content.
  // Unless it's very very old content. Everything in the query is already
  // type safe cause none of it is coming from outside here.
  $query->add_where("(bio_history.timestamp IS NULL AND (bio.changed > (***CURRENT_TIME***-$limit) $clause)) OR bio_history.timestamp < bio.changed $clause2");
}

/**
 * Real implementation of hook_views_query_alter().
 * 
 * For any bio_* table, we need to make sure the bio_nid is added.
 */
function _bio_views_query_alter(&$query, $view, $summary, $level) {
  // See if we need to add the Bio module.
  if (_bio_should_add_nid($query, $view)) {
    $query->ensure_table('bio');
    $query->add_field('nid', 'bio', 'bio_nid');
  }
}

/**
 * Helper function to determine if we need bio.nid in the query.
 * 
 * @todo This looks like a bit of a hack. What happens if I install the biology
 * module? :P
 */
function _bio_should_add_nid($query, $view) {
  foreach ($view->field as $field) {
    if (strpos($field['tablename'], 'bio') === 0) {
      return true;
    }
  }

  foreach ($view->filter as $filter) {
    if (strpos($filter['tablename'], 'bio') === 0) {
      return true;
    }
  }

  return false;
}

/**
 * Real implemenation of hook_views_default_tables().
 *
 * Default views are:
 * - recent_biographies:
 *   A listing of teasers of recent Bio nodes, regradless of what the Bio node
 *   type is.
 * - author_tracker:
 *   Just like the usual tracker, but using Bio node titles instead of the user
 *   name for the "author" column.
 *
 * These Views are disabled by default.
 */
function _bio_views_default_views() {
  // A little set up to make these views look nicer :-)
  $bio_type = bio_get_type();
  $bio_name = node_get_types('name', $bio_type);
  
  $view = new stdClass();
  $view->disabled = true;
  $view->name = 'recent_biographies';
  $view->description = 'A list of recently created biographies, from the bio module. The url changes to whatever your bio node type is, as does the title.';
  $view->access = array(
  );
  $view->view_args_php = '';
  $view->page = TRUE;
  $view->page_title = "Recent $bio_name Entries";
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = '';
  $view->page_empty_format = '1';
  $view->page_type = 'teaser';
  $view->url = $bio_type;
  $view->use_pager = TRUE;
  $view->nodes_per_page = '10';
  $view->sort = array(
    array(
      'tablename' => 'node',
      'field' => 'created',
      'sortorder' => 'DESC',
      'options' => 'normal',
    ),
  );
  $view->argument = array(
  );
  $view->field = array(
  );
  $view->filter = array(
    array(
      'tablename' => 'node',
      'field' => 'biotype',
      'operator' => '=',
      'options' => '',
      'value' => 'bio',
    ),
  );
  $view->exposed_filter = array(
  );
  $view->requires = array(node);
  $views[$view->name] = $view;
  
  $view = new stdClass();
  $view->disabled = true;
  $view->name = 'tracker_bio';
  $view->description = 'Shows all new activity on system using the bio node title as the author name.';
  $view->access = array(
  );
  $view->view_args_php = '';
  $view->page = TRUE;
  $view->page_title = 'Recent posts';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = '';
  $view->page_empty_format = '1';
  $view->page_type = 'table';
  $view->url = 'tracker';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '25';
  $view->menu = TRUE;
  $view->menu_title = 'Recent posts';
  $view->menu_tab = FALSE;
  $view->menu_tab_weight = '0';
  $view->menu_tab_default = FALSE;
  $view->menu_tab_default_parent = NULL;
  $view->menu_parent_tab_weight = '0';
  $view->menu_parent_title = '';
  $view->sort = array(
    array(
      'tablename' => 'node_comment_statistics',
      'field' => 'last_comment_timestamp',
      'sortorder' => 'DESC',
      'options' => 'normal',
    ),
  );
  $view->argument = array(
    array(
      'type' => 'uid',
      'argdefault' => '2',
      'title' => 'recent posts for %1',
      'options' => '',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $view->field = array(
    array(
      'tablename' => 'node',
      'field' => 'type',
      'label' => 'Type',
    ),
    array(
      'tablename' => 'node',
      'field' => 'title',
      'label' => 'Title',
      'handler' => 'views_handler_field_nodelink_with_mark',
      'options' => 'link',
    ),
    array(
      'tablename' => 'bio',
      'field' => 'title',
      'label' => 'Author',
      'handler' => 'views_handler_field_nodelink',
      'options' => 'link',
    ),
    array(
      'tablename' => 'node_comment_statistics',
      'field' => 'comment_count',
      'label' => 'Replies',
      'handler' => 'views_handler_comments_with_new',
    ),
    array(
      'tablename' => 'node_comment_statistics',
      'field' => 'last_comment_timestamp',
      'label' => 'Last Post',
      'handler' => 'views_handler_field_since',
    ),
  );
  $view->filter = array(
    array(
      'tablename' => 'node',
      'field' => 'status',
      'operator' => '=',
      'options' => '',
      'value' => '1',
    ),
    array(
      'tablename' => 'node',
      'field' => 'biotype',
      'operator' => '!=',
      'options' => '',
      'value' => 'bio',
    ),
  );
  $view->exposed_filter = array(
  );
  $view->requires = array(node_comment_statistics, node, bio);
  $views[$view->name] = $view;

  return $views;
}
