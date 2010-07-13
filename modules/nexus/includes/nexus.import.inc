<?php

function nexus_import_form($form_state){
  $form['#attributes'] = array(
    'enctype' => "multipart/form-data"
  );
  if($_SESSION['import_file']){
    $form_state['storage']['import_file'] = $_SESSION['import_file'];
    unset($_SESSION['import_file']);
  }
  if($form_state['storage']['import_file']){
    $form['title'] = array(
      '#type' => 'textfield', 
      '#title' => t('Title'), 
      '#required' => true
    );
    $import_file = $form_state['storage']['import_file']['data'];
    $placeholders = db_placeholders($import_file['taxa'], 'text');
    if(isset($form_state['values']['vid'])){
      $vid = $form_state['values']['vid'];
    }else{
      if(!$vid = db_result(db_query('SELECT td.vid FROM {term_data} td INNER JOIN {vocabulary_node_types} v ON td.vid = v.vid WHERE td.name IN (' . $placeholders . ') AND v.type = "nexus_project" ORDER BY COUNT(*) DESC LIMIT 1', $import_file['taxa']))){
        drupal_set_message(t('No vocabulary could be found with terms matching the import file.'), 'warning');
      }
    }
    $form['taxonomy']['vid'] = array(
      '#type' => 'select', 
      '#title' => t('Vocabulary'), 
      '#options' => nexus_get_vocabularies(), 
      '#description' => t('Please select which vocabulary the taxa in the import file belong to.'), 
      '#default_value' => $vid, 
      '#required' => true, 
      '#ahah' => array(
        'path' => 'nexus/callback/import', 
        'wrapper' => 'nexus-taxa', 
        'method' => 'replace', 
        'effect' => 'fade'
      )
    );
    $form['taxonomy']['taxa'] = array(
      '#value' => '', 
      '#prefix' => '<div id="nexus-taxa">', 
      '#suffix' => '</div>'
    );
    if($vid){
      $params = $import_file['taxa'];
      $params[] = $vid;
      $result = db_query('SELECT tid, name FROM term_data WHERE name IN (' . $placeholders . ') AND vid = %d', $params);
      while($term = db_fetch_object($result)){
        $tids[$term->name] = $term->tid;
      }
      foreach($import_file['taxa'] as $taxon){
        if(@array_key_exists($taxon, $tids)){
          $form['taxonomy']['taxa'][$taxon] = array(
            '#type' => 'hidden', 
            '#value' => $tids[$taxon], 
            '#prefix' => '<p class="nexus-taxa-match">' . $taxon . '</p>', 
            '#parents' => array(
              'taxonomy', 
              $taxon
            )
          );
        }else{
          $form['taxonomy']['taxa'][$taxon] = array(
            '#type' => 'textfield', 
            '#maxlength' => 100, 
            '#autocomplete_path' => 'taxonomy/autocomplete/' . $vid, 
            '#title' => $taxon, 
            '#description' => t('A matching taxon for %taxon could not be found in vocabulary %vocabulary. If there is a matching term, please choose it now.', array(
              '%taxon' => $taxon, 
              '%vocabulary' => $form['taxonomy']['vid']['#options'][$vid]
            )), 
            '#parents' => array(
              'taxonomy', 
              $taxon
            )
          );
        }
      }
    }
    $form['previous'] = array(
      '#type' => 'submit', 
      '#value' => t('Previous'), 
      '#submit' => array(
        'nexus_import_form_previous_submit'
      )
    );
    $form['import'] = array(
      '#type' => 'submit', 
      '#value' => t('Import'), 
      '#submit' => array(
        'nexus_import_form_import_submit'
      )
    );
    $form['datatype'] = array(
      '#type' => 'hidden', 
      '#value' => $form_state['storage']['import_file']['datatype']
    );
  }else{
    $form['import_file'] = array(
      '#type' => 'file', 
      '#title' => t('File to import'), 
      '#description' => t('Select the file you want to import.')
    );
    $form['submit'] = array(
      '#type' => 'submit', 
      '#value' => t('Upload'), 
      '#validate' => array(
        'nexus_import_form_upload_validate'
      ), 
      '#submit' => array(
        'nexus_import_form_upload_submit'
      )
    );
  }
  return $form;
}

