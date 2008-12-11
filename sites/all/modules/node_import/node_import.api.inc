<?php 
// $Id: node_import.api.inc,v 1.8.2.1.2.11 2008/10/18 08:06:55 robrechtj Exp $

/**
 * @file
 * Public API of node_import to import CSV files with nodes.
 */

ini_set('auto_detect_line_endings', TRUE);

/**
 * Loads the hooks for the supported modules.
 */
function node_import_load_supported() {
  static $loaded = FALSE;
  if (!$loaded) {
    $path = drupal_get_path('module', 'node_import') . '/supported';
    $files = drupal_system_listing('.*\.inc$', $path, 'name', 0);
    foreach ($files as $module_name => $file) {
      if (module_exists($module_name)) {
        include_once($file->filename);
      }
    }
    $loaded = TRUE;
  }
}

/**
 * Get a list of supported nodes types.
 *
 * @param $check_permission
 *   Boolean. If TRUE, only returns the types for which the
 *   user has 'create' permissions. If FALSE, then all types
 *   which node_import supports are returned (even if the user
 *   is not allowed to create the nodes).
 *
 * @return
 *   Array ($type => $name) with all supported types.
 *
 * @see hook_node_import_types()
 */
function node_import_types($check_permission = TRUE) {
  static $types = NULL;
  static $permitted_types = NULL;

  if (!isset($types)) {
    $types = array();
    foreach (module_invoke_all('node_import_types') as $type => $name) {
      $types[$type] = $name;
    }
    asort($types);
  }

  if (!isset($permitted_types)) {
    $permitted_types = array();
    foreach ($types as $type => $name) {
      if (node_access('create', $type)) {
        $permitted_types[$type] = $name;
      }
    }
  }

  return $check_permission ? $permitted_types : $types;
}

/**
 * Get a list of supported node fields for a given type.
 *
 * @param $type
 *   String. Node type (key in the array returned by node_import_types()).
 *
 * @return
 *   Array ($field => $name) with all the supported fields
 *   for this node type.
 *
 * @see hook_node_import_fields()
 */
function node_import_fields($type) {
  static $fields = array();
  if (!isset($fields[$type])) {
    $fields[$type] = (array)module_invoke_all('node_import_fields', $type);
    asort($fields[$type]);
  }
  return $fields[$type];
}

/**
 * Try to find a likely mapping for given headers.
 *
 * @param $type
 *   String. Node type (key in the array returned by node_import_types()).
 * @param $headers
 *   Array of string. Column headers of a CSV or TSV file.
 *
 * @return
 *   Array of string. A likely mapping given the $type and $header.
 */
function node_import_automap($type, $headers) {
  // See if we saved a mapping already.
  $sql = "SELECT mapping FROM {node_import_mappings} WHERE type = '%s' AND csv_headers = '%s'";
  $obj = db_fetch_object(db_query($sql, $type, serialize($headers)));
  if ($obj) {
    return unserialize($obj->mapping);
  }

  // Try to automap by looking at the column headers.
  $fields = node_import_fields($type);
  $titles_to_fields = array();
  $fields_to_fields = array();
  foreach ($fields as $field => $title) {
    $titles_to_fields[strtolower($title)] = $field;
    $fields_to_fields[strtolower($field)] = $field;
  }
  $mapping = array();
  foreach (array_map('strtolower', $headers) as $header) {
    if (isset($titles_to_fields[$header])) {
      $mapping[] = $titles_to_fields[$header];
    }
    else if (isset($fields_to_fields[$header])) {
      $mapping[] = $fields_to_fields[$header];
    }
    else {
      $mapping[] = "";
    }
  }
  return $mapping;
}

/**
 * Helper function to get a user from a string. This function will
 * return the uid of the user found or NULL if no user matching
 * the string was found.
 *
 * If the string provided is an integer, then this function will
 * assume that the user is identified by uid.
 *
 * Otherwise, the function will lookup a user with a name matching
 * the string.
 *
 * @param $name
 *   String. Name or uid of a user.
 *
 * @return
 *   Uid of the user found, or NULL.
 */
function node_import_userreference($name) {
  static $uids = array();
  $name = trim($name);

  if (!isset($uids[$name])) {
    $uids[$name] = NULL;
    if (strlen($name) > 0 &&
        (is_numeric($name) && intval($name) > 0 && ($user = user_load(array('uid' => intval($name))))) ||
        ($user = user_load(array('name' => $name))) ||
        ($user = user_load(array('mail' => $name)))) {
      $uids[$name] = $user->uid;
    }
  }

  return $uids[$name];
}

/**
 * Helper function to get a node from a string. This function will
 * return the nid of the node found or NULL if no node matching
 * the string was found.
 *
 * If the string provided is an integer, then this function will
 * assume that the node is identified by nid.
 *
 * Otherwise, the function will lookup a node with a title matching
 * the string.
 *
 * @param $title
 *   String. Title or nid of a node.
 *
 * @return
 *   Nid of the node found, or NULL.
 */
function node_import_nodereference($title) {
  static $nids = array();
  $title = trim($title);

  if (!isset($nids[$title])) {
    $nids[$title] = NULL;
    if (strlen($title) > 0 && is_numeric($title) && intval($title) > 0 && ($node = node_load(intval($title), NULL, TRUE))) {
      $nids[$title] = $node->nid;
    }
    else if (strlen($title) > 0 && ($node = db_fetch_object(db_query("SELECT nid FROM {node} WHERE title = '%s'", $title)))) {
      $nids[$title] = $node->nid;
    }
    else if (strlen($title) > 0 && ($node = db_fetch_object(db_query("SELECT nid FROM {node} WHERE title SOUNDS LIKE '%s'", $title)))) {
      $nids[$title] = $node->nid;
    }
  }

  return $nids[$title];
}

