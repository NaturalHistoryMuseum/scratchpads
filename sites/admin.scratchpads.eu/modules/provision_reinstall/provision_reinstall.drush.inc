<?php
/**
 * @file
 */

/**
 * Implements hook_drush_command().
 */
function provision_reinstall_drush_command() {
  $items = array();

  $items['provision-reinstall'] = array(
    'description' => 'Reinstall a site.',
    'examples' => array(
      'drush @site provision-reinstall' => 'Invokes Provision Delete and Install tasks to simulate a site reinstall.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT
  );

  return $items;
}

/**
 * Drush Provision Reinstall task callback.
 *
 * Invokes Provision Delete and Install tasks to simulate a site reinstall.
 */
function drush_provision_reinstall() {
  provision_backend_invoke(d()->name, "provision-delete");
  provision_backend_invoke(d()->name, "provision-install");
}
