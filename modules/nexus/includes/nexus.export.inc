<?php

function nexus_export(){
  //  Ensure form is submitted
  if(!isset($_POST['nid'])){
    drupal_goto();
  }
  $node = node_load($_POST['nid']);
  $format = $_POST['format'];
  header("Content-Type: application/force-download");
  header("Content-Type: application/octet-stream");
  header("Content-Type: application/download");
  $filename = _nexus_export_get_file_name($node) . '.' . $format;
//  $filename = _nexus_export_get_file_name($node) . '.xml';
  header("Content-Disposition: attachment; filename=" . $filename);
  module_load_include('inc', 'nexus', 'includes/nexus.matrix');
  $matrix_data = nexus_project_get_matrix_data($node);
  // First column is for taxa
  array_shift($matrix_data->columns);

  // Filter columns for selected characters
  function _nexus_export_filter_columns($column){
    return (in_array($column['id'], $_POST['characters_to_export']));
  }
  $matrix_data->columns = array_filter($matrix_data->columns, "_nexus_export_filter_columns");
  $func = 'nexus_export_' . $format;
  if(function_exists($func)){
    echo $func($node, $matrix_data);
  }
  exit();
}

function _nexus_export_get_file_name($node){
  return preg_replace("/[^\.\-\s_a-zA-Z\d]/", "", str_replace(' ', '_', drupal_strtolower($node->title))) . '-' . time();
}

