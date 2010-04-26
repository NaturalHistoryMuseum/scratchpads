<?php

/**
 * Callback for the menu item admin/content/taxonomy/edit/tui/%vid
 */
function tui_admin_vocabulary_edit($vocabulary){
  $tree = array();
  $all_tids = array();
  $results = db_query(db_rewrite_sql('SELECT tid FROM {term_data} t WHERE tid IN (SELECT tid FROM {term_hierarchy} WHERE parent = 0) AND vid = %d', 't', 'tid'), $vocabulary->vid);
  while($tid = db_result($results)){
    $tree[$tid] = db_result(db_query(db_rewrite_sql('SELECT COUNT(*) FROM {term_hierarchy} t WHERE parent = %d', 't', 'tid'), $tid));
    $all_tids[$tid] = $tid;
  }
  if(arg(6)){
    // We have a tid set, we need to open out to the tid, else we'll just show 
    // the very root of the tree.
    $tids = explode(',', arg(6));
    $num_tids = count($tids);
    for($i=0; $i<$num_tids; $i++){
      $branch = false;
      if(isset($tids[$i])){
        $parents = taxonomy_get_parents_all($tids[$i]);
        while($parents){
          $term = array_shift($parents);
          $key = array_search($term->tid, $tids);
          if($key){
            unset($tids[$key]);
          }
          if(is_array($branch)){
            foreach($branch as $tid => $stuff){
              $all_tids[$tid] = $tid;
            }
          }
          $branch = array($term->tid => $branch);
        }
        $tree[$term->tid] = array_pop($branch);
        $all_tids[$term->tid] = $term->tid;
      }
    }
  }
  return theme('tui_page', $tree, $vocabulary, $all_tids);
}

/**
 * hook_help
 */
function tui_help($path, $arg){
  switch($path){
    case 'admin/content/taxonomy/edit/tui':
      return '<p>'.t('Detailed TUI help').'</t>';
  }
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
        'callbacks' => array(
          'form' => url('tui/getform', array('absolute' => TRUE)),
          'full_tree' => url('tui/gettree', array('absolute' => TRUE)),
          'tree' => url('tui/getchildren', array('absolute' => TRUE)),
          'move' => url('tui/moveterm', array('absolute' => TRUE))
        )
      )
    ),'setting');
  
  // Add the JS required.
  variable_set('jquery_update_compression_type','none');
  jquery_ui_add(array('ui.tabs', 'ui.draggable'));
  jquery_ui_add(array('ui.tabs', 'ui.droppable'));
  drupal_add_css(drupal_get_path('module', 'jquery_ui') . '/jquery.ui/themes/smoothness/ui.all.css');
  
  // We add the form, so that all the extras (collapse.js etc) are added.  We 
  // then hide the form.
  module_load_include('admin.inc', 'taxonomy');
  
  return '<div id="tui">
  <div id="tui-tree">
    <div id="tui-tree-container">
      '.theme('tui_tree', $taxonomy_tree).'
    </div>
  </div>
  <div id="tui-form">
    <div id="tui-form-container">
      <div id="tui-form-noform">'.tui_help('admin/content/taxonomy/edit/tui', array()).'</div>
      <div class="hidden">'.drupal_get_form('taxonomy_form_term', $vocabulary, array()).'</div>
    </div>
  </div>
  <div id="tui-clear"></div>
</div>';
}

/**
 * Return the form to ajax
 */
function tui_get_term_form($tid){
  $tid = substr($tid, 4);
  module_load_include('admin.inc', 'taxonomy');
  $term = (array)taxonomy_get_term($tid);
  $vocabulary = taxonomy_vocabulary_load($term['vid']);
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
