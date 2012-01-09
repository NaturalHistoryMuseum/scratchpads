<?php

/**
 * @file
 * Class for opened_vs_closed_by_category metric.
 */

class ProjectIssueOpenedVsClosedByCategory extends ProjectIssueMetric {

  public function computeSample() {
    // Load options.
    $options = $this->currentSample->options;

    // Initialize the projects array.
    $data_types = $this->newProjectOpenedVsClosed();
    $this->projectIssueMetricInitProjects($data_types);

    $where_pieces = array();
    $args = array();
    $where_args = array();

    // The initial arguments are the 'open' project issue status options.
    $open_states = project_issue_state(0, FALSE, FALSE, 0, TRUE);

    // TODO: Until http://drupal.org/node/214347 is resolved, we don't want
    // the 'fixed' status in the list of open statuses, so unset it here.
    unset($open_states[PROJECT_ISSUE_STATE_FIXED]);

    $open_states = array_keys($open_states);

    $args = array_merge($args, $open_states);

    // Restrict to only the passed nids.
    if (!empty($options['object_ids'])) {
      $where_pieces[] = "pid IN (". db_placeholders($options['object_ids']) .")";
      $where_args = array_merge($where_args, $options['object_ids']);
    }

    if (empty($where_pieces)) {
      $where = '';
    }
    else {
      $where = ' AND ' . implode(' AND ', $where_pieces);
      $args = array_merge($args, $where_args);
    }

    // Use the historical calculation if requested.
    if (isset($options['historical']) && $options['historical'] == '1') {
      $this->buildSampleResultsHistorical($open_states, $where, $where_args);
    }
    // Otherwise, use the more efficient 'current' calculation.
    else {
      // Build total open issues counts.
      $this->buildSampleResultsCurrent('open', $open_states, $where, $args);
      // Build total closed issues counts.
      $this->buildSampleResultsCurrent('closed', $open_states, $where, $args);
    }

    // Add in total counts across all categories.
    foreach ($this->currentSample->values as $project_id => $values_array) {
      $this->currentSample->values[$project_id]['total_open'] = $values_array['bug_open'] + $values_array['feature_open'] + $values_array['task_open'] + $values_array['support_open'];
      $this->currentSample->values[$project_id]['total_closed'] = $values_array['bug_closed'] + $values_array['feature_closed'] + $values_array['task_closed'] + $values_array['support_closed'];
    }
  }

  /**
   * Builds the current values for total open/closed issues per project.
   *
   * @param $state
   *   Issue state, 'open' or 'closed'.
   * @param $open_states
   *   An array of state IDs that are considered 'open'.
   * @param $where
   *   Additional filters for the query.
   * @param $args
   *   Query arguments.
   */
  protected function buildSampleResultsCurrent($state, $open_states, $where, $args) {

    // Determine IN or NOT IN based on the state we're querying for.
    $in = $state == 'open' ? 'IN' : 'NOT IN';

    // Pull all issues grouped by project/category -- this query will miss any
    // projects that don't have issues, but we wouldn't want to show any data
    // for them, anyways.
    $result = db_query("SELECT pid, category, COUNT(nid) AS count FROM {project_issues} WHERE sid $in (" . db_placeholders($open_states) . ")$where GROUP BY pid, category", $args);
    // TODO: When http://drupal.org/node/115553 lands, this hard-coded list of
    // categories will need to be handled differently.
    $categories = array('bug', 'feature', 'task', 'support');
    while ($row = db_fetch_object($result)) {
      // Add the total count for the category to the values array.
      if (in_array($row->category, $categories)) {
        $this->currentSample->values[$row->pid]["{$row->category}_{$state}"] = (int)$row->count;
      }
    }
  }

  /**
   * Builds the historical values for total open/closed issues per project.
   *
   * @param $open_states
   *   An array of state IDs that are considered 'open'.
   * @param $where
   *   Additional filters for the query.
   * @param $args
   *   Query arguments.
   */
  protected function buildSampleResultsHistorical($open_states, $where, $args) {

    // Load current sample.
    $sample = $this->currentSample;

    $endstamp = $sample->sample_endstamp;

    // Pull last possible issue nid for the end of the period being measured.
    $max_nid = db_result(db_query("SELECT MAX(nid) FROM {node} WHERE created <= %d", $endstamp));

    if (!empty($max_nid)) {

      // Build a map of the last comment numbers for all issue nodes prior to
      // the end of the period being measured.
      $last_comment_map = array();
      $query_args = array_merge(array($max_nid, $endstamp), $args);
      $comments = db_query("SELECT nid, pid, MAX(comment_number) AS last FROM {project_issue_comments} WHERE nid <= %d AND timestamp <= %d$where GROUP BY nid, pid", $query_args);
      while ($comment = db_fetch_object($comments)) {
        $last_comment_map[$comment->pid][$comment->nid] = $comment->last;
      }
      // Get all issues created before the end of the period being measured.
      $query_args = array_merge(array($max_nid), $args);
      $issues = db_query("SELECT nid, pid, original_issue_data FROM {project_issues} WHERE nid <= %d$where", $query_args);
      // TODO: When http://drupal.org/node/115553 lands, this hard-coded list of
      // categories will need to be handled differently.
      $categories = array('bug', 'feature', 'task', 'support');
      while ($issue = db_fetch_object($issues)) {
        // Determine if the issue in question has any comments associated with
        // it.  If so, use the data from the last comment map to pull the state
        // of the issue at the end of the sample period.
        if (isset($last_comment_map[$issue->pid][$issue->nid])) {
          $issue_data = db_fetch_object(db_query("SELECT * FROM {project_issue_comments} WHERE pid = %d AND nid = %d AND comment_number = %d", $issue->pid, $issue->nid, $last_comment_map[$issue->pid][$issue->nid]));
        }
        // No comments on the issue, so use the original issue data.
        else {
          $issue_data = (object) unserialize($issue->original_issue_data);
        }

        $state = in_array($issue_data->sid, $open_states) ? 'open' : 'closed';

        // Add to the total count for the category in the values array.
        if (in_array($issue_data->category, $categories)) {
          $this->currentSample->values[$issue->pid]["{$issue_data->category}_{$state}"]++;
        }
      }

      // Clean up potentially huge arrays that are sucking memory.
      unset ($last_comment_map, $issues);
    }
    // The end of the sample is so early there aren't even any nodes created --
    // just bail on the entire sample.
    else {
      return FALSE;
    }
  }

  /**
   * Builds the starting values for a project for this metric.
   *
   * @return
   *   An associative array, keys are metric values, values are the default
   *   starting count for the metric values.
   */
  protected function newProjectOpenedVsClosed() {
    return array(
      'bug_open' => 0,
      'feature_open' => 0,
      'task_open' => 0,
      'support_open' => 0,
      'bug_closed' => 0,
      'feature_closed' => 0,
      'task_closed' => 0,
      'support_closed' => 0,
      'total_open' => 0,
      'total_closed' => 0,
    );
  }
}
