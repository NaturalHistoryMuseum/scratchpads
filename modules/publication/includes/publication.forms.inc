<?php

function publication_form(&$node, $form_state) {

  $type = node_get_types('type', $node);
  
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => $type->title_label,
    '#required' => TRUE,
    '#default_value' => $node->title,
    '#weight' => -5
  );

  if($type->has_body){
        
    $form['body_field']['body'] = array(
      '#type' => 'textarea',
      '#title' => $type->body_label,
      '#default_value' => $node->body,
      '#required' => ($type->body_required ? TRUE : FALSE)
    );

    $form['body_field']['format'] = filter_form($node->format);
    
  }  
  
  $func = '_'.$node->type.'_node_form';
  
  if(function_exists($func)){
    
    $func($form, $node);
    
  }
  
  return $form;
  
}





function _publication_node_form(&$form, &$node) {

  $form['vocabulary'] = array(
    '#weight' => -1,
    '#type' => 'select',
    '#title' => t('Classification'),
    '#options' => publication_get_classification_vocabularies(),
    '#default_value' => $node->vocabulary,
    '#ahah' => array(
      'path' => 'publication/js/'. $node->nid,
      'wrapper' => 'classification-names',
      'method' => 'replace',
      'effect' => 'fade',
      'progress' => array('type' =>  'bar', 'message' => t('Loading classification...')),
    )
  );

  // Get the nomenclature form fields
  
  if($node->vocabulary){

    if($term_node = _term_node_get_content_type_for_vid($node->vocabulary)){

      module_load_include('inc', 'content', 'includes/content.crud');

      $fields = content_field_instance_read(array('type_name' => $term_node));

      if(count($fields)){

        foreach($fields as $field){

          $options[$field['field_name']] = $field['widget']['label'];

        }

        $form['nomenclature_fields'] = array(
          '#type' => 'checkboxes', 
          '#default_value' => $node->nomenclature_fields,
          '#options' => $options,
          '#parents' => array('nomenclature_fields'),
          '#default_value' => (isset($node->nomenclature_fields) ? $node->nomenclature_fields : array_keys($options))
        );

      }
      
    }else{
      
      $vocabulary = taxonomy_vocabulary_load($node->vocabulary);
      
      $form['nomenclature_fields'] = array(
        '#value' => theme('publication_message', t('<p> There is no term node associated with the @vocabulary vocabulary, so there are no nomenclature fields to select.</p>', array('@vocabulary' => $vocabulary->name))),
      );
      
    }

  }else{
    
    $form['nomenclature_fields'] = array(
      '#value' => theme('publication_message', t('<p>A vocabulary needs to be selected for this publication before nomenclature fields can be chosen.</p><p>If you have just chosen a vocabulary, please <em>save</em> the publication.</p>')),
    );    
    
  }
  
  $form['term_field_defaults'] = publication_node_form_term_fields_element($node->term_field_defaults);
  
}


function publication_node_form_term_fields_element($default_values){

  $element = array();
  $element['#tree'] = true;
  
  $spm_headers = variable_get('spm_headers', array());
  $spm_fields = variable_get('spm_fields', array());

  foreach($spm_fields as $spm_group => $spm_field){
    
    $options = array();
    
    foreach($spm_field as $field_name => $field){
    
      $options[$field_name] = $field['name'];
      
    }
    
    $element[$spm_group] = array(
      '#type' => 'checkboxes', 
      '#title' => $spm_headers[$spm_group]['name'], 
      '#default_value' => array_values($default_values[$spm_group]),
      '#options' => $options,
    );
     
  };
  
  $element['#theme'] = 'publication_term_fields_element';
  
  return $element;
  
}

function _publication_taxon_description_node_form(&$form, &$node) {
 
  $publication_nid = $_GET['publication_nid'];
  $publication_vid = $_GET['publication_vid'];
  
  $publication_node = node_load($publication_nid, $publication_vid);
  
  $form['term_field_defaults'] = array(
    '#type' => 'value',
    '#value' => $publication_node->term_field_defaults
  );
  
  if($node->term_fields){
  
    $term_field_defaults = $node->term_fields;
  
  }else{
    
    $term_field_defaults = $publication_node->term_field_defaults;            
  
  }
  
  $form['term_fields'] = publication_node_form_term_fields_element($term_field_defaults);
  $form['term_fields']['#allow_reset'] = is_array($node->term_fields);
   
  return $form;
  
}

