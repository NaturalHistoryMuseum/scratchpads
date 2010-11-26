<?php

function theme_publication_node_add_form($form){
  return '<div class="messages error" style="padding:1em;"><h1><a href="http://en.wikipedia.org/wiki/Software_release_life_cycle#Alpha" target="_blank">Alpha software</a> warning</h1><p>Users should be aware that the Publication module is provided as is.  It is still a work in progress, and has only been tested under very specific conditions.  If you\'re still keen to try to create a "Publication", then please provide feedback in the usual way.</p></div>' . drupal_render($form['title']) . drupal_render($form['group_publication_metadata']['field_publication_title']) . drupal_render($form['og_selective']) . drupal_render($form['og_private']) . drupal_render($form['og_register']) . drupal_render($form['og_directory']) . drupal_render($form['options']) . theme('publication_node_form_base_elements', $form);
}

function theme_publication_node_edit_form($form){
  publication_add_files();
  // Add the taxonomy tree callback path
  if(!$form['#node']->vocabulary){
    $callback_path = url(TAXONOMY_TREE_CALLBACK_PATH . '/' . $form['#node']->nid);
    drupal_add_js(array(
      'taxonomyTree' => array(
        'path' => $callback_path
      )
    ), 'setting');
  }
  $groups = fieldgroup_groups('publication');
  foreach($groups as $group_name => $group){
    $closing_tab_content = null;
    if(array_key_exists($group_name, $form)){
      $id = form_clean_id(drupal_strtolower($group['label']));
      $tab_items[] = '<a href="#' . $id . '"><span>' . $group['label'] . '</span></a>';
      $tab_content .= '<div id="' . $id . '">';
      switch($group_name){
        case 'group_publication_metadata':
          $tab_content .= drupal_render($form['title']);
          $tab_content .= drupal_render($form[$group_name]['field_publication_title']);
          $tab_content .= drupal_render($form['body_field']);
          $tab_content .= drupal_render($form[$group_name]['field_publication_keywords']);
          $closing_tab_content = drupal_render($form[$group_name]['field_publication_supplementary']);
          $closing_tab_content .= drupal_render($form[$group_name]['field_publication_ack']);
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
        case 'group_publication_term_fields':
          $tab_content .= '<p>' . t('Please enter the default term fields to use for taxon names in this publication. These term fields can then be changed for individual taxa under <a class="tab-link" href="#sections">sections</a>.') . '</p>';
          $tab_content .= drupal_render($form['term_field_defaults']);
          $tab_content .= drupal_render($form['term_field_updates']);
          break;
        case 'group_publication_sections':
          $tab_content .= drupal_render($form['field_publication_section']);
          break;
      }
      $tab_content .= drupal_render($form[$group_name]);
      if($closing_tab_content){
        $tab_content .= $closing_tab_content;
      }
      $tab_content .= '</div>';
    }
  }
  $output .= '<div id="publication-tabs">';
  $output .= theme('item_list', $tab_items, null, 'ul', array(
    'class' => 'ui-tabs-nav'
  ));
  $output .= $tab_content;
  $output .= theme('publication_node_form_base_elements', $form);
  $output .= '</div>';
  return $output;
}

function theme_publication_image_caption_node_form($form){
  publication_add_modal_child_files();
  $output = drupal_render($form['body_field']);
  $output .= theme('publication_node_form_base_elements', $form);
  return $output;
}

function theme_publication_section_node_form($form){
  publication_add_modal_child_files();
  $output = drupal_render($form['title']);
  $output .= drupal_render($form['reset_manuscript_name']);
  $output .= drupal_render($form['body_field']);
  $output .= drupal_render($form['term_fields']);
  $output .= drupal_render($form['reset_term_fields']);
  $output .= theme('publication_node_form_base_elements', $form);
  return $output;
}

function theme_publication_taxon_section_node_form($form){
  publication_add_modal_child_files();
  $output = drupal_render($form['title']);
  $output .= drupal_render($form['reset_manuscript_name']);
  $output .= drupal_render($form['field_publication_taxon_citation']);
  $output .= drupal_render($form['field_publication_taxon_material']);
  $output .= theme('publication_node_form_base_elements', $form);
  return $output;
}

function theme_publication_taxon_description_node_form($form){
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
  $output .= '<div id="buttons">' . drupal_render($form['buttons']) . '</div>';
  return $output;
}

// Individual field theming */
function theme_publication_field_publication_images($element, $node){
  $output = theme('imagecache', 'thumbnail', $node->field_imagefield[0]['filepath'], $node->title);
  return $output;
}

function theme_publication_field_publication_biblio($element, $node){
  $output = $node->title . ' <em>' . $node->biblio_year . '</em>';
  return $output;
}

function theme_publication_field_publication_authors($element, $node){
  $output = $node->field_title[0]['value'];
  $output .= ' ' . $node->field_givennames[0]['value'];
  $output .= ' ' . $node->field_familyname[0]['value'];
  return $output;
}

