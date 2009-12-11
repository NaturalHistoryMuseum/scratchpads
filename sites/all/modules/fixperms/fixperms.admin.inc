<?php
/**
 * Callback for Fixperms admin
 */
function fixperms_admin_settings($form_state){
  
  // Default fieldset.
  $form['default'] = array(
    '#type' => 'fieldset',
    '#title' => 'Default values for new content types',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#suffix' => '</div>',
    '#theme' => 'user_admin_perm'
  );
  $form['override'] = array(
    '#type' => 'checkbox',
    '#title' => t('Override'),
    '#description' => t('Check this box to set the default for ALL custom content types (including newly created ones). Or you can "Fine tune" exactly what each role can do below. (Note, even if the box is unchecked, the default will be applied to newly created content types)'),
    '#prefix' => '<div class="fixperms sticky">',
    '#weight' => -1000,
    '#default_value' => variable_get('fixperms_override', 0)
  );
  // Values per content type fieldset
  $form['percck'] = array(
    '#type' => 'fieldset',
    '#title' => 'Fine Tune Values for creating/editing/deleting of specific content types.',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#theme' => 'user_admin_perm'
  );
  // Fill out the role_names
  $result = db_query('SELECT rid, name FROM {role} ORDER BY rid');
  while ($role = db_fetch_array($result)) {
    $form['default']['role_names'][$role['rid']] = array('#value' => $role['name'], '#tree' => 1);
    $form['percck']['role_names'][$role['rid']] = array('#value' => $role['name'], '#tree' => 1);
  }
  // Fill out the permissions array
  $fixperms_permissions = fixperms_perm();
  foreach($fixperms_permissions as $permission){
    $form['default']['permission'][$permission]['#value'] = $permission;
  }

  // Get the default_values
  $result = db_query('SELECT rid, perm FROM {permission} ORDER BY rid');
  while ($role = db_fetch_array($result)) {
    $role_permissions[$role['rid']] = explode(", ", $role['perm']);
    $percck_role_permissions[$role['rid']] = $role_permissions[$role['rid']];
  }
  foreach($role_permissions as $rid => $permissions){
    $temp_permissions = array();
    foreach($fixperms_permissions as $fixperms_permission){
      if(in_array($fixperms_permission, $permissions)){
        $temp_permissions[] = $fixperms_permission;
      }
      $role_permissions[$rid] = $temp_permissions;
    }
  }
  // Fill out the checkboxes.
  $new_fixperms = array();
  foreach($fixperms_permissions as $fixperms_permission){
    $new_fixperms[$fixperms_permission] = '';
  }
  foreach($form['default']['role_names'] as $rid => $value){
    $form['default']['checkboxes']['default_'.$rid] = array(
      '#type' => 'checkboxes',
      '#options' => $new_fixperms,
      '#default_value' => $role_permissions[$rid]
    );
  }

  // Checkboxes for the per content type section
  $percck_permissions = array();
  $content_type_number = 1;
  foreach(content_types() as $content_type_id => $content_type_array){
    if($content_type_array['module'] == 'node'){
      $form['percck']['permission'][$content_type_number]['#value'] = $content_type_array['name'];
      $form['percck']['permission']['create '.$content_type_id.' content']['#value'] = 'create '.$content_type_id.' content';
      $form['percck']['permission']['edit own '.$content_type_id.' content']['#value'] = 'edit own '.$content_type_id.' content';
      $form['percck']['permission']['delete own '.$content_type_id.' content']['#value'] = 'delete own '.$content_type_id.' content';
      $form['percck']['permission']['edit any '.$content_type_id.' content']['#value'] = 'edit any '.$content_type_id.' content';
      $form['percck']['permission']['delete any '.$content_type_id.' content']['#value'] = 'delete any '.$content_type_id.' content';
      $percck_permissions = array_merge($percck_permissions, array(
        'create '.$content_type_id.' content' => '',
        'delete own '.$content_type_id.' content' => '',
        'edit own '.$content_type_id.' content' => '',
        'delete any '.$content_type_id.' content' => '',
        'edit any '.$content_type_id.' content' => ''
      ));
      $content_type_number ++;
    }
  }
  foreach($percck_role_permissions as $rid => $permissions){
    $temp_permissions = array();
    foreach(array_keys($percck_permissions) as $percck_permission){
      if(in_array($percck_permission, $permissions)){
        $temp_permissions[] = $percck_permission;
      }
      $percck_role_permissions[$rid] = $temp_permissions;
    }
  }
  foreach($form['percck']['role_names'] as $rid => $value){
    
    $form['percck']['checkboxes']['percck_'.$rid] = array(
      '#type' => 'checkboxes',
      '#options' => $percck_permissions,
      '#default_value' => $percck_role_permissions[$rid]
    );
  }
  $form['submit'] = array('#type' => 'submit', '#value' => t('Save permissions'));
  return $form;
}

