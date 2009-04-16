<?php

require_once("classification_functions.class");
require_once("classification.help.inc");
global $names;
$names = new Names;

/**
 * Form to display double tree using jsTree: http://code.google.com/p/jstree/
 * jsTree 0.9.1
 *
 * Copyright (c) 2008 Ivan Bozhanov (vakata.com)
 *
 * Dual licensed under the MIT and GPL licenses:
 *   http://www.opensource.org/licenses/mit-license.php
 *   http://www.gnu.org/licenses/gpl.html
 *
 * Date: 2008-10-09
 * 
 * Placed as is in a folder called jsTree within this classification module along with some modifications
 * see comments within tree_component.js
 */
function classification_edit_form(&$form_state,$tid='') {

  $vid = classification_get_vid('Taxa');

  $form = array(
    '#cache' => TRUE,
  );
  
  // Find out if there is at least one term in the default, Taxa vocabulary
  $term = db_fetch_object(db_query("SELECT tid FROM term_data WHERE vid = %d LIMIT 1", $vid));
  
  if(!$term->tid) {
  	$message = t('Sorry, before you can edit your primary classification, you must first import your names.');
  	$form['message'] = array(
  	  '#type' => 'markup',
  	  '#value' => '<div class="messages error">' . $message . '</div>',
  	);
  	return $form;
  }
  
  global $module_path;
  $module_path = drupal_get_path('module', 'classification') .'/';
  
  drupal_add_css($module_path .'css/classification.css');
  drupal_add_js($module_path .'jsTree/css.js');
  drupal_add_js($module_path .'jsTree/jquery.listen.js');
  drupal_add_js($module_path .'jsTree/tree_component.js');
  drupal_add_js($module_path .'jsTree/jquery.metadata.js');
  drupal_add_js($module_path .'jsTree/jquery.cookie.js');
  drupal_add_js($module_path .'jsTree/jquery.hotkeys.js');

  drupal_add_js($module_path .'js/jquery.jdMenu/jquery.dimensions.js');
  drupal_add_js($module_path .'js/jquery.jdMenu/jquery.positionBy.js');
  drupal_add_js($module_path .'js/jquery.jdMenu/jquery.bgiframe.js');
  drupal_add_js($module_path .'js/jquery.jdMenu/jquery.jdMenu.js');
  drupal_add_js($module_path .'js/jquery.vcenter.js');
  drupal_add_js($module_path .'js/jquery.scrollTo.js');
  
  drupal_add_js($module_path . 'js/classification_editing.js');
  drupal_add_js($module_path .'js/classification.js');

  if($tid) {
    drupal_add_js(array('classification_default' => '#n' . $tid), 'setting');	
  }
  
  $form['status_bar'] = array(
    '#type' => 'markup',
    '#value' => '<div id="message"></div>',
  );

  $shortname = variable_get('shortname','');

  $tree_menu = '
    <ul id="tree_actions" class="jd_menu">
      <li class="mainmenu accessible"><a href="#">' . t('Edit') . '</a>
        <ul>
          <li class="new"><a href="#" onclick="javascript:TREE.create();return false">' . t('New Child') . '</a></li>
          <li class="edit"><a href="#" onclick="javascript:TREE.rename();return false">' . t('Rename') . '</a></li>
          <li class="cut"><a href="#" onclick="javascript:TREE.cut();return false">' . t('Cut') . '</a></li>
          <li class="paste"><a href="#" onclick="javascript:TREE.paste();return false">' . t('Paste') . '</a></li>
          <li class="delete separator"><a href="#" onclick="javascript:TREE.remove();return false">' . t('Delete') . '</a></li>
        </ul>
      </li>
      <li class="mainmenu accessible"><a href="#">' . t('View') . '</a>
        <ul>
          <li class="filter"><a href="#" onclick="javascript:EDIT.filter();return false">' . t('Filter') . '</a></li>
          <li class="remove_filter"><a href="#" onclick="javascript:EDIT.remove_filter();return false">' . t('No Filter') . '</a></li>
          <li class="refresh separator"><a href="#" onclick="javascript:TREE.refresh();return false">' . t('Refresh') . '</a></li>
          <li class="close"><a href="#" onclick="javascript:TREE.close_all();return false">' . t('Collapse') . '</a></li>
        </ul>
      </li>
    </ul>';
      
  $form['tree_wrapper'] = array(
    '#type' => 'markup',
    '#value' => ' ',
    '#prefix' => '<div class="tree_wrapper"><div id="tree_actions_bar">' . $tree_menu . '</div>',
    '#suffix' => '</div>',
  );
  
  $form['tree_wrapper']['root'] = array(
    '#type' => 'textfield',
    '#autocomplete_path' => 'classification/autocomplete/' . $vid,
    '#default_value' => '',
    '#size' => 40,
    '#title' => t('Parent Taxon'),
    '#prefix' => '<div id="reroot"><a href="#" onclick="javascript:EDIT.close_filter(); return false;"><img style="float:right" src="/' . $module_path . 'images/cross.png" title="' . t('Close') . '"></a>',
    '#suffix' => '<div id="edit-root-save"><a href="#" onclick="javascript:EDIT.apply_filter(); return false;"><img src="/' . $module_path . 'images/chart_organisation_add.png" title="' . t('Apply Filter') . '"></a> <a href="#" onclick="javascript:EDIT.remove_filter(); return false;"><img src="/' . $module_path . 'images/chart_organisation_delete.png" title="' . t('Remove Filter') . '"></a></div>',
    '#ahah' => array(
     	'event' => 'blur',
	    'path' => 'classification/checkterm/' . $vid,
	    'wrapper' => 'reroot_tid',
	    'progress' => 'none',
    ),
  );
  
  $form['tree_wrapper']['root_tid'] = array(
    '#type' => 'markup',
    '#value' => '<div id="reroot_tid"></div>',
    '#suffix' => '</div>',
  );
    
    
  $form['tree_wrapper']['tree'] = array(
    '#type' => 'markup',
    '#value' => '<div id="classification_tree"></div>',
  );

  $form['tabs_opener'] = array(
     '#type' => 'markup',
     '#value' => ' ',
     '#prefix' => '<div class="tree_wrapper"><div id="tabs-wrapper" class="classification_tabs"><ul class="tabs primary classification_tabs"><li class="active"><a href="#" onclick="javascript:EDIT.tab_selector(1);return false;">Name Data</a></li><li><a href="#" onclick="javascript:EDIT.tab_selector(2);return false;">Other Classifications</a></li><li><a href="#" onclick="javascript:EDIT.tab_selector(3);return false;">Settings</a></li></ul></div>',
  );

  $form['alternate_wrapper_opener'] = array(
    '#type' => 'markup',
    '#value' => '<div id="alternate_wrapper">',
  );

  $form += classification_metadata_form();
  $form += classification_additional_trees_form();
  $form += classification_settings_form();
  
  $form['alternate_wrapper_closer'] = array(
    '#type' => 'markup',
    '#value' => '</div>',
  );
  
  $form['tabs_closer'] = array(
     '#type' => 'markup',
     '#value' => ' ',
     '#suffix' => '</div><div class="clear"></div>',
  );

return $form;
}

