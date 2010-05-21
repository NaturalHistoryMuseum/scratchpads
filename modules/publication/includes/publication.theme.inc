<?php

function theme_publication_node_add_form($form){

  $output .= drupal_render($form['title']);
  $output .= drupal_render($form['group_publication_metadata']['field_publication_title'][0]['value']);
  $output .= drupal_render($form['og_selective']);
  $output .= drupal_render($form['og_private']);
  $output .= drupal_render($form['og_register']);
  $output .= drupal_render($form['og_directory']);
  $output .= drupal_render($form['options']);  
  
  $output .= theme('publication_node_form_base_elements', $form);
  
  return $output;
  
}

function theme_publication_node_edit_form($form){
  
  publication_add_files();
  
  // Add the taxonomy tree callback path
  
  if(!$form['#node']->vocabulary){
  
    $callback_path = url(TAXONOMY_TREE_CALLBACK_PATH . '/' . $form['#node']->nid);
    drupal_add_js(array('taxonomyTree' => array('path' => $callback_path)), 'setting');
    
  }

  $groups = fieldgroup_groups('publication');
  
  foreach($groups as $group_name => $group){
     
     if(array_key_exists($group_name, $form)){
   
       $id = form_clean_id(drupal_strtolower($group['label']));
       $tab_items[] = '<a href="#'.$id.'"><span>'.$group['label'].'</span></a>';
   
       $tab_content .= '<div id="'.$id.'">';
       
       switch($group_name){
         
         case 'group_publication_metadata':
   
           $tab_content .= drupal_render($form['title']);
           $tab_content .= drupal_render($form[$group_name]['field_publication_title']);
           $tab_content .= drupal_render($form['body_filter']);
           $tab_content .= drupal_render($form[$group_name]['field_publication_keywords']);
           $form[$group_name]['#title'] = t('Authors');
         
         break;
         
         case 'group_publication_privacy':
         
           $tab_content .= drupal_render($form['og_description']);
           $tab_content .= drupal_render($form['og_selective']);
           $tab_content .= drupal_render($form['og_private']);
           $tab_content .= drupal_render($form['og_register']);
           $tab_content .= drupal_render($form['og_directory']);
           
           // Node publishing options
           $tab_content .= drupal_render($form['options']);
         
         break;
         
         case 'group_publication_taxa':
         
         $tab_content .= '<div class="publication-field"><div class="status-messages"></div>';
         $tab_content .= drupal_render($form['vocabulary']);
         $tab_content .= drupal_render($form['taxonomy']);
         $tab_content .= drupal_render($form['vocabulary_auto_select']);
         $tab_content .= '</div>';
            
         break;
   
         // Nomenclature is not currently in use...
         // case 'group_publication_nomenclature':
         // 
         //   $tab_content .= drupal_render($form['nomenclature_fields']);
         // 
         // break;
         
         case 'group_publication_term_fields':
         
           $tab_content .= drupal_render($form['term_field_defaults']);
           $tab_content .= drupal_render($form['term_field_updates']);
         
         break;
         
         case 'group_publication_sections':
         
           $tab_content .= drupal_render($form['field_publication_section']);
         
         break;
         
       }
   
       $tab_content .= drupal_render($form[$group_name]);
       
       $tab_content .= '</div>';
   
     }
   }
   

  $output .= '<div id="publication-tabs">';
  $output .= theme('item_list', $tab_items, null, 'ul', array('class' => 'ui-tabs-nav'));
  $output .= $tab_content;
  
  $output .= theme('publication_node_form_base_elements', $form);
  
  $output .= '</div>';

  return $output;

}

function theme_publication_image_caption_node_form($form){
  
  publication_add_modal_child_files();
  
  $output = drupal_render($form['body_filter']);  

  $output .= theme('publication_node_form_base_elements', $form);
  
  return $output;
  
}


function theme_publication_section_node_form($form){
  
  publication_add_modal_child_files();
  
  $output = drupal_render($form['title']); 
  $output .= drupal_render($form['reset_manuscript_name']);  
  $output .= drupal_render($form['body_filter']); 
  $output .= drupal_render($form['term_fields']);
  $output .= drupal_render($form['reset_term_fields']);  
  $output .= theme('publication_node_form_base_elements', $form);
  
  return $output;
  
}

function theme_publication_taxon_treatment_node_form($form){
  
  publication_add_modal_child_files();

  $output = drupal_render($form['term_fields']);
  $output .= theme('publication_node_form_base_elements', $form);
  
  return $output;
  
}

