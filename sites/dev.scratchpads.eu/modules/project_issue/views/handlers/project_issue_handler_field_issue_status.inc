<?php


/**
 * Field handler to display the current status of an issue.
 */
class project_issue_handler_field_issue_status extends views_handler_field {
  function render($values) {
    module_load_include('inc', 'project_issue', 'issue');
    return check_plain(project_issue_state($values->{$this->field_alias}));
  }
}

