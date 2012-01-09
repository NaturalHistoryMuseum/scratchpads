<?php

/**
 * Implementation of hook_form().
 */
function project_project_form($node, $form_state) {
  global $user;
  project_project_set_breadcrumb($node);

  $form = array();

  // Set an #after_build where we load JS used for this form so that the JS is
  // always loaded, even when the form is cached (e.g. after a validation
  // failure).
  $form['#after_build'] = array('project_project_form_after_build');

  /* Project taxonomy */
  if (project_use_taxonomy()) {
    $project_type_vid = _project_get_vid();
    $tree = taxonomy_get_tree($project_type_vid);
    $top_level = array();
    $options = array();
    foreach ($tree as $i => $term) {
      if ($term->parents[0] == 0) {
        $last_top = $term->tid;
        $top_level[$term->tid] = check_plain($term->name);
      }
      else {
        $options[$last_top][$term->tid] = $term->name;
      }
    }
    // See if there are any project specific taxonomy terms already
    // saved in this node (i.e. we're editing an existing project) and
    // if so, extract the right default values for our custom form
    // elements...
    $current_top = NULL;
    $current_options = array();

    if (!empty($node->taxonomy)) {
      // Depending on whether we're previewing the node or not,
      // $node->taxonomy will be provided in one of two ways.
      if (isset($form_state['node_preview'])) {
        // In node previews, $node->taxonomy is an array of vocabularies,
        // each of which is an array of selected tids.
        if (isset($node->taxonomy[$project_type_vid])) {
          foreach ($node->taxonomy[$project_type_vid] as $key => $value) {
            if (isset($top_level[$key])) {
              $current_top = $key;
            }
            else {
              $current_options[$key] = $key;
            }
          }
        }
      }
      else {
        // $node->taxonomy is an array of term objects
        // when we're not previewing the node.
        foreach ($node->taxonomy as $tid => $obj) {
          if (isset($top_level[$tid])) {
            $current_top = $tid;
          }
          else {
            $current_options[$tid] = $tid;
          }
        }
      }
    }

    $form['project_taxonomy'] = array(
      '#type' => 'fieldset',
      '#weight' => '-30',
      '#title' => t('Project categories'),
      '#collapsible' => TRUE,
      '#theme' => 'project_project_node_form_taxonomy',
    );

    $form['project_taxonomy']['project_type'] = array(
      '#title' => t('Project type'),
      '#type' => 'radios',
      '#options' => $top_level,
      '#default_value' => $current_top,
      '#required' => TRUE,
    );
    $select_size = max(5, 2*count($top_level));
    foreach (array_keys($top_level) as $tid) {
      if (isset($options[$tid])) {
        $values = $options[$tid];
        $form['project_taxonomy']["tid_$tid"] = array(
          '#title' => t('@type categories', array('@type' => $top_level[$tid])),
          '#type' => 'select',
          '#multiple' => TRUE,
          '#options' => $values,
          '#default_value' => $current_options,
          '#attributes' => array('size' => min($select_size, count($values))),
        );
      }
      else {
        // provide a div for all top-level terms so that outside elements
        // (vocabularies) can be shifted into the project taxonomy fieldset
        // in the same paradigm as the modules sub-terms
        $form['project_taxonomy']["tid_$tid"] = array(
          '#type' => 'item',
          '#value' => '',
          '#id' => 'edit-tid-' . $tid,
        );
      }
    }
  }

  /* Project properties */
  // We can't put the title and body inside $node->project or core gets
  // confused (e.g node_body_field() and friends).  So, we put the core node
  // fields in their own fieldset (for which is #tree is FALSE).
  $form['project_node'] = array(
    '#type' => 'fieldset',
    '#title' => t('Project information'),
    '#collapsible' => TRUE,
  );
  $form['project_node']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Project title'),
    '#default_value' => isset($node->title) ? $node->title : NULL,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  // This is sort of a hack: We want the 'uri' to be in the $node->project
  // array during validation and submission of this form, to protect the $node
  // namespace, even though from a usability standpoint, this field belongs
  // right up next to the title.  So, we add a 'project' subarray in here
  // for which #tree is TRUE, and put the 'uri' field in there.  That way, it
  // still lives inside the "Project information" fieldset as far as the UI is
  // concerned, but the value shows up in the $node->project array for
  // validation and submission as far as FAPI is concerned.
  $form['project_node']['project'] = array('#tree' => TRUE);

  // Setup sandbox checkbox for new projects
  if (!isset($node->nid)) {
    // Check permissions and set sandbox value and form element status
    if (user_access('create sandbox projects') && !user_access('create full projects')) {
      // Force sandbox, disable checkbox
      $is_sandbox = isset($node->project['sandbox']) ? $node->project['sandbox'] : TRUE;
      $disabled = TRUE;
    }
    elseif (!user_access('create sandbox projects') && user_access('create full projects')) {
      // Force full project, disable checkbox
      $is_sandbox = isset($node->project['sandbox']) ? $node->project['sandbox'] : FALSE;
      $disabled = TRUE;
    }
    elseif ((user_access('create sandbox projects') && user_access('create full projects')) || user_access('administer projects')) {
      // User has full permissions, allow checkbox, use default sandbox value
      $is_sandbox = variable_get('project_sandbox_default', FALSE);
      $disabled = FALSE;
    }
    $form['project_node']['project']['sandbox'] = array(
      '#type' => 'checkbox',
      '#title' => t('Sandbox'),
      '#default_value' => $is_sandbox,
      '#description' => '', // @TODO need a good generic description of sandboxes
      '#disabled' => $disabled,
    );
  }
  else {
    $is_sandbox = $node->project['sandbox'];
    $form['project_node']['project']['sandbox'] = array(
      '#type' => 'value',
      '#value' => $is_sandbox,
    );
    if ($is_sandbox) {
      $form['project_node']['project']['sandbox_help'] = array(
        '#type' => 'markup',
        '#value' => t('This project is currently a sandbox.'),
        '#prefix' => '<div>', // Required to show up in fieldset, see http://api.drupal.org/api/drupal/developer--topics--forms_api_reference.html/6#markup
        '#suffix' => '</div>',
      );
      if (project_promote_project_access($node)) {
        $form['project_node']['project']['sandbox_help']['#value'] = t('This project is currently a sandbox. <a href="!url">Promote this project</a>', array('!url' => url("node/$node->nid/edit/promote")));
      }
    }

  }

  $form['project_node']['project']['uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Short project name'),
    '#default_value' => isset($node->project['uri']) ? $node->project['uri'] : NULL,
    '#size' => 40,
    '#maxlength' => 50,
    '#description' => (variable_get('project_enable_alias', TRUE)) ? t('This will be used to generate a /project/&lt;shortname&gt;/ URL for your project.') : '',
    // Only mark this item required if the project is not a sandbox, and it has
    // already been created, since project.js will mark it as required
    // automatically if it's a new project. This is safe to mark as not required
    // since project_project_validate() checks that this value is not empty.
    '#required' => (!$is_sandbox && isset($node->nid)) ? TRUE : FALSE,
  );
  // Updating of the uri has been disabled.
  if (!variable_get('project_allow_uri_update', TRUE)) {
    // For new projects, allow editing for initial submission, and warn the
    // user that the field will not be editable afterwards.
    if (empty($node->nid)) {
      $form['project_node']['project']['uri']['#description'] .= ' '. t('You may not edit this value after your project has been saved.');
    }
    // Convert the form element to an item so it's not editable.  #value
    // and #input need to be set explicitly so the uri value will be passed
    // to the submit handler.
    else {
      $form['project_node']['project']['uri']['#type'] = 'item';
      $form['project_node']['project']['uri']['#value'] = $node->project['uri'];
      $form['project_node']['project']['uri']['#input'] = TRUE;
    }
  }

  // If project is a sandbox, and it is an existing node, and we are
  // auto-generating the short name, disable the short name field
  if (variable_get('project_sandbox_numeric_shortname', FALSE) && $is_sandbox && isset($node->nid)) {
    $form['project_node']['project']['uri']['#disabled'] = TRUE;
  }

  $form['project_node']['body_field'] = node_body_field($node, t('Description'), 1);

  $form['project'] = array(
    '#type' => 'fieldset',
    '#title' => t('Project resources'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $form['project']['homepage'] = array(
    '#type' => 'textfield',
    '#title' => t('Homepage'),
    '#default_value' => isset($node->project['homepage']) ? $node->project['homepage'] : NULL,
    '#size' => 40,
    '#maxlength' => 255,
    '#description' => t('Link to project homepage.'),
  );
  $form['project']['documentation'] = array(
    '#type' => 'textfield',
    '#title' => t('Documentation'),
    '#default_value' => isset($node->project['documentation']) ? $node->project['documentation'] : NULL,
    '#size' => 40,
    '#maxlength' => 255,
    '#description' => t('Link to project documentation.'),
  );
  $form['project']['license'] = array(
    '#type' => 'textfield',
    '#title' => t('License'),
    '#default_value' => isset($node->project['license']) ? $node->project['license'] : NULL,
    '#size' => 40,
    '#maxlength' => 255,
    '#description' => t('Link to project license.'),
  );
  $form['project']['screenshots'] = array(
    '#type' => 'textfield',
    '#title' => t('Screenshots'),
    '#default_value' => isset($node->project['screenshots']) ? $node->project['screenshots'] : NULL,
    '#size' => 40,
    '#maxlength' => 255,
    '#description' => t('Link to project screenshots.'),
  );
  $form['project']['changelog'] = array(
    '#type' => 'textfield',
    '#title' => t('Changelog'),
    '#default_value' => isset($node->project['changelog']) ? $node->project['changelog'] : NULL,
    '#size' => 40,
    '#maxlength' => 255,
    '#description' => t('Link to changelog.'),
  );
  $form['project']['demo'] = array(
    '#type' => 'textfield',
    '#title' => t('Demo site'),
    '#default_value' => isset($node->project['demo']) ? $node->project['demo'] : NULL,
    '#size' => 40,
    '#maxlength' => 255,
    '#description' => t('Link to a live demo.'),
  );

  return $form;
}

/**
 * After-build callback for the project_project node form.
 *
 * Add JS for use on the project_project form because with the form cached
 * the main form function isn't called again (and so JS isn't loaded).
 */
function project_project_form_after_build($form, &$form_state) {
  // Since this form is used infrequently, don't allow the js to be aggregated.
  drupal_add_js(drupal_get_path('module', 'project') .'/project.js', 'module', 'header', FALSE, TRUE, FALSE);

  // The sandbox JS logic depends on a setting, so we need to pass that down
  // to the JS here as well.
  drupal_add_js(array('project' => array('project_sandbox_numeric_shortname' => variable_get('project_sandbox_numeric_shortname', FALSE))), 'setting');

  return $form;
}

/**
 * Implementation of hook_validate().
 *
 * @param $node
 *   An object containing values from the project node form.  Note that since
 *   this isn't a fully-loaded $node object, not all values will necessarily
 *   be in the same location as they would after a node_load().
 */
function project_project_validate(&$node) {
  // Bail if user hasn't done a preview yet.
  if (!isset($node->title)) {
    return $node;
  }

  // Make sure title isn't already in use
  if (db_result(db_query("SELECT COUNT(*) FROM {node} WHERE type = '%s' AND status = %d AND title = '%s' AND nid <> %d", $node->type, 1, $node->title, $node->nid))) {
    form_set_error('title', t('This project name is already in use.'));
  }

  // Validate uri.
  if ($error = project_validate_project_shortname($node)) {
    form_set_error('project][uri', $error);
  }

  // Make sure all URL fields actually contain URLs.
  $fields = array(
    'homepage' => t('Homepage'),
    'changelog' => t('Changelog'),
    'demo' => t('Demo site'),
  );
  foreach ($fields as $uri => $name) {
    if ($node->project[$uri] && !preg_match('/^(http|https|ftp):\/\//i', $node->project[$uri])) {
      form_set_error("project][$uri", t('The %field field is not a valid URL.', array('%field' => $name)));
    }
  }
}

function project_project_set_breadcrumb($node = NULL, $extra = NULL) {
  $breadcrumb = project_project_get_breadcrumb($node, $extra);
  drupal_set_breadcrumb($breadcrumb);
}

function project_project_get_breadcrumb($node = NULL, $extra = NULL) {
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), NULL);

  /*
    @TODO: This is not an optimal way to do this, because it makes the
    assumption that there is only one menu item that links to /project (or
    whatever it happens to be called).  Also, it makes the assumption that the
    URL alias is intact for the project node (in other words, it's path is
    aliased to 'project/<project short name>' However, since in D6 we no
    longer have $_menu I'm not sure of a better way to do this.
  */
  if (!empty($node->path)) {
    // Get the path of the project node and remove the name of the project.
    $path = array();
    $path = explode('/', $node->path);
    $path = array_slice($path, 0, count($path) - 1);
    $path = implode('/', $path);
    $menu_link = db_fetch_object(db_query("SELECT * FROM {menu_links} ml INNER JOIN {menu_router} m ON m.path = ml.router_path WHERE ml.hidden = %d AND ml.link_path = '%s' ORDER BY ml.weight", 0, $path));
    if (!empty($menu_link)) {
      $breadcrumb[] = l($menu_link->link_title, 'project', array('title' => t('Browse projects')));
    }
  }

  if (!empty($node->nid) && project_use_taxonomy()) {
    $result = db_query(db_rewrite_sql('SELECT t.tid, t.* FROM {term_data} t INNER JOIN {term_hierarchy} h ON t.tid = h.tid INNER JOIN {term_node} r ON t.tid = r.tid WHERE h.parent = %d AND t.vid = %d AND r.vid = %d', 't', 'tid'), 0, _project_get_vid(), $node->vid);
    if ($term = db_fetch_object($result)) {
      $breadcrumb[] = l($term->name, project_term_path($term));
    }
  }

  if (is_array($extra)) {
    $breadcrumb = array_merge($breadcrumb, $extra);
  }
  elseif ($extra && !empty($node)) {
    $breadcrumb[] = l($node->title, 'node/'. $node->nid);
  }
  return $breadcrumb;
}

