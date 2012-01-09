<?php

/**
 * @file
 * API documentation for hooks invoked (provided) by the Project issue module.
 */

/**
 * Alter hook for the internal links at the top of issue node pages.
 *
 * @param array $links
 *   Reference to an array of all the "Jump to" links to render at the top of
 *   issue node pages. By default, these are the links for "Most recent
 *   comment", "Add new comment", etc.
 * @param stdClass $node
 *   The fully-loaded project_issue node the links belong to.
 *
 * @see project_issue_internal_links()
 * @see drupal_alter()
 */
function hook_project_issue_internal_links_alter(&$links, $node) {
  $links[] = l(t('Most excellent comment'), "node/$node->nid", array('fragment' => 'excellent')); // If only it were so easy. ;)
}
