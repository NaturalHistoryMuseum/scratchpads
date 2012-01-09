<?php


/**
 * @file
 * Autocomplete callback functions for the Project issue tracking module.
 *
 * Each function returns a JSON string for use with JS autocomplete fields.
 */

/**
 * Return valid issue-enabled project names for comma-separated input.
 */
function project_issue_autocomplete_issue_project($string = '') {
  $matches = array();

  // The user enters a comma-separated list of projects. We only autocomplete
  // the last one.
  $array = drupal_explode_tags($string);
  $last_string = trim(array_pop($array));

  if ($last_string != '') {
    $result = db_query_range(db_rewrite_sql("SELECT n.title FROM {node} n INNER JOIN {project_issue_projects} p ON n.nid = p.nid WHERE n.status = %d AND LOWER(n.title) LIKE LOWER('%%%s%%') AND p.issues = %d"), 1, $last_string, 1, 0, 10);

    $prefix = count($array) ? implode(', ', $array) .', ' : '';
    while ($project = db_fetch_object($result)) {
      $title = $project->title;
      // Commas and quotes in terms are special cases, so encode 'em.
      if (strpos($title, ',') !== FALSE || strpos($title, '"') !== FALSE) {
        $title = '"'. str_replace('"', '""', $project->title) .'"';
      }
      $matches[$prefix . $title] = check_plain($project->title);
    }
  }

  drupal_json($matches);
}


/**
 * Return valid issue-enabled project names based on a user's own projects.
 *
 * Only returns matches for project titles from issues the user has either
 * submitted or commented on.
 */
function project_issue_autocomplete_user_issue_project($uid, $string = '') {
  $matches = array();

  // The user enters a comma-separated list of projects. We only autocomplete
  // the last one.
  $array = drupal_explode_tags($string);
  $last_string = trim(array_pop($array));

  if ($last_string != '') {
    // We have to do a DISTINCT() here because with the LEFT JOIN on
    // {comments}, we can get a lot of duplicate matches, and then our range
    // limit will prevent us from showing all the distinct options.
    $result = db_query_range(db_rewrite_sql("SELECT DISTINCT(n.title) FROM {node} n INNER JOIN {project_issue_projects} pip ON n.nid = pip.nid INNER JOIN {project_issues} pi ON n.nid = pi.pid INNER JOIN {node} pin ON pi.nid = pin.nid LEFT JOIN {comments} c ON c.nid = pi.nid WHERE pip.issues = %d AND n.status = %d AND pin.status = %d AND (pin.uid = %d OR c.uid = %d) AND LOWER(n.title) LIKE LOWER('%%%s%%')"), 1, 1, 1, $uid, $uid, $last_string, 0, 10);

    $prefix = count($array) ? implode(', ', $array) .', ' : '';
    while ($project = db_fetch_object($result)) {
      $title = $project->title;
      // Commas and quotes in terms are special cases, so encode 'em.
      if (strpos($title, ',') !== FALSE || strpos($title, '"') !== FALSE) {
        $title = '"'. str_replace('"', '""', $project->title) .'"';
      }
      $matches[$prefix . $title] = check_plain($project->title);
    }
  }

  drupal_json($matches);
}

/**
 * Handles the auto-complete callback for the nodereference widget.
 *
 * Instead of returning a value, this function sends it to the browser by
 * calling drupal_json(). The returned value is a JSON-encoded array of matches,
 * where the array keys and values are both '#NID: TITLE', where NID is the node
 * ID, and title is the node title. In the values, the title is run through
 * check_plain, but not in the keys.
 *
 * @param $string
 *   String the user typed.
 */
function project_issue_autocomplete_issues_nodereference($string) {
  $matches = array();
  $results = project_issue_autocomplete_issues_search($string);
  foreach ($results as $nid => $title) {
    // NID here is coming from the node table, so doesn't need to be
    // sanitized. In the array key, we don't sanitize the title either,
    // because that is what the nodereference module is expecting, but we
    // do want it sanitized in the output.
    $matches["#$nid: " . $title] = "#$nid: " . check_plain($title);
  }
  drupal_json($matches);
}

/**
 * Matches issues against input (partial node ID or partial title).
 *
 * @param $string
 *   User submitted text to match against the issue node ID or title.
 * @param $items
 *   Number of matches to return.
 *
 * @return
 *   Associative array of issues that match the input string, with node ID as
 *   the key, and title as the value. If there are no matches, an empty array
 *   is returned.
 */
function project_issue_autocomplete_issues_search($string, $items = 10) {
  $matches = array();

  // Match against node IDs first.
  if (is_numeric($string)) {
    // Try to find issues whose ID starts with this number.
    $result = db_query_range(db_rewrite_sql("SELECT DISTINCT(n.nid), n.title FROM {node} n WHERE n.status = 1 AND n.type = 'project_issue' AND n.nid LIKE '%s%'"), $string, 0, $items);
  }
  while ($issue = db_fetch_object($result)) {
    $matches[$issue->nid] = $issue->title;
  }

  // If we don't have the required number of items, match against the title,
  // using a full-text match of whatever was entered.
  if (count($matches) < $items) {
    $needed = $items - count($matches);
    $values = array();
    // Make sure that any matches that we've already found are excluded.
    if (!empty($matches)) {
      $values = array_keys($matches);
      $sql = "SELECT n.nid, n.title FROM {node} n WHERE n.status = 1 AND n.type = 'project_issue' AND n.nid NOT IN (" . db_placeholders($values) . ") AND n.title LIKE '%%%s%%'";
    }
    else {
      $sql = "SELECT n.nid, n.title FROM {node} n WHERE n.status = 1 AND n.type = 'project_issue' AND n.title LIKE '%%%s%%'";
    }
    // We need the string to match against.
    $values[] = $string;
    $result = db_query_range(db_rewrite_sql($sql), $values, 0, $needed);
  }
  while ($issue = db_fetch_object($result)) {
    $matches[$issue->nid] = $issue->title;
  }

  return $matches;
}