function project_project_view($node, $teaser = false, $page = false) {
  $node = node_prepare($node, $teaser);

  if ($page) {
    // Breadcrumb navigation
    project_project_set_breadcrumb($node);

    // If theme_project_release_project_download_table is implemented, format
    // the download table.  If this function is not implemented (eg. if the
    // project_release module is not enabled), there will not be an error
    // but of course there will be no release table.
    $project_table_output = theme('project_release_project_download_table', $node);
    if (!empty($project_table_output)) {
      $node->content['download_table'] = array(
        '#value' => $project_table_output,
        '#weight' => 1,
      );
    }

    // Retrieve nested array of sections of links to display on node page.
    $all_links = project_get_project_link_info($node);

    // Format links in $all_links for display in the project_project node.
    // Keep track of the section with the heaviest weight (which will be last)
    // so we can add a final clear after it to make sure the floating link
    // sections do not interfere with other formatting in the node's content.
    $max_weight = -10000;
    $last_section = '';
    foreach($all_links as $section => $values) {
      // Only add the section if there are links, and section type is "inline".
      if (!empty($values['links']) && (empty($values['type']) || $values['type'] == 'inline')) {
        $weight = !empty($values['weight']) ? $values['weight'] : 0;
        $node->content[$section] = array(
          '#value' => theme('item_list', $values['links'], isset($values['name']) ? $values['name'] : NULL),
          '#weight' => !empty($values['weight']) ? $values['weight'] : 0,
          '#prefix' => '<div class="project-links-section" id="project-links-section-'. $section .'">',
          '#suffix' => '</div>',
        );
        if (!empty($values['clear'])) {
          $node->content[$section]['#suffix'] .= '<br style="clear:both;" />';
        }
        if ($weight >= $max_weight) {
          $last_section = $section;
          $max_weight = $weight;
        }
      }
    }
    // We only want to add a clearing <br> after the final section if that
    // section didn't already add a clear for itself (e.g. the heaviest
    // section might already clear from hook_project_page_link_alter()).
    if (empty($all_links[$last_section]['clear']) && !empty($last_section)) {
      $node->content[$last_section]['#suffix'] .= '<br style="clear:both;" />';
    }
  }
  return $node;
}