function nexus_export_sdd($node, $matrix_data){
  //  Ids used for terms, columns, states & groups    
  $t = array(); // terms
  $c = array(); // columns
  $s = array(); // states
  $g = array(); // groups
  $doc = new DomDocument('1.0', 'UTF-8');
  $doc->preserveWhiteSpace = false;
  $doc->formatOutput = true;
  $Datasets = $doc->createElement("Datasets");
  $Datasets->setAttribute('xsi:schemaLocation', 'http://rs.tdwg.org/UBIF/2006/ http://rs.tdwg.org/UBIF/2006/Schema/1.1/SDD.xsd');
  $Datasets->setAttribute('xmlns', 'http://rs.tdwg.org/UBIF/2006/');
  $Datasets->setAttribute('xmlns:xsi', 'http://www.w3.org/2001/XMLSchema-instance');
  $TechnicalMetadata = $doc->createElement("TechnicalMetadata");
  $TechnicalMetadata->setAttribute('created', date("Y-m-d\TH:i:s\Z"));
  $Generator = $doc->createElement("Generator");
  $Generator->setAttribute('name', 'Scratchpads');
  $Generator->setAttribute('version', '1');
  $Generator->setAttribute('notes', 'http://scratchpads.eu');
  $TechnicalMetadata->appendChild($Generator);
  $Datasets->appendChild($TechnicalMetadata);
  $Dataset = $doc->createElement("Dataset");
  $Dataset->setAttribute('xml:lang', 'en-us');
  $Dataset->appendChild(_nexus_export_sdd_representation_xml($doc, $node->title));
  // Taxon names
  $TaxonNames = $doc->createElement("TaxonNames");
  // Taxon hierarchy
  $Nodes = $doc->createElement("Nodes");
  // Use $matrix_data not $node->taxonomy as terms need to be in the correct order
  // parents before child terms to work in lucid
  foreach($matrix_data->data as $data){
    $tid = $data['id'];
    $t[$tid] = count($t);
    $TaxonName = $doc->createElement("TaxonName");
    $TaxonName->setAttribute('id', 't' . $t[$tid]);
    $TaxonName->appendChild(_nexus_export_sdd_representation_xml($doc, $data['taxa']));
    $TaxonNames->appendChild($TaxonName);
    // Hierarchy
    $Node = $doc->createElement("Node");
    $Node->setAttribute('id', 'tn' . $t[$tid]);
    $Node->setAttribute('debuglabel', $tid . ':' . $data['taxa']);
    // Get the information for the taxon hierarchies              
    if(is_array($parents = taxonomy_get_parents($tid))){
      $parent = array_shift($parents);
      if(array_key_exists($parent->tid, $node->taxonomy)){
        $Parent = $doc->createElement("Parent");
        $Parent->setAttribute('ref', 'tn' . $t[$parent->tid]);
        $Node->appendChild($Parent);
      }
    }
    $TaxonName = $doc->createElement("TaxonName");
    $TaxonName->setAttribute('ref', 't' . $t[$tid]);
    $Node->appendChild($TaxonName);
    $Nodes->appendChild($Node);
  }
  $Dataset->appendChild($TaxonNames);
  // Add the taxon hierachy nodes
  $TaxonHierarchies = $doc->createElement("TaxonHierarchies");
  $TaxonHierarchy = $doc->createElement("TaxonHierarchy");
  $TaxonHierarchy->appendChild(_nexus_export_sdd_representation_xml($doc, 'Default Entity Tree'));
  $TaxonHierarchyType = $doc->createElement("TaxonHierarchyType");
  $TaxonHierarchyType->appendChild($doc->createTextNode('UnspecifiedTaxonomy'));
  $TaxonHierarchy->appendChild($TaxonHierarchyType);
  $TaxonHierarchy->appendChild($Nodes);
  $TaxonHierarchies->appendChild($TaxonHierarchy);
  $Dataset->appendChild($TaxonHierarchies);
  $Characters = $doc->createElement("Characters");
  $Nodes = $doc->createElement("Nodes");
  $DescriptiveConcepts = $doc->createElement("DescriptiveConcepts");
  $state_count = 0;
  foreach($matrix_data->columns as $column){
    $c[$column['id']] = array(
      'index' => count($c),
      'type' => $column['type']
    );
    switch($column['type']){
      case 'quantitative':
        $Character = $doc->createElement("QuantitativeCharacter");
        break;
      case 'free':
        $Character = $doc->createElement("TextCharacter");
        break;
      default:
        $Character = $doc->createElement("CategoricalCharacter"); // Controlled characters
        $States = $doc->createElement("States");
        foreach($column['states'] as $state_key => $state){
          if(is_numeric($state_key)){
            $s[$column['id']][$state_key] = $state_count;
            $StateDefinition = $doc->createElement("StateDefinition");
            $StateDefinition->appendChild(_nexus_export_sdd_representation_xml($doc, $state['state']));
            $StateDefinition->setAttribute('id', 's' . $state_count);
            $States->appendChild($StateDefinition);
            $state_count++;
          }
        }
        break;
    }
    $Character->setAttribute('id', 'c' . $c[$column['id']]['index']);
    $Character->appendChild(_nexus_export_sdd_representation_xml($doc, $column['term'], $column['description']));
    if($column['type'] == 'controlled'){
      $Character->appendChild($States);
    }
    $Characters->appendChild($Character);
    if(!isset($g[$column['groupID']])){
      $g[$column['groupID']] = count($g);
      // Create a new group
      $Node = $doc->createElement("Node");
      $Node->setAttribute('id', 'cn' . $g[$column['groupID']]);
      $Node->setAttribute('debuglabel', $column['group']);
      $DescriptiveConcept = $doc->createElement("DescriptiveConcept");
      $DescriptiveConcept->setAttribute('ref', 'dc' . $g[$column['groupID']]);
      $Node->appendChild($DescriptiveConcept);
      $Nodes->appendChild($Node);
      // Groups also need to be descriptive concepts          
      $DescriptiveConcept = $doc->createElement("DescriptiveConcept");
      $DescriptiveConcept->setAttribute('id', 'dc' . $g[$column['groupID']]);
      // Load the group term so we have the description
      $group_term = taxonomy_get_term($column['groupID']);
      $DescriptiveConcept->appendChild(_nexus_export_sdd_representation_xml($doc, $column['group'], $group_term->description));
      $DescriptiveConcepts->appendChild($DescriptiveConcept);
    }
    $CharNode = $doc->createElement("CharNode");
    $Parent = $doc->createElement("Parent");
    $Parent->setAttribute('ref', 'cn' . $g[$column['groupID']]);
    $Character = $doc->createElement("Character");
    $Character->setAttribute('ref', 'c' . $c[$column['id']]['index']);
    $CharNode->appendChild($Parent);
    $CharNode->appendChild($Character);
    $Nodes->appendChild($CharNode);
  }
  // Create a descriptive concept block for modifiers (sush as the unknown (?) flag)
  $DescriptiveConcept = $doc->createElement("DescriptiveConcept");
  $DescriptiveConcept->setAttribute('id', 'dc' . count($matrix_data->columns));
  $DescriptiveConcept->appendChild(_nexus_export_sdd_representation_xml($doc, t('Fixed set of modifiers supported in Scratchpads')));
  $Modifiers = $doc->createElement("Modifiers");
  $Modifier = $doc->createElement("Modifier");
  $Modifier->setAttribute('id', 'mod1');
  // Add 'uncertain' modifier - equivalent to ?
  $Modifier->appendChild(_nexus_export_sdd_representation_xml($doc, t('uncertain')));
  $ModifierClass = $doc->createElement("ModifierClass");
  $ModifierClass->appendChild($doc->createTextNode('Certainty'));
  $Modifier->appendChild($ModifierClass);
  // Need to have a proportion range to work in lucid
  $ProportionRange = $doc->createElement("ProportionRange");
  $ProportionRange->setAttribute('lowerestimate', 0.0);
  $ProportionRange->setAttribute('upperestimate', 0.5);
  $Modifier->appendChild($ProportionRange);
  $Modifiers->appendChild($Modifier);
  $DescriptiveConcept->appendChild($Modifiers);
  $DescriptiveConcepts->appendChild($DescriptiveConcept);
  $Dataset->appendChild($DescriptiveConcepts);
  $Dataset->appendChild($Characters);
  //  Character trees 
  $CharacterTrees = $doc->createElement("CharacterTrees");
  $CharacterTree = $doc->createElement("CharacterTree");
  $CharacterTree->appendChild(_nexus_export_sdd_representation_xml($doc, 'Character list'));
  $CharacterTree->appendChild($Nodes);
  $CharacterTrees->appendChild($CharacterTree);
  $Dataset->appendChild($CharacterTrees);
  // Descriptive concepts section 
  // Coded descriptions - the matrix data
  $CodedDescriptions = $doc->createElement("CodedDescriptions");
  $cd = 0;
  foreach($matrix_data->data as $i => $data){
    $CodedDescription = $doc->createElement("CodedDescription");
    $CodedDescription->setAttribute('id', 'cd' . $cd++);
    $CodedDescription->appendChild(_nexus_export_sdd_representation_xml($doc, $data['taxa']));
    $Scope = $doc->createElement("Scope");
    $TaxonName = $doc->createElement("TaxonName");
    $TaxonName->setAttribute('ref', 't' . $t[$data['id']]);
    $Scope->appendChild($TaxonName);
    $CodedDescription->appendChild($Scope);
    $SummaryData = $doc->createElement("SummaryData");
    foreach($data as $column_id => $state_data){
      if(is_numeric($column_id) && isset($c[$column_id])){
        switch($c[$column_id]['type']){
          case 'free':
            if($state_data){
              $Data = $doc->createElement("TextChar");
              $Content = $doc->createElement("Content");
              $Content->appendChild($doc->createTextNode($state_data));
              $Data->appendChild($Content);
            }
            break;
          case 'quantitative':
            $Data = $doc->createElement("Quantitative");
            if(strpos($state_data, '-')){
              $range = explode('-', $state_data);
              $Data->appendChild(_nexus_export_sdd_measure_xml($doc, 'Min', $range[0]));
              $Data->appendChild(_nexus_export_sdd_measure_xml($doc, 'UMethLower', $range[0]));
              $Data->appendChild(_nexus_export_sdd_measure_xml($doc, 'Max', $range[1]));
              $Data->appendChild(_nexus_export_sdd_measure_xml($doc, 'UMethUpper', $range[1]));
            }else{
              $Data->appendChild(_nexus_export_sdd_measure_xml($doc, 'Min', $state_data));
            }
            break;
          default: // Controlled character
            $Data = $doc->createElement("Categorical");
            if(strpos($state_data, '/')){
              $states = explode('/', $state_data);
            }elseif(strpos($state_data, '+')){
              $states = explode('+', $state_data);
            }else{
              $states = array(
                $state_data
              );
            }
            foreach($states as $individual_state){
              switch($individual_state){
                case '?': // Unknown
                  $column_index = $c[$column_id]['index'];
                  // Need to set ? on every state, so loop through all states in the column
                  foreach(array_keys($matrix_data->columns[$column_index]['states']) as $column_state_key){
                    if(is_numeric($column_state_key)){
                      $State = $doc->createElement("State");
                      $State->setAttribute('ref', 's' . $s[$column_id][$column_state_key]);
                      $Modifier = $doc->createElement("Modifier");
                      $Modifier->setAttribute('ref', 'mod1');
                      $Modifier->setAttribute('debuglabel', 'uncertain');
                      $State->appendChild($Modifier);
                      $Data->appendChild($State);
                    }
                  }
                  break;
                case '-': // Not applicable
                  // Currently we do nothing with these - just leave them blank
                  break;
                default: // A controlled state has been selected
                  $State = $doc->createElement("State");
                  $State->setAttribute('ref', 's' . $s[$column_id][$individual_state]);
                  $Data->appendChild($State);
                  break;
              }
            }
            break;
        }
        $Data->setAttribute('ref', 'c' . $c[$column_id]['index']);
        $SummaryData->appendChild($Data);
      }
    }
    // Only add $SummaryData if there are states - otherwise lucid 3.3 breaks
    if($SummaryData->hasChildNodes()){
      $CodedDescription->appendChild($SummaryData);
    }
    $CodedDescriptions->appendChild($CodedDescription);
  }
  $Dataset->appendChild($CodedDescriptions);
  $Datasets->appendChild($Dataset);
  $doc->appendChild($Datasets);
  return $doc->saveXML();
}

