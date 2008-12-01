<?php

/**
 * Form to coordinate the export of hierarchical classification
 */
    
function classification_export_form(){
  $module_path = drupal_get_path('module', 'classification') .'/';
  drupal_add_css($module_path . 'css/classification.css');

  $vid = classification_get_vid('Taxa');

  $form = array();

  $sql = db_query("SELECT tid FROM term_data WHERE vid = %d LIMIT 1", $vid);
  $result = db_fetch_object($sql);
  
  if(!$result->tid) {
  	$message = t('Sorry, before you can export your primary classification, you must first import your names.');
  	$form['message'] = array(
  	  '#type' => 'markup',
  	  '#value' => '<div class="messages error">' . $message . '</div>',
  	);
  	return $form;
  }
  
/*
  foreach (module_invoke('taxonomy', 'get_vocabularies') as $vid => $voc) 
	{
	$vocs[$vid] = $voc->name;
	}

  $form['vid'] = array(
    '#type' => 'select',
    '#title' => t('Export vocabulary'),
    '#options' => $vocs,
    '#description' => t('The vocabulary which you would like to export.'),
  );
*/
  
  $help = theme('advanced_help_topic', 'classification' , 'export');

  $form['help'] = array(
    '#type' => 'markup',
    '#value' => $help,
    '#prefix' => '<div class="help_icon">',
    '#suffix' => '</div><br />',
  );
  
  $form['file_export'] = array(
    '#type' => 'fieldset',
    '#title' => t('Export Your Classification to File'),
  );
  
  $ifopts = array(0=>"- FILE FORMAT -",1=>"Parent-child without synonyms",2=>"Parent-child with synonyms",3=>"Taxon Concept Schema export for EOL");
  
  $form['file_export']['export_format'] = array(
    '#type' => 'select',
    '#title' => t('Export file format'),
    '#options' => $ifopts,
//    '#description' => t('The file format of the vocabulary being exported'),
  );
    
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Export!'),
    '#attributes' => array('class' => 'classification-buttons export'),
  );
  
  return $form;
}

/**
 * Export the vocabulary
**/
function classification_export_form_submit($form, &$form_state)
{
$vid = classification_get_vid('Taxa');
if ($form_state['values']['export_format'] < 3)
{
    header('Content-Type: text/tab-separated-values');
    header('Content-Disposition: attachment; filename=taxonomyexport.txt');
    if($form_state['values']['export_format'] == 1)
    {
//  	_classification_recurse_export(0, $form_state['values']['vid']);
	_classification_recurse_export(0, $vid);
    }
    elseif($form_state['values']['export_format'] == 2)
    {
//  	_classification_recurse_export_with_synonyms(0, $form_state['values']['vid']);
	_classification_recurse_export_with_synonyms(0, $vid);
    }
}
else
{
    header('Content-type: text/xml');
    echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n";
    echo "<results xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n";
    echo "<TaxonConcepts>\n\n";
    _classification_recurse_export_tcs(0, $vid, "   ");
    echo "</TaxonConcepts>\n\n";
    echo "</results>";
}
exit;
}

// Parent-child format without synonyms
function _classification_recurse_export($tid, $vid){
  $results = db_query('
  SELECT
    td.tid tid,
    td.name child,
    td.description description,
    p.name parent
  FROM 
    term_data td 
  INNER JOIN 
    term_hierarchy th ON td.tid = th.tid 
  LEFT JOIN
    term_data p ON p.tid = th.parent
  WHERE 
    td.vid = %d AND
    th.parent = %d',$vid, $tid);
    while($row = db_fetch_array($results))
    {
    echo $row['child']."\t".$row['parent']."\t".$row['description']."\n";
    _classification_recurse_export($row['tid'],$vid);
    }
}

// Parent-child format with synonyms: listed synonyms, but split them into separate rows
function _classification_recurse_export_with_synonyms($tid, $vid)
{
 $results = db_query('
SELECT
 td.tid tid,
 td.name child,
 td.description description,
 p.name parent,
 s.name synonym
FROM
 term_data td
INNER JOIN
 term_hierarchy th ON td.tid = th.tid
LEFT JOIN
 term_data p ON p.tid = th.parent
LEFT JOIN
 term_synonym s ON s.tid = th.tid
WHERE
 td.vid = %d AND
 th.parent = %d',$vid, $tid);

 while($row = db_fetch_array($results))
    {
    echo $row['child']."\t".$row['parent']."\t".$row['description']."\t".$row['synonym']."\n";
    _classification_recurse_export_with_synonyms($row['tid'],$vid);
    }
}

// TCS export
function _classification_recurse_export_tcs($tid, $vid, $prefix)
{
 $results = db_query('
SELECT
 td.tid tid,
 td.name child,
 td.description description,
 p.name parent,
 s.name synonym
FROM
 term_data td
INNER JOIN
 term_hierarchy th ON td.tid = th.tid
LEFT JOIN
 term_data p ON p.tid = th.parent
LEFT JOIN
 term_synonym s ON s.tid = th.tid
WHERE
 td.vid = %d AND
 th.parent = %d',$vid, $tid);

 while($row = db_fetch_array($results))
 {
    echo $prefix."<TaxonConcept id=\"".htmlspecialchars($row['tid'])."\">\n";
    while(list($key,$val)=each($row))
    {
	if ($key != "tid")
		{
		echo $prefix."<".$key.">".htmlspecialchars($val)."</".$key.">\n";
		}
    }
    echo $prefix."</TaxonConcept>\n";
    _classification_recurse_export_tcs($row['tid'], $vid, $prefix."  ");
 }
}

//  drupal_set_message(print_r($synonym_data, TRUE)); // !!! DEBUG
