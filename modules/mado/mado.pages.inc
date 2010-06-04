<?php
// $Id:$

/**
 * @file
 * Page callbacks for the Mado module
 */

/**
 * Front page mado style
 */
function mado_front_page(){
  module_load_include('theme-functions.inc','mado');
  $theme = mado_get_theme_front_page();
  if(!$theme){
    drupal_goto('node');
  }
  // Add mado CSS
  drupal_add_css(drupal_get_path('module','mado').'/mado.css');
  // Add the callback URL for sorting
  drupal_add_js(array('mado' => array(
    'callback_url' => url('mado/save',array('absolute'=>TRUE)),
    'ident' => 'frontpage',
    'finished_sorting' => l('<img title="Click to save" alt="Lock" src="'.base_path().drupal_get_path('module','mado').'/images/lock.png"/>',$_GET['q'],array('html' => TRUE))
  )), 'setting');
  // Add jquery_ui
  jquery_ui_add(array('ui.sortable'),'none');
  
  if(user_access('mado sort')){
    $output .= '<div class="mado_sort">
        <h1 id="mado-start-sort"><img title="Click to edit the layout of this page" alt="Unlock" src="'.base_path().drupal_get_path('module','mado').'/images/unlock.png"/></h1>
      </div>';
    drupal_add_js(drupal_get_path('module', 'mado')."/mado.js");
    jquery_ui_add(array('ui.dialog'));
    jquery_ui_add(array('ui.draggable'));
    drupal_add_css(drupal_get_path('module', 'jquery_ui') . '/jquery.ui/themes/smoothness/ui.all.css');
  }      
  $output .= '<div id="mado">';
  
  $before_divider = TRUE;
  foreach($theme as $id => $block){
    if($id == 'divider'){
      $before_divider = FALSE;
    }
    $output .= theme('mado_block', $block, $id, $before_divider);
  }      
  $output .= '</div>';
  if(user_access('mado sort')){
    $output .= '<div id="mado_apply">
        <p>'.l('Apply to all unset pages','mado/applytoall',array('query' => array('ident' => $vids[0].'/'.$tids[0]))).'</p>
        <p>'.l('Apply to just this page','taxonomy/term/'.$tids[0]).'</p>
      </div>';
  }
  // Blank title
  drupal_set_title('');
  return $output;  
}

/**
 * Menu callback; displays all nodes associated with a term.
 */