function publication_taxon_description_node_form_reset($form, &$form_state){
  
  publication_db_query('UPDATE publication_taxon_description_node SET term_fields = NULL', array('vid' => $form_state['values']['vid']));

  $args = array(
    'type' => $form['#node']->type,
    'nid' => $form_state['values']['nid'],
    'op' => 'Reset',
  );

  $form_state['rebuild'] = true;
  
  modalframe_close_dialog($args);
  
}




/**
 * Implementation of hook_form_alter().
 */
function publication_form_alter(&$form, $form_state, $form_id) {

  switch ($form_id){
    
    case 'publication_node_form':
    
      $node = $form['#node'];

      if($node->nid){                
        
        $form['publication_nid'] = array('#type' => 'hidden', '#value' => $node->nid);
        
        $form['publication_vid'] = array('#type' => 'hidden', '#value' => $node->vid);
        
        // #default_value so it can be changed by js
        $form['current_tab'] = array('#type' => 'hidden', '#default_value' => NULL);
        
        if(isset($form_state['values']['vocabulary'])){
        
          $vid = $form_state['values']['vocabulary'];
        
        }else{
        
          $vid = $form['vocabulary']['#default_value'];
        
        }

        foreach(element_children($form['taxonomy']) as $key){

          if($key != $vid){

            unset($form['taxonomy'][$key]);

          }

        }

        unset($form['taxonomy']['#type']);

        unset($form['taxonomy'][$vid]['#title']);

        // Auto select terms from group nodes
        if($node && $form_state['values']['vocabulary_auto_select']){

          $groups = og_group_child_nids($node->nid);

          if(count($groups)){

            $form['taxonomy'][$vid]['#default_value'] = publication_nodes_get_terms($groups, $vid);

          }

        }

        taxonomy_tree_form_add_treeview($form);

        if($vid){
          
          $form['taxonomy']['#prefix'] = '<div id="classification-names">';
          $form['taxonomy']['#suffix'] = '</div>';
                    
          
        }else{
          
          $form['taxonomy'] = array('#value' => '<div id="classification-names"></div>');
          
        }
        
        $form['vocabulary_auto_select'] = array(
          '#type' => 'checkbox',
          '#title' => t('Auto-select terms from content attached to this publication?'),
          '#default_value' => 1
        );

        $fields = publication_field_info();  
        
        foreach($fields as $field){
        
          if($field['type_name'] == $node->type && $field['widget']['type'] == 'nodereference_autocomplete'){
            
            if($field['group']){
              
              $form[$field['group']]['#attributes']['class'] .= ' publication-noderef';
              publication_field_publication_nodereference_alter($form[$field['group']][$field['field_name']], $field, $node->nid);
              
            }else{
              
              publication_field_publication_nodereference_alter($form[$field['field_name']], $field, $node->nid);
              
            }
            
          }
          
        }
        
        if(!$vid){
          
          $form['field_publication_section'] = array(
            '#value' => theme('publication_message', t('Please select a vocabulary.'))
            );
          
        }elseif(!count($form['taxonomy'][$vid]['#default_value'])){
          
          $form['field_publication_section'] = array(
            '#value' => theme('publication_message', t('Please select at least one taxon name.'))
          );
          
        }else{

          publication_field_publication_section_alter($element, $form['field_publication_section']['#default_value']);          

          $form['field_publication_section'] = $element;
          $form['field_publication_section']['#tree'] = true;
          $form['field_publication_section']['#weight'] = 0;
          $form['field_publication_section']['#theme'] = 'publication_field_publication_section';
          
        }

        $form['#theme'] = 'publication_node_edit_form';

        array_unshift($form['#submit'], 'publication_node_edit_form_submit');
        
        
      }else{ // Node add form
        
        // Default organic group membership to "Invite only"
        $form['og_selective']['#default_value'] = 2; 
        $form['og_description'] = array('#type' => 'hidden', '#value' => NULL);
        $form['#theme'] = 'publication_node_add_form';
        array_unshift($form['#submit'], 'publication_node_add_form_submit');

      }
      
      // Alter both add & edit forms
      
      $form['title']['#description'] = t('A short name to refer to this publication (will not appear in the published article)');

      _publication_node_form_alter_base_elements($form); 
      
      unset($form['buttons']['preview']);

    break;
    
    case 'content_add_more_js': // Callback when new items are added to the form

      $field_id = arg(4);

      if(array_key_exists($field_id, $form)){
        
        if(is_array($nids = @explode(',', $_POST['selectedItems']))){

          $form[$field_id]['#suffix'] = theme('publication_'.$field_id, $nids, $_POST['publication_nid']);

          $form[$field_id]['#publication_nid'] = $_POST['publication_nid'];

          switch($field_id){
            
            case 'field_publication_authors':
            
              $form[$field_id]['#referenceable_types'] = 'profile';
            
            break;

            case 'field_publication_images':
            
              $form[$field_id]['#referenceable_types'] = 'image';
            
            break;
            
            case 'field_publication_biblio':
            
              $form[$field_id]['#referenceable_types'] = 'biblio';
            
            break;
                        
          }

          $form[$field_id]['#after_build'] = array('publication_field_nodereference_after_build');

        }

      }
    
    break;
    
    case 'publication_image_caption_node_form':

      if(!_publication_node_form_is_modal()){
        drupal_set_message('Sorry, image captions can only be added / edited from within a publication.', 'error');
        drupal_goto();
      }

      $publication_nid = $_GET['publication_nid'];

      $image_node = node_load(arg(2));
              
      $publication_node = node_load($publication_nid);
      
      $title = t('@op caption for image @image', array(
        '@op' => (empty($form['#node']->nid) ? 'Add' : 'Edit'),
        '@image' => $image_node->title
      ));

      drupal_set_title($title);
      
      // Set default values - they won't be rendered so don't need to hide them
      $form['title']['#value'] = 'Publication image caption : '.$publication_node->title.' : '.$image_node->title;        
      
      $form['field_publication_caption_parent']['#default_value'][0]['nid'] = $publication_node->nid;
      $form['field_publication_caption_image']['#default_value'][0]['nid'] = $image_node->nid;
      
      // Store the image node title to use in messages cos the generated node title is rubbish
      $form['#image_node_title'] = $image_node->title;

      unset($form['buttons']['preview']);
      
      $form['buttons']['cancel'] = array('#type' => 'submit', '#value' => t('Cancel'), '#weight' => 10);
      
      $form['buttons']['delete']['#submit'] = array('publication_node_delete_submit'); 
      
      $form['buttons']['submit']['#submit'][] = 'publication_modal_image_caption_node_form_submit'; 
    
      _publication_node_form_alter_base_elements($form); 

    break;
    
    case 'publication_section_node_form':

      if(!_publication_node_form_is_modal()){
        drupal_set_message('Sorry, section nodes can only be added / edited from within a publication.', 'error');
        drupal_goto();
      }  

      $node = $form['#node'];

      unset($form['buttons']['preview']);
      
      $form['publication_form_build_id'] = array(
        '#type' => 'value',
        '#value' => $_GET['publication_form_build_id']
      );
        
      
    $form['buttons']['delete']['#submit'] = array('publication_node_delete_submit');
    
    $form['buttons']['submit']['#submit'] = array('publication_section_modal_node_form_submit');
    
    $form['buttons']['cancel'] = array('#type' => 'submit', '#value' => t('Cancel'), '#weight' => 10); 

     _publication_node_form_alter_base_elements($form);
     $form['field_publication_section']['#type'] = 'value';   
      
    break;
    
    case 'publication_taxon_section_node_form':
    
      $node = $form['#node'];
    
      $term = array_shift($node->taxonomy);

      $form['title']['#description'] = t('Enter the final published name for this taxon.');

      $form['taxonomy'][$term->vid] = array(
      '#type' => 'value',
      '#value' => $term->tid
      );

      $form['term'] = array(
        '#type' => 'value',
        '#value' => $term
      );

      if(isset($term->name) && $term->name != $node->title){

        $form['reset_manuscript_name'] = array(
          '#type' => 'checkbox', 
          '#title' => t('Reset manuscript name to %name', array('%name' => $term->name)), 
        );

      }

      unset($form['buttons']['delete']);
      unset($form['buttons']['preview']);
      
      $form['buttons']['submit']['#submit'] = array('publication_taxon_section_modal_node_form_submit');

      $form['buttons']['cancel'] = array('#type' => 'submit', '#value' => t('Cancel'), '#weight' => 10); 

       _publication_node_form_alter_base_elements($form);     
       $form['field_publication_section']['#type'] = 'value'; 
    
    break;
    
    case 'publication_taxon_description_node_form':
    
    if(!_publication_node_form_is_modal()){
      drupal_set_message('Sorry, taxon descriptions can only be added / edited from within a publication.', 'error');
      drupal_goto();
    }
    
    $form['title']['#type'] = 'value';    
    $form['buttons']['submit']['#submit'][] = 'publication_taxon_description_modal_node_form_submit';
    
    if($form['term_fields']['#allow_reset']){
     
      $form['buttons']['reset'] = array(
        '#type' => 'submit', 
        '#weight' => 5,
        '#submit' => array('publication_taxon_description_node_form_reset'),
        '#value' => t('Reset to publication defaults') 
      );
      
    }
    
    unset($form['buttons']['delete']);
    unset($form['buttons']['preview']);
    $form['buttons']['cancel'] = array('#type' => 'submit', '#value' => t('Cancel'), '#weight' => 10); 
    
    _publication_node_form_alter_base_elements($form);
    
    break;
    
    case 'views_exposed_form':

      $view = $form['#parameters'][1]['view'];

      if($view->name == _publication_get_nodereference_view_name()){
        
        // Ensure these values are available when user submits filter form
        $form['vocabulary'] = array('#type' => 'hidden', '#value' => $_GET['vocabulary']);
        $form['vid'] = array('#type' => 'hidden', '#value' => $_GET['vid']);
        
        // // Convert the group drop down to a checkbox
        $form['nid'] = array(
          '#type' => 'hidden', 
          '#value' => $_GET['nid'], 
        );
        
        if(array_key_exists('group_nid', $form)){
        
          $form['group_nid'] = array(
            '#type' => 'hidden', 
            '#value' => ($_GET['filter_by_group'] ? $_GET['nid'] : 'All'), 
          );
        
          $form['filter_by_group'] = array(
            '#type' => 'checkbox', 
            '#title' => $form['#info']['filter-group_nid']['label'],
          );
        
          unset($form['#info']['filter-group_nid']);
          
        }
        
        if(array_key_exists('node_author_in_group', $form)){
          
          $form['node_author_in_group'] = array(
            '#type' => 'hidden', 
            '#value' => ($_GET['filter_by_author'] ? $_GET['nid'] : NULL),
          );
          
          $form['filter_by_author'] = array(
            '#type' => 'checkbox', 
            '#title' => $form['#info']['filter-node_author_in_group']['label'],
          );
          
        }
        
        $form['submit']['#value']= t('Apply filter');
        $form['submit']['#weight'] = 10;

        $class = 'collapsible';

        if(!$_GET['view_name']){
          $class .= ' collapsed';
        }
                
        $form['#prefix'] = '<fieldset class="'.$class.'"><legend>'.t('Filter items').'</legend>';
        $form['#suffix'] = '</fieldset>';
        
        
      }
    
    break;
    
    case 'taxonomy_form_vocabulary':
    
      // This will always be the same as publication node
      unset($form['content_types']['nodes']['#options']['publication_section']);
      array_unshift($form['#submit'], 'publication_taxonomy_form_vocabulary_submit');

    break;
    
  }
  
}