function theme_publication_field_publication_section($element){
  drupal_add_tabledrag('publication-draggable-section', 'match', 'parent', 'section-parent', 'section-parent', 'section-id');
  $output = '<div id="publication-section"><div class="status-messages"></div>';
  if(count(element_children($element))){
    $header = array(
      'Title',
      'Weight',
      'Operations'
    );
    $rows = array();
    foreach(element_children($element) as $key){
      $row = array();
      $row[] = array(
        'data' => theme('indentation', $element[$key]['#depth']) . drupal_render($element[$key]['title']),
        'class' => 'section-title'
      );
      $nid = $element[$key]['nid']['#value'];
      $row[] = array(
        'data' => drupal_render($element[$key]['section_id']) . drupal_render($element[$key]['nid']) . drupal_render($element[$key]['section_parent']),
        'class' => 'section-data'
      );
      $row[] = drupal_render($element[$key]['ops']);
      $rows[$nid] = array(
        'data' => $row,
        'class' => $element[$key]['#attributes']['class']
      );
    }
    $output .= theme('table', $header, $rows, array(
      'id' => 'publication-draggable-section'
    ));
    $output .= '<div id="add-section"><p>';
    $output .= l('Add section', 'node/add/publication-section', array(
      'attributes' => array(
        'class' => 'modal'
      )
    ));
    $output .= '</p>';
    $output .= '<p class="description">';
    $output .= t('Add custom section to the publication or to an individual taxon description');
    $output .= '</p></div>';
  }
  $output .= '</div>';
  return $output;
}

function theme_publication_message($message, $type = 'status'){
  $output .= "<div class=\"messages $type\">\n";
  $output .= $message;
  $output .= "</div>\n";
  return $output;
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

function theme_term_fields_list($fields){
  $spm_fields = variable_get('spm_fields', array());
  foreach($fields as $group_name => $group_fields){
    foreach(array_filter($group_fields) as $selected_field){
      $term_field_items[] = $spm_fields[$group_name][$selected_field]['name'];
    }
  }
  $output = '<div class="term-fields-list">';
  $output .= theme('item_list', $term_field_items);
  $output .= '</div>';
  return $output;
}

/**
 * BAsed on theme_content_multiple_values()
 * Theme an individual form element.
 *
 * Combine multiple values into a table with drag-n-drop reordering.
 * 
 * Added: check there are items
 */
function theme_publication_content_multiple_values($element){
  $field_name = $element['#field_name'];
  $field = content_fields($field_name);
  $output = '';
  if($field['multiple'] >= 1){
    $table_id = $element['#field_name'] . '_values';
    $order_class = $element['#field_name'] . '-delta-order';
    $required = !empty($element['#required']) ? '<span class="form-required" title="' . t('This field is required.') . '">*</span>' : '';
    $header = array(
      array(
        'data' => t('!title: !required', array(
          '!title' => $element['#title'],
          '!required' => $required
        )),
        'colspan' => 3
      ),
      t('Order')
    );
    $rows = array();
    // Sort items according to '_weight' (needed when the form comes back after
    // preview or failed validation)
    $items = array();
    foreach(element_children($element) as $key){
      if($key !== $element['#field_name'] . '_add_more'){
        $items[] = &$element[$key];
      }
    }
    usort($items, '_content_sort_items_value_helper');
    // Add the items as table rows.
    foreach($items as $key => $item){
      $item['_weight']['#attributes']['class'] = $order_class;
      $delta_element = drupal_render($item['_weight']);
      if($nid = (is_numeric($item['#default_value']['nid']) ? $item['#default_value']['nid'] : $_POST['selectedItems'][$key])){
        $node = node_load($nid);
      }
      $cells = array(
        array(
          'data' => '',
          'class' => 'content-multiple-drag'
        ),
        array(
          'data' => drupal_render($item),
          'class' => 'item'
        ),
        // Get some pretty content for the autocomplete field
        theme('publication_' . $field['field_name'], $item, $node),
        array(
          'data' => $delta_element,
          'class' => 'delta-order'
        )
      );
      $rows[] = array(
        'data' => $cells,
        'class' => 'draggable ' . $item['#attributes']['class'] // Changed to allow class names
      );
    }
    $output .= theme('table', $header, $rows, array(
      'id' => $table_id,
      'class' => 'content-multiple-table'
    ));
    $output .= $element['#description'] ? '<div class="description">' . $element['#description'] . '</div>' : '';
    $output .= drupal_render($element[$element['#field_name'] . '_add_more']);
    drupal_add_tabledrag($table_id, 'order', 'sibling', $order_class);
  }else{
    foreach(element_children($element) as $key){
      $output .= drupal_render($element[$key]);
    }
  }
  return $output;
}
?>