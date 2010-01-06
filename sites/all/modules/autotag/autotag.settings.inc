<?php
/**
 * autotag settings page
 */
function autotag_admin_settings() {
  $form['autotag_without_user_input'] = array(
    '#type' => 'checkbox',
    '#title' => t('No user input required before auto tagging.'),
    '#default_value' => variable_get('autotag_without_user_input', false),
    '#description' => t('If checked, no user input will be required to automatically tag a node')
  );
  return system_settings_form($form);
}