/**
 * Helper function - make all the required base elements type value
 *
 * @param string $form 
 * @return void
 * @author Ben Scott
 */
function _publication_node_form_alter_base_elements(&$form){

  $form['revision'] = array('#type' => 'value', '#value' => $form['revision_information']['revision']['#default_value']);
  $form['status'] = array('#type' => 'value', '#value' => $form['options']['status']['#default_value']);
  
  unset($form['author']['#type']);
  
  foreach(element_children($form['author']) as $key){
    
    $form['author'][$key]['#type'] = 'value';
    
  }
  
   $form['#pre_render'][] = 'publication_pre_render_node_form';
  
}



function publication_field_publication_nodereference_alter(&$element, $field, $nid){
  
  $nids = array();

  // Get all the NIDs for this element
  foreach(element_children($element) as $key){
    if($element[$key]['#default_value']['nid']){
      $nids[] = $element[$key]['#default_value']['nid'];
    }
  }

  $element['#suffix'] = theme('publication_'.$field['field_name'], $nids, $nid).'</div>';
  
  // Set up some vauales to pass to after_build function
  $element['#publication_nid'] = $nid;
  $element['#referenceable_types'] = current($field['referenceable_types']);
  $element['#after_build'] = array('publication_field_nodereference_after_build'); 
  
}





