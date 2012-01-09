<?php

/**
 * @file
 * Class for new_releases metric.
 */

class ProjectReleaseMetricNewReleases extends ProjectReleaseMetric {

  /**
   * Counts new releases made for each project during any given sample period.
   *
   * @return
   *   An associative array keyed by project nid, where the value is an array
   *   containing the number of new releases created for that project in the
   *   current sample period.
   */
  public function computeSample() {

    $sample = $this->currentSample;
    $options = $sample->options;
    $args = array($sample->sample_startstamp, $sample->sample_endstamp);

    // Initialize the projects array.
    $data_types = array(
      'releases' => 0,
    );
    $this->projectReleaseMetricInitProjects($data_types);

    // Restrict to only the passed project nids.
    if (!empty($options['object_ids'])) {
      $where = " WHERE prn.pid IN (". db_placeholders($options['object_ids']) .")";
      $args = array_merge($args, $options['object_ids']);
    }
    else {
      $where = '';
    }

    // Pull all release nodes created during the specified time.
    $nodes = db_query("SELECT prn.pid, COUNT(nr.nid) AS releases FROM {project_release_nodes} prn INNER JOIN {node} nr ON prn.nid = nr.nid AND nr.created >= %d AND nr.created < %d$where GROUP BY prn.pid", $args);
    while ($node = db_fetch_object($nodes)) {
      $this->currentSample->values[$node->pid]['releases'] = (int)$node->releases;
    }
  }
}

