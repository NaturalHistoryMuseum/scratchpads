<?php

/**
 * Implementation of hook_pre_provision_install
 */
function drush_hostmaster_scratchpad_pre_provision_install($url = NULL){
  drush_set_option('site_title', drush_get_option('site_title', FALSE), 'site');
  drush_set_option('client_title', drush_get_option('client_title', FALSE), 'site');
  drush_set_option('client_email', drush_get_option('client_email', FALSE), 'site');
  drush_set_option('fullname', drush_get_option('fullname', FALSE), 'site');
  drush_set_option('institution', drush_get_option('institution', FALSE), 'site');
  drush_set_option('taxonomic_scope', drush_get_option('taxonomic_scope', FALSE), 'site');
  drush_set_option('googleapi', drush_get_option('googleapi', FALSE), 'site');
  drush_set_option('clustrmaphtml', drush_get_option('clustrmaphtml', FALSE), 'site');
  drush_set_option('mission_statement', drush_get_option('mission_statement', FALSE), 'site');
}