/**
 * Generate a text XML block
 * @param $doc
 * @param $label_text
 * @param $detail_text
 */
function _nexus_export_sdd_representation_xml($doc, $label_text, $detail_text = null){
  $Representation = $doc->createElement("Representation");
  $Label = $doc->createElement("Label");
  $Label->appendChild($doc->createTextNode($label_text));
  $Representation->appendChild($Label);
  if($detail_text){
    $Detail = $doc->createElement("Detail");
    $Detail->appendChild($doc->createTextNode($detail_text));
    $Representation->appendChild($Detail);
  }
  return $Representation;
}

/**
 * Generate a measure XML block
 * @param $doc
 * @param $label_text
 * @param $detail_text
 */
function _nexus_export_sdd_measure_xml($doc, $type, $value){
  if(is_numeric($value)){
    $Measure = $doc->createElement("Measure");
    $Measure->setAttribute('type', $type);
    $Measure->setAttribute('value', $value);
  }
  return $Measure;
}

function nexus_export_nex($node, $matrix_data){
  $groups = array();
  $output = "#NEXUS\r";
  $output .= "\r";
  $output .= "[ File saved by " . variable_get('site_name', 'Drupal') . ", " . format_date(time(), 'custom', 'D M d H:i:s Y') . "]\r";
  $output .= "\r";
  $output .= "BEGIN TAXA;\r";
  $output .= "\tDIMENSIONS NTAX=" . count($node->taxonomy) . ";\r";
  $output .= "\tTAXLABELS\r";
  $taxon_counter = 1;
  foreach($node->taxonomy as $term){
    $output .= "\t\t'" . $term->name . "'\r";
    $taxon[$term->tid] = $taxon_counter;
    $taxon_counter++;
  }
  $output .= "\t;\r";
  $output .= "ENDBLOCK;\r";
  $output .= "\r";
  $func = 'nexus_export_nex_' . $_POST['character_type'] . '_characters';
  if(function_exists($func)){
    $output .= $func($matrix_data, $taxon);
  }
  return $output;
}

