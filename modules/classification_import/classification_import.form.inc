<?php

/**
 * Form callback for the import
 */
function classification_import_form(){
  $tid = arg(4);
  if($tid){
    // We've been sent a tid, hence we can get on with the import
    $form = module_invoke_all('classification_import_form');
    // Ensure that the tid in question (FIXME: tid is actually vid, but I'm not 
    // going to change it just yet) does not currently have an import going. if
    // an import is going, we'll warn users.
    if(db_result(db_query('SELECT COUNT(*) FROM {classification_import_temp} WHERE vid = %d', $tid))){
      $form['import_already_running'] = array(
        '#weight' => -1000,
        '#type' => 'fieldset',
        '#title' => t('Import is already running'),
        '#collapsed' => FALSE,
        '#collapsible' => FALSE,
        'warning_message' => array(
          '#type' => 'markup',
          '#value' => '<h1 class="error">An import is already running.</h1><p>You must wait for the current import to finish before you can start another import into this vocabulary.</p>
          <p>If it looks like a previous import has failed, please feel free to cancel it.</p>' 
        ),
        'cancel_current_import' => array(
          '#type' => 'submit',
          '#value' => 'Cancel current import'
        )
      );
    }
    if(!count($form)){
      drupal_set_message(t('You must install an Import module first.'), 'error');
    }
    $form['tid'] = array(
      '#type' => 'value',
      '#value' => arg(4)
    );
    $form['#attributes'] = array(
      'enctype' => "multipart/form-data"
    );
    return $form;
  }
}

/**
 * Submit function for the cancel
 */
function classification_import_form_submit(&$form, &$form_state){
  // What have we got here?
  if($form_state['values']['op'] == $form_state['values']['cancel_current_import']){
    // Delete the excess entries from the database
    db_query('DELETE FROM {classification_import_temp} WHERE vid = %d', $form_state['values']['tid']);
  }
}