<?php
// $Id: event.inc,v 1.3.2.1.2.2 2008/03/17 13:06:02 robrechtj Exp $

if (file_exists(drupal_get_path('module', 'event') .'/event_timezones.inc')) {
  include_once(drupal_get_path('module', 'event') . '/event_timezones.inc');
}
else {
  include_once(drupal_get_path('module', 'date') .'/date_timezones.inc');
}

/**
 * Implementation of hook_node_import_fields().
 */
function event_node_import_fields($type) {
  if (event_is_enabled($type)) {
    return array(
      'node_import_event_start' => t('Event: Complete start date or timestamp'),
      'node_import_event_end' => t('Event: Complete end date or timestamp'),
      'start_year' => t('Event: Start year'),
      'start_month' => t('Event: Start month'),
      'start_day' => t('Event: Start day'),
      'start_hour' => t('Event: Start hour'),
      'start_minute' => t('Event: Start minute'),
      'end_year' => t('Event: End year'),
      'end_month' => t('Event: End month'),
      'end_day' => t('Event: End day'),
      'end_hour' => t('Event: End hour'),
      'end_minute' => t('Event: End minute'),    
    );
  }
}

/**
 * Implementation of hook_node_import_prepare().
 */
function event_node_import_prepare(&$node, $preview = FALSE) {
  if (!event_is_enabled($node->type)) {
    return;
  }
  $errors = array();

  foreach (array('start', 'end') as $what) {
    $whatdate = 'node_import_event_' . $what;
    
    // Hard code a default date since event module seems to break if you don't
    if (! isset($node->{$what .'_year'})) $node->{$what .'_year'} = 1970;
    if (! isset($node->{$what .'_month'})) $node->{$what .'_month'} = 1;
    if (! isset($node->{$what .'_day'})) $node->{$what .'_day'} = 1;
    if (! isset($node->{$what .'_hour'})) $node->{$what .'_hour'} = 0;
    if (! isset($node->{$what .'_minute'})) $node->{$what .'_minute'} = 0;
    if (isset($node->$whatdate)) {
      if (($time = node_import_valid_date($node->$whatdate)) > -1) {
        $node->{$what .'_year'} = date('Y', $time);
        $node->{$what .'_month'} = date('n', $time);
        $node->{$what .'_day'} = date('d', $time);
        $node->{$what .'_hour'} = date('H', $time);
        $node->{$what .'_minute'} = date('i', $time);
      }
      unset($node->$whatdate);
    }
    if (module_exists('jscalendar')) {
      // jscalendar.module supplies $node->{$what.'_date'} and event.module
      // always uses that when jscalendar is enabled. So we need to set
      // it. The format is: YYYY.MM.DD.hh.mm.ss (where dots are ignored).
      $parts = array(
        $node->{$what .'_year'},
        $node->{$what .'_month'},
        $node->{$what .'_day'},
        $node->{$what .'_hour'},
        $node->{$what .'_minute'},
        "00",
      );
      $node->{$what .'_date'} = implode(' ', $parts);
    }
  }

  // Let event module add proper event related fields on the basis of the above
  event_nodeapi($node, 'submit');

  return $errors;
}

/**
 * Implementation of hook_node_import_global().
 */
function event_node_import_global($type, $globals) {
  if (event_is_enabled($type)) {
    if (variable_get('event_timezone_input', 'site') == 'input') {
      $form = array();
      $form['timezone'] = array(
        '#type' => 'select',
        '#title' => t('Time zone'),
        '#default_value' => event_timezone_map(variable_get('date_default_timezone', 0)),
        '#options' => event_zonelist(),
        '#description' => t('Select the time zone all imported events occur in.'),
      );
      return $form;
    }
  }
}

/**
 * Implementation of hook_node_import_static().
 */
function event_node_import_static($type) {
  if (event_is_enabled($type)) {
    if (variable_get('configurable_timezones', 1)
        && $user->uid
        && strlen($user->timezone)
        && (variable_get('event_timezone_input', 'site') == 'user')) {
      return array('event_info' => 1, 'timezone' => event_timezone_map($user->timezone));
    }
    else if (variable_get('event_timezone_input', 'site') != 'input') {
      return array('event_info' => 1, 'timezone' => event_timezone_map(variable_get('date_default_timezone', 0)));
    }
  }
}
