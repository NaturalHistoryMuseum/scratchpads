<?php
/* $Id: content.inc,v 1.1.2.2.2.18 2008/11/03 20:23:54 robrechtj Exp $ */

/**
 *  CCK data can be imported into any column of a CCK field.
 *  Most fields have a 'value' column, but there are others that 
 *  can be used, for example:
 *   - Textareas have a 'value' and a 'format'
 *   - Date fields have a 'value', if date-specific timezones are
 *       used they also have a 'timezone' and 'offset'
 *   - Nodereference fields have a 'nid' column
 *   - Userreference fields have a 'uid' column
 * 
 *  For nodereference and userreference fields you can either import the
 *  numeric value for a nid or uid, or a text value for the title or
 *  name. If a numeric value is imported, it is used as is.
 *  If a title or name is imported, the module will search for 
 *  the nid or uid that goes with that title or name.
 * 
 *  Date values can be imported as unix timestamps, as ISO values, or as
 *  any text that can be interpreted by the php strtotime function.
 *  The date 'timezone' column expects a full timezone name like 'US/Central', 
 *  timezone abbreviations are not unique and will not work.
 *  The date 'offset' column is the plus or minus value in seconds.
 * 
 *  @todo
 *  Multiple values are not importing correctly yet -- CCK is creating
 *  three blank values before the valid values, so a fix to CCK is needed.
 *  Once fixed the idea is that multiple values can be imported into any 
 *  CCK field that has multiple values enabled by separating the values 
 *  with a double pipe (||).
 */


/**
 * Implementation of hook_node_import_fields().
 */
function content_node_import_fields($type) {
  $types = (array) content_types();
  $fields = array();

  if (isset($types[$type])) {
    $content_type = $types[$type];
    foreach ($content_type['fields'] as $field) {
      // Create a potential input field for each field column by creating a dummy name for each column.
      $db_info = content_database_info($field);
      foreach ($db_info['columns'] as $column => $info) {
        $fields[$field['field_name'] .'_'. $column] = $field['widget']['label'] .' '. $column .' ('. $field['field_name'] .')';
      }
    }
  }
  return $fields;
}

/**
 * Implementation of hook_node_import_prepare().
 */
