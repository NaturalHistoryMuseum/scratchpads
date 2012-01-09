<?php

/**
 * @file
 * Class for reporters_participants_per_project metric.
 */

class ProjectIssueReportersParticipantsByProject extends ProjectIssueMetric {

  public function computeSample() {
    // Initialize the projects array.
    $this->projectIssueMetricInitProjects(array(
      'reporters' => 0,
      'participants' => 0,
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
      $where = ' WHERE ' . implode(' AND ', $where_pieces);
    }

    // Pull the count of unique reporters per project.
    $projects = db_query("SELECT DISTINCT pi.pid, n.uid AS uid FROM {project_issues} pi INNER JOIN {node} n ON pi.nid = n.nid AND n.created >= %d AND n.created < %d$where", $args);

    $project_participants = array();
    while ($project = db_fetch_object($projects)) {
      // Increment the number of reporters for the project, and also store
      // them as a participant.
      $this->currentSample->values[$project->pid]['reporters']++;
      $project_participants[$project->pid][$project->uid] = TRUE;
    }

    // Pull the count of unique participants per project.
    $projects = db_query("SELECT DISTINCT pi.pid, c.uid FROM {project_issue_comments} pi INNER JOIN {comments} c ON pi.cid = c.cid AND pi.timestamp >= %d AND pi.timestamp < %d$where", $args);

    // Add in participants from comments.  This will overwrite the reporters if
    // they are the same uid, thus avoiding double counting.
    while ($project = db_fetch_object($projects)) {
      $project_participants[$project->pid][$project->uid] = TRUE;
    }

    // Store the total participants for each project.
    foreach ($project_participants as $pid => $participants) {
      $this->currentSample->values[$pid]['participants'] = count($participants);
    }

    unset($project_participants);
  }
}
