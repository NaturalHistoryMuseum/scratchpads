<?php

/**
 * Field handler to provide a list of files associated with a release node.
 *
 * Based heavily on views_handler_field_upload_fid.inc from
 * contributions/modules/views/modules/upload (forked from revision 1.7).
 * However, we're using separate tables ({project_release_file}, not
 * {upload}), we support a slightly different set of tokens, we need different
 * download links, different handler options, etc. So, it wasn't really
 * possible to extend views_handler_field_upload_fid.inc...
 */
class project_release_handler_field_files extends views_handler_field_prerender_list {
  function construct() {
    parent::construct();
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_file'] = array('default' => FALSE);
    $options['max_files'] = array('default' => 0);
    $options['max_weight'] = array('default' => '');
    $options['file_sort'] = array('default' => 'filename');
    $options['file_sort_order'] = array('default' => 'DESC');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to_file'] = array(
      '#title' => t('Link this field to download the file'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_file']),
    );
    $max_options[0] = t('Show all');
    for ($n = 1; $n <= 10; $n++) {
      $max_options[$n] = $n;
    }
    $form['max_files'] = array(
      '#title' => t('Maximum number of files to display'),
      '#type' => 'select',
      '#options' => $max_options,
      '#default_value' => $this->options['max_files'],
    );
    $form['max_weight'] = array(
      '#title' => t('Maximum file weight to display'),
      '#type' => 'textfield',
      '#default_value' => $this->options['max_weight'],
      '#size' => 2,
      '#description' => t('Leave blank to display files regardless of weight.'),
    );
    $form['file_sort'] = array(
      '#title' => t('Sort files by'),
      '#type' => 'select',
      '#options' => array(
        'weight' => t('File weight'),
        'filename' => t('File name'),
        'filesize' => t('File size'),
        'timestamp' => t('File timestamp'),
        'fid' => t('File ID'),
      ),
      '#default_value' => $this->options['file_sort'],
    );
    $form['file_sort_order'] = array(
      '#title' => t('Sort file direction'),
      '#type' => 'select',
      '#options' => array(
        'ASC' => t('Ascending'),
        'DESC' => t('Descending'),
      ),
      '#default_value' => $this->options['file_sort_order'],
    );
  }

  function pre_render($values) {
    $nids = array();
    $this->items = array();
    foreach ($values as $result) {
      $nids[] = $result->{$this->field_alias};
    }
    if (!empty($nids)) {
      if ($this->options['file_sort'] == 'weight') {
        $order_by = 'prf.weight';
      }
      else {
        $order_by = 'f.' . $this->options['file_sort'];
      }
      $order_by .= ' ' . $this->options['file_sort_order'];
      $wheres[] = 'prf.nid IN (' . implode(', ', $nids) . ')';
      if (is_numeric($this->options['max_weight'])) {
        // We just checked it's numeric, but to be extra safe, cast to an int.
        $wheres[] = 'prf.weight <= ' . (int)$this->options['max_weight'];
      }
      $where = implode(' AND ', $wheres);
      $result = db_query("SELECT prf.nid, prf.fid, prf.filehash, prf.weight, f.uid, f.filename, f.filepath, f.filesize, f.filemime, f.timestamp FROM {project_release_file} prf LEFT JOIN {files} f ON prf.fid = f.fid WHERE $where ORDER BY $order_by");
      while ($file = db_fetch_array($result)) {
        $file['filename'] = check_plain($file['filename']);
        $file['filemime'] = check_plain($file['filemime']);
        $file['filesize'] = format_size($file['filesize']);
        $file['timestamp'] = format_date($file['timestamp']);
        $file_link = theme('project_release_download_link', $file['filepath'], NULL, TRUE);
        $file['filepath'] = $file_link['href'];
        if (!empty($this->options['link_to_file']) ) {
          $file['make_link'] = TRUE;
          $file['path'] = $file['filepath'];
        }
        // Enforce the maximum # of files per node as configured in the field.
        if (!empty($this->options['max_files']) && !empty($this->items[$file['nid']]) && (count($this->items[$file['nid']]) >= $this->options['max_files'])) {
          continue;
        }
        $this->items[$file['nid']][$file['fid']] = $file;
      }
    }
  }

  function render_item($count, $item) {
    return $item['filename'];
  }

  function document_self_tokens(&$tokens) {
    $tokens['[' . $this->options['id'] . '-fid' . ']'] = t('The file ID for the file.');
    $tokens['[' . $this->options['id'] . '-name' . ']'] = t('The name of the attached file.');
    $tokens['[' . $this->options['id'] . '-ext' . ']'] = t("The filename extenstion (e.g. 'zip' or 'tar.gz') of the attached file.");
    $tokens['[' . $this->options['id'] . '-type' . ']'] = t('The MIME type of the attached file.');
    $tokens['[' . $this->options['id'] . '-path' . ']'] = t('The path of the attached file.');
    $tokens['[' . $this->options['id'] . '-size' . ']'] = t('The size of the attached file.');
    $tokens['[' . $this->options['id'] . '-hash' . ']'] = t('The MD5 hash of the attached file.');
    $tokens['[' . $this->options['id'] . '-time' . ']'] = t('The date and time when the file was created or uploaded.');
    $tokens['[' . $this->options['id'] . '-weight' . ']'] = t('The weight of this file relative to others attached to the release.');
  }

  function add_self_tokens(&$tokens, $item) {
    $tokens['[' . $this->options['id'] . '-fid' . ']'] = $item['fid'];
    $tokens['[' . $this->options['id'] . '-name' . ']'] = $item['filename'];
    $tokens['[' . $this->options['id'] . '-type' . ']'] = $item['filemime'];
    $tokens['[' . $this->options['id'] . '-path' . ']'] = $item['filepath'];
    $tokens['[' . $this->options['id'] . '-size' . ']'] = $item['filesize'];
    $tokens['[' . $this->options['id'] . '-hash' . ']'] = $item['filehash'];
    $tokens['[' . $this->options['id'] . '-time' . ']'] = $item['timestamp'];
    $tokens['[' . $this->options['id'] . '-weight' . ']'] = $item['weight'];
    $file_parts = explode('.', basename($item['filename']));
    $ext = array_pop($file_parts);
    // See if the previous extension is '.tar' and if so, add that, so we see
    // 'tar.gz' or 'tar.bz2' instead of just 'gz'.
    $ext2 = array_pop($file_parts);
    if ($ext2 == 'tar') {
      $ext = $ext2 . '.' . $ext;
    }
    $tokens['[' . $this->options['id'] . '-ext' . ']'] = $ext;
  }
}
