<?php


/** 
* Implementation of hook_theme(). 
*/ 
function nexus_theme() { 
  
  return array(
    
    // Use for the list of projects to display
    'nexus_project_list_item' => array(
      'arguments' => array('node' => NULL),
      'template' => 'nexus-project',
    ),
    
    'nexus_character_form' => array( 
      'arguments'=>array(), 
    ),

    'nexus_note_node_form' => array( 
      'arguments'=>array(), 
    ),

    'nexus_row_data' => array( 
      'arguments'=>array('term' => null), 
    ),
    
    'nexus_column_data' => array( 
      'arguments'=>array('term' => null), 
    ),
    
    'nexus_project_node_edit_form' => array(
      'arguments' => array('form' => NULL),
    ),
    
    'nexus_project_node_add_form' => array(
      'arguments' => array('form' => NULL),
    ),
    
    'nexus_matrix' => array(
      'arguments' => array('node' => NULL),
    ),
  );
  
} 




function theme_nexus_matrix($matrix_editor, $node){

  $output .= '<div id="character-groups"></div>';

  $output .= theme('matrix_editor', $matrix_editor->options, $matrix_editor->columns, $matrix_editor->data, $matrix_editor->filter);

   $path = drupal_get_path('module', 'nexus');
   
  drupal_add_js(array('nexusCallback' => url('nexus/callback')), 'setting');
  
  drupal_add_js($path.'/js/nexus.js');
  drupal_add_js($path.'/js/nexus.slickgrid.js');
   
  // Add the beauty tips  
  drupal_set_html_head('<!--[if IE]><script type="text/javascript" src="/'.$path.'/js/plugins/excanvas/excanvas.js"></script><![endif]-->');
  
  drupal_add_js($path.'/js/plugins/bt/jquery.bt.js');
  drupal_add_css($path.'/js/plugins/bt/jquery.bt.css');
  
  drupal_add_js('var intersection_info = '.drupal_to_js($matrix_editor->intersection_info), 'inline');

  drupal_add_js("$(function(){
    \rNEXUS.init(".$node->nid.");\n
    })",'inline');
  drupal_add_css($path.'/css/nexus.css');
  
  
  return $output;
  
}


function theme_nexus_row_data($term, $nid, $no_tree = false){
  
  $path = drupal_get_path('module', 'nexus');
  
  $output .= '<div class="columns" id="left">';
  
  $output .= '<h1>'.$term->name.'</h1>';
  
  if($term->description){
    $output .= '<p>'.$term->description.'</p>';
  }
  
  $parents = taxonomy_get_parents_all($term->tid);
  
  if(count($parents)){
    
    for($i = (count($parents) - 1); $i >= 0; $i--){

      $output .= $conj.$parents[$i]->name;
      
      $conj = ' &raquo; ';
      
    }
    
  }
  
  $output .= '</div>';

    if($no_tree == 'false'){

    $output .= '<div class="columns" id="right"><div class="inner">';
      
    // Does this term have child terms in this project?
    $children = nexus_get_children_in_project($nid, $term->tid);
      
      if(count($children)){
    
        $output .= '<h4>Inherit controlled states from children</h4>';
        $output .= '<p>'.t('This taxa has child taxa in this project it can automatically inherit controlled states from').'</p>';
        $output .= '<p>'.t('Please click inherit to update the states.').'</p>';
    
        $options = array('html' => true, 'attributes' => array('id' => 'inherit-states'));
        $image = theme('image', $path.'/images/inherit_button.gif', t('Inherit'), t('Inherit'));
    
        $output .= l($image, '', $options);
    
      }
      
    $output .= '</div></div>';
    
    }
  
  return $output;
  
}



function theme_nexus_column_data($term){
  
  $output = '<h1>'.$term->name.'</h1>';
  $output .= '<p>'.$term->description.'</p>';

  return $output;
  
}


function theme_nexus_character_form($form){
  
  $path = drupal_get_path('module', 'nexus');
    
  drupal_add_js($path.'/js/nexus.js'); 
  drupal_add_css($path.'/css/nexus.css');
  
  $output = drupal_render($form); 
  return $output;
  
}


function theme_nexus_note_node_form($form){
  
  $buttons = drupal_render($form['buttons']);
  
  $output = '<div class="inner">';
  $output .= drupal_render($form); 
  $output .= '</div>';
  $output .= $buttons;
  
  return $output;
  
}

function theme_nexus_project_node_add_form($form) {

  
  $path = drupal_get_path('module', 'nexus');
  drupal_add_js($path.'/js/nexus.forms.js');
  drupal_add_css($path.'/css/nexus.css');
  return drupal_render($form);
  
}



function theme_nexus_project_node_edit_form($form) {
  
  $output .= '<div id="edit-taxa" class="tab"><div class="columns" id="left">';
  $output .= drupal_render($form['taxonomy']);
  $output .= '</div><div class="columns" id="right">';
  $output .= drupal_render($form['save_taxonomy']);
  $output .= '</div></div>';

  $output .= '<div id="settings" class="tab">';
  $output .= drupal_render($form);
  $output .= '</div>';
  
  return $output;
  
}





?>