function fixperms_admin_settings_submit($form, &$form_state) {
  // Handle the submit here, saving the shit into the permissions table YEAH!
  // Get the rids (could probably get these from the form, but I'm being lazy).
  $result = db_query('SELECT rid FROM {role} ORDER BY rid');
  while ($rid = db_result($result)) {
    $rids[$rid] = $rid;
  }
  // Lets get each of the "default_$rid" values and then set them.
  // We do this even if the box isn't checked, as these values will
  // be used as the defaults for new content types.
  foreach($rids as $rid){
    // Get the current settings.s
    $current_permission = drupal_map_assoc(explode(", ", db_result(db_query('SELECT perm FROM {permission} WHERE rid = %d', $rid))));
    foreach($form_state['values']['default_'.$rid] as $permission){
      if($permission){
        // Add
        $current_permission[$permission] = $permission;
      } else {
        // Remove
        unset($current_permission[$permission]);
      }
      db_query("UPDATE {permission} SET perm = '%s' WHERE rid = %d", implode(", ", $current_permission), $rid);
    }
  }
  if($form_state['values']['override']){
    // We're going to set all the content types exactly the same, yeah.  We need
    // to set the fixperms permissions, and then just run _fixperms_submit.
    variable_set('fixperms_override', 1);
    _fixperms_submit();
  } else {
    // Delete the override variable to ensure that any changes the user makes
    // won't get overridden later.
    variable_del('fixperms_override');
    foreach($rids as $rid){
      // Get the current settings.s
      $current_permission = drupal_map_assoc(explode(", ", db_result(db_query('SELECT perm FROM {permission} WHERE rid = %d', $rid))));
      foreach($form_state['values']['percck_'.$rid] as $permission){
        if($permission){
          // Add
          $current_permission[$permission] = $permission;
        } else {
          // Remove
          unset($current_permission[$permission]);
        }
        db_query("UPDATE {permission} SET perm = '%s' WHERE rid = %d", implode(", ", $current_permission), $rid);
      }
    }
  }
}

function _fixperms_submit($form, $form_state){
  // Need to add a check to see if we're submitting a new node,
  // as if we are, we need to set the permissions, regardless
  // of the value of fixperms_override.
  if(variable_get('fixperms_override', 0)){
    $current_permissions = array();
    $result = db_query("SELECT rid, perm FROM {permission}");
    while($row = db_fetch_array($result)){
      $current_permissions[$row['rid']] = $row['perm'].", ";
    }
    $content_types = array_keys(content_types());
    $perms = fixperms_perm();
    $fixperms_do_not_remove = variable_get('fixperms_do_not_remove', false);
    foreach($current_permissions as $rid => $permission){
      foreach($perms as $perm){
        // If the perm is in the current_permission, then we need to set all the
        // content type permissions, else we need to remove them.
        if(strpos($permission,$perm) === FALSE){
          if(!$fixperms_do_not_remove){
            // The string isn't in there, we need to remove all the content type 
            foreach($content_types as $type){
              $type = str_replace("type", $type, $perm).", ";
              $permission = str_replace($type, "", $permission);
            }
          }
        } else {
          // String is in there, lets add all the content type values 
          foreach($content_types as $type){
            $type = str_replace("type", $type, $perm).", ";
            // First remove
            $permission = str_replace($type, "", $permission);
            // Then add
            $permission .= $type;
          }
        }
      }
      // Save the permission
      db_query("UPDATE {permission} SET perm = '%s' WHERE rid = %d", substr($permission,0,-2), $rid);
    }
  } elseif($form['#node_type']->type != $form_state['values']['type']){
    // We have a new content type, lets add the default values for it
    $fixperms_permissions = fixperms_perm();
    $result = db_query('SELECT rid, perm FROM {permission}');
    while($row = db_fetch_array($result)){
      $row['perm'] = $row['perm'] . ", ";
      foreach($fixperms_permissions as $permission){
        if(strpos($row['perm'], "$permission,") !==0){
          $row['perm'] = $row['perm'] . str_replace("type", $form_state['values']['type'], $permission) . ", ";
        }
      }
      db_query("UPDATE {permission} SET perm = '%s' WHERE rid = %d", substr($row['perm'],0, -2), $row['rid']);
    }
  }
}

function theme_fixperms_admin_settings($form){
  drupal_add_css(drupal_get_path('module','fixperms').'/fixperms.css');
  return drupal_render_form($form);
}