<?php

// $Id: views_plugin_display_page.inc,v 1.8.2.4 2010/03/11 00:01:34 merlinofchaos Exp $
/**
 * @file
 * Contains the page display plugin.
 */
/**
 * The plugin that handles a full page.
 *
 * @ingroup views_display_plugins
 */
class views_plugin_display_batch_page extends views_plugin_display_page{

  function option_definition(){
    $options = parent::option_definition();
    $options['batch_items'] = array(
      'default' => 200
    );
    return $options;
  }

  /**
   * Provide the summary for page options in the views UI.
   *
   * This output is returned as an array.
   */
  function options_summary(&$categories, &$options){
    // It is very important to call the parent function here:
    parent::options_summary($categories, $options);
    $categories['batch'] = array(
      'title' => t('Batch settings')
    );
    $options['batch_items'] = array(
      'category' => 'batch',
      'title' => t('Batch items'),
      'value' => $this->get_option('batch_items')
    );
  }

  /**
   * Provide the default form for setting options.
   */
  function options_form(&$form, &$form_state){
    // It is very important to call the parent function here:
    parent::options_form($form, $form_state);
    switch($form_state['section']){
      case 'batch_items':
        $form['#title'] .= t('Batch items');
        $form['batch_items'] = array(
          '#type' => 'textfield',
          '#description' => t('The number of items to process per batch run.'),
          '#default_value' => $this->get_option('batch_items')
        );
        break;
    }
  }

  function options_validate(&$form, &$form_state){
    // It is very important to call the parent function here:
    parent::options_validate($form, $form_state);
    switch($form_state['section']){
      case 'batch_items':
        if(!is_numeric($form_state['values']['batch_items']) && $form_state['values']['batch_items'] > 0){
          form_error($form['batch_items'], t('Please enter a numeric value.'));
        }
        break;
    }
  }

  function options_submit(&$form, &$form_state){
    // It is very important to call the parent function here:
    parent::options_submit($form, $form_state);
    switch($form_state['section']){
      case 'batch_items':
        $this->set_option('batch_items', $form_state['values']['batch_items']);
        break;
    }
  }

  /**
   * The display page handler returns a normal view, but it also does
   * a drupal_set_title for the page, and does a views_set_page_view
   * on the view.
   */
  function execute(){
  	
    if($cached_view = views_batch_page_cache_get($this->view)){
      views_set_page_view($this);
      if(!empty($this->view->build_info['fail'])){return drupal_not_found();}
      $this->view->get_breadcrumb(TRUE);
      // And the title, which is much easier.
      drupal_set_title(filter_xss_admin($this->view->get_title()));
      // And now render the view.
      // Add all the JS files
      foreach($cached_view->data['scripts'] as $batch_scripts){
        foreach($batch_scripts as $type => $batch_script){
          switch($type){
            case 'core':
            case 'module':
            case 'theme':
              foreach($batch_script as $script => $params){
                drupal_add_js($script, $type);
              }
              break;
            case 'setting':
              foreach($batch_script as $setting){
                drupal_add_js($setting, 'setting');
              }
              break;
            case 'inline':
              foreach($batch_script as $inline){
                drupal_add_js($inline['code'], 'inline', 'header', $inline['defer']);
              }
              break;
          }
        }
      }
      // Add all the CSS files
      foreach($cached_view->data['css'] as $batch_css){
        foreach($batch_css as $media => $style_sheets){
          foreach($style_sheets as $type => $style_sheet){
            foreach($style_sheet as $file => $preprocess){
              drupal_add_css($file, $type, $media, $preprocess);
            }
          }
        }
      }
      // The output has already been created by the batch process
      return $cached_view->data['output'];
    }elseif(isset($_GET['id'])){
      include_once 'includes/batch.inc';
      return _batch_page();
    }else{
      $this->batch_execute();
    }
  }

  /**
   * 
   * Pass the execution to a batch process
   */
  function batch_execute(){
    $batch = array(
      'operations' => array(
        array(
          'views_batch_page_process',
          array(
            $this->view,
            $this->get_option('batch_items')
          )
        )
      ),
      'finished' => 'views_batch_page_complete',
      'title' => t('Loading view'),
      'init_message' => t('Loading the view.'),
      'error_message' => t('Sorry, there was an error loading the view.')
    );
    batch_set($batch);
    $uri = ltrim(request_uri(), '/');
    batch_process($uri, $uri);
  }
}