function publication_field_publication_section_alter(&$element, $nids, $parent = 0, $depth = 0){

  foreach($nids as $value){

    $nid = $value['nid'];

    if(!array_key_exists($nid, $element) && $child_node = node_load($nid)){ // prevent killing the site if user has selected parent as child

      switch($child_node->type){
        
        case 'publication_section':
        
          $title = '<span class="title">'.$child_node->title.'</span>';

          $op = l('Edit', 'node/'.$nid.'/edit', array('attributes' => array('class' => 'modal'))) .
                l('Remove', 'publication/'.$nid.'/delete', array('attributes' => array('class' => 'modal')));
                
          $class = 'section';
        
        break;

        case 'publication_taxon_section':
                  
          $term = array_shift($child_node->taxonomy);

          if($term->name != $child_node->title){

          $title = '<span class="title manuscript-name" title="'.$term->name.'">'.$child_node->title.'</span>';
        
          }else{
            
          $title = '<span class="title">'.$child_node->title.'</span>';  
            
          }
        
          $op = l('Edit', 'node/'.$nid.'/edit', array('attributes' => array('class' => 'modal')));
          
          $class = 'taxon-section';
        
        break;        

        case 'publication_taxon_description':
        
          $title = '<span class="title">'.$child_node->title.'</span>';
        
          if(is_array($child_node->term_fields)){

            $title .= '<br />' . theme('term_fields_list', $child_node->term_fields);

          }

          $op = l('Edit term fields', 'node/'.$nid.'/edit', array('attributes' => array('class' => 'modal')));
          
          $class = 'taxon-description tabledrag-leaf';
        
        break;
        
      }
      
      $class .= ' draggable draggable-section-'.$nid;

      $element[$nid] = array(
        'title'  => array('#value' => $title),
         'section_id' => array(
           '#type' => 'hidden',
           '#value' => $nid,
           '#attributes' => array('class' => 'section-id')
         ),
         'nid' => array(
           '#type' => 'hidden',
           '#value' => $nid,
           '#name' => 'field_publication_section[nids][]'
         ),
         '#depth' => $depth,
         'section_parent' => array(
           '#type' => 'textfield',
           '#default_value' => $parent,
           '#name' => 'field_publication_section['.$nid.'][section_parent]',
           '#attributes' => array('class' => 'section-parent')
          ),
          'ops' => array(
            '#value' => $op
            ),
          '#attributes' => array('class' => $class)
      );

      if(isset($child_node->field_publication_section[0]['nid'])){
      
        publication_field_publication_section_alter($element, $child_node->field_publication_section, $child_node->nid, $depth + 1);
        
      }
     
    }

  }
  
}


