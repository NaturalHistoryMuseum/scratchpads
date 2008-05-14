<?php
// $Id: location.inc,v 1.4.4.2 2008/03/17 13:02:50 robrechtj Exp $

/**
 * Implementation of hook_node_import_fields().
 */
function location_node_import_fields($type) {
  if (!variable_get('location_maxnum_'. $type, 0)) {
    return;
  }

  $fields = array();
  foreach ((array)location_field_names() as $field => $fieldname) {
    if (variable_get('location_'. $field .'_'. $type, $field == 'country' ? 1 : 0)) {
      $fields['node_import_location_'. $field] = t('Location: @fieldname', array('@fieldname' => $fieldname));
    }
  }
  if (user_access('submit latitude/longitude')) {
    $fields['node_import_location_latitude'] = t('Location: Latitude');
    $fields['node_import_location_longitude'] = t('Location: Longitude');
  }
  return $fields;
}

/**
 * Implementation of hook_node_import_prepare().
 */
function location_node_import_prepare(&$node, $preview = FALSE) {
  if (!variable_get('location_maxnum_'. $node->type, 0)) {
    return;
  }

  $errors = array();
  $location = array();

  $location_fields = array_keys((array)location_field_names());
  $location_fields[] = 'latitude';
  $location_fields[] = 'longitude';

  foreach ($location_fields as $field) {
    $importfield = 'node_import_location_'. $field;
    if (isset($node->$importfield)) {
      $location[$field] = trim($node->$importfield);
      unset($node->$importfield);
    }
  }

  // Try to find a valid country and province code.
  if (isset($location['country']) && $location['country'] != '') {
    $country_code = _node_import_location_get_country_code($location['country']);
    if ($country_code == '') {
      $errors[] = t('%input is not a valid country name or code.', array('%input' => $location['country']));
    }
    else {
      $location['country'] = $country_code;
    }
  }

  $province_code = '';
  if (isset($location['province']) && $location['province'] != '') {
    $province_code = _node_import_location_get_province_code($location['province'], $country_code);
    if ($province_code == '') {
      $errors[] = t('%input is not a valid state or province name or code.', array('%input' => $location['province']));
    }
    else {
      $location['province'] = $province_code;
    }
  }

  // Validate required fields (see location_nodeapi('validate')).
  foreach ((array)location_field_names() as $field => $fieldname) {
    $workflow = variable_get('location_'. $field .'_'. $node->type, $field == 'country' ? 1 : 0);
    switch ($workflow) {
      case 0: // not collected
        unset($location[$field]);
        break;
      case 1: // not required
        break;
      case 2: // required
        if (!isset($location[$field]) || strlen($location[$field]) == 0) {
          $errors[] = t('The %field field is required.', array('%field' => $fieldname));
        }
        break;
    }
  }

  // Validate submitted latitude/longitude (see location_nodeapi('validate')).
  if (user_access('submit latitude/longitude')) {
    if (empty($location['latitude']) || empty($location['longitude'])) {
      if (empty($location['latitude']) xor empty($location['longitude'])) {
        $errors[] = t('You must fill in both latitude and longitude or leave them both blank.');
      }
    }
    else {
      if (!is_numeric($location['latitude']) || $location['latitude'] > 90.0 || $location['latitude'] < -90.0) {
        $errors[] = t('The latitude must be a numeric value between -90.0 and 90.0.');
      }
      if (!is_numeric($location['longitude']) || $location['longitude'] > 180.0 || $location['longitude'] < -180.0) {
        $errors[] = t('The longitude must be a numeric value between -180.0 and 180.0.');
      }
    }
  }

  // Add geocoding if available and needed.
  if (!($preview || count($errors))) {
    if (isset($location['latitude']) && isset($location['longitude'])) {
      $location['source'] = LOCATION_LATLON_USER_SUBMITTED;
    }
    else {
      $lc = location_latlon_exact($location);
      if (isset($lc)) {
        $location['latitude'] = $lc['lat'];
        $location['longitude'] = $lc['lon'];
        $location['source'] = LOCATION_LATLON_GEOCODED_EXACT;
      }
      else {
        $lc = location_latlon_rough($location);
        if (isset($lc)) {
          $location['latitude'] = $lc['lat'];
          $location['longitude'] = $lc['lon'];
          $location['source'] = LOCATION_LATLON_GEOCODED_APPROX;
        }
      }
    }
  }

  if (!empty($location)) {
    $node->locations = array(0 => $location);
  }

  return $errors;
}

/**
 * Convert the country from human-readable form
 * to two-letter country code.
 */
function _node_import_location_get_country_code($country) {
  static $iso_list;
  static $country_list;

  if (!isset($iso_list)) {
    $iso_list = location_get_iso3166_list();
    $country_list = array_flip(array_map('strtolower', $iso_list));
  }

  $country = trim(strtolower($country));

  if (isset($iso_list[$country])) {
    return $country;
  }

  if (isset($country_list[$country])) {
    return $country_list[$country];
  }

  return '';
}

/**
 * Convert the province from human-readable form
 * to a province code.
 */
function _node_import_location_get_province_code($province, $country) {
  static $codes = array();
  if (!isset($codes[$country])) {
    $form = _location_province_select_options('', FALSE, $country);
    $codes[$country] = (array)$form['#options'];
  }

  $province = strtolower($province);
  foreach ((array)$codes[$country] as $code => $name) {
    if (in_array(strtolower($code), array($province, "$country-$province")) || $province == strtolower($name)) {
      return $code;
    }
  }

  return '';
}