/**
 * hook_nodeapi() implementation specific for project nodes.
 * @see project_nodeapi().
 */
function project_project_nodeapi(&$node, $op, $arg) {
  $language = isset($node->language) ? $node->language : '';
  switch ($op) {
    case 'presave':
      // Remove terms from vocabularies that have associations for other
      // project types.
      if (!empty($node->taxonomy)) {
        foreach ($node->taxonomy as $vid => $terms) {
          $associated = variable_get('project_type_associated_tid_' . $vid, FALSE);
          if ($associated && $associated != $node->project_type) {
            unset($node->taxonomy[$vid]);
          }
        }
      }
      break;

    case 'insert':
      _project_save_taxonomy($node);
      if (module_exists('path') && variable_get('project_enable_alias', TRUE)) {
        path_set_alias("node/$node->nid", 'project/'. $node->project['uri'], NULL, $language);
      }
      break;

    case 'update':
      _project_save_taxonomy($node);
      if (module_exists('path') && variable_get('project_enable_alias', TRUE)) {
        path_set_alias("node/$node->nid");  // Clear existing alias.
        path_set_alias("node/$node->nid", 'project/'. $node->project['uri'], NULL, $language);
      }
      break;
  }
}

function project_project_insert($node) {
  // Auto generate the short name (uri) if necessary. We have to do this here,
  // since we need the node ID to exist before storing the short name.
  if (variable_get('project_sandbox_numeric_shortname', FALSE) && $node->project['sandbox']) {
    $node->project['uri'] = $node->nid;
  }
  db_query("INSERT INTO {project_projects} (nid, uri, homepage, changelog, demo, screenshots, documentation, license, sandbox) VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d)", $node->nid, $node->project['uri'], $node->project['homepage'], $node->project['changelog'], $node->project['demo'], $node->project['screenshots'], $node->project['documentation'], $node->project['license'], $node->project['sandbox']);
//  project_release_scan_directory($node->project['uri']);
  $perms = array_fill_keys(array_keys(project_permission_load($node)), 1);
  project_maintainer_save($node->nid, $node->uid, $perms);
}

