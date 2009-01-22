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

  $form['title'] = array(
    '#type' => 'markup',
    '#value' => '<h2>' . t('Export') . '</h2>' . $message,
   );
    
  $form['file_export'] = array(
    '#type' => 'fieldset',
    '#title' => t('File Format'),
  );
  
  $ifopts = array(0=>"- FILE FORMAT -",1=>"Parent-child text file",2=>"Taxon Concept Schema XML file (gzipped)");
  
  $form['file_export']['export_format'] = array(
    '#type' => 'select',
    '#options' => $ifopts,
  );
    
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Export'),
    '#attributes' => array('class' => 'classification-buttons export'),
  );
  
  $form['show_xml'] = array(
    '#prefix' => '<br /><br /><br />',
    '#type' => 'fieldset',
    '#title' => t('History of XML Exports'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['show_xml']['show_folder'] = array(
    '#value' => '<p>'.get_xml_files().'</p>',
  );
  
  return $form;
}

function get_xml_files()
{
$xml_files = "";
$xml_prefix = file_create_url('classification');
$xml_files_array = file_scan_directory(file_create_path('classification'), '.gz');
foreach ($xml_files_array as $file) {
      $xml_files.= '<a href='.$xml_prefix.'/'.$file->basename.'>'.$file->basename.'</a><br />';
      }
if ($xml_files == "") $xml_files = 'Sorry, you must first export a file before it will be made available for download.';
return $xml_files;
}

/**
 * Export the vocabulary
**/
function classification_export_form_submit($form, &$form_state)
{
	$vid = classification_get_vid('Taxa');
	if ($form_state['values']['export_format'] == 1)
	{
		$currname = date("Ymd_Gi_").'taxonomyexport.txt';
		header('Content-Type: text/tab-separated-values');
		header('Content-Disposition: attachment; filename='.$currname);
		_classification_recurse_export_with_synonyms(0, $vid);
	}
	else
	{
		$sitename = variable_get('site_name','');
		$shortname = variable_get('shortname' ,'');
		ob_start();
		echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n";
		echo "<DataSet xmlns=\"http://www.tdwg.org/schemas/tcs/1.01\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.tdwg.org/schemas/tcs/1.01 http://www.tdwg.org/standards/117/files/TCS101/v101.xsd\">\n";
		echo "<MetaData>";
		echo "<Simple>LifeDesk: ".$sitename."; http://lifedesk.eol.org/".$shortname."</Simple>";
		echo "</MetaData>";
		echo "<TaxonNames>\n\n";
		_classification_recurse_export_tcs_1(0, $vid, "   ");
		echo "</TaxonNames>\n\n";
		echo "<TaxonConcepts>\n\n";
		_classification_recurse_export_tcs_2(0, $vid, "   ");
		echo "</TaxonConcepts>\n\n";
		echo "</DataSet>";
		$out = ob_get_contents();
		ob_end_clean();
//		header('Content-type: text/xml');
//		header('Content-Disposition: attachment; filename=taxonomyexport.xml');
		$currname = date("Ymd_Gi_").'classificationexport.xml.gz';
		file_save_data(gzencode($out), 'classification/'.$currname, FILE_EXISTS_REPLACE);
		drupal_set_message(t('Classification export successfully produced.'), 'status');
		drupal_goto('classification/export');
//		header('Content-type: x-gzip');
//		header('Content-Disposition: attachment; filename='.$currname);
		echo gzencode($out);		
	}
exit;
}

// Parent-child format with synonyms: listing synonyms and vernaculars, but split them into separate rows
function _classification_recurse_export_with_synonyms($tid, $vid)
{
 $results = db_query('
SELECT
 td.tid tid,
 td.name child,
 td.description description,
 p.name parent,
 s.name synonym,
 r2.tid1 is_synonym
FROM
 term_data td
INNER JOIN
 term_hierarchy th ON td.tid = th.tid
LEFT JOIN
 term_data p ON p.tid = th.parent
LEFT JOIN
 term_relation r1 ON r1.tid2 = td.tid
LEFT JOIN
 term_data s ON r1.tid1 = s.tid
LEFT JOIN
 term_relation r2 ON r2.tid1 = td.tid
WHERE
 td.vid = %d AND
 th.parent = %d',$vid, $tid);

 while($row = db_fetch_array($results))
    {
    if ($row['is_synonym'] == "") echo $row['child']."\t".$row['parent']."\t".$row['description']."\t".$row['synonym']."\n";
    _classification_recurse_export_with_synonyms($row['tid'],$vid);
    }
}

// TCS export
function _classification_recurse_export_tcs_1($tid, $vid, $prefix)
{
 $results = db_query('
SELECT
 td.tid tid,
 td.name Simple,
 f2.title Flag,
 f1.value Value
FROM
 term_data td
INNER JOIN
 term_hierarchy th ON td.tid = th.tid
LEFT JOIN
 term_data p ON p.tid = th.parent
LEFT JOIN
 taxonomy_enhancer_value_text f1 ON f1.tid = td.tid
LEFT JOIN
 taxonomy_enhancer_fields f2 ON f2.fid = f1.fid
WHERE
 td.vid = %d AND
 th.parent = %d',$vid, $tid);

// Synonyms do not have rank here
 while($row = db_fetch_array($results))
 {
    if ($row['Flag'] != "Vernacular language")
	{
	echo $prefix."<TaxonName id=\"n".htmlspecialchars($row['tid'])."\">\n";
	if ($row['Simple'] != "") echo $prefix."<Simple>".htmlspecialchars($row['Simple'])."</Simple>\n";
    	if ($row['Flag'] == "Rank") echo $prefix."<Rank>".htmlspecialchars($row['Value'])."</Rank>\n";
	echo $prefix."</TaxonName>\n";
	}
    _classification_recurse_export_tcs_1($row['tid'], $vid, $prefix."  ");
 }
}

// Duplicates synonyms and synonym parents
function _classification_recurse_export_tcs_2($tid, $vid, $prefix)
{
 $results = db_query('
SELECT
 td.tid tid,
 td.name Name,
 td.description CharacterCircumscription,
 p.tid ParentId,
 r3.name TaxonRelation,
 r1.tid1 RelatedTaxonId,
 f2.title Flag,
 f1.value Value
FROM
 term_data td
INNER JOIN
 term_hierarchy th ON td.tid = th.tid
LEFT JOIN
 term_data p ON p.tid = th.parent
LEFT JOIN
 term_relation r1 ON r1.tid2 = td.tid
LEFT JOIN
 term_relation_has_type r2 ON r2.trid = r1.trid
LEFT JOIN
 term_relation_types r3 ON r3.rtid = r2.rtid
LEFT JOIN
 taxonomy_enhancer_value_text f1 ON f1.tid = td.tid
LEFT JOIN
 taxonomy_enhancer_fields f2 ON f2.fid = f1.fid
WHERE
 td.vid = %d AND
 th.parent = %d',$vid, $tid);

 while($row = db_fetch_array($results))
 {
    echo $prefix."<TaxonConcept id=\"c".htmlspecialchars($row['tid'])."\">\n";
	if ($row['Name'] != "")
	{
		if ($row['Flag'] != "Vernacular language") 
		{
			echo $prefix."<Name scientific=\"true\" ref=\"n".htmlspecialchars($row['tid'])."\">".htmlspecialchars($row['Name'])."</Name>\n";
		}
		else
		{
			echo $prefix."<Name scientific=\"false\" ref=\"n".htmlspecialchars($row['tid'])."\" language=\"".htmlspecialchars($row['Value'])."\">".htmlspecialchars($row['Name'])."</Name>\n";
		}
	}
	echo $prefix."<TaxonRelationships>\n";
	echo $prefix."<TaxonRelationship type=\"is child taxon of\">\n".$prefix."<ToTaxonConcept ref=\"c".htmlspecialchars($row['ParentId'])."\" linkType=\"local\"/></TaxonRelationship>\n";
// probably make 'case' here for all types covered by TCS and/or LifeDesks
	if ($row['TaxonRelation'] == "synonym") 
	{
	echo $prefix."<TaxonRelationship type=\"has synonym\">\n".$prefix."<ToTaxonConcept ref=\"c".htmlspecialchars($row['RelatedTaxonId'])."\" linkType=\"local\"/></TaxonRelationship>\n";
	}
	if ($row['TaxonRelation'] == "vernacular name") 
	{
	echo $prefix."<TaxonRelationship type=\"has vernacular\">\n".$prefix."<ToTaxonConcept ref=\"c".htmlspecialchars($row['RelatedTaxonId'])."\" linkType=\"local\"/></TaxonRelationship>\n";
	}
	echo $prefix."</TaxonRelationships>\n";
    echo $prefix."</TaxonConcept>\n";
    _classification_recurse_export_tcs_2($row['tid'], $vid, $prefix."  ");
 }
}