function mado_term_page($str_tids = '', $depth = 0, $op = 'page') {
  $terms = taxonomy_terms_parse_string($str_tids);
  if ($terms['operator'] != 'and' && $terms['operator'] != 'or') {
    drupal_not_found();
  }
  
  if ($terms['tids']) {
    $result = db_query(db_rewrite_sql('SELECT t.vid, t.tid, t.name FROM {term_data} t WHERE t.tid IN ('. db_placeholders($terms['tids']) .')', 't', 'tid'), $terms['tids']);
    $tids = array(); // we rebuild the $tids-array so it only contains terms the user has access to.
    $names = array();
    $vids = array();
    while ($term = db_fetch_object($result)) {
      $tids[] = $term->tid;
      $names[] = $term->name;
      $vids[] = $term->vid;
    }    
    // We'll get the vid from the first term, we can't start pissing about
    // checking every bloody term
    module_load_include('theme-functions.inc','mado');
    $theme = mado_get_theme($vids[0], $tids[0]);
    if(!$theme || $op != 'page'){
      module_load_include("pages.inc", "taxonomy");
      return taxonomy_term_page($str_tids, $depth, $op);
    }
    if ($names) {
      // Add mado CSS
      drupal_add_css(drupal_get_path('module','mado').'/mado.css');
      // Add the callback URL for sorting
      drupal_add_js(array('mado' => array(
        'callback_url' => url('mado/save',array('absolute'=>TRUE)),
        'ident' => $vids[0].'/'.$tids[0],
	      'finished_sorting' => l('<img title="Click to save" alt="Lock" src="'.base_path().drupal_get_path('module','mado').'/images/lock.png"/>',$_GET['q'],array('attributes' => array('onclick' => "$('#mado_apply').dialog('open');return false;"), 'html' => TRUE))
      )), 'setting');
      // Add jquery_ui
      jquery_ui_add(array('ui.sortable'),'none');
      // Set the page title      
      $title = check_plain(implode(', ', $names));
      drupal_set_title($title);
      // Adds a feed URL - pretty neat
      drupal_add_feed(url('taxonomy/term/'. $str_tids .'/'. $depth .'/feed'), 'RSS - '. $title);
      
      // Build breadcrumb based on first hierarchy of first term:
      $current->tid = $tids[0];
      $breadcrumb = array();
      while ($parents = taxonomy_get_parents($current->tid)) {
        $current = array_shift($parents);
        $breadcrumb[] = l($current->name, 'taxonomy/term/'. $current->tid);
      }
      $breadcrumb[] = l(t('Home'), NULL);
      $breadcrumb = array_reverse($breadcrumb);
      drupal_set_breadcrumb($breadcrumb);
      
      if(user_access('mado sort')){
        $output .= '<div class="mado_sort">
            <h1 id="mado-start-sort"><img title="Click to edit the layout of this page" alt="Unlock" src="'.base_path().drupal_get_path('module','mado').'/images/unlock.png"/></h1>
          </div>';
        drupal_add_js(drupal_get_path('module', 'mado')."/mado.js");
        jquery_ui_add(array('ui.dialog'));
        jquery_ui_add(array('ui.draggable'));
        drupal_add_css(drupal_get_path('module', 'jquery_ui') . '/jquery.ui/themes/smoothness/ui.all.css');
      }
      
      $output .= '<div id="mado">';
      
      $before_divider = TRUE;
      foreach($theme as $id => $block){
        if($id == 'divider'){
          $before_divider = FALSE;
        }
        $output .= theme('mado_block', $block, $id, $before_divider);
      }      
      $output .= '</div>';
      if(user_access('mado sort')){
        $output .= '<div id="mado_apply">
            <p>'.l('Apply to all unset pages','mado/applytoall',array('query' => array('ident' => $vids[0].'/'.$tids[0]))).'</p>
            <p>'.l('Apply to just this page','taxonomy/term/'.$tids[0]).'</p>
          </div>';
      }
      return $output;
    }
  }
  drupal_not_found();
}

/**
 * These are simply provided so that they can be overridden
 */
