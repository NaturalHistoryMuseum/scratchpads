<?php

/**
 * Callback for the menu item admin/content/taxonomy/edit/tui/%vid
 */
function tui_admin_vocabulary_edit($vocabulary){
  $tids = array();
  if(arg(6)){
    $tids = explode(',', arg(6));
  }
  $all_tids = array();
  module_load_include('tree.inc','tui');
  $tree = tui_get_tree($vocabulary->vid, $tids, $all_tids);
  return theme('tui_page', $tree, $vocabulary, $all_tids);
}

/**
 * Theme for above callback.
 */
function theme_tui_page($taxonomy_tree, $vocabulary, $all_tids){
  drupal_add_css(drupal_get_path('module', 'tui').'/lib/bt/jquery.bt.css');
  drupal_add_css(drupal_get_path('module', 'tui').'/tui.css');
  drupal_add_js(drupal_get_path('module', 'tui').'/tui.js');
  drupal_add_js(drupal_get_path('module', 'tui').'/lib/bt/jquery.bt.js');
  drupal_add_js(array(
      'tui' => array(
        'opentids' => $all_tids,
        'vocabulary' => arg(5),
        'callbacks' => array(
          'form' => url('tui/getform', array('absolute' => TRUE)),
          'full_tree' => url('tui/gettree', array('absolute' => TRUE)),
          'tree' => url('tui/getchildren', array('absolute' => TRUE)),
          'move' => url('tui/moveterm', array('absolute' => TRUE)),
          'page' => url('admin/content/taxonomy/edit/tui/'.arg(5), array('absolute' => TRUE)),
          'delete' => url('tui/deleteterm', array('absolute' => TRUE)),
          'nextorprevious' => url('tui/nextorprevious', array('absolute' => TRUE))
        ),
        'dialog' => array(
          'delete' => array(
            'title' => t('Delete?'),
            'content' => '<p>'.t('Are you sure you want to delete').'<br/> <span id="tui-dialog-term-name"></span>?</p>'
          )
        )
      )
    ),'setting');
  
  // Add the JS required.
  variable_set('jquery_update_compression_type','none');
  jquery_ui_add(array('ui.tabs', 'ui.draggable', 'ui.dialog', 'ui.droppable'));
  drupal_add_css(drupal_get_path('module', 'jquery_ui') . '/jquery.ui/themes/smoothness/ui.all.css');
  
  // We add the form, so that all the extras (collapse.js etc) are added.  We 
  // then hide the form.
  module_load_include('admin.inc', 'taxonomy');

  drupal_set_title(t('Editing %vocabulary', array('%vocabulary' => $vocabulary->name)));

  return '<div id="tui">
  <div id="tui-tree">
    <div id="tui-tree-container">
      <div id="tui-tree-links">
        <p>
          <img id="tui-previous" src="'.base_path().drupal_get_path('module','tui').'/images/previous.png" alt="'.t('Previous').'" title="'.t('Previous').'"/>
          <img id="tui-add" src="'.base_path().drupal_get_path('module','tui').'/images/add.png" alt="'.t('Add term').'" title="'.t('Add term').'"/>
          <img id="tui-delete" src="'.base_path().drupal_get_path('module','tui').'/images/delete.png" alt="'.t('Delete term').'" title="'.t('Delete term').'"/>
          <img id="tui-next" src="'.base_path().drupal_get_path('module','tui').'/images/next.png" alt="'.t('Next').'" title="'.t('Next').'"/>
        </p>       
      </div>
      <div id="tui-tree-subcontainer">
        '.theme('tui_tree', $taxonomy_tree).'
      </div>
    </div>
  </div>
  <div id="tui-form">
    <div id="tui-form-container">
      <div id="tui-form-noform">'.tui_help('admin/content/taxonomy/edit/tui', array()).'</div>
      <div class="hidden">'.drupal_get_form('taxonomy_form_term', $vocabulary, array()).'</div>
    </div>
  </div>
  <div id="tui-clear">
    <div id="tui-clear-container">
      <p>'.l('Link to this page.','admin/content/taxonomy/edit/tui/'.arg(5).'/'.arg(6), array('attributes' => array('id' => 'tui-link-back'))).'</p>
    </div>
  </div>
</div>';
}

/**
 * Return the form to ajax
 */
function tui_get_term_form($tid){
  $tid_parts = explode('-', $tid);
  if($tid_parts[0] == 'new'){
    $vocabulary = taxonomy_vocabulary_load($tid_parts[1]);
    $term = array();
  } else {
    $term = (array)taxonomy_get_term($tid_parts[1]);
    $vocabulary = taxonomy_vocabulary_load($term['vid']);
  }
  module_load_include('admin.inc', 'taxonomy');
  echo drupal_get_form('taxonomy_form_term', $vocabulary, $term);
}

/**
 * Theme the taxonomy form
 */
function tui_taxonomy_form_pre_render($form){
  $tab_html = '<div id="tabs"><ul>';
  // Specially for the term_node module
  if(isset($form['term_node']['node_form'])){
    foreach($form['term_node']['node_form'] as $key => $value){
      if(is_array($value) && isset($value['#type']) && $value['#type'] == 'fieldset'){
        $form[$key] = $form['term_node']['node_form'][$key];
        unset($form['term_node']['node_form'][$key]);
      }
    }
  }
  $num_tabs = 0;
  foreach($form as $key => $value){
    if(isset($value['#type']) && $value['#type'] == 'fieldset'){
      $num_tabs ++;
    }
  }
  if($num_tabs>4){
    $class = ' class="tui-squeeze-tabs"';
  }
  foreach($form as $key => $value){
    if(isset($value['#type']) && $value['#type'] == 'fieldset'){
      $tab_html .= '<li'.$class.' title="' . $value['#title'] . '"><a href="#fragment-' . $key . '"><span>';
      if($class && strlen($value['#title'])>8){
        $tab_html .= substr($value['#title'], 0, 5).'...';
      } else {
        $tab_html .= $value['#title'];
      }
      $tab_html .= '</span></a></li>';
      $form[$key]['#prefix'] = '<div id="fragment-' . $key . '">'; 
      $form[$key]['#suffix'] = '</div>';
      $form[$key]['#collapsible'] = FALSE;
      $form[$key]['#collapsed'] = FALSE;
    }
  }     
  $tab_html .= '</ul>';
  $form['tabs'] = array(
    '#weight' => -1000,
    '#value' => $tab_html
  );
  // Just tweak the buttons, as they can sometimes split the tabs and the 
  // content of the tab.
  $form['submit']['#weight'] = 1000;
  unset($form['delete']);
  return $form;
}