function publication_field_nodereference_after_build($element){
  
  $field_name = $element[0]['#field_name'];
  
  foreach(element_children($element) as $key){
  
    $element[$key]['nid']['#element_validate'] = array('publication_field_nodereference_element_validate');
  
  }
  
  $element[$field_name.'_add_more']['#value'] = t('Edit publication @label', array('@label' => drupal_strtolower($element['#title'])));
  
  $element[$field_name.'_add_more']['#prefix'] .= l(t('Create @label', array('@label' => drupal_strtolower($element['#title']))), 'node/add/'.$element['#referenceable_types'], array('query' => array('gids[]' => $element['#publication_nid']), 'attributes' => array('target' => '_blank', 'class' => 'create-'.$field_name)));
  
  return $element;
  
}


/**
 * Additional submit function for vocabulary form
 * Ensure publication_section node has the same vocabularies as publications
 *
 * @param string $form 
 * @param string $form_state 
 * @return void
 * @author Ben Scott
 */
function publication_taxonomy_form_vocabulary_submit($form, &$form_state){

  if($form_state['values']['nodes']['publication']){

    $form_state['values']['nodes']['publication_taxon_section'] = 'publication_taxon_section';
    
  }
  
}

/**
 * Submit function for add publication form; set og description
 *
 * @param string $form 
 * @param string $form_state 
 * @return void
 * @author Ben Scott
 */
