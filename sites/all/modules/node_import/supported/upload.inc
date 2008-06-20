<?php

/**
 * @file
 * Support for upload module.
 */

/**
 * Implementation of hook_node_import_fields().
 */
function upload_node_import_fields($type) {
  $items = array();
  if (variable_get('upload_'. $type, 1)) {
    $items['node_import_files'] = t('Upload: File attachments');
  }
  return $items;
}

/**
 * Implementation of hook_node_import_prepare().
 */
function upload_node_import_prepare(&$node, $preview) {
  $errors = array();
  if (variable_get('upload'. $node->type, 1)) {
    if (empty($node->node_import_files)) {
      unset($node->node_import_files);
    }
    else {
      $filename = $node->node_import_files;
      $filepath = variable_get('file_directory_path', 'files');
      $check = file_create_path($filename);
      if (!file_exists($check)) {
        $errors[] = t('File attachment %filename not found. You need to upload it to %filepath first.', array('%filename' => $filename, '%filepath' => $filepath));
        unset($node->node_import_files);
      }
    }
  }
  return $errors;
}

// Add mime_content_type() function if it does not exist.
if (!function_exists('mime_content_type')) {
  function mime_content_type($filename) {
    $finfo = finfo_open(FILEINFO_MIME);
    $mimetype = finfo_file($finfo, $filename);
    finfo_close($finfo);
    return $mimetype;
  }
}

/**
 * Implementation of hook_node_import_postprocess().
 */
function upload_node_import_postprocess(&$node, $preview, $error) {
  if (isset($node->node_import_files)) {
    if ($preview) {
      return t('File attachment %filename.', array('%filename' => $node->node_import_files));
    }
    else if (!$error) {
      $fid = db_next_id('{files}_fid');
      $filename = $node->node_import_files;
      $filepath = file_create_path($filename);
      $filemime = mime_content_type($filepath);
      $filesize = filesize($filepath);

      db_query("INSERT INTO {files} (fid, nid, filename, filepath, filemime, filesize) VALUES (%d, %d, '%s', '%s', '%s', %d)", $fid, $node->nid, $filename, $filepath, $filemime, $filesize);
      db_query("INSERT INTO {file_revisions} (fid, vid, list, description) VALUES (%d, %d, %d, '%s')", $fid, $node->vid, 1, '');
      unset($node->node_import_files);
    }
  }
}