function project_project_update($node) {
  // Auto generate the short name (uri) if necessary. We have to do this here,
  // since we need the node ID to exist before storing the short name.
  if (variable_get('project_sandbox_numeric_shortname', FALSE) && $node->project['sandbox']) {
    $node->project['uri'] = $node->nid;
  }
  db_query("UPDATE {project_projects} SET uri = '%s', homepage = '%s', changelog = '%s', demo = '%s', screenshots = '%s', documentation = '%s', license = '%s', sandbox = %d WHERE nid = %d", $node->project['uri'], $node->project['homepage'], $node->project['changelog'], $node->project['demo'], $node->project['screenshots'], $node->project['documentation'], $node->project['license'], $node->project['sandbox'], $node->nid);
//  project_release_scan_directory($node->project['uri']);
  $perms = array_fill_keys(array_keys(project_permission_load($node)), 1);
  project_maintainer_save($node->nid, $node->uid, $perms);
}

function project_project_delete($node) {
  db_query('DELETE FROM {project_projects} WHERE nid = %d', $node->nid);
}

function project_project_retrieve($key = 0) {
  if (!is_numeric($key)) {
    $nid = project_get_nid_from_uri($key);
  }
  $node = node_load($nid);
  return ($node->type == 'project_project') ? $node : NULL;
}

