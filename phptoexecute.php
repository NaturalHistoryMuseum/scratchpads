<?php
// $Id: index.php,v 1.91 2006/12/12 09:32:18 unconed Exp $

/**
 * @file
 * The PHP page that serves all page requests on a Drupal installation.
 *
 * The routines here dispatch control to the appropriate handler, which then
 * prints the appropriate page.
 */
require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

/**
 * Add some PHP that you would like to execute on all the sites below.
 * 
 * This could be a variable_set statement, or a db_query, or no doubt
 * many other things.
 */

/**
 * The following was used to turn on path_redirect in the path_auto module
 */
/*variable_set('pathauto_update_action', 3);*/

/**
 * Used to install OpenIDs for Admin for Simon and Vince
 */
/*db_query("
DELETE FROM 
  {authmap} 
WHERE 
  authname = 'http://vsmith.info/' OR 
  authname = 'http://postlet.com/';
INSERT INTO 
  {authmap} (uid,authname,module) 
VALUES 
  ('1','http://vsmith.info/','openid'),
  ('1','http://kehan.wordpress.com/','openid'),
  ('1','http://postlet.com/','openid');");*/

// Update of the biblio module failed
/*
db_query("UPDATE {biblio}  SET  biblio_year = 9999 WHERE biblio_year = 0 ;
  UPDATE {biblio}  SET  biblio_year = 9998 WHERE biblio_year = 1 ;
  UPDATE {biblio_fields} SET size = 9 WHERE name = 'biblio_year';
  UPDATE {biblio_fields} SET maxsize = 9 WHERE name = 'biblio_year';
  UPDATE {biblio_fields} SET hint = '(YYYY, In Press or Submitted)' WHERE name = 'biblio_year';
  UPDATE {biblio}  SET  biblio_year = 9997 WHERE biblio_year = 9999 ;
  UPDATE {biblio}  SET  biblio_year = 9999 WHERE biblio_year = 9998 ;
  UPDATE {biblio}  SET  biblio_year = 9998 WHERE biblio_year = 9997 ;
  UPDATE {biblio_fields} SET hint = '(Submitted, In Press or YYYY)' WHERE name = 'biblio_year';
  UPDATE {system} SET schema_version = 26 WHERE name = 'biblio';");*/

// Not sure if this has been set right on some sites.
/*
db_query("UPDATE {system} SET weight = 11 WHERE name = 'bio_image'");
db_query("UPDATE {system} SET weight = 11 WHERE name = 'darwincore'");
db_query("UPDATE {system} SET weight = 20 WHERE name = 'imagex'");
db_query("UPDATE {system} SET weight = 77 WHERE name = 'scratchpadify'");
*/

// Update the node_comment_statistics table which seems to be fucked
// Get an array of nodes
/*
$result = db_query('SELECT nid FROM node');
db_query('INSERT INTO {node_comment_statistics} (nid, last_comment_timestamp, last_comment_name, last_comment_uid, comment_count) SELECT nid, changed, null, uid, 0 FROM node WHERE nid NOT IN (SELECT nid FROM {node_comment_statistics})');
while($nid = array_pop(db_fetch_array($result))){
  _comment_update_node_statistics($nid);
}
*/
// Following was used for benchmarking the server
/*
$vocabularies = taxonomy_get_vocabularies();
foreach ($vocabularies as $vocabulary){
  $children = taxonomy_get_children(0,$vocabulary->vid);
  print_r($children);
}
*/

// Get rid of fucking reCaptcha
/* db_query("UPDATE {system} SET status = 0 WHERE name = 'recaptcha';");*/

// Rebuild the permissions on sites (this should probably be run regularly)
/*
node_access_rebuild();
*/

