<?php

/**
 * @file
 * Base class for Project issue metrics.
 */

class ProjectIssueMetric extends SamplerMetric {

  public function projectIssueMetricInitProjects($data_types) {
    // Load options.
    $options = $this->currentSample->options;
    if (!empty($options['object_ids'])) {
      $object_ids = $options['object_ids'];
    }
    else {
      $object_ids = $this->trackObjectIDs();
    }
    foreach ($object_ids as $object_id) {
      $this->currentSample->values[$object_id] = $data_types;
    }
  }

  public function trackObjectIDs() {
    $nids = array();
    $projects = db_query("SELECT nid FROM {project_projects}");
    while ($project = db_fetch_object($projects)) {
      $nids[] = $project->nid;
    }
    return $nids;
  }
}