function nexus_import_form_upload_validate(&$form, &$form_state){ //  global $user;
  if($_FILES['files']['name']['import_file'] == ''){
    form_set_error('import_file', t('You didn\'t upload a file'));
  }
  $limits = _upload_file_limits($user);
  $validators = array(
    'file_validate_extensions' => array(
      'nex'
    ), 
    'file_validate_size' => array(
      $limits['file_size'], 
      $limits['user_size']
    )
  );
  // Save new file uploads.
  if($file = file_save_upload('import_file', $validators, FALSE, FALSE)){
    $file->list = variable_get('upload_list_default', 1);
    $file->description = $file->filename;
    $file->weight = 0;
    $file->new = TRUE;
    $form_state['values']['import_file'] = (array)$file;
  }else{
    form_set_error('import_file');
  }
}

/**
 * Upload button submitted
 * Store the parsed file & pregress step by 1
 * @param $form
 * @param $form_state
 */
function nexus_import_form_upload_submit(&$form, &$form_state){
  //	Batch process the file
  if(is_array($lines = file($form_state['values']['import_file']['filepath']))){
    $batch = array(
      'operations' => array(
        array(
          'nexus_import_batch_process_file', 
          array(
            $lines
          )
        )
      ), 
      'finished' => 'nexus_import_batch_process_file_finished', 
      'title' => t('Uploading file'), 
      'error_message' => t('The upload process has encountered an error.'), 
      'file' => drupal_get_path('module', 'nexus') . '/includes/nexus.import.inc'
    );
    batch_set($batch);
  }else{
    drupal_set_message(t('There was an error reading the file'));
  }
}