/**
 * Form for handling metadata for names
 */
function classification_metadata_form() {
  	
  $form['name_metadata'] = array(
     '#type' => 'markup',
     '#value' => '',
     '#prefix' => '<div class="alternate_content" id="classification_name_metadata">',
     '#suffix' => '</div>',
  );

  $form['name_metadata']['tid'] = array(
     '#type' => 'hidden',
     '#value' => '',
  );

  $form['name_metadata']['parent_tid'] = array(
     '#type' => 'hidden',
     '#value' => '',
  );
  
  $form['name_metadata']['name'] = array(
     '#type' => 'markup',
     '#value' => '<div id="metadata_taxon_title"></div>',
  );

  $form['name_metadata']['rank'] = array(
    '#type' => 'select',
    '#options' => classification_allowed_values('rank'),
    '#title' => t('Taxonomic Rank'),
    '#prefix' => '<div style="width:60%;float:left">',
    '#suffix' => '</div>',
    '#description' => t('Leave unselected if not applicable'),
  );

  $action_menu = '
    <ul id="metadata_actions" class="metadata_actions">
      <li class="classification_images">' . l(t('Images'), '') . ' <span></span></li>
      <li class="classification_references">' . l(t('References'), '') . ' <span></span></li>
      <li class="classification_view">' . l(t('Description'),'') . '<span></span></li>
    </ul>';
      
  $form['name_metadata']['action_menu'] = array(
    '#type' => 'markup',
    '#value' => $action_menu,
    '#prefix' => '<div style="width:40%;float:right">',
    '#suffix' => '</div><div style="clear:both"></div>',
  );
  
  $form['name_metadata']['relation_id'] = array(
    '#type' => 'hidden',
    '#value' => '',
  );

  $sql = db_query("SELECT rtid, name FROM {term_relation_types} ORDER BY rtid");
  $relationships[''] = 'taxonomic child';
    while ($row = db_fetch_object($sql)) {
     $relationships[$row->rtid] = check_plain($row->name);
    }
  	
  $form['name_metadata']['relation_type'] = array(
    '#type' => 'select',
    '#options' => $relationships,
    '#attributes' => array('onchange' => 'javascript:EDIT.check_vern(this.value)'),
  	'#title' => t('Relation to ') . '<span id="metadata_parent_name"></span>',
  	'#description' => t('Leave unselected if not applicable. Taxonomic Rank must be set to "None".'),
  );

  $form['name_metadata']['vern_lang'] = array(
    '#type' => 'select',
    '#options' => classification_allowed_values('vernacular_language'),
    '#title' => t('Vernacular Language'),
    '#description' => t('Leave unselected if not applicable'),
  );

  $form['name_metadata']['notes'] = array(
	'#type' => 'fieldset',
	'#title' => t('Taxonomic Notes'),
	'#collapsible' => TRUE,
	'#collapsed' => TRUE,
  );

  $form['name_metadata']['notes']['description'] = array(
    '#type' => 'textarea',
    '#cols' => 30,
  );

  $form['name_metadata']['button'] = array(
    '#type' => 'markup',
    '#value' => '<div style="width:200px; margin:0px auto;text-align:center"><button class="classification-buttons save" onclick="javascript:EDIT.save_metadata();return false">' . t('Save') . '</button></div>',
  );

// Dummy submit button with AHAH to circumvent the Enter key from re-rendering the page
  $form['name_metadata']['dummy_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#attributes' => array('class' => 'classification-buttons hidden'),
    '#ahah' => array(
       'event' => 'click',
       'path' => 'classification/update_metadata',
       'wrapper' => 'message',
       'progress' => array('type' => 'none', 'message' => ''),
    ),
  );
 
  return $form;
}

/**
 * Allowed values from taxonomy_enhancer_fields table
 */
function classification_allowed_values($field) {
  $vid = classification_get_vid('Taxa');
  $result = db_fetch_object(db_query("SELECT settings FROM {taxonomy_enhancer_fields} WHERE fid = '%s' and vid = %d", 'field_' . strtolower($field), $vid));
  $settings = drupal_unpack($result,'settings');
  
  $values = $settings->allowed_values;
  $allowed_values[''] = '--None--';
  foreach ($values as $value) {
  	$allowed_values[$value] = $value;
  }
  
  return $allowed_values;
}

/**
 * Form for handling additional classifications
 */
function classification_additional_trees_form() {

  $form['trees'] = array(
     '#type' => 'markup',
     '#value' => ' ',
     '#prefix' => '<div class="alternate_content" id="classification_alternate_trees">',
     '#suffix' => '</div>',
  );
  
  $classifications = taxonomy_get_vocabularies();
  $classification_list[0] = 'None';
  	foreach($classifications as $classification) {
  		 if (preg_match("/alternate_/i", $classification->name)) {
  		 	 $classification_list['' . $classification->vid . ''] = str_replace("alternate_","",check_plain($classification->name));
  	   }
    }

  $form['trees']['selections'] = array(
     '#type' => 'select',
     '#title' => t('Select other classifications'),
     '#options' => $classification_list,
     '#default_value' => 0,
     '#description' => t('Source classification(s) from which you may drag names'),
     '#attributes' => array('onchange' => 'javascript:EDIT.get_tree(this.value)'),
  );
  
  $form['trees']['classification_trees'] = array(
     '#type' => 'markup',
     '#value' => ' ',
     '#prefix' => '<div id="classification_tree_alternate">',
     '#suffix' => '</div>',
  );
  
  return $form;
}

