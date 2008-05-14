<?php
function _phptemplate_variables($hook, $vars) {
  if (module_exists('advanced_forum')) {
    $vars = advanced_forum_addvars($hook, $vars);
  }

  return $vars;
}