function project_cvs($nid = 0) {
  if ($project = node_load($nid)) {
    if (node_access('view', $project)) {
      $_REQUEST['nid'] = $nid;
      $output = module_invoke('cvs', 'show_messages');
      drupal_set_title(t('CVS messages for %name', array('%name' => $project->title)));
      project_project_set_breadcrumb($project, TRUE);
      return $output;
    }
    else {
      drupal_access_denied();
    }
  }
  else {
    drupal_not_found();
  }
}

function _project_save_taxonomy(&$node) {
  if (project_use_taxonomy() && $node->project_type) {
    // First, clear out all terms from the project-specific taxonomy
    // in this node. We'll re-add the right ones based on what's saved.
    // This way, we're sure to clear out things that have been changed.
    $vid = _project_get_vid();
    $result = db_query('SELECT tid FROM {term_data} WHERE vid = %d', $vid);
    $args = array($node->nid, $node->vid);
    $terms = array();
    while ($item = db_fetch_object($result)) {
      $terms[] = $item->tid;
    }
    if (count($terms) > 1) {
      $sql = 'DELETE FROM {term_node} WHERE nid = %d AND vid = %d AND tid IN ('. db_placeholders($terms) .')';
      $args = array_merge($args, $terms);
      db_query($sql, $args);
    }
    $tid = $node->project_type;
    _project_db_save_taxonomy($node->nid, $tid, $node->vid);
    $tid_field = 'tid_' . $tid;
    if (isset($node->$tid_field)) {
      foreach ($node->$tid_field as $tid) {
        _project_db_save_taxonomy($node->nid, $tid, $node->vid);
      }
    }
  }
}

