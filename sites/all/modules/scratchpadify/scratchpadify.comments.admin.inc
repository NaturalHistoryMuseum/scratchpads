<?php

/**
 * Form for Scratchpad comments.
 */
function scratchpadify_comment_settings(){
  $permissions = variable_get('scratchpadify_anon_perms', array());
  return system_settings_form(array(
    'scratchpadify_anon_perms' => array(
      '#tree' => TRUE,
      'post comments' => array(
        '#title' => 'Allow anonymous comments',
        '#type' => 'checkbox',
        '#tree' => TRUE,
        '#default_value' => isset($permissions['post comments']) ? $permissions['post comments'] : 0,
        '#description' => t('If checked, anonymous users will be able to post comments to your site.  Comments will not be visible without first being approved by an Editor or Maintainer.  Please note, this value will be changed on the next cron run (last cron run was "%last_cron_run" -> contact an administrator if this is more than one hour.)', array(
          '%last_cron_run' => variable_get('cron_last', 0) ? format_interval(time() - variable_get('cron_last', 0)) . ' ago' : 'never'
        ))
      )
    )
  ));
}