function nexus_import_batch_process_file($lines, &$context){
  //	Define how many lines to process per pass
  $lines_to_process = 100;
  if(!isset($context['sandbox']['progress'])){
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = count($lines);
    $context['sandbox']['regex'] = "/^'?(.+?)'?$/";
  }
  $max_in_this_loop = $context['sandbox']['progress'] + $lines_to_process;
  for($context['sandbox']['progress']; $context['sandbox']['progress'] < $max_in_this_loop && $context['sandbox']['progress'] < $context['sandbox']['max']; $context['sandbox']['progress']++){
    // Set btach file progress counters
    $line = trim($lines[$context['sandbox']['progress']]);
    if(_nexus_import_skip_line($line)){
      continue;
    }
    if(strpos(strtolower($line), 'symbols')){
      preg_match('/.*?SYMBOLS="([0-9]+).*"/i', $line, $symbols);
      $context['results']['base'] = substr($symbols[1], 0, 1);
    }
    //    Checkdata type = standard (& maybe DNA)
    if(strpos(strtolower($line), 'datatype')){
      preg_match('/.*?DATATYPE\s?=\s?([A-Z]+)\s.*/i', $line, $datatype);
      if(!(strtolower($datatype[1]) == 'standard' || strtolower($datatype[1]) == 'dna')){ // If this isn't a standard nexus file, error out
        drupal_set_message(t('Sorry, this file could not be imported - only standard or DNA datatype nexus files are currently supported.'), 'error');
        $context['results']['error'] = true;
        $context['finished'] = 1;
        return;
      }else{
        $context['results']['datatype'] = strtolower($datatype[1]);
      }
    }
    switch($line){
      case 'TAXLABELS':
      case 'Taxlabels':
      case 'taxlabels':
        $context['sandbox']['key'] = 'taxa';
        $context['sandbox']['ref'] = &$context['results']['data'][$context['sandbox']['key']];
        $context['message'] = t('Processing taxa');
        break;
      case 'CHARLABELS':
      case 'Charlabels':
      case 'charlabels':
        $context['sandbox']['key'] = 'chars';
        $context['sandbox']['regex'] = "/\[([0-9]+)\]\s'?(.+?)'?$/i";
        $context['sandbox']['ref'] = &$context['results']['data'][$context['sandbox']['key']];
        $context['message'] = t('Processing characters');
        break;
      case 'STATELABELS':
      case 'Statelabels':
      case 'statelabels':
        $context['sandbox']['key'] = 'states';
        $context['sandbox']['regex'] = "/^'?(.+?)'?,{0,1}$/i";
        $context['sandbox']['ref'] = &$context['results']['data'][$context['sandbox']['key']];
        $context['message'] = t('Processing character states');
        break;
      case 'MATRIX':
      case 'Matrix':
      case 'matrix':
        $context['sandbox']['key'] = 'matrix';
        $context['sandbox']['ref'] = &$context['results']['data'][$context['sandbox']['key']];
        if($context['results']['datatype'] == 'dna'){
          $context['sandbox']['regex'] = "/^'?(.+?)'?\s{2,}([ACGTUYRKW\-\?]+)/i";
        }else{
          $context['sandbox']['regex'] = "/(?>'(.+)'\s+|(.+)\s+)([{}0-9()?-\s]+)/i";
        }
        $context['message'] = t('Processing matrix data');
        break;
      case '[Character comments]':
        $context['sandbox']['key'] = 'char_comments';
        $context['sandbox']['ref'] = &$context['results']['data'][$context['sandbox']['key']];
        $context['sandbox']['regex'] = "/TEXT\sCHARACTER=([0-9]*)\sTEXT='?(.*?)'?;$/i";
        $context['message'] = t('Processing character comments');
        break;
      case '[Attribute comments]':
        $context['sandbox']['key'] = 'attribute_comments';
        $context['sandbox']['ref'] = &$context['results']['data'][$context['sandbox']['key']];
        $context['sandbox']['regex'] = "/TEXT\sTAXON=([0-9]*)\sCHARACTER=([0-9]*)\sTEXT='?(.*?)'?;$/i";
        $context['message'] = t('Processing attribute comments');
        break;
      case 'BEGIN SETS;':
      case 'Begin sets;':
      case 'begin sets;':
        $context['sandbox']['key'] = 'groups';
        $context['sandbox']['ref'] = &$context['results']['data'][$context['sandbox']['key']];
        $context['sandbox']['regex'] = "/CHARSET\s'?(.*?)'?\s=\s(.*);/i";
        $context['message'] = t('Processing character groups');
        break;
      case is_numeric($line):
        $context['sandbox']['ref'] = &$context['results']['data'][$context['sandbox']['key']][$line];
        break;
      case ';':
      case 'ENDBLOCK;':
      case 'Endblock;':
      case 'endblock;':
        unset($context['sandbox']['key']);
        break;
      default:
        if($context['sandbox']['key']){
          preg_match_all($context['sandbox']['regex'], $line, $matches);
          if(is_array($matches)){
            array_shift($matches);
            $matches = array_filter($matches);
            if($c = count($matches)){
              if($c > 1){
                $ms = array();
                foreach($matches as $match){
                  if($m = array_shift($match)){
                    $ms[] = _nexus_import_tidy_line($m);
                  }
                }
                $context['sandbox']['ref'][] = $ms;
              }else{
                $context['sandbox']['ref'][] = _nexus_import_tidy_line(array_shift(array_shift($matches)));
              }
            }
          }
        }
        break;
    }
  }
  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
}

