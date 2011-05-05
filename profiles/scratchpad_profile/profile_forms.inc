<?php

/**
 * Define form used by clustrmap installer task
 */
function scratchpad_personal($form_state, $url){
  $user = user_load(array(
    'uid' => 1
  )); // Still uid 1 as we've not moved it
  $name_parts = explode(" ", $user->name);
  $familyname = array_pop($name_parts);
  $givenname = implode(" ", $name_parts);
  return array(
    '#action' => $url,
    '#redirect' => FALSE,
    'title' => array(
      '#required' => TRUE,
      '#type' => 'textfield',
      '#title' => st('Title')
    ),
    '#action' => $url,
    '#redirect' => FALSE,
    'given' => array(
      '#required' => TRUE,
      '#type' => 'textfield',
      '#title' => st('Given Name(s)'),
      '#default_value' => $givenname
    ),
    'family' => array(
      '#required' => TRUE,
      '#type' => 'textfield',
      '#title' => st('Family Name'),
      '#default_value' => $familyname
    ),
    'institution' => array(
      '#required' => FALSE,
      '#type' => 'textfield',
      '#title' => st('Institution')
    ),
    'expertise' => array(
      '#required' => FALSE,
      '#type' => 'textfield',
      '#title' => st('Area of Taxonomic expertise')
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => st('Save and continue')
    )
  );
}

/**
 * Save the form shit
 */
function scratchpad_personal_submit($form, &$form_state){
  variable_set('personal_submitted', TRUE);
  $node = new stdClass();
  $node->type = 'profile';
  $node->uid = 2;
  $node->field_title = array(
    array(
      'value' => $form_state['values']['title']
    )
  );
  $node->field_givennames = array(
    array(
      'value' => $form_state['values']['given']
    )
  );
  $node->field_familyname = array(
    array(
      'value' => $form_state['values']['family']
    )
  );
  $node->field_institution = array(
    array(
      'value' => $form_state['values']['institution']
    )
  );
  $node->field_taxonomicinterest = array(
    array(
      'value' => $form_state['values']['expertise']
    )
  );
  $node->title = "{$form_state['values']['title']} {$form_state['values']['given']} {$form_state['values']['family']}";
  $node->auto_nodetitle_applied = TRUE;
  node_save($node);
}

/**
 * Define form used by gmapkey installer task
 */
function scratchpad_gmapkey($form_state, $url){
  return array(
    '#action' => $url,
    '#redirect' => FALSE,
    'gmapkey' => array(
      '#type' => 'textfield',
      '#title' => st('Google Maps API Key'),
      '#default_value' => '',
      '#required' => TRUE,
      '#description' => st('A Google maps API key is required for your Scratchpad to function properly.  One can be obtained from <a href="http://code.google.com/apis/maps/signup.html">http://code.google.com/apis/maps/signup.html</a>.')
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => st('Save and continue')
    )
  );
}

/**
 * Save the form shit
 */
function scratchpad_gmapkey_submit($form, &$form_state){
  // Set the google API key
  variable_set('googlemap_api_key', $form_state['values']['gmapkey']);
  variable_set('gmap_default', array(
    'width' => '100%',
    'height' => '400px',
    'latlong' => '56,11',
    'zoom' => 3,
    'maxzoom' => 14,
    'styles' => array(
      'line_default' => array(
        '0000ff',
        5,
        45,
        '',
        ''
      ),
      'poly_default' => array(
        '000000',
        3,
        25,
        'ff0000',
        45
      )
    ),
    'controltype' => 'Large',
    'mtc' => 'standard',
    'maptype' => 'Hybrid',
    'baselayers' => array(
      'Map' => 1,
      'Satellite' => 0,
      'Hybrid' => 1,
      'Physical' => 1
    ),
    'behavior' => array(
      'locpick' => '',
      'nodrag' => 0,
      'nokeyboard' => 1,
      'nomousezoom' => 1,
      'nocontzoom' => 0,
      'autozoom' => 1,
      'dynmarkers' => 1,
      'overview' => 0,
      'collapsehack' => 1,
      'scale' => 0
    ),
    'markermode' => 0,
    'line_colors' => array(
      '#00cc00',
      '#ff0000',
      '#0000ff'
    )
  ));
}

/**
 * Define form used by clustrmap installer task
 */
function scratchpad_clustrmap($form_state, $url){
  return array(
    '#action' => $url,
    '#redirect' => FALSE,
    'clustrmap' => array(
      '#type' => 'textarea',
      '#title' => st('ClustrMap HTML Code'),
      '#description' => st('A ClustrMap for your site can be obtained from <a href="http://clustrmaps.com/">http://clustrmaps.com/</a>.')
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => st('Save and continue')
    )
  );
}

/**
 * Save the form shit
 */
function scratchpad_clustrmap_submit($form, &$form_state){
  // Create a block with the required code in it.
  if(trim($form_state['values']['clustrmap']) != ''){
    $box = array(
      'body' => $form_state['values']['clustrmap'],
      'info' => 'ClustrMap',
      'title' => ''
    );
    scratchpad_block_add($box);
  }
  variable_set('clustrmap_submitted', TRUE);
}

/**
 * Define form used by clustrmap installer task
 */
function scratchpad_mission($form_state, $url){
  return array(
    '#action' => $url,
    '#redirect' => FALSE,
    'slogan' => array(
      '#type' => 'textfield',
      '#title' => st('Slogan'),
      '#description' => st('Displayed along side your site title')
    ),
    'mission' => array(
      '#type' => 'textarea',
      '#title' => st('Mission statement'),
      '#description' => st('Your site\'s mission statement.  This should be used to describe your site to naive users')
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => st('Save and continue')
    )
  );
}

/**
 * Save the form shit
 */
function scratchpad_mission_submit($form, &$form_state){
  // Create a block with the required code in it.
  if(trim($form_state['values']['mission']) != ''){
    variable_set('site_mission', $form_state['values']['mission']);
  }
  if(trim($form_state['values']['slogan']) != ''){
    variable_set('site_slogan', $form_state['values']['slogan']);
  }
  variable_set('mission_submitted', TRUE);
}

/**
 * Add a box/block and display it
 */
function scratchpad_block_add($box){
  db_query("INSERT INTO {boxes} (body, info, format) VALUES ('%s', '%s', 1)", $box['body'], $box['info']);
  $delta = db_last_insert_id('boxes', 'bid');
  foreach(list_themes() as $theme){
    db_query("INSERT INTO {blocks} (module,delta,theme,status,region,cache,title) VALUES ('block', %d, '%s', 1, 'left', %d, '%s')", $delta, $theme->name, BLOCK_NO_CACHE, $box['title']);
  }
  return;
}