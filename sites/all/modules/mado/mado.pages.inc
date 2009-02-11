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
function theme_mado_block($block_content, $arguments, $view) {
  if($block_content){
    $output = '<div';
    if(isset($arguments['#css'])){
      $output .= ' class="'.$arguments['#css'].'"';
    }
    $output .= "><div class=\"mado_block";
    if(isset($arguments['#scroll']) && $arguments['#scroll']){
      $output .= " mado_scroll";
    }
    $output .='">';
    if(isset($view->display['default']->display_options['title']) && $view->display['default']->display_options['title'] !=''){
      $output .="<h1>".$view->display['default']->display_options['title']."</h1>";
    }
    if(isset($view->display['default']->display_options['filters']['type']['value']) && count($view->display['default']->display_options['filters']['type']['value'])==1){
      $output .= "<h1>".l(t('Add'), 'node/add/'.array_pop($view->display['default']->display_options['filters']['type']['value']), array('query'=>array('tid'=>arg(2))))."</h1>";
    }
    return $output."<div class=\"content\">$block_content</div></div></div>";
  } else {
    return false;
  }
}

function theme_mado_row($theme_row){
  $blocks = element_children($theme_row);
  $output = '<div';
  if(isset($theme_row['#css']) && $theme_row['#css']!=''){
    $output .= ' class="'.$theme_row['#css'].'"';
  }
  $output .='>';
  foreach($blocks as $block){
    $view = views_get_view($theme_row[$block]['#view']);
    $output .= theme('mado_block', $view->preview(NULL, array(arg(2))), $theme_row[$block], $view); // Add arguments    
  }
  return $output."</div>";
}