function nexus_import_batch_process_file_finished($success, $results, $operations){
  if(!$results['error']){ // Results error can be set in batch process; if it isset there will already be an error message 
    if($success){
      if(!count($results['data']['taxa'])){
      	
      	foreach($results['data']['matrix'] as $matrix){
      		$results['data']['taxa'][] = $matrix[0];
      	}

      }elseif(!count($results['data']['chars']) && $results['datatype'] != 'dna'){
        drupal_set_message(t('There are no characters in this file to import.'), 'error');
        return;
      }
      $_SESSION['import_file'] = $results;
    }else{
      drupal_set_message(t('An error occurred while processing the file'), 'error');
    }
  }
}

/**
 * Previous button click; User going back to stage 1; 
 * @param $form
 * @param $form_state
 */
function nexus_import_form_previous_submit(&$form, &$form_state){
  unset($form_state['storage']['import_file']);
}

function nexus_import_form_import_submit(&$form, &$form_state){
  $batch = array(
    'operations' => array(
      array(
        'nexus_import_batch_process_import', 
        array(
          $form_state['storage']['import_file']['data'], 
          $form_state['storage']['import_file']['base'], 
          $form_state['values']
        )
      )
    ), 
    'finished' => 'nexus_import_batch_process_import_finished', 
    'title' => t('Importing'), 
    'error_message' => t('Sorry, there was an error importing the file.'), 
    'progressive' => TRUE, 
    'file' => drupal_get_path('module', 'nexus') . '/includes/nexus.import.inc'
  );
  batch_set($batch);
}

function nexus_import_batch_process_import($import_file, $base, $form_values, &$context){
  global $user;
  $taxonomy = array();
  if(!$number_of_groups = count($import_file['groups'])){
    $number_of_groups = 1;
  }
  $character_vid = nexus_get_character_vid();
  if(!isset($context['sandbox']['progress'])){
    $context['message'] = t('Creating the character project');
    $context['sandbox']['max'] = $number_of_groups;
    if(is_array($import_file['attribute_comments'])){
      $context['sandbox']['max'] += count($import_file['attribute_comments']);
    }
    // Get the taxon values to use
    foreach($form_values['taxonomy'] as $i => $taxon){
      if(!is_numeric($taxon)){
        if((strlen($taxon) > 0) && $term = nexus_get_term_by_name_and_vid($taxon, $form_values['vid'])){
          $form_values['taxonomy'][$i] = $term[0]->tid;
        }else{
          continue;
        }
      }
      $taxonomy[$form_values['vid']][] = $form_values['taxonomy'][$i];
    }
    //  Save the node at this point so we know the tid to use for the character group parent
    $node = _nexus_import_save_node('nexus_project', $form_values['title'], $taxonomy, array(
      'taxa_vid' => $form_values['vid']
    ));
    $context['results']['node'] = $node;
    $context['sandbox']['matrix'] = nexus_import_parse_matrix($import_file['matrix'], $form_values['taxonomy'], $form_values['datatype'], $form_values['vid'], $base);
    $context['sandbox']['progress'] = 0;
  }elseif($context['sandbox']['progress'] < $number_of_groups){
    // Create groups
    $context['message'] = t('Creating characters');
    $group_term = array(
      'vid' => $character_vid, 
      'parent' => $context['results']['node']->character_root_tid, 
      'weight' => $context['sandbox']['progress']
    );
    $func = 'nexus_import_add_' . $form_values['datatype'] . '_character';
    // If there's no groups, create a default one 
    if(!$import_file['groups']){ // No groups
      $group_term['name'] = 'Default';
      taxonomy_save_term($group_term);
      // No groups so get all chars
      if($form_values['datatype'] == 'dna'){
        $m = reset($context['sandbox']['matrix']);
        for($x = 1; $x <= strlen($m['characters']); $x++){
          $func($x, $x, $x, $character_vid, $group_term, $import_file, $context, $form_values);
        }
      }else{
        foreach($import_file['chars'] as $character_order => $character){
          $func($character[0], $character[1], $character_order, $character_vid, $group_term, $import_file, $context, $form_values);
        }
      }
    }elseif($group = $import_file['groups'][$context['sandbox']['progress']]){
      if(strpos($group[1], '-')){
        $exploded_group = explode('-', $group[1]);
        $characters = range($exploded_group[0], $exploded_group[1]);
      }else{
        $characters = explode(' ', $group[1]);
      }
      $group_term['name'] = $group[0];
      taxonomy_save_term($group_term);
      foreach($characters as $character_order => $character){
        if(!$character_name = _nexus_import_get_char_name($import_file, $character)){
          $character_name = $character_order;
        }
        // Call add character function
        $func($character, $character_name, $character_order, $character_vid, $group_term, $import_file, $context, $form_values);
      }
    }
    $context['sandbox']['progress']++;
  }elseif($attribute_comment = $import_file['attribute_comments'][$context['sandbox']['progress'] - $number_of_groups]){
    $context['message'] = t('Creating comments');
    $taxon = $import_file['taxa'][$attribute_comment[0] - 1];
    $taxonomy = array(
      $form_values['vid'] => array(
        $form_values['taxonomy'][$taxon]
      ), 
      $character_vid => array(
        $context['sandbox']['character_terms'][$attribute_comment[1]]['tid']
      )
    );
    _nexus_import_save_node('nexus_note', 'NOTE: ' . $taxon . '(' . $context['sandbox']['character_terms'][$attribute_comment[1]]['name'] . ')', $taxonomy, array(
      'body' => $attribute_comment[2]
    ));
    $context['sandbox']['progress']++;
  }
  $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
}