function publication_node_add_form_submit($form, &$form_state){

  $form_state['values']['og_description'] =  truncate_utf8($form_state['values']['field_publication_title'][0]['value'], 140, TRUE, TRUE);

}

/**
 * Updated nodereference validate funciton - don't check node titles match 'cos we're rewriting them
 *
 * @param string $element 
 * @param string $form_state 
 * @return void
 * @author Ben Scott
 */
function publication_field_nodereference_element_validate(&$element, &$form_state){

  $field_name = $element['#field_name'];
  $type_name = $element['#type_name'];
  $field = content_fields($field_name, $type_name);
  $field_key  = $element['#columns'][0];
  $delta = $element['#delta'];
  $value = $element['#value'][$field_key];

  $nid = NULL;
  if (!empty($value)) {
    preg_match('/^(?:\s*|(.*) )?\[\s*nid\s*:\s*(\d+)\s*\]$/', $value, $matches);
    if (!empty($matches)) {
      // Explicit [nid:n].
      list(, $title, $nid) = $matches;
    }
  }
  
  form_set_value($element, $nid, $form_state);

}


/**
 * Submit function; ensure user can select nothing
 *
 * @param string $form 
 * @param string $form_state 
 * @return void
 * @author Ben Scott
 */
function publication_node_edit_form_submit($form, &$form_state){

  $node = $form['#node'];

  $vid = $form_state['values']['vocabulary'];

  if(key($form_state['values']['taxonomy'][$vid]) == 0){
    
    $form_state['values']['taxonomy'][$vid] = array();
    
  }
  
  $parent_nodes = array();
  $nids = array();
  
  ($vid ? $tids = $form_state['values']['taxonomy'][$vid] : $tids = array());

  // Get all deselected terms
  
  if($deselected_tids = array_diff($form['taxonomy'][$vid]['#default_value'], $form_state['values']['taxonomy'][$vid])){
    
    $nids = array();
    
    publication_deselect_taxon_treatment_sections($node, $deselected_tids, $nids);
    
  }

  foreach($form['#post']['field_publication_section']['nids'] as $nid){

    if(!in_array($nid, $nids['deselected_nids'])){

     if($parent_id = $form['#post']['field_publication_section'][$nid]['section_parent']){

       publication_orphan_section($nid, $parent_id);
       
       $parent_nodes[$parent_id][] = array('nid' => $nid);

     }else{
       
       publication_orphan_section($nid, $node->nid);
       
       $nids[] = array('nid' => $nid);

     }
      
    }
      
  }

  // Add any orphaned nids to the publication parent
  if($orphaned_nids = array_diff($nids['orphaned_nids'], $nids['deselected_nids'])){
    
    foreach($orphaned_nids as $orphaned_nid){
      
      if($orphaned_nid != $node->nid){
        
      $nids[] = array('nid' => $orphaned_nid);
      
      }
    }
    
  }
  
  if(count($parent_nodes)){
    
    foreach($parent_nodes as $parent_nid => $values){
      
      $parent_node = node_load($parent_nid, NULL, TRUE);
      $parent_node->field_publication_section = $values;
      node_save($parent_node);
      
    }    
    
  }
  
  if($new_tids = array_diff($form_state['values']['taxonomy'][$vid], $form['taxonomy'][$vid]['#default_value'])){

    foreach($new_tids as $tid){

     $nids[] = array(
       'nid' => publication_create_taxon_section_node($node->vocabulary, $tid)
     );

    }
    
  }

  $form_state['values']['field_publication_section'] = $nids;

  if($form['#post']['deleted_node']){
    
    foreach($form['#post']['deleted_node'] as $type => $deleted_node){
      
      $nid = array_shift($deleted_node);
      
      switch($type){
        
        case 'publication_section':
        
          publication_orphan_section($nid, $node->nid);
        
        break;
        
        case 'publication_image_caption':
        
          $image_caption_node = node_load($nid, null, true);

          $image_caption_node->field_publication_caption_image = array(array('nid' => NULL));

          node_save($image_caption_node);
        
        break;
        
      }
      
    }
    
  }
  
  // Add all contributors to the group   
 foreach($form_state['values']['field_publication_authors'] as $field){
   
   if($field['nid']){
   
      $profile_node = node_load($field['nid']);

      $account = user_load($profile_node->uid);
     
      og_subscribe_user($node->nid, $account);
     
   }
   
   
 }

}

