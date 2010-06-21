<?php

function migrate_image_to_imagefield($field_name = 'field_imagefield'){
  $ret = array();
  // Ensure that the following modules are enabled
  module_rebuild_cache();
  $modules = array(
    'imagecache',
    'imageapi',
    'imageapi_imagemagick',
    'filefield',
    'imagefield'
  );
  foreach($modules as $module){
    if(!db_result(db_query("SELECT COUNT(*) FROM {system} WHERE name = '%s'", $module))){
      $ret['#abort'] = array(
        'success' => FALSE,
        'query' => t('A required module is missing.')
      );
      return $ret;
    }
  }
  // Set the variable for the Image gallery
  variable_set('image_gallery_vid', variable_get('image_gallery_nav_vocabulary', 0));
  module_enable($modules);
  include_once './includes/install.inc';
  drupal_install_modules($modules);
  $ret[] = array(
    'success' => TRUE,
    'query' => t('Required modules installed.')
  );
  // Clear caches (we'll do this again at the end).
  drupal_rebuild_theme_registry();
  node_types_rebuild();
  menu_rebuild();
  cache_clear_all('schema', 'cache');
  drupal_clear_css_cache();
  drupal_clear_js_cache();
  // Need to manually call init, as it isn't called when a module is installed.
  imagefield_init();
  filefield_init();
  // We'll get the current sizes that are being used by the image module.  If 
  // all the names match ones being provided by other modules, then we happy, 
  // else we'll need to enable the imagecache_ui module and create extra presets
  $image_sizes = variable_get('image_sizes', array());
  $imagecache_sizes = module_invoke_all('imagecache_default_presets');
  $presets_to_create = array();
  foreach($image_sizes as $name => $values){
    if($name != '_original' && !isset($imagecache_sizes[$name])){
      $presets_to_create[$name] = $values;
      $ret[] = array(
        'success' => TRUE,
        'query' => t('Created additional preset !name', array(
          '!name' => $name
        ))
      );
    }
  }
  if(count($presets_to_create)){
    module_enable(array(
      'imagecache_ui'
    ));
    drupal_install_modules(array(
      'imagecache_ui'
    ));
    foreach($presets_to_create as $name => $values){
      $preset = imagecache_preset_save(array(
        'presetname' => $name
      ));
      $action = array(
        'presetid' => $preset['presetid'],
        'weight' => '0',
        'module' => 'imagecache',
        'action' => ($values['operation'] == 'scale') ? 'imagecache_scale' : 'imagecache_scale_and_crop',
        'data' => array(
          'width' => $values['width'],
          'height' => $values['height'],
          'upscale' => 0
        )
      );
      imagecache_action_save($action);
    }
  }
  // Disable the image module
  $module_cache = module_rebuild_cache();
  $uninstall_modules = array_merge(array(
    'image' => 'image'
  ), $module_cache['image']->info['dependents']);
  $image_attach_was_enabled = module_exists('image_attach');
  module_disable($uninstall_modules);
  $ret[] = array(
    'success' => TRUE,
    'query' => t('Disabled modules (!modules)', array(
      '!modules' => implode(", ", $uninstall_modules)
    ))
  );
  // Create a content type "image"
  $type = new stdClass();
  $type->type = 'image';
  $type->name = t('Image');
  $type->description = t('An image (with thumbnail). This is ideal for publishing photographs or screenshots.');
  $type->custom = 1;
  $type->custom = 1;
  $type->locked = 1;
  $type->body_label = t('Notes');
  $type->title_label = t('Title');
  $type->has_title = 1;
  $type->has_body = 1;
  $type->module = 'node';
  node_type_save($type);
  // Create a field which will be used for storing the images for the new content
  // type.
  $field = array(
    'label' => 'Image',
    'field_name' => $field_name,
    'type_name' => 'image',
    'type' => 'filefield',
    'widget_type' => 'imagefield_widget',
    'file_extensions' => 'jpg gif png',
    'progress_indicator' => 'bar',
    'file_path' => 'images',
    'group' => false,
    'required' => 0,
    'multiple' => '1',
    'module' => 'filefield',
    'widget_module' => 'imagefield'
  );
  module_load_include('crud.inc', 'content', 'includes/content');
  content_field_instance_create($field);
  $ret[] = array(
    'success' => TRUE,
    'query' => t('Recreated image content type')
  );
  // We need to add the field to all the content types that have image_attach
  // enabled
  // FIXME - DO THIS
  $node_types = node_get_types();
  foreach($node_types as $type => $type_details){
    if(variable_get('image_attach_' . $type, FALSE)){
      $field['type_name'] = $type;
      content_field_instance_create($field);
      $ret[] = array(
        'success' => TRUE,
        'query' => t('Associated imagefield with "!name" content type.', array(
          '!name' => $type
        ))
      );
    }
  }
  // We need to set this field to use the thumbnail and preview imagecache 
  // presets that we're created using the scratchpadify module.
  // All node types
  db_query("UPDATE {content_node_field_instance} SET display_settings = '%s' WHERE field_name = 'field_imagefield'", 'a:6:{s:6:"weight";i:0;s:6:"parent";s:0:"";s:5:"label";a:1:{s:6:"format";s:6:"hidden";}s:6:"teaser";a:2:{s:6:"format";s:42:"imagefield__lightbox2__thumbnail__original";s:7:"exclude";i:0;}s:4:"full";a:2:{s:6:"format";s:42:"imagefield__lightbox2__thumbnail__original";s:7:"exclude";i:0;}i:0;b:0;}');
  // Image node type
  db_query("UPDATE {content_node_field_instance} SET display_settings = '%s' WHERE type_name = 'image' AND field_name = 'field_imagefield'", 'a:6:{s:6:"weight";i:0;s:6:"parent";s:0:"";s:5:"label";a:1:{s:6:"format";s:6:"hidden";}s:6:"teaser";a:2:{s:6:"format";s:41:"imagefield__lightbox2__thumbnail__preview";s:7:"exclude";i:0;}s:4:"full";a:2:{s:6:"format";s:15:"preview_default";s:7:"exclude";i:0;}i:0;b:0;}');
  // Create entries for each node in the content_type_xxx table.
  $sql = "INSERT INTO {content_type_image} (vid, nid)
          SELECT 
            n.vid, n.nid 
          FROM {node} n
          INNER JOIN {upload} u ON n.nid = u.nid
          INNER JOIN {files} f ON u.fid = f.fid
          WHERE 
            n.type = 'image' AND 
            f.filename = '_original'";
  $ret[] = update_sql($sql);
  // Create entries for each image in the content_field_xxx table.
  $sql = "INSERT INTO {content_$field_name} (vid, nid, delta, " . $field_name . "_fid, " . $field_name . "_list, " . $field_name . "_data)
          SELECT 
            n.vid, n.nid, 0, f.fid, 1, CONCAT('a:1:{s:11:\"description\";s:', CHAR_LENGTH(n.title), ':\"', n.title, '\";}')
          FROM {node} n 
          INNER JOIN image i ON n.nid = i.nid 
          INNER JOIN files f ON i.fid = f.fid 
          WHERE 
            n.type = 'image' AND 
            i.image_size = '_original'";
  $ret[] = update_sql($sql);
  // Set the needed filename in the files table.
  $image_path = file_create_path(variable_get('image_default_path', 'images'));
  $length = strlen($image_path) + 2;
  $sql = "UPDATE {files} SET filename = SUBSTRING(filepath, $length) WHERE filename = '_original'";
  $ret[] = update_sql($sql);
  // Loop over the image_attach records
  // FIXME - Give the option to attach the image as either the file, or as a 
  // nodereference.  Nodereference may be more desirable for those users who
  // associate a lot of metadata with an image.
  if($image_attach_was_enabled){
    // Because MySQL is shite, we'll do a select query, and then update/insert
    // using the results.
    $results = db_query("SELECT n.nid, n.vid, " . $field_name . "_fid, " . $field_name . "_data FROM {node} n, {content_$field_name} c, {image_attach} i WHERE i.iid = c.nid AND i.nid = n.nid;");
    if(!$results){
      $ret[] = array(
        'success' => FALSE,
        'query' => t('Failed with initial image_attach query.')
      );
    }
    while($row = db_fetch_array($results)){
      $result = db_query("INSERT INTO {content_$field_name} (nid, vid, delta, " . $field_name . "_fid, " . $field_name . "_list, " . $field_name . "_data) VALUES (%d, %d, 0, %d, 1, '%s')", $row['nid'], $row['vid'], $row[$field_name . '_fid'], $row[$field_name . '_data']);
      if(!$result){
        $ret[] = array(
          'success' => FALSE,
          'query' => t('Failed with image_attach.')
        );
      }else{
        $ret[] = array(
          'success' => TRUE,
          'query' => t('Attached image to node nid:!nid', array(
            '!nid' => $row['nid']
          ))
        );
      }
    }
  }
  // Move files with "+" in them (remove the +).
  $results = db_query("SELECT * FROM {files} WHERE filepath LIKE '%%+%%'");
  while($row = db_fetch_array($results)){
    if(rename($row['filepath'], str_replace('+', '', $row['filepath']))){
      $ret[] = update_sql("UPDATE {files} SET filename = REPLACE(filename, '+', ''), filepath = REPLACE(filepath, '+', '') WHERE fid = ".$row['fid']);
    } else {
      $ret[] = array(
        'success' => FALSE,
        'query' => t('Failed to rename !image_filename with FID !fid', array('!image_filename' => $row['filename'], '!fid' => $row['fid']))
      );      
    }
  }
  // ADD Rewrites from current taxonomy/term based URLs to image/[tid] URLS
  // 
  // Clear ALL CACHE, AND SESSIONS  
  drupal_rebuild_theme_registry();
  node_types_rebuild();
  menu_rebuild();
  cache_clear_all('*', 'cache_views', TRUE);
  cache_clear_all('*', 'cache_views_data', TRUE);
  cache_clear_all('*', 'cache_menu', TRUE);
  drupal_clear_css_cache();
  drupal_clear_js_cache();
  return $ret;
}