<?php

/**
 * @file
 * Support for image module.
 */

/**
 * Implementation of hook_node_import_fields().
 */
function image_node_import_fields($type) {
  $items = array();
  if ($type == 'image') {
    $items['node_import_images'] = t('Image: Image');
  }
  return $items;
}

/**
 * Implementation of hook_node_import_prepare().
 */
function image_node_import_prepare(&$node, $preview) {
  $errors = array();
  if ($node->type == 'image') {
    if (empty($node->node_import_images)) {
      $errors[] = t('You need to specify an image for this content type.');
      unset($node->node_import_images);
    }
    else {
      $filename = $node->node_import_images;
      $filepath = variable_get('file_directory_path', 'files') .'/'. variable_get('image_default_path', 'images');
      $check = file_create_path($filepath .'/'. $filename);
      if (!file_exists($check)) {
        $errors[] = t('Image %filename not found. You need to upload it to %filepath first.', array('%filename' => $filename, '%filepath' => $filepath));
        unset($node->node_import_images);
      }
      else if (!$image_info = image_get_info($check)) {
        // Ensure it's a valid image.
        $errors[] = t('The file %filename does not appear to be an image.', array('%filename' => $filename));
        unset($node->node_import_images);
      }
      else if (!$preview) {
        $filename = $node->node_import_images;
        $filepath = variable_get('file_directory_path', 'files') .'/'. variable_get('image_default_path', 'images');
        $filepath = file_create_path($filepath .'/'. $filename);

        $node->images = array(IMAGE_ORIGINAL => $filepath);
      }
    }
  }
  return $errors;
}

/**
 * Implementation of hook_node_import_postprocess().
 */
function image_node_import_postprocess(&$node, $preview, $error) {
  if (isset($node->node_import_images)) {
    if ($preview) {
      return t('Image %filename.', array('%filename' => $node->node_import_images));
    }
  }
}