function content_node_import_prepare(&$node, $preview = FALSE) {
  $errors = array();
  $globals = $node->content_import_node;
  unset($node->content_import_node);

  $types = (array) content_types();
  if (isset($types[$node->type])) {
    $content_type = $types[$node->type];
    foreach ($content_type['fields'] as $field) {
      $field_name = $field['field_name'];

      // Find the column names for this field type and process each.
      $db_info = content_database_info($field);
      foreach ($db_info['columns'] as $column => $info) {
        $dummy_name = $field_name .'_'. $column;

        // Force a node value for missing data.
        if (!isset($node->$dummy_name) || strlen($node->$dummy_name) == 0) {
          $values = array(0 => $globals['fields'][$dummy_name]);
        }
        else {
          // Explode multiple values to create the $delta and $value for each.
          if ($field['multiple']) {
            $values = explode('||', $node->$dummy_name);
          }
          else {
            $values = array(0 => $node->$dummy_name);
          }
        }

        foreach ($values as $delta => $value) {
          switch ($field['type']) {
            case 'date':
            case 'datestamp':
              // Get properly formatted date for value column.
              // Assume timezone and offset values are correct as input.
              if ($column == 'value' && $value != '') {
                if (variable_get('date_api_version', 0) >= 5.2) {
                  // Date 2 API
                  include_once('./'. drupal_get_path('module', 'date') .'/date_token.inc');
                  if ($field['type'] == 'date') {
                    // Is the imported date a unix timestamp? If so convert it to ISO date.
                    if (is_numeric($value) && date_is_valid($value, DATE_UNIX)) {
                      $value = date_convert($value, DATE_UNIX, DATE_ISO);
                    }
                    // If the imported date is a (partial) ISO date, convert it
                    // to date's ISO date.
                    else if (($date_array = date_convert($value, DATE_ISO, DATE_ARRAY)) != NULL) {
                      $value = date_convert($date_array, DATE_ARRAY, DATE_ISO);
                    }
                    // Otherwise, use strtotime and unix2iso to convert 
                    // partial ISO or text date into full ISO.
                    else {
                      $value = date_convert(strtotime($value), DATE_UNIX, DATE_ISO);
                    }
                  }
                  else {
                    // Is this already a unix timestamp? If not try strototime.
                    if (!is_numeric($value) || !date_is_valid($value, DATE_UNIX)) {
                      $value = strtotime($value);
                    }
                  }
                }
                else {
                  // Date 1 API
                  include_once('./'. drupal_get_path('module', 'date') .'/date.inc');
                  if ($field['type'] == 'date') {
                    // Is the imported date a unix timestamp? If so convert it to ISO date.
                    if (is_numeric($value) && date_is_valid($value, DATE_UNIX)) {
                      $value = date_unix2iso($value);
                    }
                    // If the imported date is a (partial) ISO date, convert it
                    // to date's ISO date.
                    else if (($date_array = date_iso2array($value)) != 'ERROR') {
                      $value = date_array2iso($date_array);
                    }
                    // Otherwise, use strtotime and unix2iso to convert 
                    // partial ISO or text date into full ISO.
                    else {
                      $value = date_unix2iso(strtotime($value));
                    }
                  }
                  else {
                    // Is this already a unix timestamp? If not try strototime.
                    if (!is_numeric($value) || !date_is_valid($value, DATE_UNIX)) {
                      $value = strtotime($value);
                    }
                  }
                }
              }
              break;

            case 'nodereference':
              $tmp = node_import_nodereference($value);
              $value = (isset($tmp) ? $tmp : $value);
              break;

            case 'userreference':
              $tmp = node_import_userreference($value);
              $value = (isset($tmp) ? $tmp : $value);
              break;

            case 'number_integer':
              if (strlen($value) > 0) {
                $value = intval($value);
              }
              break;

            case 'number_decimal':
              if (strlen($value) > 0) {
                $value = floatval($value);
              }
              break;
          }

          // Set the correct node field values.
          if (!isset($node->$field_name)) {
            $node->$field_name = array($delta => array($column => $value));
          }
          else if (!isset($node->{$field_name}[$delta])) {
            $node->$field_name += array($delta => array($column => $value));
          }
          else {
            $node->{$field_name}[$delta] += array($column => $value);
          }
        }

        // We need to handle a couple of field widget types specially.
        switch ($field['widget']['type']) {
          case 'options_select':
          case 'options_onoff':
          case 'options_buttons':
            $keys = array();
            $allowed_values = _optionwidgets_options($field, $node);
            $allowed_keys = array();
            foreach ($allowed_values as $key => $value) {
              $key = (string)$key;
              $allowed_keys[strtolower($key)] = $key;
              $allowed_keys[strtolower($value)] = $key;
            }
            
            foreach ($node->$field_name as $delta => $value_list) {
              $value = strtolower($value_list['value']);
              if (isset($allowed_keys[$value])) {
                $keys[] = $allowed_keys[$value];
              }
              else {
                $errors[] = t('%value is not an allowed value for %label.', array('%value' => $value_list['value'], '%label' => $field['widget']['label']));
              }
            }

            if ($field['multiple'] || $field['widget']['type'] == 'options_onoff') {
              $node->$field_name += array('keys' => $keys);
            }
            else {
              $node->$field_name += array('key' => reset($keys));
            }
            break;

          case 'nodereference_autocomplete':
            foreach ($node->$field_name as $delta => $value) {
              $value['node_name'] = $value['nid'] .' [nid:'. $value['nid'] .']';
              $node->{$field_name}[$delta] = $value;
            }
            break;

          case 'nodereference_select':
            $nids = array();
            foreach ($node->$field_name as $delta => $nid_list) {
              $nids[$nid_list['nid']] = $nid_list['nid'];
            }
            if ($field['multiple']) {
              $node->$field_name += array('nids' => $nids);
            }
            else {
              $node->$field_name += array('nid' => reset($nids));
            }
            break;

          case 'link':
            foreach ($node->$field_name as $delta => $link_list) {
              if (!isset($link_list['attributes']) || strlen($link_list['attributes']) == 0) {
                $node->{$field_name}[$delta]['attributes'] = $field['widget']['default_value'][0]['attributes'];
              }
            }
            break;
        }

        // Unset the dummy column value.
        unset($node->$dummy_name);
      }

    }
  }

  return $errors;
}

/**
 * Implementation of hook_node_import_global().
 */
function content_node_import_global($type, $global_values) {
  $form = array();
  $form['content_import_node']['fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Field options'),
    '#description' => t('Select the default values you want to assign to all imported fields unless specifically set otherwise in the CSV file'),
    '#tree' => TRUE,
  );
  $types = (array) content_types();
  if (isset($types[$type])) {
    $content_type = $types[$type];
    foreach ($content_type['fields'] as $field) {
      // Create a default value for each field column by creating a dummy name for each column.
      $db_info = content_database_info($field);
      foreach ($db_info['columns'] as $column => $info) {
        $value = ((!empty($field['widget']['default_value'][0]['value'])) ?  $field['widget']['default_value'][0]['value'] : '');
        $form['content_import_node']['fields'][$field['field_name'] .'_'. $column] = array(
          '#type' => 'textfield',
          '#title' => $field['widget']['label'] .' '. $column,
          '#default_value' => $value,
        );
      }
    }
  }
  return $form;
}