function nexus_export_nex_controlled_characters($matrix_data, $taxon){
  $column_count = 0;
  foreach($matrix_data->columns as $i => $column){
    if($column['type'] != 'controlled'){
      continue;
    }
    $column_count++;
    $character_labels .= "\t\t[" . $column_count . "] '" . $column['term'] . "'\r";
    $state_labels .= "\t\t" . $column_count . "\r";
    if($column['description']){
      $character_comments .= "\tTEXT CHARACTER=" . $column_count . " TEXT='" . $column['description'] . "';\r";
    }
    $tid = $matrix_data->data[$i]['id'];
    $nid = $column['id'];
    if($matrix_data->intersection_info[$nid][$tid] && strlen($matrix_data->intersection_info[$nid][$tid]['note']['body'])){
      $attribute_comments .= "\tTEXT TAXON=" . $taxon[$tid] . " CHARACTER=" . $column_count . " TEXT='" . $matrix_data->intersection_info[$nid][$tid]['note']['body'] . "';\r";
    }
    $last_column_key = end(array_keys($column['states']));
    foreach($column['states'] as $state_key => $state){
      if(is_numeric($state_key)){
        $state_labels .= "\t\t\t'" . $state['state'] . "'";
        if($state_key == $last_column_key){
          $state_labels .= ",\r";
        }else{
          $state_labels .= "\r";
        }
      }
    }
    //  Groups
    if(!isset($groups[$column['groupID']])){
      $groups[$column['groupID']] = array(
        'name' => $column['group'],
        'chars' => $column_count
      );
    }else{
      $groups[$column['groupID']]['chars'] .= ' ' . $column_count;
    }
    $indent = ($column_count % 11 == 0);
    // Matrix data
    foreach($matrix_data->data as $data_key => $data){
      if($indent){
        $matrix[$data_key] .= ' ';
      }
      if(isset($data[$column['id']])){
        if(strpos($data[$column['id']], '/')){
          $matrix[$data_key] .= "(" . str_replace('/', '', $data[$column['id']]) . ")";
        }elseif(strpos($data[$column['id']], '+')){
          $matrix[$data_key] .= "{" . str_replace('+', '', $data[$column['id']]) . "}";
        }else{
          $matrix[$data_key] .= $data[$column['id']];
        }
      }else{
        $matrix[$data_key] .= '?';
      }
    }
  }
  $output .= "BEGIN CHARACTERS;\r";
  $output .= "\tDIMENSIONS NCHAR=" . $column_count . ";\r";
  $output .= "\tFORMAT DATATYPE=STANDARD MISSING=? GAP=- SYMBOLS=\"0123456789\";\r";
  $output .= "\tCHARLABELS\r";
  $output .= $character_labels;
  $output .= "\t;\r";
  $output .= "\tSTATELABELS\r";
  $output .= $state_labels;
  $output .= "\t;\r";
  $output .= "\tMATRIX\r";
  foreach($matrix as $id => $data){
    $output .= "\t\t" . str_pad("'" . $matrix_data->data[$id]['taxa'] . "'", 50);
    $output .= $data;
    $output .= "\r";
  }
  $output .= "\t;\r";
  $output .= "ENDBLOCK;\r\r";
  //  Notes
  $output .= "BEGIN NOTES;\r";
  if($character_comments){
    $output .= "\t[Character comments]\r";
    $output .= $character_comments;
    $output .= "\r";
  }
  if($attribute_comments){
    $output .= "\t[Attribute comments]\r";
    $output .= $attribute_comments;
    $output .= "\r";
  }
  $output .= "ENDBLOCK;\r\r";
  // Groups
  $output .= "BEGIN SETS;\r";
  foreach($groups as $group){
    $output .= "\tCHARSET '" . $group['name'] . "' = " . $group['chars'] . ";\r\r";
  }
  $output .= "ENDBLOCK;\r";
  return $output;
}