function theme_publication_node_form_base_elements($form){
  
  $output = '';

  foreach(element_children($form) as $key){
    
    if($form[$key]['#type'] == 'hidden' || $form[$key]['#type'] == 'token' || $form[$key]['#type'] == 'value'){
  
      $output .= drupal_render($form[$key]);
      
    }
    
  }
  
  $output .= drupal_render($form['author']);
  
  $output .= '<div id="buttons">'.drupal_render($form['buttons']).'</div>';

  
  return $output;
  
}



// Individual field theming */

function theme_publication_field_publication_images($nids = array(), $publication_nid){
  
  $output = '<div id="publication-images" class="clearfix publication-field"><div class="status-messages"></div>';
  
  foreach($nids as $nid){
    
    if($nid){
     
     $output .= _publication_node_get_image($nid, $publication_nid);
      
    }
    
  }
  
  $output .= '</div>';

  return $output;
  
}

function theme_publication_field_publication_biblio($nids = array(), $publication_nid){
  
  $output = '<div id="publication-biblio" class="clearfix">';
  
  $nids = implode('+', array_filter($nids));
  
  $output .= publication_get_view_output('publication_display', array($nids), 'page_1');
  
  $output .= '</div>';

  return $output;
  
}

function theme_publication_field_publication_authors($nids = array(), $publication_nid){
  
  $output = '<div id="publication-contrib" class="clearfix">';
  
  $nids = implode('+', array_filter($nids));
  
  $output .= publication_get_view_output('publication_display', array($nids), 'page_2');
  
  $output .= '</div>';

  return $output;
  
}



function theme_publication_field_publication_section($element){

  drupal_add_tabledrag('publication-draggable-section', 'match', 'parent', 'section-parent', 'section-parent', 'section-id');  
    
  $output = '<div id="publication-section"><div class="status-messages"></div>';
  
  if(count(element_children($element))){
   
    $header = array('Title', 'Weight', 'Operations');

    $rows = array();

    foreach (element_children($element) as $key) {

      $row = array();

      $row[] = array(
        'data' => theme('indentation', $element[$key]['#depth']) . drupal_render($element[$key]['title']),
        'class' => 'section-title'
      );

      $nid = $element[$key]['nid']['#value'];

      $row[] = array(
        'data' => drupal_render($element[$key]['section_id']) .
                  drupal_render($element[$key]['nid']) .
                  drupal_render($element[$key]['depth']) .
                  drupal_render($element[$key]['section_parent']),
        'class' => 'section-data'
      );    

      $row[] = drupal_render($element[$key]['ops']);

      $rows[$nid] = array('data' => $row, 'class' => $element[$key]['#attributes']['class']);

    }

    $output .= theme('table', $header, $rows, array('id' => 'publication-draggable-section'));

    $output .= '<p>';
    $output .= l('Add section', 'node/add/publication-section', array('attributes' => array('class' => 'modal')));
    $output .= '</p>';   
    
  }
  
  $output .= '</div>';
  
  return $output;
  
}

function theme_publication_message($message, $type = 'status'){
  
  $output .= "<div class=\"messages $type\">\n";
  $output .=  $message;
  $output .= "</div>\n";
  
  return $output;
  
}


/*    VIEWS THEMING    */

function theme_publication_view_filter_select($element) {
  
  $node = new stdClass();

  $node->vid = $_GET['vid'];
  $vocabulary = $_GET['vocabulary'];
  
  $node_terms = taxonomy_node_get_terms_by_vocabulary($node, $vocabulary);
  
  $select = '';
  $size = $element['#size'] ? ' size="'. $element['#size'] .'"' : '';
  _form_set_class($element, array('form-select'));
  $multiple = $element['#multiple'];
  return theme('form_element', $element, '<select name="'. $element['#name'] .''. ($multiple ? '[]' : '') .'"'. ($multiple ? ' multiple="multiple" ' : '') . drupal_attributes($element['#attributes']) .' id="'. $element['#id'] .'" '. $size .'>'. publication_view_filter_select_options($element, $node_terms) .'</select>');
  
}

function theme_publication_term_fields_element($element){
  
  $count = count(element_children($element)) / 2;
  $counter = 0;
  $output = '<div class="clearfix term-fields"><div class="column">';
  
  foreach(element_children($element) as $key){

    if($count == $counter){
      $output .= '</div><div class="column">';
    }
    
   $output .= drupal_render($element[$key]);
   $counter++;

  }
  
  $output .= '</div></div>';
  
  return $output;
  
}





?>