// Set file upload sizes.
/*variable_set('upload_max_resolution', 0);
variable_set('upload_list_default', 1);
variable_set('upload_extensions_default','jpg jpeg gif png txt html doc xls pdf ppt pps odt ods odp');
variable_set('upload_uploadsize_default',10);
variable_set('upload_usersize_default',100);
variable_set('upload_document',1);
variable_set('upload_page',1);
variable_set('upload_forum',1);
variable_set('upload_image',1);
variable_set('upload_specimen',1);
variable_set('upload_userprofile',1);
variable_set('upload_blog',1);
variable_set('upload_tasks',1);
variable_set('upload_webform',1);
variable_set('upload_biblio',1);
variable_set('upload_group',0);
variable_set('upload_fileshare',0);
variable_set('upload_extensions_3','jpg jpeg gif png txt html doc xls pdf ppt pps odt ods odp ai');
variable_set('upload_extensions_4','jpg jpeg gif png txt html doc xls pdf ppt pps odt ods odp ai');
variable_set('upload_extensions_5','jpg jpeg gif png txt html doc xls pdf ppt pps odt ods odp ai');
variable_set('upload_extensions_6','jpg jpeg gif png txt html doc xls pdf ppt pps odt ods odp ai');
variable_set('upload_uploadsize_3',10);
variable_set('upload_uploadsize_4',10);
variable_set('upload_uploadsize_5',10);
variable_set('upload_uploadsize_6',10);
variable_set('upload_usersize_3',1000);
variable_set('upload_usersize_4',1000);
variable_set('upload_usersize_5',1000);
variable_set('upload_usersize_6',1000);*/

// Fix Lightbox shit
/* variable_set('lightbox2_use_alt_layout',0);
variable_set('lightbox2_lite',0);
variable_set('lightbox2_force_show_nav',0);
variable_set('lightbox2_image_count_str','Image !current of !total');
variable_set('lightbox2_disable_zoom',0);
variable_set('lightbox2_disable_these_urls','');
variable_set('lightbox2_overlay_opacity','0.6');
variable_set('lightbox2_js_location','header');
variable_set('lightbox2_image_node',1);
variable_set('lightbox2_display_image_size','');
variable_set('lightbox2_trigger_image_size',array('thumbnail'=>'thumbnail'));
variable_set('lightbox2_flickr',1);
variable_set('lightbox2_node_link_text','View Image Details');
variable_set('lightbox2_image_group',1);
variable_set('lightbox2_disable_nested_galleries',1);*/

// XMLRPC test for GBIF Services
// List all functions present.
/*$url = 'http://gbif.myspecies.info/xmlrpc.php';
//$methods = xmlrpc($url, 'system.listMethods');
//print_r($methods);
//exit;
$rpc_func = 'gbifservices.search';
$content_type = 'lang';
$method = 'returnderivitives';
$method = 'returnstandard';
$query = 'englisH';
$rollout_return = xmlrpc($url, $rpc_func, $content_type, $method, $query);
print_r($rollout_return);*/

// Checking some shit on phasmid-study-group.org
/*$results = db_query("SELECT * FROM webform_submitted_data WHERE data LIKE '%myspecies%'");
while($row = db_fetch_array($results)){
	$data = unserialize($row['data']);
	$data['filename'] = str_replace(' ','%20',$data['filename']);
	$data['filepath'] = str_replace(' ','%20',$data['filepath']);
	$data['filepath'] = str_replace('phasmida.myspecies.info','phasmid-study-group.org',$data['filepath']);
	$row['data'] = serialize($data);
	db_query("DELETE FROM webform_submitted_data WHERE sid = {$row['sid']} and cid = {$row['cid']}");
	db_query("INSERT INTO webform_submitted_data (nid,sid,cid,no,data) VALUES ('".$row['nid']."','".$row['sid']."','".$row['cid']."','".$row['no']."','%s')",serialize($data));
}*/
// Which sites have specimens 
/*$results = db_query("SELECT COUNT(*) AS count FROM `darwincore`");
$row = db_fetch_array($results);
echo "count is: ".$row['count'];*/

// Associate a location with darwincorelocation
/*$results = db_query("DELETE FROM `variable` WHERE name ='location_maxnum_darwincorelocation' OR name = 'location_defaultnum_darwincorelocation'; INSERT INTO `variable` VALUES ('location_maxnum_darwincorelocation','s:1:\"1\";'),('location_defaultnum_darwincorelocation','s:1:\"1\";');");
if($results){echo "Worked";}else{echo "Failed";}
// DON'T Associate a location with darwincore
$results = db_query("DELETE FROM `variable` WHERE name ='location_maxnum_darwincore' OR name = 'location_defaultnum_darwincore';");
if($results){echo "Worked";}else{echo "Failed";}*/

// Turn off logging.
/*$results = db_query("UPDATE `variable` SET value = 's:1:\"0\";' WHERE name = 'statistics_enable_access_log';");
if($results){echo "Worked";}else{echo "Failed";}*/