function theme_mado_block($block, $id, $before_divider = TRUE) {
  $title = '&nbsp;';
  $content = '';
  $hidden_content = '<div>'.t('This block is currently either hidden, or has no content. Content (if there is any) will be displayed once it is moved above the divider.').'</div>';
  switch($block['#type']){
    case 'view':      
      $view = views_get_view($block['#name']);
      if($view && !$view->disabled){
        $view->get_total_rows = 'TRUE';
        $view_content = $view->preview($display_id, array(arg(2)));
        if((isset($view->total_rows) && !$view->total_rows) && trim($view->display['default']->display_options['empty'])==''){
          $content = $hidden_content;
          $block['#hidden'] = TRUE;
        } else {
          // A little messy (Shit, VERY MESSY), but here we pull out the added
          // links for "Sort" and "Add" if they have been added by the views-sort
          // module.
          $start_of_links = strpos($view_content, '<ul class="view_sort-links">');
          if($start_of_links){
            $end_of_links = strpos($view_content, "</ul>", $start_of_links);
            $list_of_links = substr($view_content,$start_of_links+28,$end_of_links-$start_of_links-28);
            if(isset($block['#hidden']) && $block['#hidden']){
              $content = $hidden_content;
            } else {
              $content = substr($view_content,0,$start_of_links).substr($view_content,$end_of_links+5);
            }
          } else {        
            if(isset($block['#hidden']) && $block['#hidden']){
              $content = $hidden_content;
            } else {
              $content = $view_content;
            }
          }
        }
        if(isset($view->display['default']->display_options['title']) && $view->display['default']->display_options['title'] !=''){
          $title = $view->display['default']->display_options['title'];
        } elseif($view->description) {
          $title = $view->description;
        } else {
          $title = $view->name;
        }
        if(strlen($title)>50){
          $title_words = explode(" ", $title);
          $title = '';
          while(strlen($title)<40){
            $title .= " ".array_shift($title_words);
          }
          $title .="...";
        }
      } else {
        return false;
      }
      break;
    case 'block':
      $name_parts = explode("/",$block['#name']);
      if(count($names_parts == 2)){
        $module = $name_parts[0];
        $delta = $name_parts[1];
        $module_block = module_invoke($module,'block','view',$delta);
        if(isset($module_block['subject']) && $module_block['subject']!=''){
          $title = $module_block['subject'];
        } else {
          $temp_block = module_invoke($module, 'block', 'list', $delta);
          $title = $temp_block[$delta]['info'];
        }
        if(isset($block['#hidden']) && $block['#hidden']){
          $content = $hidden_content;
        } else {
          $content = $module_block['content'];
        }
        if(trim($content)==''){
          $content = $hidden_content;
          $block['#hidden'] = TRUE;
        }
      } else {
        return false;
      }
      break;
    case 'divider':
      $id = 'divider';
      $block['#css'] = 'mado_block mado-handle';
      $content = '<div class="mado-handle" style="height: 100%;width: 100%"><h1>'.t('Content below this block will be hidden').'</h1></div>'; // Can't be empty as then the block would be hidden
      break;
    default:
      return false;
      break;
  }
  $is_there_content = preg_replace("/[\ \r\n\t]/","",trim(strip_tags($content, '<img>')));
  if ($is_there_content=='' || $is_there_content == 'EditExportClone'){
    $content = $hidden_content;
    $block['#hidden'] = TRUE;
  }
  //$content = "***".trim(strip_tags($content, '<img>'))."***";
  // Remove slashes from the ID field as they screw with jquery.
  $output .= '<div id="'.str_replace("/","-slash-",$id).'" class="'.(isset($block['#css'])? $block['#css'].' ':"").'mado_block'.((isset($block['#hidden'])&&$block['#hidden'])?" mado-hidden":"").'">';
  if($id != 'divider' && $before_divider){
    $output .= '<div class="mado_close">
    <img src="'.base_path().drupal_get_path('module','mado').'/images/close.png" alt="Close"/>
  </div>';
  }
  $output .='<div>
    <div class="boxtop">
      <div class="bc ctr"></div>
      <div class="bc ctl"></div>
    </div>
    <div class="boxcontent">';
  if($id != 'divider'){
    $output .= '<div class="boxtitle mado-handle">
          <h1>'.$title.'</h1>';
    if(user_access('mado sort')){
      $list_of_links .= '<li><a class="resize-x resize"><img src="'.base_path().drupal_get_path('module','mado').'/images/arrow-x.png" alt="Resize x"/></a></li><li><a class="resize-y resize"><img src="'.base_path().drupal_get_path('module','mado').'/images/arrow-y.png" alt="Resize y"/></a></li>';
    }
    if($list_of_links != ''){
      $output .= '<ul class="mado-title-links">'.$list_of_links.'</ul>';
    }
    $output .= '</div>';
  }
  $output .='<div class="subboxcontent">
        <div class="mado_content'. ((isset($block['#scroll']) && $block['#scroll']) ? " mado_scroll": "") .'">
          '.$content.'
        </div>
      </div>
    </div>
    <div class="boxbtm">
      <div class="bc cbr"></div>
      <div class="bc cbl"></div>
    </div>
  </div></div>';
  // finally, give other modules an opportunity to mangle output
  module_invoke_all('mado_block_alter', $output);
  return $output;
}