/**
 * Get options for the settings form
 */
function classification_allowed_options($field) {
  if ($field != "rank")
  {
  $vid = classification_get_vid('Taxa');
  $result = db_fetch_object(db_query("SELECT settings FROM {taxonomy_enhancer_fields} WHERE fid = '%s' and vid = %d", 'field_' . strtolower($field), $vid));
  if($result) 
    {
    $settings = drupal_unpack($result,'settings');
    $allowed = $settings->allowed_values;
    }
  }
  else
  {
  $result = db_query("SELECT value FROM {taxonomy_enhancer_value_text} WHERE fid = 'field_rank'");
  $all_ranks = array();
  while($row = db_fetch_object($result))
	{
	$all_ranks[] = $row->value;
	}
  $allowed = array_unique($all_ranks);
  }
  foreach ($allowed as $value) 
          {
  	  $_allowed .= $value . "\n";
          }
return trim($_allowed);
}

/**
 * Form for handling settings
 */
function classification_settings_form() {
	
	global $module_path;
  
  $form['settings'] = array(
     '#type' => 'markup',
     '#value' => '',
     '#prefix' => '<div class="alternate_content" id="classification_settings">',
     '#suffix' => '</div>',
  );

  $form['settings']['ranks'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomic Ranks'),
  );	
  
  $form['settings']['ranks']['ranks_list'] = array(
    '#type' => 'textarea',
    '#default_value' => classification_allowed_options('rank'),
    '#title' => t('One taxonomic rank per line (in order of appearance)'),
    '#cols' => 30,
    '#required' => TRUE,
  );

 $form['settings']['show_rank_help'] = array(
    '#prefix' => '',
    '#type' => 'fieldset',
    '#title' => t('Recommended rank names (in hierarchical order) '),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['settings']['show_rank_help']['show_rank_table'] = array(
    '#value' => '<p>'.classification_rank_table(classification_rank_list()).'</p>',
  );

  $form['settings']['verns'] = array(
    '#type' => 'fieldset',
    '#title' => t('Vernacular Languages'),
  );
  
  $form['settings']['verns']['vern_list'] = array(
    '#type' => 'textarea',
    '#default_value' => classification_allowed_options('vernacular_language'),
    '#title' => t('One language (<a href="http://www.loc.gov/standards/iso639-2/php/English_list.php" target="_blank">ISO 639-1</a>) per line'),
    '#cols' => 30,
    '#required' => TRUE,
  );
  
  $form['settings']['names_display_editor'] = array(
    '#type' => 'fieldset',
    '#title' => t('Classification Editor Display Options'),
  );
  
  $options = array(0 => t('Show all names (vernaculars, synonyms, <em>etc.</em>)'), 1 => t('Show valid names and synonyms (hide vernaculars)'), 2 => t('Only show valid names'));
  $default_editor = variable_get('classification_editor_names',0);
  
  $form['settings']['names_display_editor']['display_options_editor'] = array(
    '#type' => 'radios',
    '#title' => t('Visibility of names in the private classification editor'),
    '#options' => $options,
    '#default_value' => $default_editor,
  );
  
  $form['settings']['settings_submit'] = array(
    '#type' => 'markup',
    '#value' => '<div style="width:200px; margin:0px auto;text-align:center"><button class="classification-buttons save" onclick="javascript:EDIT.update_settings();return false">' . t('Update') . '</button></div>',
  );
  
  return $form;
}

/**
 * Callback for rendering tree
 */
 function classification_js_tree($tid='') {
 	   drupal_set_header('Content-Type: text/plain; charset=utf-8');   
     $matches = array();
     $vid = classification_get_vid('Taxa');
 	  
     if(!$tid) {
        $tid = 0;
     }
     else {
        $tid = str_replace("n", "",$tid);
     }
 
     $names_options = variable_get('classification_editor_names',0);
     
     switch ($names_options) {
     	 // Show all names
     	 case 0:
         $result = db_query(db_rewrite_sql("
         SELECT 
           td.tid as tid, 
           td.name as name, 
           trt.name as relation, 
           trt.rtid as relation_type
         FROM
           {term_data} td
         JOIN 
           {term_hierarchy} th on (td.tid=th.tid) 
         LEFT JOIN 
         	 ({term_relation} tr 
         	    JOIN {term_relation_has_type} trht ON (tr.trid=trht.trid) 
         	    JOIN {term_relation_types} trt ON (trht.rtid=trt.rtid)
         	  ) ON td.tid=tr.tid1 
		     WHERE 
		       td.vid = %d AND th.parent = %d 
		     ORDER BY 
		       trt.rtid, td.name"),$vid,$tid);
         break;
       // Hide vernaculars
       case 1:
         $result = db_query(db_rewrite_sql("
         SELECT 
           td.tid as tid, 
           td.name as name, 
           trt.name as relation, 
           trt.rtid as relation_type
         FROM
           {term_data} td
         JOIN 
           {term_hierarchy} th on (td.tid=th.tid) 
         LEFT JOIN 
         	 ( {term_relation} tr 
			       JOIN {term_relation_has_type} trht on (tr.trid=trht.trid) 
			       JOIN {term_relation_types} trt on (trht.rtid=trt.rtid)
		       )  ON td.tid=tr.tid1 
		     WHERE 
		       td.vid = %d AND th.parent = %d AND ( trt.rtid <> 9 OR trt.rtid IS NULL ) 
		     ORDER BY td.name"),$vid,$tid);
         break;
       // Hide all except valid
       case 2:
         $result = db_query(db_rewrite_sql("
         SELECT 
           td.tid as tid, 
           td.name as name, 
           trt.name as relation, 
           trt.rtid as relation_type
         FROM
           {term_data} td
         JOIN 
           {term_hierarchy} th on (td.tid=th.tid) 
         LEFT JOIN 
         	 ( {term_relation} tr 
			       JOIN {term_relation_has_type} trht on (tr.trid=trht.trid) 
			       JOIN {term_relation_types} trt on (trht.rtid=trt.rtid)
		       ) ON td.tid=tr.tid1 
		     WHERE 
		       td.vid = %d AND th.parent = %d AND trt.rtid IS NULL 
		     ORDER BY 
		       td.name"),$vid,$tid);
         break;
       // Show all names
       default:
         $result = db_query(db_rewrite_sql("
         SELECT 
           td.tid as tid, 
           td.name as name, 
           trt.name as relation, 
           trt.rtid as relation_type
         FROM
           {term_data} td
         JOIN 
           {term_hierarchy} th on (td.tid=th.tid) 
         LEFT JOIN 
         	 ( {term_relation} tr 
			       JOIN {term_relation_has_type} trht on (tr.trid=trht.trid) 
			       JOIN {term_relation_types} trt on (trht.rtid=trt.rtid)
		       ) ON td.tid=tr.tid1 
		     WHERE 
		       td.vid = %d AND th.parent = %d 
		     ORDER BY 
		       trt.rtid, td.name"),$vid,$tid);        
     }

     while ($object = db_fetch_object($result)) {
       $tid = $object->tid;
       $name = $object->name;
       $patterns = array('/name/', '/ = homotypic/', '/ = heterotypic/');
       $relation = trim(preg_replace($patterns,'',$object->relation));
       $matches[] = array('name' => check_plain($name), 'tid' => $tid, 'relation' => $relation);
     }

     echo classification_to_js($matches);
}

/**
 * Callback for rendering alternate tree
 */
 function classification_js_tree_alternate($vid,$tid='') {
 	   drupal_set_header('Content-Type: text/plain; charset=utf-8');   
     $matches = array();
 	  
     if(!$tid) {
        $tid = 0;
     }
     else {
        $tid = str_replace("n", "",$tid);
     }
     
     $result = db_query(db_rewrite_sql("
         SELECT 
           td.tid as tid, 
           td.name as name, 
           trt.name as relation, 
           trt.rtid as relation_type
         FROM
           {term_data} td
         JOIN 
           {term_hierarchy} th on (td.tid=th.tid) 
         LEFT JOIN 
         	 ( {term_relation} tr 
			       JOIN {term_relation_has_type} trht on (tr.trid=trht.trid) 
			       JOIN {term_relation_types} trt on (trht.rtid=trt.rtid)
		       ) ON td.tid=tr.tid1 
		     WHERE 
		       td.vid = %d AND th.parent = %d 
		     ORDER BY 
		       td.name"),$vid,$tid);

     while ($object = db_fetch_object($result)) {
       $tid = $object->tid;
       $name = $object->name;
       $patterns = array('/name/', '/ = homotypic/', '/ = heterotypic/');
       $relation = trim(preg_replace($patterns,'',$object->relation));
       $matches[] = array('name' => check_plain($name), 'tid' => $tid, 'relation' => $relation);
     }

     echo classification_to_js($matches,$vid);
}

/**
 * Build some json for the tree renderer
 */
function classification_to_js($var,$vid='') {
   if(!$var) return;
   $str = '';
   if(count($var) > 1) {
      $str .= "[" . "\n";
   }
   foreach($var as $parent) {
      $str .= "{" . "\n";
      $str .= "\"attributes\" : { \"id\" : \"n" . $parent['tid'] . "\"";
      if($parent['relation']) {
      	$str .= ", \"class\" : \"" . $parent['relation'] . "\"";
      }
      $str .= " }," . "\n";
      $str .= "\"data\" : \"" . $parent['name'] . "\"";

      if ($vid) {
         $children = classification_count_children_alternate($parent['tid'],$vid);
      }
      else {
      	 $children = classification_count_children($parent['tid']);
      }
      
      if($children > 0){
         $str .= "," . "\n" . "\"state\" : \"closed\"" . "\n";
      }
      else {
      	 $str .= "\n";
      }
      
      $str .= "}";
      $counter++;
      if($counter !== count($var)){
         $str .= ","  . "\n";
      }
   }
   if(count($var) > 1) {
      $str .= "\n" . "]" . "\n";
   }

   return $str;
}

/**
 * Helper function to determine if term has children
 */
function classification_count_children($tid) {
   $vid = classification_get_vid('Taxa');
   
   $names_options = variable_get('classification_editor_names',0);
   
   switch ($names_options) {
   	 // Show all names
   	 case 0:
       $result = db_query(db_rewrite_sql("
       SELECT 
         COUNT(*) AS count 
       FROM 
         {term_data} td 
       JOIN 
         {term_hierarchy} th ON (td.tid = th.tid) 
       WHERE 
         td.vid = %d AND th.parent = %d"), $vid,$tid);
       break;
     // Hide vernaculars
     case 1:
       $result = db_query(db_rewrite_sql("
         SELECT 
           COUNT(*) as count
         FROM
           {term_data} td
         JOIN 
           {term_hierarchy} th on (td.tid=th.tid) 
         LEFT JOIN 
         	 ({term_relation} tr 
         	    JOIN {term_relation_has_type} trht ON (tr.trid=trht.trid) 
         	    JOIN {term_relation_types} trt ON (trht.rtid=trt.rtid)
         	  ) ON td.tid=tr.tid1 
		     WHERE 
		       td.vid = %d AND th.parent = %d AND ( trt.rtid <> 9 OR trt.rtid IS NULL )"),$vid,$tid);
       break;
     // Hide all except valid
     case 2:
       $result = db_query(db_rewrite_sql("
       SELECT 
         COUNT(*) AS count
       FROM
         {term_data} td
       JOIN 
         {term_hierarchy} th on (td.tid=th.tid) 
       LEFT JOIN 
         ( {term_relation} tr 
			     JOIN {term_relation_has_type} trht on (tr.trid=trht.trid) 
			     JOIN {term_relation_types} trt on (trht.rtid=trt.rtid)
		     ) ON td.tid=tr.tid1 
       WHERE 
         td.vid = %d AND th.parent = %d AND trt.rtid IS NULL"), $vid,$tid);
       break;
     // Show all names by default
     default:
       $result = db_query(db_rewrite_sql("
       SELECT 
         COUNT(*) AS count 
       FROM 
         {term_data} td 
       JOIN 
         {term_hierarchy} th ON (td.tid=th.tid) 
       WHERE 
         td.vid = %d AND th.parent = %d"), $vid,$tid);
   }
   
   $object = db_fetch_object($result);
   return $object->count;
}

/**
 * Helper function to determine if term in alternate classification has children
 */
function classification_count_children_alternate($tid,$vid) {
   
       $result = db_query(db_rewrite_sql("
       SELECT 
         COUNT(*) AS count 
       FROM 
         {term_data} td 
       JOIN 
         {term_hierarchy} th ON (td.tid=th.tid) 
       WHERE 
         td.vid = %d AND th.parent = %d"), $vid,$tid);
   
   $object = db_fetch_object($result);
   return $object->count;
}

/**
 * Callback for autocomplete
 */
function classification_autocomplete($vid,$string='') {
	global $names;
  $regexp = '%(?:^|,\ *)("(?>[^"]*)(?>""[^"]* )*"|(?: [^",]*))%x';
  preg_match_all($regexp, $string, $matches);
  $array = $matches[1];

  $last_string = trim(array_pop($array));
  $matches = array();
  $classification_autocomplete = variable_get('taxon_description_autocomplete_size',3);
  if (strlen($last_string) >= $classification_autocomplete ) {
    $result = db_query_range(db_rewrite_sql("
    SELECT 
      a.tid, a.name 
    FROM 
      {term_data} a
    INNER JOIN 
      {taxonomy_enhancer_value_text} b ON a.tid = b.tid 
    WHERE 
      a.vid = %d AND b.fid = 'field_rank' AND b.value <> 'Species' AND  b.value <> 'Subspecies' AND LENGTH(b.value)>1 AND LOWER(a.name) LIKE LOWER('%%%s%%') ORDER BY a.name", 't', 'tid'), $vid, $last_string, 0, 10);

    while ($tag = db_fetch_object($result)) {
      $n = $tag->name;

    $matches[$n] = check_plain($names->canonical_form($tag->name));
    }
  }
  classification_autocomplete_to_json($matches);
}

function classification_autocomplete_to_json($var = NULL) {
  drupal_set_header('Content-Type: text/javascript; charset=utf-8');
  if (isset($var)) {
    echo classification_autocomplete_to_js($var);
  }
}

function classification_autocomplete_to_js($var) {
  switch (gettype($var)) {
    case 'boolean':
      return $var ? 'true' : 'false'; // Lowercase necessary!
    case 'integer':
    case 'double':
      return $var;
    case 'resource':
    case 'string':
      return '"'. str_replace(array("\r", "\n", "<", ">", "&"),
                              array('\r', '\n', '\x3c', '\x3e', '\x26'),
                              addslashes($var)) .'"';
    case 'array':
      // Arrays in JSON can't be associative. If the array is empty or if it
      // has sequential whole number keys starting with 0, it's not associative
      // so we can go ahead and convert it as an array.
      if (empty ($var) || array_keys($var) === range(0, sizeof($var) - 1)) {
        $output = array();
        foreach ($var as $v) {
          $output[] = classification_autocomplete_to_js($v);
        }
        return '[ '. implode(', ', $output) .' ]';
      }
      // Otherwise, fall through to convert the array as an object.
    case 'object':
      $output = array();
      foreach ($var as $k => $v) {
        $output[] = classification_autocomplete_to_js(strval($k)) .': '. classification_autocomplete_to_js($v);
      }
      return '{ '. implode(', ', $output) .' }';
    default:
      return 'null';
  }
}

/**
 * Callback to check tid in autocomplete
 */
function classification_checkterm($vid){

  $form = array(
    '#post' => $_POST,
  );
  $name = $_REQUEST['root'];

  $result = db_fetch_object(db_query(db_rewrite_sql("SELECT tid FROM {term_data} WHERE vid = %d AND LOWER(name) = LOWER('%s')"), $vid, $name));
  
  drupal_json(array('status' => TRUE,'data' => $result->tid));
}

/**
 * Callback to get parent tid for use in the editor
 */
function classification_get_parent(){
  $form = array(
    '#post' => $_POST,
  );
  $child = $_REQUEST['child'];
  $child_parent = $_REQUEST['child_parent'];
  
  $result = db_fetch_object(db_query(db_rewrite_sql("SELECT parent as tid FROM {term_hierarchy} WHERE tid = %d"), $child_parent));
  
  drupal_json(array('child' => $child, 'dest' => $result->tid));
}

/**
 * Callback to get parent tids for use in autoexpanding the editor
 */
function classification_get_parents($tid){
  $current->tid = $tid;
  $ancestry = array();
  while ($parents = taxonomy_get_parents($current->tid)) {
     $current = array_shift($parents);
     $ancestry[] = 'n'.$current->tid;
  }
  $ancestry = array_reverse($ancestry);
  $ancestry = implode(",", $ancestry);

  drupal_json(array('ancestry' => $ancestry));
}

/**
 * Callback for inline editing of name
 */
function classification_edit_name() {
  global $user;

  $form = array(
    '#post' => $_POST,
  );
  
  $tid = $_REQUEST['tid'];
  $value = $_REQUEST['value'];
  
  $form_state = array('submitted' => FALSE);
  
  if($tid && $value) {
    $term_array = taxonomy_get_term($tid);
    $value_db = $term_array->name;
  
    if($value !== $value_db) {
	   //perform the edit
       db_query("UPDATE {term_data} SET name = '%s' WHERE tid = %d", $value, $tid);
	   //log the edit
	   $action = ($value_db == 'Taxon') ? $value . ' added' : $value_db . ' changed to ' . $value;
	   db_query("INSERT INTO {classification_log} (uid,action,changed) VALUES (%d,'%s',%d)", $user->uid, $action, time());
       drupal_json(array('status' => 'edited', 'message' => t('Name successfully edited')));
    }
    else {
       drupal_json(array('status' => '', 'message' => ''));
    }
  }
  else {
    drupal_json(array('status' => 'failed', 'message' => t('Failed to receive correct POST message')));
  }
}

/**
 * Build form for name metadata using jQuery to adjust selections
 */
 function classification_get_metadata() {
 	global $names;

	$form = array(
    '#post' => $_POST,
  );
  
  $tid = $_REQUEST['tid'];

  // Get name of taxon and ranks
  $result = db_fetch_object(db_query("
  SELECT 
    td.name as name, 
    td.description as description,
    tevt.fid, 
    CASE tevt.fid WHEN 'field_rank' THEN tevt.value END AS rank,
    CASE tevt.fid WHEN 'field_vernacular_language' THEN tevt.value END AS vern
  FROM 
    {term_data} td 
  LEFT JOIN 
    {taxonomy_enhancer_value_text} tevt ON td.tid = tevt.tid 
  WHERE td.tid = %d", $tid));

  $taxon = $result->name;
  $description = $result->description;
  $rank = $result->rank;
  $vern = $result->vern;

  // Get parent, relationship identifier (trid) and relationship type (rtid)
  $result = db_fetch_object(db_query("
  SELECT 
    trht.trid as relation_id,
    trht.rtid as relation_type,
    th.parent as parent_tid,
    td.name as parent_name
  FROM 
    {term_relation_has_type} trht 
  INNER JOIN 
    {term_relation} tr ON trht.trid = tr.trid 
  RIGHT JOIN
    ( {term_hierarchy} th 
      LEFT JOIN
      {term_data} td ON th.parent = td.tid
    ) ON tr.tid1 = th.tid 
  WHERE th.tid = %d", $tid));

  $parent_tid = $result->parent_tid;
  $parent_name = $result->parent_name;
  $relation_id = $result->relation_id;
  $relation_type = $result->relation_type;
  
  $form_state = array('submitted' => FALSE);
  
  // Italicize the genus, species, subspecies names as well as everything other than vernaculars
  $rank_arr = array(
    "genus", 
    "species", 
    "subspecies"
  );
  
  if(in_array(strtolower($rank),$rank_arr)) {
  	$taxon = $names->italicized_form($taxon);
  }
  if (is_numeric($relation_type) && $relation_type != 9) {
  	$taxon = $names->italicized_form($taxon);
  }
  
  // Discover if nodes (and what kinds) are present (e.g. image, biblio, taxon_description)
  $result = db_query("
  SELECT 
    n.nid, n.type, COUNT(*) AS count 
  FROM {node} n 
  JOIN {classification_node} tn ON (n.nid=tn.nid) 
  WHERE tn.tid = %d 
  GROUP BY type",$tid);
  
  $media = array();
  while ($row = db_fetch_object($result)) {
    $media[$row->type] = $row->count;
  }
  
  if($result) {
    $taxon_description = TRUE;
  }
  else {
  	$taxon_description = FALSE;
  }
  
  $output = array('tid' => $tid, 'parent_tid' => $parent_tid, 'parent_name' => $parent_name, 'taxon' => $taxon, 'rank' => $rank, 'vern_lang' => $vern, 'description' => check_plain($description), 'relation_id' => $relation_id, 'relation_type' => check_plain($relation_type), 'media' => $media); 

  drupal_json(array('status' => TRUE,'data' => $output));
}

/**
 * Callback to save name metadata
 */
function classification_update_metadata() {
	global $user;
	
	$form = array(
      '#post' => $_POST,
      '#programmed' => FALSE,
    );
  
	$tid = $_REQUEST['tid'];
	$parent_tid = $_REQUEST['parent'];
	$rank = $_REQUEST['rank'];
	$relation_id = $_REQUEST['relation_id'];
	$relation_type = $_REQUEST['relation_type'];
	$vern_lang = $_REQUEST['vern_lang'];
	$description = $_REQUEST['description'];
	
 	$form_state = array('submitted' => FALSE);
 	
 	//if a rank and a relationship, former takes precedence
 	if($rank && $relation_type) {
 		//First, clear out relationship metadata if the name already existed
 		db_query("DELETE FROM {term_relation} WHERE trid = %d",$relation_id);
 		db_query("DELETE FROM {term_relation_has_type} WHERE trid = %d",$relation_id); 		
 		db_query("DELETE FROM {taxonomy_enhancer_value_text} WHERE tid = %d",$tid);
 		
 		db_query("INSERT INTO {taxonomy_enhancer_value_text} (tid,fid,value,delta) VALUES (%d,'field_rank','%s',%d)",$tid,$rank,0);
 		drupal_json(array('status' => 'failed', 'data' => '', 'message' => t('You have to set the rank to "taxonomic child" if you want to modify the relationship.')));
 	}

  //if a relationship and no rank, there are several possibilities
  elseif(!$rank && $relation_type) {
    switch($relation_type) {
    	//vernacular name
    	case 9:
    	  //First, clear out the relationship metadata (except the term_relation) and add it again
    	  db_query("DELETE FROM {term_relation_has_type} WHERE trid = %d",$relation_id); 		
 		    db_query("DELETE FROM {taxonomy_enhancer_value_text} WHERE tid = %d",$tid);
 		    //case where name didn't already exist, we need a $relation_id
 		     if(!$relation_id) {
 		     	  db_query("INSERT INTO {term_relation} (tid1,tid2) VALUES (%d,%d)",$tid,$parent_tid);
 		     	  //Now we need the trid
 		     	  $result = db_fetch_object(db_query(db_rewrite_sql("SELECT trid FROM {term_relation} WHERE tid1 = %d && tid2 = %d"),$tid,$parent_tid));
 		     	  $relation_id = $result->trid;
 		     }
 		     //Now we add the relation metadata
 		     db_query("INSERT INTO {term_relation_has_type} (trid,rtid) VALUES (%d,%d)",$relation_id,$relation_type);
 		     db_query("INSERT INTO {taxonomy_enhancer_value_text} (tid,fid,value,delta) VALUES (%d,'field_vernacular_language','%s',%d)",$tid,$vern_lang,0);
 		     drupal_json(array('status' => 'edited', 'data' => '', 'message' => t('Nomenclature successfully updated')));
    	  break;
    	//Anything other than a vernacular
    	default:
    	  //First we clear out the relationship metadata (except the term_relation) and add it again
    	  db_query("DELETE FROM {term_relation_has_type} WHERE trid = %d",$relation_id);
    	  db_query("DELETE FROM {taxonomy_enhancer_value_text} WHERE tid = %d",$tid);
    	  //case where name didn't already exist, we need a $relation_id
 		     if(!$relation_id) {
 		     	  db_query("INSERT INTO {term_relation} (tid1,tid2) VALUES (%d,%d)",$tid,$parent_tid);
 		     	  //Now we need the trid
 		     	  $result = db_fetch_object(db_query(db_rewrite_sql("SELECT trid FROM {term_relation} WHERE tid1 = %d && tid2 = %d"),$tid,$parent_tid));
 		     	  $relation_id = $result->trid;
 		     }
 		     //Now we add the relation metadata
 		     db_query("INSERT INTO {term_relation_has_type} (trid,rtid) VALUES (%d,%d)",$relation_id,$relation_type);

             //log the adjustment
             $name_array = taxonomy_get_term($tid);
             $name = $name_array->name;
             $relationship = db_fetch_object(db_query("SELECT trt.name FROM {term_relation_types} trt INNER JOIN {term_relation_has_type} trht ON (trt.rtid = trht.rtid) INNER JOIN {term_relation} tr ON (trht.trid=tr.trid) WHERE tr.tid1 = %d", $tid));
             $action = $name . ' changed to ' . $relationship->name;
             db_query("INSERT INTO {classification_log} (uid,action,changed) VALUES (%d,'%s',%d)",$user->uid,$action,time());

 		     drupal_json(array('status' => 'edited', 'data' => '', 'message' => t('Nomenclature successfully updated')));
    }
  
  }
  
  //Just assign the rank
  elseif($rank && !$relation_type) {
	    //First, we learn what the rank used to be for logging purposes
	    $old_rank = db_result(db_query("SELECT value FROM {taxonomy_enhancer_value_text} WHERE tid=%d",$tid));
  	    //Next, we remove the rank to accommodate a new name and also remove anything in term_relation in case this is elevated from synonym
  	    db_query("DELETE FROM {term_relation} WHERE tid1 = %d AND tid2 = %d",$tid,$parent_tid);
 		db_query("DELETE FROM {taxonomy_enhancer_value_text} WHERE tid = %d",$tid);
 		db_query("INSERT INTO {taxonomy_enhancer_value_text} (tid,fid,value,delta) VALUES (%d,'field_rank','%s',%d)",$tid,$rank,0);

        //log the adjustment if necessary
        if($rank !== $old_rank) {
          $name_array = taxonomy_get_term($tid);
          $name = $name_array->name;
          $action = $name . ' changed to rank ' . $rank;
          db_query("INSERT INTO {classification_log} (uid,action,changed) VALUES (%d,'%s',%d)",$user->uid,$action,time());
        }

 		drupal_json(array('status' => 'edited', 'data' => '', 'message' => t('Nomenclature successfully updated')));
  }
  else {
  	// Do nothing if both $rank and $relation_type are blank
  	drupal_json(array('status' => 'failed', 'data' => '', 'message' => t('Please select a rank or a relationship')));
  }
  
  db_query("UPDATE {term_data} SET description = '%s' WHERE tid = %d",$description,$tid);
  
}

/**
 * Callback to save settings
 */
function classification_update_settings() {
	$form_state = array('submitted' => FALSE);

	$form = array(
    '#post' => $_POST,
    '#programmed' => FALSE,
  );
  
	$ranks = $_REQUEST['ranks_list'];
	$vern_lang = $_REQUEST['vern_list'];
	$display_option = $_REQUEST['display_options_editor'];
	
	$vid = classification_get_vid('Taxa');
 	
 	if(!$ranks || !$vern_lang) {
 		drupal_json(array('status' => 'failed', 'message' => t('At least one rank or one language are required')));
 	}
 	else {
 	  variable_set('classification_editor_names',$display_option);
 	  classification_update_lists('rank','Rank',$ranks);
 	  classification_update_lists('vernacular_language','Vernacular Language',$vern_lang);
    cache_clear_all();
 	
    drupal_json(array('status' => 'updated','message' => t('Settings successfully updated')));
  }
}

function classification_update_lists($fid,$title,$list) {
  $vid = classification_get_vid('Taxa');
  $list_arr = explode("\n",trim($list));
  foreach ($list_arr as $list) {
    $opt[] = $list;
  }
  
  $field = array(
    'vid' => $vid,
    'fid' => 'field_' . $fid,
    'title' => $title,
    'module' => 'classification',
    'type' => 'options',
    'weight' => 0,
    'settings' => array(),
  );
  $field['settings']['allowed_values'] = $opt;
  $field['settings']['options_type'] = 'select';
  
  db_query('DELETE FROM {taxonomy_enhancer_fields} WHERE vid = %d AND fid = "%s"', $vid, 'field_' . $fid);
  drupal_write_record('taxonomy_enhancer_fields', $field);
}

/**
 * Callback to add a new name
 */
function classification_add_name() {
  global $user;
  $form_state = array('submitted' => FALSE);
  $form = array(
    '#post' => $_POST,
    '#programmed' => FALSE,
  );
  
  $vid = classification_get_vid('Taxa');
  $parent_tid = $_REQUEST['parent_tid'];
  $value = 'Taxon';
  
  if($parent_tid) {
  	$result = db_fetch_object(db_query("SELECT max(tid) AS tid FROM {term_data}"));
    $tid = $result->tid + 1;
    //perform the insertion in various tables
    db_query("INSERT INTO {term_data} (tid,vid,name) VALUES (%d,%d,'%s')",$tid,$vid,$value);
    db_query("INSERT INTO {term_hierarchy} (tid,parent) VALUES (%d,%d)",$tid,$parent_tid);
    db_query("INSERT INTO {term_relation} (tid1,tid2) VALUES (%d,%d)",$tid,$parent_tid);
    
    $parent = db_fetch_object(db_query("SELECT name FROM {term_data} WHERE tid = %d",$parent_tid));
    
    drupal_json(array('status' => 'added', 'message' => t('Name data successfully added'), 'tid' => $tid, 'parent_name' => $parent->name));
  }
  else {
  	drupal_json(array('status' => 'failed', 'message' => t('Failed to receive correct POST message')));
  }
 }

/**
 * Callback to delete a name
 */
function classification_delete_name() {
  global $user;
  $form = array(
    '#post' => $_POST,
  );
  
  $tid = $_REQUEST['tid'];
  $form_state = array('submitted' => FALSE);
  
  if($tid) {
  	$vid = classification_get_vid('Taxa');
  	$tree = taxonomy_get_tree($vid,$tid);
  	if(count($tree) > 0) {
  	  foreach ($tree as $branch) {
  	    db_query("DELETE FROM {taxonomy_enhancer_value_text} WHERE tid = %d",$branch->tid);
        $res = db_query(db_rewrite_sql("SELECT trid FROM {term_relation} WHERE tid1 = %d OR tid2 = %d"),$branch->tid,$branch->tid);
        while ($term = db_fetch_object($res)) {
          db_query("DELETE FROM {term_relation_has_type} WHERE trid = %d", $term->trid);
        }
        //delete all the children terms
        taxonomy_del_term($branch->tid);
      }
    }

    db_query("DELETE FROM {taxonomy_enhancer_value_text} WHERE tid = %d",$tid);
  	$res = db_rewrite_sql(db_query("SELECT trid FROM {term_relation} where tid1 = %d OR tid2 = %d"),$tid, $tid);
    while ($term = db_fetch_object($res)) {
    	db_query("DELETE FROM {term_relation_has_type} WHERE trid = %d", $term->trid);
    }
    
    //create the action taken for logging
    $name_array = taxonomy_get_term($tid);
    $name = $name_array->name;
    $action = $name . ' and its children deleted';

    //delete the term
    taxonomy_del_term($tid);

    //log the action
    db_query("INSERT INTO {classification_log} (uid,action,changed) VALUES (%d,'%s',%d)", $user->uid,$action,time());
     
    drupal_json(array('status' => 'edited', 'message' => t('Name and its children (if any) successfully deleted')));
  }
  else {
  	 drupal_json(array('status' => 'failed', 'message' => t('Failed to receive correct POST message')));
  }
}

/**
 * Callback to move a name during drag/drop and during cut/paste
 */
function classification_move_name() {
  global $user;

  $form = array(
    '#post' => $_POST,
  );
  
  $child = $_REQUEST['child'];
  $new_parent = $_REQUEST['new_parent'];
  
  $form_state = array('submitted' => FALSE);

  //get the old parent for logging later
  $old_parent_array = db_fetch_object(db_query(db_rewrite_sql('SELECT t.name FROM {term_data} t INNER JOIN {term_hierarchy} h ON h.parent = t.tid WHERE h.tid = %d'), $child));
  $old_parent = $old_parent_array->name;
  
  if($child) {
     db_query("DELETE FROM {term_hierarchy} WHERE tid = %d", $child);
     db_query("INSERT INTO {term_hierarchy} (parent, tid) VALUES (%d, %d)", $new_parent, $child);
     // Update relationships if any
     db_query("UPDATE {term_relation} SET tid2 = %d WHERE tid1 = %d",$new_parent,$child);

     //get the new parent & the child term names for logging purposes
     $child_name_array = taxonomy_get_term($child);
     $child_name = $child_name_array->name;
     $new_parent_array = taxonomy_get_term($new_parent);
     $new_parent_name = $new_parent_array->name;
     $action = $child_name . ' moved from ' . $old_parent . ' to ' . $new_parent_name;
     db_query("INSERT INTO {classification_log} (uid,action,changed) VALUES (%d,'%s',%d)",$user->uid,$action,time());

     drupal_json(array('status' => 'moved', 'message' => t('Name successfully moved')));
  }
  else {
  	 drupal_json(array('status' => 'failed', 'message' => t('Failed to receive correct POST message')));
  }
}

/**
 * Callback to move a name during drag/drop from source tree into destination tree
 */
function classification_copy_name_alternate() {
  global $user;
  $form = array(
    '#post' => $_POST,
  );
  
  $child = str_replace("_copy","",$_REQUEST['child']);
  $new_parent_prev = $_REQUEST['new_parent'];
  $new_parent = db_fetch_object(db_query(db_rewrite_sql("SELECT parent as tid FROM {term_hierarchy} WHERE tid = %d"), $new_parent_prev)); // thii is a hack, maybe it is better to adjust JSTree instead
  
  $form_state = array('submitted' => FALSE);
  
    if($child && $new_parent) {
     $vid_old = db_fetch_object(db_query("SELECT vid FROM term_data WHERE tid = %d LIMIT 1", $child));
     $vid_new = classification_get_vid('Taxa');
     $children = taxonomy_get_tree($vid_old->vid, $child);

     db_query("DELETE FROM {term_hierarchy} WHERE tid = %d", $child);
     db_query("INSERT INTO {term_hierarchy} (parent, tid) VALUES (%d, %d)", $new_parent->tid, $child);
     // Update relationships if any
     db_query("UPDATE {term_relation} SET tid2 = %d WHERE tid1 = %d",$new_parent->tid,$child);

     db_query("UPDATE {term_data} SET vid = %d WHERE tid = %d", $vid_new, $child);
     foreach ($children as $offspring) {
       db_query("UPDATE {term_data} SET vid = %d WHERE tid = %d", $vid_new, $offspring->tid);
       }

     //log the activity
     $name_array = taxonomy_get_term($child);
     $name = $name_array->name;
     $action = $name . ' and all its children (if any) inserted from an alternate classification';
     db_query("INSERT INTO {classification_log} (uid,action,changed) VALUES (%d,'%s',%d)", $user->uid,$action,time());

     drupal_json(array('status' => 'moved', 'message' => t('Name successfully copied into your classification')));
  }
  else {
  	 drupal_json(array('status' => 'failed', 'message' => t('Failed to receive correct POST message')));
  }
}

/**
 * Produce an array of child names other than vernaculars given a parent tid
 */
function classification_get_synonyms_json($tid=NULL) {
  $output = array();
  if($tid) {
    $vid = classification_get_vid('Taxa');
    $result = db_query(db_rewrite_sql("
     SELECT 
        td.tid as tid, 
        td.name as name,
        trt.name as relation 
     FROM
        {term_data} td 
     JOIN 
        {term_hierarchy} th on (td.tid=th.tid) 
     LEFT JOIN 
      	({term_relation} tr 
     	    JOIN {term_relation_has_type} trht ON (tr.trid=trht.trid) 
     	    JOIN {term_relation_types} trt ON (trht.rtid=trt.rtid)
     	  ) ON td.tid=tr.tid1 
	   WHERE 
	      td.vid = %d AND th.parent = %d AND trt.rtid <> 9  
	   ORDER BY 
	      td.name"),$vid,$tid);
	      
    while ($object = db_fetch_object($result)) {
       $tid = $object->tid;
       $name = $object->name;
       $relation = $object->relation;
       $output[] = array('tid' => $tid, 'name' => check_plain($name) , 'relation' => $relation);
    }
  }
  drupal_json(array('status' => 'OK', 'content' => $output));
}