function nexus_import_add_standard_character($character, $character_name, $character_order, $character_vid, $group_term, $import_file, &$context, &$form_values){
  $character_term = array(
    'name' => $character_name,  // chars are indexed from 0 
    'vid' => $character_vid, 
    'parent' => $group_term['tid'], 
    'weight' => $character_order, 
    'description' => _nexus_import_get_char_description($import_file, $character)
  );
  taxonomy_save_term($character_term);
  $context['sandbox']['character_terms'][$character] = $character_term;
  //states  
  if(is_array($import_file['states'][$character])){
    foreach($import_file['states'][$character] as $state_order => $state){
      $taxonomy = array(
        $character_vid => $character_term['tid']
      );
      foreach($context['sandbox']['matrix'] as $tid => $matrix_data){
        if(is_array($matrix_data[$character]['states']) && in_array($state_order, $matrix_data[$character]['states'])){
          $taxonomy[$form_values['vid']][] = $tid;
        }
        if(isset($matrix_data[$character]['relationship'])){
          nexus_insert_relationship($matrix_data[$character]['relationship'], $character_term['tid'], $tid);
        }
      }
      _nexus_import_save_node('nexus_controlled_state', $state, $taxonomy);
    }
  }
}

function nexus_import_add_dna_character($character, $character_name, $character_order, $character_vid, $group_term, $import_file, &$context, &$form_values){
  $character_term = array(
    'name' => $character_name,  // chars are indexed from 0 
    'vid' => $character_vid, 
    'parent' => $group_term['tid'], 
    'weight' => $character_order, 
    'description' => _nexus_import_get_char_description($import_file, $character)
  );
  taxonomy_save_term($character_term);
  $context['sandbox']['character_terms'][$character] = $character_term;
  //  Mark as being DNA
  nexus_update_term_setting($character_term['tid'], 'dna', 1);
  $taxonomy = array(
    $character_vid => $character_term['tid']
  );
  foreach($context['sandbox']['matrix'] as $matrix_data){
    if(strlen($state = substr($matrix_data['characters'], $character - 1, 1)) > 0){
      $taxonomy[$form_values['vid']] = $matrix_data['tid'];
      _nexus_import_save_node('nexus_free_state', 'NEXUS DNA STATE: ' . $character_term['name'] . ' (' . $matrix_data['tid'] . ')', $taxonomy, array(
        'body' => $state
      ));
    }
  }
}

