<?php

/**
 * @file
 * Class for new_issues_comments_by_project metric.
 */

class  ProjectIssueNewIssuesCommentsByProject extends ProjectIssueMetric {

  public function computeSample() {
    // Initialize the projects array.
    $this->projectIssueMetricInitProjects(array(
      'new_issues' => 0,
      'new_comments' => 0,
      'new_total' => 0,
    ));

    // Load options.
    $sample = $this->currentSample;

    $where_pieces = array();
    $args = array();

    $args = array_merge($args, array($sample->sample_startstamp, $sample->sample_endstamp));

    // Restrict to only the passed projects.
    if (!empty($sample->options['object_ids'])) {
      $where_pieces[] = "pi.pid IN (". db_placeholders($sample->options['object_ids']) .")";
      $args = array_merge($args, $sample->options['object_ids']);
    }

    if (empty($where_pieces)) {
      $where = '';
    }
    else {
      $where = ' AND ' . implode(' AND ', $where_pieces);
    }

    // Pull the count of new issues per project.
    $projects = db_query("SELECT pi.pid, COUNT(pi.nid) AS count FROM {project_issues} pi INNER JOIN {node} n ON pi.nid = n.nid WHERE n.created >= %d AND n.created < %d$where GROUP BY pi.pid", $args);

    while ($project = db_fetch_object($projects)) {
      $this->currentSample->values[$project->pid]['new_issues'] = (int)$project->count;
      $this->currentSample->values[$project->pid]['new_total'] = (int)$project->count;
    }

    // Pull the count of new issue comments per project.
    $projects = db_query("SELECT pi.pid, COUNT(pi.cid) AS count FROM {project_issue_comments} pi WHERE pi.timestamp >= %d AND pi.timestamp < %d$where GROUP BY pi.pid", $args);

    while ($project = db_fetch_object($projects)) {
      $this->currentSample->values[$project->pid]['new_comments'] = (int)$project->count;
      // Add the comment count to the total.
      $this->currentSample->values[$project->pid]['new_total'] = $this->currentSample->values[$project->pid]['new_total'] + (int)$project->count;
    }
  }
}
