<?php
/**
 * @file
 * Expose the migrate feature to hostmaster.
 */

/**
 * Implements hook_hosting_feature().
 */
function hosting_reinstall_hosting_feature() {
  $features = array();

  $features['reinstall'] = array(
    'title' => t('Site reinstall'),
    'description' => t('Provides a Hosting task to reinstall sites.'),
    'status' => HOSTING_FEATURE_ENABLED,
    'module' => 'hosting_reinstall',
  );

  return $features;
}