function _nexus_import_get_char_name($import_file, $character){
  foreach($import_file['chars'] as $char){
    if($char[0] == $character){return $char[1];}
  }
}

function _nexus_import_get_char_description($import_file, $character){
  foreach($import_file['char_comments'] as $char){
    if($char[0] == $character){return $char[1];}
  }
}

function nexus_import_parse_matrix($matrix, $taxonomy, $datatype, $vid, $base = 0){
  $parsed_data = array();
  switch($datatype){
    case 'dna':
      foreach($matrix as $matrix_data){
        if(!array_key_exists($matrix_data[0], $parsed_data)){
          $term = reset(nexus_get_term_by_name_and_vid($matrix_data[0], $vid));
          $parsed_data[$matrix_data[0]] = array(
            'characters' => $matrix_data[1], 
            'tid' => $term->tid
          );
        }else{
          $parsed_data[$matrix_data[0]]['characters'] .= $matrix_data[1];
        }
      }
      break;
    default:
      foreach($matrix as $matrix_data){
        $character_associations = preg_split('//', str_replace(' ', '', $matrix_data[1]), -1, PREG_SPLIT_NO_EMPTY);
        $term_name = trim($matrix_data[0]);
        if($tid = $taxonomy[$term_name]){
          $count = 1;
          $buffer = false;
          foreach($character_associations as $character_association){
            // Buffer is set to true for character associations with multiple states selected
            if(!$buffer){
              $char = array();
            }
            if($character_association == '('){ // Multistate OR
              $char['relationship'] = '/';
              $buffer = true;
            }elseif($character_association == '{'){ // Multistate AND
              // Don't need to specify relationship: default is AND 
              $buffer = true;
            }elseif($character_association == '}' || $character_association == ')'){ // END OF Multistate
              $buffer = false;
            }elseif($character_association == '?' || $character_association == '-'){ // Unknown
              $char['relationship'] = $character_association;
            }else{
              $char['states'][] = $character_association - $base;
            }
            if(!$buffer){
              $parsed_data[$tid][$count] = $char; // Subtract base to allow for keys starting from 1 etc.,
              $count++;
            }
          }
        }
      }
      break;
  }
  return $parsed_data;
}

function _nexus_import_save_node($type, $title, $taxonomy = array(), $properties = array()){
  global $user;
  $node = new stdClass();
  $node->title = $title;
  $node->author = $user->name;
  $node->type = $type;
  $node->uid = $user->uid;
  $node->status = 1;
  if(count($taxonomy)){
    $node->taxonomy = $taxonomy;
  }
  if(count($properties)){
    foreach($properties as $property => $value){
      $node->$property = $value;
    }
  }
  node_save($node);
  return $node;
}

function nexus_get_term_by_name_and_vid($name, $vid){
  $db_result = db_query(db_rewrite_sql("SELECT t.tid, t.* FROM {term_data} t WHERE LOWER(t.name) = LOWER('%s') AND t.vid = %d", 't', 'tid'), array(
    trim($name), 
    $vid
  ));
  $result = array();
  while($term = db_fetch_object($db_result)){
    $result[] = $term;
  }
  return $result;
}

function nexus_import_batch_process_import_finished($success, $results, $operations){
  if($success){
    // Redirect to the newly created project
    drupal_set_message(t('Character project %title has been created.', array(
      '%title' => $results['node']->title
    )));
        drupal_goto('node/' . $results['node']->nid .'/edit');
  }else{
    drupal_set_message(t('An error occurred while importing the file'), 'error');
  }
}

function _nexus_import_skip_line($line){
  if(strlen($line) < 1){return true;}
}

function _nexus_import_tidy_line($line){
  // NDE exports single 's as double 's
  return str_replace("''", "'", $line);
}

function theme_nexus_import_form($form){
  $path = drupal_get_path('module', 'nexus');
  drupal_add_css($path . '/css/import.css');
  return drupal_render($form);
}