function _project_db_save_taxonomy($nid, $tid, $vid) {
  db_query('INSERT INTO {term_node} (nid, tid, vid) VALUES (%d, %d, %d)', $nid, $tid, $vid);
}

/**
 * Adds the 'project-taxonomy-element' div to the project_type
 * and term select box on the project_project node form.
 */
function theme_project_project_node_form_taxonomy($form) {
  $output = '';
  foreach (element_children($form) as $child) {
    $output .= '<div class="project-taxonomy-element">';
    $output .= drupal_render($form[$child]);
    $output .= '</div>';
  }
  return $output;
}

/**
 * Build a nested array of sections of links to display on project_project node pages.
 */
function project_get_project_link_info($node = NULL) {
  static $all_links;

  // We only need to build the links array once per page.
  if (is_array($all_links)) {
    return $all_links;
  }

  // Resources section
  $all_links['resources'] = array(
    'name' => t('Resources'),
    'weight' => 4,
    'type' => 'inline',
  );
  foreach (array('homepage' => t('Home page'), 'documentation' => t('Read documentation'), 'license' => t('Read license'), 'changelog' => t('Read complete log of changes'), 'demo' => t('Try out a demonstration'), 'screenshots' => t('Look at screenshots')) as $uri => $name) {
    if (!empty($node->project[$uri])) {
      $all_links['resources']['links'][$uri] = l($name, $node->project[$uri]);
    }
  }

  // Developer section
  $all_links['development'] = array(
    'name' => t('Development'),
    'weight' => 8,
    'type' => 'inline',
  );
  $links = array();

  if (project_use_cvs($node)) {
    $links['view_cvs_messages'] = l(t('View CVS messages'), 'project/cvs/'. $node->nid);
  }
  $all_links['development']['links'] = $links;

  // Allow other modules to add sections of links and/or links to the sections defined above.
  drupal_alter('project_page_link', $all_links, $node);

  return $all_links;
}

/**
 * Page callback for promote subtab.
 */
function project_promote_project_page($node) {
  project_project_set_breadcrumb($node);
  drupal_set_title(check_plain($node->title));
  return drupal_get_form('project_promote_project_form', $node);
}

/**
 * Form callback for project promote form.
 */
function project_promote_project_form($form_state, $project) {
  $form = array();

  $form['help'] = array(
    '#type' => 'markup',
    '#value' => t('You are about to promote the project %project_name to a full project. This action is not reversible.', array('%project_name' => $project->title)),
  );

  $form['confirm'] = array(
    '#type' => 'checkbox',
    '#title' => t('I understand that this action cannot be undone and wish to proceed anyway'),
    '#default_value' => FALSE,
    '#description' => t('Please confirm that you understand the implications of promoting this project.')
  );

  // Only show the shortname field if it's auto-generated for sandbox projects.
  if (variable_get('project_sandbox_numeric_shortname', FALSE)) {
    // Unset this in case the user cancels on the confirm form so we don't leak
    // this into the $_SESSION permanently. Also ensures we start clean.
    unset($_SESSION['project_promote_project_shortname']);
    $form['shortname'] = array(
      '#type' => 'textfield',
      '#title' => t('Short project name'),
      '#default_value' => isset($project->project['uri']) ? $project->project['uri'] : NULL,
      '#size' => 40,
      '#maxlength' => 50,
      '#description' => (variable_get('project_enable_alias', TRUE)) ? t('This will be used to generate a /project/&lt;shortname&gt;/ URL for your project.') : '',
      '#required' => TRUE,
    );
  }

  // Update help text if the user is not allowed to update the short name at a
  // later time. 
  if (!variable_get('project_allow_uri_update', TRUE)) {
      $form['shortname']['#description'] .= ' '. t('You may not edit this value after the project has been promoted.');
  }

  $form['pid'] = array('#type' => 'value', '#value' => $project->nid);
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Promote to full project'),
    '#weight' => 45,
  );

  return $form;
}

