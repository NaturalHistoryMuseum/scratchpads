<?php


/**
 * Filter issues based on their category.
 */
class project_issue_handler_filter_issue_category extends views_handler_filter_in_operator {
  function get_value_options() {
    module_load_include('inc', 'project_issue', 'issue');
    // All categories, not plural
    $this->value_options = project_issue_category(FALSE, FALSE);
  }
}