function nexus_export_nex_dna_characters($matrix_data, $taxon){
  $column_count = 0;
  foreach($matrix_data->columns as $i => $column){
    if($column['type'] != 'dna'){
      continue;
    }
    $column_count++;
    if($column['description']){
      $character_comments .= "\tTEXT CHARACTER=" . $column_count . " TEXT='" . $column['description'] . "';\r";
    }
    $tid = $matrix_data->data[$i]['id'];
    $nid = $column['id'];
    if($matrix_data->intersection_info[$nid][$tid] && strlen($matrix_data->intersection_info[$nid][$tid]['note']['body'])){
      $attribute_comments .= "\tTEXT TAXON=" . $taxon[$tid] . " CHARACTER=" . $column_count . " TEXT='" . $matrix_data->intersection_info[$nid][$tid]['note']['body'] . "';\r";
    }
    $dna_sets = 0;
    foreach($matrix_data->data as $data){
      if(isset($data[$column['id']])){
        $l = strlen($data[$column['id']]);
        if($matrix_data->columns[$i]['max'] < $l){
          $total_chars += ($l - $matrix_data->columns[$i]['max']);
          $matrix_data->columns[$i]['max'] = $l;
        }
      }
      $dna_sets++;
    }
    //  Groups
    if(!isset($groups[$column['groupID']])){
      $groups[$column['groupID']] = array(
        'name' => $column['group'],
        'from' => $groups[$last_group_id]['to'] + 1,
        'to' => $groups[$last_group_id]['to'] + $matrix_data->columns[$i]['max']
      );
    }else{
      $groups[$column['groupID']]['to'] += $matrix_data->columns[$i]['max'];
    }
    $last_group_id = $column['groupID'];
  }
  $output .= "BEGIN CHARACTERS;\r";
  $output .= "\tDIMENSIONS NCHAR=" . $total_chars . ";\r";
  $output .= "\tFORMAT DATATYPE=DNA " . ($dna_sets > 1 ? 'interleave' : '') . " MISSING=? GAP=-;\r";
  $output .= "\tMATRIX\r";
  foreach($matrix_data->columns as $i => $column){
    if($column['max'] < 1){
      continue;
    }
    $output .= "\t\t[" . $column['term'] . "]\r";
    foreach($matrix_data->data as $data){
      $output .= "\t\t" . str_pad("'" . $data['taxa'] . "'", 50);
      $output .= str_pad($data[$column['id']], $column['max'], '-');
      $output .= "\r";
    }
    $output .= "\r";
  }
  $output .= "\t;\r";
  $output .= "ENDBLOCK;\r\r";
  //  Notes
  $output .= "BEGIN NOTES;\r";
  if($character_comments){
    $output .= "\t[Character comments]\r";
    $output .= $character_comments;
    $output .= "\r";
  }
  if($attribute_comments){
    $output .= "\t[Attribute comments]\r";
    $output .= $attribute_comments;
    $output .= "\r";
  }
  $output .= "ENDBLOCK;\r\r";
  // Groups
  $output .= "BEGIN SETS;\r";
  foreach($groups as $group){
    if($group['from'] == $group['to']){
      $output .= "\tCHARSET '" . $group['name'] . "' = " . $group['from'] . ";\r\r";
    }else{
      $output .= "\tCHARSET '" . $group['name'] . "' = " . $group['from'] . "-" . $group['to'] . ";\r\r";
    }
  }
  $output .= "ENDBLOCK;\r";
  return $output;
  return $output;
}

