<?php
// $Id:$

/**
 * @file
 * Page callbacks for the Mado module
 */

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
    $theme = mado_get_theme($vids[0]);
    if(!$theme || $op != 'page'){
      module_load_include("pages.inc", "taxonomy"); 
      return taxonomy_term_page($str_tids, $depth, $op);
    }

    if ($names) {
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
      
      $output = '';
      
      foreach($theme as $theme_row){
        $output .= theme('mado_row', $theme_row);
      }      
      return $output;
    }
  }
  drupal_not_found();
}

/**
 * These are simply provided so that they can be overridden
 */
function theme_mado_block($block) {
  $output = '<div';
  if(isset($block['#css'])){
    $output .= ' class="'.$block['#css'].'"';
  }
  $output .= "><div class=\"mado_block";
  if(isset($block['#scroll']) && $block['#scroll']){
    $output .= " mado_scroll";
  }
  $output .='">';
  switch($block['#type']){
    case 'view':
      $view = views_get_view($block['#name']);
      if($view){
        if(isset($view->display['default']->display_options['title']) && $view->display['default']->display_options['title'] !=''){
          $output .="<h1>".$view->display['default']->display_options['title']."</h1>";
        }
        if(isset($view->display['default']->display_options['filters']['type']['value']) && count($view->display['default']->display_options['filters']['type']['value'])==1){
          $output .= "<h1>".l(t('Add'), 'node/add/'.$view->display['default']->display_options['filters']['type']['value'][0], array('query'=>array('tid'=>arg(2))))."</h1>";
        }
        if(isset($view->display['page_1'])){
          $display_id = 'page_1';
        }
        $output .= '<div class="content">'.$view->preview($display_id, array(arg(2))).'</div></div></div>';
      } else {
        return false;
      }
      break;
    case 'block':
      $name_parts = explode(".",$block['#name']);
      if(count($names_parts == 2)){
        $module = $name_parts[0];
        $delta = $name_parts[1];
        $module_block = module_invoke($module,'block','view',$delta);
        if(isset($module_block['subject']) && $module_block['subject']!=''){
          $output .= "<h1>".$module_block['subject']."</h1>";
        }
        $output .= '<div class="content">'.$module_block['content'].'</div></div></div>';
      } else {
        return false;
      }
      break;
    default:
      return false;
      break;
  }
  // finally, give other modules an opportunity to mangle output
  module_invoke_all('mado_block_alter', $output);
  return $output;
}

function theme_mado_row($theme_row){
  $blocks = element_children($theme_row);
  $output = '<div';
  if(isset($theme_row['#css']) && $theme_row['#css']!=''){
    $output .= ' class="'.$theme_row['#css'].'"';
  }
  $output .='>';
  foreach($blocks as $block){    
    $output .= theme('mado_block', $theme_row[$block]); // Add arguments    
  }
  return $output."</div>";
}