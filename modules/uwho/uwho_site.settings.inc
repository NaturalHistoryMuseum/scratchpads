<?php

/**
 * Settings page callback
 */
function uwho_site_settings(){
  return system_settings_form(array(
    'container' => array(
      '#type' => 'fieldset',
      '#title' => '',
      '#description' => t('Enter the following URL and Key into your UWho client on any other site.'),
      '#collapsible' => FALSE,
      'uwho_site_key_fieldset' => array(
        '#type' => 'fieldset',
        '#title' => t('UWho Key'),
        '#collapsible' => FALSE,
        'uwho_site_key' => array(
          '#value' => '<h2>' . variable_get('uwho_site_key', '') . '</h2>'
        )
      ),
      'uwho_site_url_fieldset' => array(
        '#type' => 'fieldset',
        '#title' => t('UWho URL'),
        '#collapsible' => FALSE,
        'uwho_site_url' => array(
          '#value' => '<h2>' . url('uwho', array('absolute' => TRUE)) . '</h2>'
        )
      )
    )
  ));
}