/**
 * Output characters in RTF format
 * @param $node
 * @param $matrix_data
 */
function nexus_export_rtf($node, $matrix_data){
  $output = '{\rtf1
{\fonttbl
{\f0 Times New Roman;}
}
\f0\fs35 Characters\par';
  $column_count = 1;
  foreach($matrix_data->columns as $column){
    $output .= '\f0\fs25 ' . $column_count . '. \i ' . $column['term'] . ' \plain:';
    foreach($column['states'] as $state_key => $state){
      if(is_numeric($state_key)){
        $output .= ' \b (' . $state_key . ') \plain ' . $state['state'];
      }
    }
    $output .= '\par ';
    if($column['description']){
      $output .= $column['description'] . '\par ';
    }
    $column_count++;
  }
  $output .= '}';
  return $output;
}

/**
 * Output characters in RTF format
 * @param $node
 * @param $matrix_data
 */
function nexus_export_lif3($node, $matrix_data){
  $taxa = array(); // array of all taxon items
  $groups = array(); // array of all groups
  $character_states = array(); 
  $doc = new DomDocument('1.0', 'UTF-8');
  $doc->preserveWhiteSpace = false;
  $doc->formatOutput = true;
  // Create lif3_key - root element 
  $lif3_key = $doc->createElement("lif3_key");
  // Create properties block - metadata 
  $properties = $doc->createElement("properties");
  $property = $doc->createElement("property");
  $property->setAttribute('key', 'KeyVersion');
  $property->setAttribute('type', 'java.lang.Float');
  $property->setAttribute('value', 1.2);
  $properties->appendChild($property);
  // Character project title
  $property = $doc->createElement("property");
  $property->setAttribute('key', 'key_title');
  $property->setAttribute('type', 'java.lang.String');
  $property->setAttribute('value', $node->title);
  $properties->appendChild($property);
  $lif3_key->appendChild($properties);
  // define items - all the taxon names, characters and states
  $items = $doc->createElement("items");
  // Create parent item for the tree structures
  $trees = $doc->createElement("trees");
  // Create a tree structure for the features
  $features_tree = $doc->createElement("tree");
  $features_tree->setAttribute('type', 'feature');
  $item_id = 2; // item IDs start from 2, no idea why
  foreach($matrix_data->columns as $column){
    // Create group if it doesn't already exists
    if(!isset($groups[$column['groupID']])){
      $item = $doc->createElement("item");
      $item->setAttribute('item_id', $item_id);
      $item->setAttribute('name', $column['group']);
      $item->setAttribute('item_type', 'feature');
      $item->setAttribute('revision', 'false');
      $item->setAttribute('weight', 1.0);
      $item->setAttribute('score_type', 'normal');
      // Load the term for the group - we want to use the description
      $group_term = taxonomy_get_term($column['groupID']);
      if($group_term->description){
        $item->setAttribute('comment', $group_term->description);
      }
      $items->appendChild($item);
      // Create a treenode for the group
      $treenode = $doc->createElement("treenode");
      $treenode->setAttribute('item_id', $item_id);
      $treenode->setAttribute('parent_id', 0);
      $features_tree->appendChild($treenode);
      // Store this group in the groups array, so we know if it's been created already
      $groups[$column['groupID']] = $item_id;
      $item_id++;
    }
    switch($column['type']){
      case 'controlled':
        $score_type = 'normal';
        break;
      case 'quantitative':
        $score_type = 'numeric';
        break;
      case 'free': // Textual characters aren't supported in lucid
        continue (2); // switch is a looping structure, so continue up another level
        break;
    }
    $item = $doc->createElement("item");
    $item->setAttribute('item_id', $item_id);
    $item->setAttribute('name', $column['term']);
    // Add comments if there's any present
    if($column['description']){
      $item->setAttribute('comments', $column['description']);
    }
    $item->setAttribute('item_type', 'feature');
    $item->setAttribute('revision', 'false');
    $item->setAttribute('weight', 1.0);
    $item->setAttribute('score_type', $score_type);
    $items->appendChild($item);
    $character_states[$column['id']] = array(
      'item_id' => $item_id,
      'type' => $column['type']
    );
    // Create a treenode for this item
    $treenode = $doc->createElement("treenode");
    $treenode->setAttribute('item_id', $item_id);
    $treenode->setAttribute('parent_id', $groups[$column['groupID']]);
    $features_tree->appendChild($treenode);
    // If this is a controlled character, create the states
    if(count($column['states'])){
      $parent_id = $item_id;
      foreach($column['states'] as $state_index => $state){
        if(is_numeric($state['nid'])){ // Ignore the ? & - characters
          $item_id++;
          // Create an item for the state
          $item = $doc->createElement("item");
          $item->setAttribute('item_id', $item_id);
          $item->setAttribute('name', $state['state']);
          $item->setAttribute('list_view_name', $state['state']);
          $item->setAttribute('item_type', 'state');
          $item->setAttribute('revision', 'false');
          $item->setAttribute('score_type', 'normal');
          $items->appendChild($item);
          $character_states[$column['id']]['states'][$state_index] = $item_id;
          // Create a treenode for the new item
          $treenode = $doc->createElement("treenode");
          $treenode->setAttribute('item_id', $item_id);
          $treenode->setAttribute('parent_id', $parent_id);
          $features_tree->appendChild($treenode);
        }
      }
    }
    $item_id++;
  }
  // Add the features tree to the trees parent item
  $trees->appendChild($features_tree);
  // Create a new tree structure for the entities
  $entities_tree = $doc->createElement("tree");
  $entities_tree->setAttribute('type', 'entity');
  // Arrays to store the scoring items
  $controlled_scoring_items = array();
  $quantitative_scoring_items = array();
  foreach($matrix_data->data as $key => $data){
    // Create taxon name items
    $item = $doc->createElement("item");
    $item->setAttribute('item_id', $item_id);
    $item->setAttribute('name', $data['taxa']);
    $item->setAttribute('item_type', 'entity');
    $item->setAttribute('revision', 'false');
    $item->setAttribute('score_type', 'normal');
    $item->setAttribute('is_list_view_node', 'true');
    $taxa[$key] = $item_id;
    $items->appendChild($item);
    // Add item to the entities tree structure
    if(isset($data['parent'])){
      $parent_id = $taxa[$data['parent']];
    }else{
      $parent_id = 0;
    }
    $treenode = $doc->createElement("treenode");
    $treenode->setAttribute('item_id', $item_id);
    $treenode->setAttribute('parent_id', $parent_id);
    $entities_tree->appendChild($treenode);
    foreach(array_filter(array_keys($data), 'is_numeric') as $character_nid){
      // Values are added as children of the states
      switch($character_states[$character_nid]['type']){
        case 'controlled':
          // Controlled state values are attached as children of the state
          if(strpos($data[$character_nid], '/') !== false){
            $state_values = explode('/', $data[$character_nid]);
          }elseif(strpos($data[$character_nid], '+') !== false){
            $state_values = explode('+', $data[$character_nid]);
          }else{
            $state_values = array(
              $data[$character_nid]
            );
          }
          foreach($state_values as $state_value){
            $state_item_id = $character_states[$character_nid]['states'][$state_value];
            if(!isset($controlled_scoring_items[$state_item_id])){
              $scoring_item = $doc->createElement("scoring_item");
              $scoring_item->setAttribute('item_id', $state_item_id);
              $controlled_scoring_items[$state_item_id] = $scoring_item;
            }
            $scored_item = $doc->createElement("scored_item");
            $scored_item->setAttribute('item_id', $item_id);
            $scored_item->setAttribute('value', 1);
            $controlled_scoring_items[$state_item_id]->appendChild($scored_item);
          }
          break;
        case 'quantitative':
          // Numeric state values are attached as children of the character
          if(!isset($quantitative_scoring_items[$character_nid])){
            $scoring_item = $doc->createElement("scoring_item");
            $scoring_item->setAttribute('item_id', $character_states[$character_nid]['item_id']);
            $quantitative_scoring_items[$character_nid] = $scoring_item;
          }
          $scored_item = $doc->createElement("scored_item");
          $scored_item->setAttribute('item_id', $item_id);
          $scored_item->setAttribute('units', '');
          $scored_data = $doc->createElement("scored_data");
          $scored_data->setAttribute('row_score_type', 1);
          $scored_data->setAttribute('omin', $data[$character_nid]);
          $scored_data->setAttribute('nmin', $data[$character_nid]);
          $scored_data->setAttribute('nmax', $data[$character_nid]);
          $scored_data->setAttribute('omax', $data[$character_nid]);
          $scored_item->appendChild($scored_data);
          $quantitative_scoring_items[$character_nid]->appendChild($scored_item);
          break;
      }
    }
    $item_id++;
  }
  // Add this tree to the trees parent item
  $trees->appendChild($entities_tree);
  // Add all items to the document root
  $lif3_key->appendChild($items);
  // Add all trees to the document root
  $lif3_key->appendChild($trees);
  // Add the container for 'normal' (controlled) scoring items
  if(count($controlled_scoring_items)){
    $controlled_container = $doc->createElement("container");
    $controlled_container->setAttribute('type', 'normal');
    foreach($controlled_scoring_items as $scoring_item){
      $controlled_container->appendChild($scoring_item);
    }
  }
  //Add the controlled character container 
  $lif3_key->appendChild($controlled_container);
  if(count($quantitative_scoring_items)){
    $quantitative_container = $doc->createElement("container");
    $quantitative_container->setAttribute('type', 'numeric');
    foreach($quantitative_scoring_items as $scoring_item){
      $quantitative_container->appendChild($scoring_item);
    }
  }
  //Add the controlled character container 
  $lif3_key->appendChild($quantitative_container);
  // Add root element to the xml document and return it
  $doc->appendChild($lif3_key);
  return $doc->saveXML();
}
