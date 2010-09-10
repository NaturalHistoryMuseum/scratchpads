<?php

/**
 * Settings page callback
 */
function uwho_client_settings(){
  return system_settings_form(array(
    'uwho_client_key' => array(
      '#title' => t('UWho Client Key'),
      '#description' => t('Enter the key from the site you\'d like to enable your users to login to.'),
      '#type' => 'textfield',
      '#default_value' => variable_get('uwho_client_key', '')
    ),
    'uwho_client_url' => array(
      '#title' => t('UWho Client URL'),
      '#description' => t('Enter the URL of the site you\'d like to enable your users to login to.  This must be EXACTLY as provided by the other site.'),
      '#type' => 'textfield',
      '#default_value' => variable_get('uwho_client_url', '')
    )
  ));
}