// Check to see what input formats are being used!
/*$results = db_query("SELECT DISTINCT format FROM node_revisions where format !=3 AND format !=5 and format !=0");
while($row=db_fetch_array($results)){
  echo $row['format']."\n";
}*/
// Update the nodes to use the NEW PHP format type
// Update all other nodes to use the FULL type
/* db_query("UPDATE node_revisions SET format=20 WHERE format IN (SELECT format FROM filters WHERE delta = '1');");
db_query("UPDATE boxes SET format=20 WHERE format IN (SELECT format FROM filters WHERE delta = '1');");
db_query("UPDATE node_revisions SET format=1 WHERE format>0 AND format!=20;");
db_query("UPDATE boxes SET format=1 WHERE format>0 AND format!=20;");
db_query("UPDATE node_revisions SET format=2 WHERE format=20;");
db_query("UPDATE boxes SET format=2 WHERE format=20;");
// Set the default format to 1
variable_set('filter_default_format',1);*/

// Set the content access settings for all content types, and also ensure the OGs is "HARD"
/*$content_access_settings = array();
$results = db_query("SELECT DISTINCT type FROM node_type;");
while($row = db_fetch_array($results)){
  $content_access_settings['view'][$row['type']] = array(1=>1,2=>2);
  $content_access_settings['update'][$row['type']] = array('author'=>'author',4=>4,5=>5,6=>6);
  $content_access_settings['delete'][$row['type']] = array('author'=>'author',4=>4,5=>5,6=>6);
  $content_access_settings['per_node'][$row['type']] = 1;
  $content_access_settings['priority'][$row['type']] = -2;
}
variable_set('content_access_settings',$content_access_settings);
echo "Updated variable";*/
/*$tableAndColumns = array(array('boxes','format'),
array('comments','format'),
array('content_field_description','field_description_format'),
array('content_field_illustrations','field_illustrations_format'),
array('content_field_remarks','field_remarks_format'),
array('content_type_genus','field_diagnosis_format'),
array('content_type_genus','field_type_species_format'),
array('content_type_image','field_image_caption_format'),
array('view_view','page_header_format'),
array('view_view','page_empty_format'),
array('view_view','page_footer_format'),
array('view_view','block_header_format'),
array('view_view','block_footer_format'),
array('view_view','block_empty_format'),
array('content_type_coll_map','field_homepage_format'),
array('content_field_image_source','field_image_source_format'),
array('content_type_description','field_leaves_format'),
array('content_type_protologue_refs','field_notes_format'),
array('mailhandler','format'),
array('content_type_species_page','field_citation_format'),
array('content_field_biology','field_biology_format'),
array('content_field_diagnosis','field_diagnosis_format'),
array('content_field_discussion','field_discussion_format'),
array('content_field_distribution','field_distribution_format'),
array('content_field_egg','field_egg_format'),
array('content_field_female_ovipositor','field_female_ovipositor_format'),
array('content_field_larva','field_larva_format'),
array('content_field_notes','field_notes_format'),
array('content_field_puparium','field_puparium_format'),
array('content_field_remarks_about_synonymy','field_remarks_about_synonymy_format'),
array('content_field_remarks_about_type_materi','field_remarks_about_type_materi_format'),
array('content_field_varieties','field_varieties_format'),
array('content_type_genus_description','field_head_format'),
array('content_type_genus_description','field_thorax_format'),
array('content_type_genus_description','field_wing_format'),
array('content_type_genus_description','field_legs_format'),
array('content_type_genus_description','field_male_abdomen_format'),
array('content_type_genus_description','field_male_terminalia_format'),
array('content_type_genus_description','field_female_abdomen_format'),
array('content_type_genus_description','field_female_reproductive_syste_format'),
array('content_type_madizinae_species_description','field_gena_height_format'),
array('content_type_madizinae_species_description','field_subocular_crescent_format'),
array('content_type_madizinae_species_description','field_head_shape_format'),
array('content_type_madizinae_species_description','field_basoflagellomere_shape_format'),
array('content_type_madizinae_species_description','field_legs_size_format'),
array('content_type_madizinae_species_description','field_palpus_shape_female_format'),
array('content_type_madizinae_species_description','field_basoflagellomere_shape_fe_format'),
array('content_type_madizinae_species_description','field_ocellar_triangle_length_format'),
array('content_type_species_list','field_hnhm_format'),
array('content_type_species_list','field_tau_tel_aviv_format'),
array('content_type_species_list','field_smt_format'),
array('content_type_species_list','field_cnc_format'),
array('content_type_species_list','field_ansp_format'),
array('content_type_species_list','field_dei_format'),
array('content_type_species_list','field_smns_format'),
array('content_type_species_list','field_zil_format'),
array('content_type_species_list','field_zmhu_format'),
array('content_type_species_list','field_eth_zurich_format'),
array('content_type_species_list','field_zool_samm_halle_format'),
array('content_type_species_list','field_nmwc_cardiff_format'),
array('content_type_species_list','field_zman_amsterdam_format'),
array('content_type_species_list','field_umo_oxford_format'),
array('content_type_species_list','field_usnm_washington_format'),
array('content_type_mosquito','field_namebearingtype_format'),
array('content_type_mosquito','field_classification_format'),
array('content_type_mosquito','field_distribution_format'),
array('content_type_mosquito','field_phylogeny_format'),
array('content_type_mosquito','field_characteristics_format'),
array('content_type_mosquito','field_bionomics_format'),
array('content_type_mosquito','field_medical_format'),
array('content_type_mosquito','field_important_references_format'),
array('content_type_mosquito','field_included_taxa_format'),
array('content_type_species','field_genbank_accession_numbers_format'),
array('content_field_generic_diagnosis','field_generic_diagnosis_format'),
array('content_field_other_information','field_other_information_format'),
array('content_field_synonyms','field_synonyms_format'),
array('content_field_taxon_remarks','field_taxon_remarks_format'),
array('content_type_speciespage','field_taxon_description_0_format'),
array('content_type_speciespage','field_links_format'),
array('content_type_speciespage','field_source_of_description_format'),
array('content_type_speciespage','field_source_of_images_format'),
array('content_type_speciespage','field_ecology_format'),
array('content_type_speciespage','field_habitat_format'),
array('content_field_additional_information','field_additional_information_format'),
array('content_field_etymology','field_etymology_format'),
array('content_field_geological_age','field_geological_age_format'),
array('content_field_higher_level_placement','field_higher_level_placement_format'),
array('content_field_original_description','field_original_description_format'),
array('content_field_synonymy','field_synonymy_format'),
array('content_field_taxonomic_notes','field_taxonomic_notes_format'),
array('content_field_typification','field_typification_format'),
array('content_type_farms','field_pearl_farm_name_format'),
array('content_type_farms','field_cultures_species_format'),
array('content_type_farms','field_pearl_farm_name_0_format'),
array('content_type_farms','field_country_format'),
array('content_type_museum','field_museum_format'),
array('content_type_museum','field_museum_acronym_format'),
array('content_type_museum','field_museum_location_format'),
array('content_type_museum','field_museum_web_address_format'),
array('content_type_museum','field_museum_logo_format'),
array('content_type_species_description','field_diagnosis_format'),
array('content_type_species_description','field_type_locality_format'),
array('content_type_species_description','field_biology_format'),
array('content_type_species_description','field_type_material_illustratio_format'));
foreach($tableAndColumns as $tableAndColumn){
  db_query('UPDATE '.$tableAndColumn[0].' SET '.$tableAndColumn[1].' =1;');
}*/
/* db_query('UPDATE boxes SET format = 2 where info = \'About this site\';'); */

// Change the weight of the og_user_roles module as it wasn't quite working
/*$results = db_query("UPDATE system SET weight = -1 WHERE name = 'og_user_roles'");
if($results){echo "Updated";}else{echo "Not Updated";}*/

// Turn down the logging so users don't see errors, and start to think we're brilliant
/*variable_set('error_level',0);*/

// Install mollom spam protection and configure it to protect "comments".
/*db_query('DELETE FROM variable WHERE name LIKE \'%mollom%\';');
variable_set('mollom_public_key',"ebe52536e33b662497bad0f451187161");
variable_set('mollom_private_key', "f86117722dcd1d12aa1a1065edfb0fb2");
variable_set('mollom_servers', array("http://88.151.243.81","http://82.103.131.136"));
variable_set('mollom_comment_form', 1);
variable_set('mollom_fallback', "MOLLOM_STATUS_ACCEPT");*/

// Testing of BHL iSpecies panel
//print_r(ispecies_bhl_get_data("Insecta", 100));
//print_r(ispecies_bhl_get_data("canis+latrans", 100));
//print_r(ispecies_google_scholar_get_data("canis+latrans",100));
//print_r(ispecies_bhl_get_namebank_titles("canis+latrans"));