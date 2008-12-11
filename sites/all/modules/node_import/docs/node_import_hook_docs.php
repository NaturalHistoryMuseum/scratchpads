<?php
/* $Id: node_import_hook_docs.php,v 1.1.4.2 2008/05/03 08:44:28 robrechtj Exp $ */

/**
 * @file
 * Documentation for hooks provided by the node_import module.
 *
 * The node import module provides extendable functionality through hooks
 * documented in this file.
 */

/**
 * Returns a list of node types which have full support for importing provided
 * by other hooks.
 *
 * @return
 *   An associative array of node types containting the value for the type
 *   child of the node object => human-readable name.
 */
function hook_node_import_types() {
  return array('story' => t('Story'));
}

/**
 * Provides a list of fields which each module understands. When importing the
 * array keys will be used to name the node object's children.
 *
 * @param $type
 *   A string containing the node type currently being processed.
 * @return
 *   An associative array of node object children names => human readable
 *   descriptions.
 */
function hook_node_import_fields($type) {
  if ($type == 'story') {
    // The story node type only implements the node's body and title.
    return array(
      'body' => t('Body'),
      'title' => t('Title'),
    );
  }
}

/**
 * Hook that is called before node_validate. This allows a module to
 * change the imported field from a user readable format to a format
 * node_save understands.
 *
 * See import_taxonomy.inc for a complex example.
 *
 * @param &$node
 *   A node object which can be changed if needed.
 * @param $preview
 *   Boolean. If TRUE this function is only run in a preview stage. The
 *   function should then avoid making permanent changes to any database
 *   table. If FALSE the function may commit permanent changes to the
 *   database that are needed to import the node.
 * @return
 *   Nothing.
 */
function hook_node_import_prepare(&$node, $preview = FALSE) {
  // The story node type doesn't need any preparing.
  return;
}

/**
 * Provides a list of values which are static for all node imports of a given
 * type.
 *
 * @param $type
 *   A string containing the node type currently being processed.
 * @return
 *   An associative array containing node object children names => values.
 */
function hook_node_import_static($type) {
  global $user;

  if ($type == 'story') {
    // Use the current user's information for importing stories.
    return array('name' => $user->name, 'uid' => $user->uid);
  }
}

/**
 * Provides form fields which will have the same user-specified value across
 * one import.
 *
 * @param $type
 *   A string containing the node type currently being processed.
 * @param $global_values
 *   An array with the previously filled in global options values.
 * @return
 *   A string containing the form which will be provided to the user.
 */
function hook_node_import_global($type, $global_values) {
  if ($type == 'story') {
    // Story nodes implement taxonomy.
    return implode('', taxonomy_node_form('story'));
  }
}

/**
 * Allows a module to react on the creation of a node by node_import.
 *
 * After the node has been created or an attempt to create it has failed
 * node_import fires this hook so other modules can react on it.
 *
 * This function can also be used to alter the node before preview. Eg
 * for an image related module to insert a preview of the to-be-imported
 * image.
 *
 * @param $node
 *   A node object that was processed.
 * @param $preview
 *   Boolean. If TRUE this function is only run in a preview stage. The
 *   function should then avoid making permanent changes to any database
 *   table. If FALSE the function may commit permanent changes to the
 *   database that are needed to import the node.
 * @param $error
 *   Boolean. If TRUE then the node was not imported successfully. If FALSE
 *   the node was imported successfully.
 * @return
 *   Nothing.
 */
function hook_node_import_postprocess(&$node, $preview, $error) {
  // The story doesn't need any postprocessing. This hook is not used for
  // actually importing the node (as are the other hooks), but rather to
  // inform a third-party module of the fact that the node was imported.
  return;
}

/**
 * Allows a module to react to the completed set of node imports.
 *
 * After all the nodes have been created or an attempt to create them has failed
 * node_import_complete fires this hook so other modules can react on it.
 *
 * @param $type
 *   A string containing the node type currently being processed.
 * @return
 *   Nothing.
 */
function hook_node_import_complete($type) {
  // This hook is not used for actually importing the node (as are the other
  // hooks), but rather to inform a third-party module of the fact that all
  // nodes have been imported.
  return;
}

