<?php

/**
 * Form callback for the import
 */
function classification_import_form(){
  $tid = arg(4);
  if($tid){
    // We've been sent a tid, hence we can get on with the import
    $form = module_invoke_all('classification_import_form');
    if(!count($form)){
      drupal_set_message(t('You must install an Import module first.'), 'error');
    }
    $form['tid'] = array(
      '#type' => 'value',
      '#value' => arg(4)
    );
    $form['#attributes'] = array('enctype' => "multipart/form-data");
    return $form;
  } else {
    // We've no tid, somebody has just clicked the tab, give them a form to
    // enable them to select a tid/vocabulary.
  }
}