/**
 * Validation handler for project promote form.
 */
function project_promote_project_form_validate($form, &$form_state) {
  if ($form_state['values']['confirm'] == 0) {
    form_set_error('confirm', t('Please check the confirmation checkbox before promoting this project'));
  }
  // Only validate the short name if it is relevant, and included in the
  // original form.
  if (variable_get('project_sandbox_numeric_shortname', FALSE)) {
    $project = node_load($form_state['values']['pid']);
    $project->project['sandbox'] = 0;
    $project->project['uri'] = $form_state['values']['shortname'];
    if ($error = project_validate_project_shortname($project)) {
      form_set_error('shortname', $error);
    }
  }
}

/**
 * Submit handler for project promote form. Redirects to a confirm form.
 */
function project_promote_project_form_submit($form, &$form_state) {
  if (isset($form_state['values']['shortname'])) {
    $_SESSION['project_promote_project_shortname'] = $form_state['values']['shortname'];
  }
  $form_state['redirect'] = 'node/' . $form_state['values']['pid'] . '/edit/promote/confirm';
}

/**
 * Build a confirmation form for promoting sandbox projects to full projects.
 */
function project_promote_project_confirm_form($form_state, $project) {
  if (empty($project)) {
    drupal_set_message(t('Error: you cannot confirm promoting a sandbox to a full project without specifying the project.'), 'error');
    return drupal_goto();
  }
  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $project->nid,
  );

  if (isset($_SESSION['project_promote_project_shortname'])) {
    $shortname = $_SESSION['project_promote_project_shortname'];
    // Note: we can't unset this value from the $_SEESSION here, or this form
    // element won't be included when building the form during submission. So,
    // we'll unset once we actually submit the confirm form.
  }
  else {
    $shortname = $project->project['uri'];
  }
  $form['shortname'] = array(
    '#type' => 'value',
    '#value' => $shortname,
  );

  // If the shortname can't otherwise be changed, make sure they understand
  // this is *really* permanent once they press "Promote".
  if (!variable_get('project_allow_uri_update', TRUE)) {
    $confirm_text = t('The project will use the shortname %shortname which can not be changed later. Furthermore, once a project is promoted, it can not be turned back into a sandbox.', array('%shortname' => $shortname));
  }
  else {
    $confirm_text = t('Once a project is promoted, it can not be turned back into a sandbox.');
  }
  return confirm_form(
    $form,
    t('Are you sure you want to promote %title from a sandbox to a full project with the shortname %shortname?', array('%title' => $project->title, '%shortname' => $shortname)),
    'node/' . $project->nid . '/edit/promote',
    $confirm_text, t('Promote'), t('Cancel')
  );
}

/**
 * Submit handler for project promote form.
 */
function project_promote_project_confirm_form_submit($form, &$form_state) {
  $project = node_load($form_state['values']['nid']);
  $project->project['sandbox'] = 0;

  unset($_SESSION['project_promote_project_shortname']);
  // Only update the shortname if it is relevant, and included in the original
  // form.
  if (variable_get('project_sandbox_numeric_shortname', FALSE)) {
    $project->project['uri'] = $form_state['values']['shortname'];
  }

  node_save($project);
  drupal_set_message(t('The project %project_name has been promoted to a full project.', array('%project_name' => $project->title)));
  $form_state['redirect'] = 'node/' . $project->nid;

  // Notify that this promotion has occurred.
  module_invoke_all('project_promote_sandbox', $project);
}