/**
 * Orphan a section if it's original parent doesn't match $parent_id
 *
 * @param string $nid 
 * @param string $parent_id 
 * @return void
 * @author Ben Scott
 */
function publication_orphan_section($nid, $new_parent_id){
  
  $cck_params = array(
    'publication_section' => $nid, 
  );

  $existing_parent_node = publication_content_node_load(NULL, $cck_params);

  if(is_object($existing_parent_node) && $existing_parent_node->nid != $new_parent_id){
  
    foreach($existing_parent_node->field_publication_section as $key => $field_value){
  
      if($field_value['nid'] == $nid){
  
        unset($existing_parent_node->field_publication_section[$key]);
        break;
  
      }
  
    }            

  node_save($existing_parent_node);
  
  }
  
}

function publication_deselect_taxon_treatment_sections($node, &$deselected_tids, &$nids, $parent_removed = false){

  // Clean up section nodes for deselected terms
  foreach($node->field_publication_section as $publication_section_key => $publication_section){

    $nid = $publication_section['nid'];
    
    if($parent_removed){
      
      foreach($node->taxon_treatment_sections as $taxon_treatment_nid => $taxon_treatment_section){
        
        $nids['orphaned_nids'][$taxon_treatment_nid] = $taxon_treatment_nid;  
        
      }
      
    }
    
    if(array_key_exists($nid, $node->taxon_treatment_sections)){
      
       $tid = $node->taxon_treatment_sections[$nid];    
         
       if (in_array($tid, $deselected_tids))
       {
       
       $nids['deselected_nids'][$nid] = $nid;   
       unset($node->field_publication_section[$publication_section_key]);
       $is_changed = true;
       
       }

    }
      
    $child_node = node_load($nid);
    publication_deselect_taxon_treatment_sections($child_node, $deselected_tids, $nids, $is_changed);

  }
  
  if($is_changed){

    node_save($node);
    
  }
  
}

/**
 * Hide all the input filter selects
 *
 * @param string $form 
 * @return void
 * @author Ben Scott
 */
function publication_pre_render_node_form(&$form) {

  static $profile;
  
  if(!isset($profile)){
  
    $filter_formats = filter_formats();
    
    foreach($filter_formats as $filter_format){
      
      if($filter_format->name == t('Full HTML')){

        $profile = wysiwyg_get_profile($filter_format->format);
        
      }
      
    }
    
  }
  
  foreach(element_children($form) as $key){
  
    $element = &$form[$key];

    if(array_key_exists('format', $element)){

      if($element['#id']){
        
        $id = $element['#id'].'-value';
        
      }elseif($element['body']['#id']){

          $id = $element['body']['#id'];
          
      }

      $class = 'wysiwyg wysiwyg-format-' . $profile->format . ' wysiwyg-editor-' . $profile->editor . ' wysiwyg-field-' . $id . ' wysiwyg-status-1';
 
      $name = $element['format'][$profile->format]['#name']; 
              
      $element['format'] = array(
        '#type' => 'hidden',
        '#value' => $profile->format,
        '#attributes' => array('checked' => 'checked', 'class' => $class),
        '#name' => $name
      );
      
    }else{
  
      publication_pre_render_node_form($element);  
      
    }
  
  }
  
  return $form; 
  
}





?>