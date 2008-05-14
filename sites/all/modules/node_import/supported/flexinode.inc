<?php
/* $Id: flexinode.inc,v 1.1 2006/07/10 09:22:10 robrechtj Exp $ */

/**
 * Implementation of hook_node_import_types().
 */
function flexinode_node_import_types() {
  $types = array();
  foreach (flexinode_content_types() as $id => $type) {
    $types['flexinode-'. $id] = $type->name;
  }
  return $types;
}

/**
 * Implementation of hook_node_import_fields().
 */
function flexinode_node_import_fields($type) {
  $type = explode('-', $type);

  if ($type[0] == 'flexinode') {
    $content_type = flexinode_load_content_type($type[1]);
    $fields = array('title' => t('Title'));
    foreach ($content_type->fields as $field) {
      $fields['flexinode_'. $field->field_id] = $field->label;
    }
    return $fields;
  }
}

/**
 * Implementation of hook_node_import_static().
 */
function flexinode_node_import_static($type) {
  $type = explode('-', $type);

  if ($type[0] == 'flexinode') {
    return array('ctype_id' => $type[1]);
  }
}

/**
 * Implementation of hook_node_import_prepare().
 */
function flexinode_node_import_prepare(&$node, $preview = FALSE) {
  $type = explode('-', $node->type);

  if ($type[0] == 'flexinode') {
    $content_type = flexinode_load_content_type($type[1]);
    foreach ($content_type->fields as $field) {
      if ($field->field_type == 'timestamp' && isset($node->{'flexinode_'. $field->field_id})) {
        if (($time = strtotime($node->{'flexinode_'. $field->field_id})) == -1) {
          form_set_error($node->{'flexinode_'. $field->field_id}, t('Date is in an unrecognisable format, for example use "9/24/2005 8:02pm"'));
        }
        else {
          $time = getdate($time);
          $node->{'flexinode_'. $field->field_id .'_year'} = $time['year'];
          $node->{'flexinode_'. $field->field_id .'_month'} = $time['mon'];
          $node->{'flexinode_'. $field->field_id .'_day'} = $time['mday'];
          $node->{'flexinode_'. $field->field_id .'_hour'} = $time['hours'];
          $node->{'flexinode_'. $field->field_id .'_minute'} = $time['minutes'];
        }
        // Hmm.
        unset($node->$whatdate);
      }
    }
  }
}
