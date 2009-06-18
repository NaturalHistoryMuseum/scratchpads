<?php

/**
 * Enter description here...
 *
 * @param unknown_type $node
 * @return unknown
 */
function theme_darwincorelocation_full_page($node) {
  // FIXME - Need to add the shit from the location module.
  $location = _theme_darwincorelocation_help($node);
  // Unset latitude and longitude, they'll be displayed by the location module
  foreach($location as $key=>$value){
    if($value[0] == 'Latitude'){
      $location[$key][1] = theme('location_latitude_dms', $value[1]);      
    }
    if($value[0] == 'Longitude'){
      $location[$key][1] = theme('location_longitude_dms', $value[1]);
    }
  }
  //$location[] = array('Something',theme('location', $node->location));
  $output = '<div class="darwincore">';
  $output .= _theme_darwincore_full_page_help($location, "Location");
  //$output .= theme('location', $node->location);
  $output .= '</div>';
  return $output;
}

function _theme_darwincorelocation_help($path){
  $countries = location_get_iso3166_list();
  $location = array(
    array("Continent Ocean",_darwincore_get_taxonomicname($path->continentocean)),
    array("Island Group",$path->islandgroup),
    array("Island",$path->island),
    array("State Province",$path->stateprovince),
    array("County",$path->county),
    array("Locality",$path->locality),
    array("Geodeticdatum",$path->geodeticdatum),
    array("Verbatim Coordinate System",$path->verbatimcoordinatesystem),
    array("Georeference Protocol",$path->georeferenceprotocol),
    array("Coordinate Uncertainty",$path->coordinateuncertainty),
    array("Georeference Remarks",$path->georeferenceremarks),
    array("Minimum Elevation",$path->minelevation),
    array("Elevation",$path->maxelevation),
    array("Minimum Depth",$path->mindepth),
    array("Maximum Depth",$path->maxdepth),
    array("Latitude",$path->locations[0]['latitude']),
    array("Longitude",$path->locations[0]['longitude']),
    array("Country Code",t($countries[$path->locations[0]['country']]))
  );
  return $location;  
}
/**
 * Custom theme for displaying the table of data
 */
function theme_darwincore_full_page($node) {
  if(isset($node->location) && is_numeric($node->location)){
    $location_node = node_load($node->location);
    $location = _theme_darwincorelocation_help($location_node);
  }
  $collection = array(
    array("Type Status",_darwincore_get_taxonomicname($node->typestatus)),
    array("Institution Code",$node->institutioncode),
    array("Collection Code",$node->collectioncode),
    array("Catalogue Number Text",$node->cataloguenumbertext),
    array("Collector Number",$node->collectornumber),
    array("Field Number",$node->fieldnumber),
    array("Collector",$node->collector)
  );  
  if($node->earliestdatecollected){
    if($node->latestdatecollected){
      $collection[] = array("Earliest Date Collected",format_date(strtotime($node->earliestdatecollected), 'custom', 'j F Y'));
    } else {
      $collection[] = array("Date Collected",format_date(strtotime($node->earliestdatecollected), 'custom', 'j F Y'));    
    }
  }
  if($node->latestdatecollected)
    $collection[] = array("Latest Date Collected",format_date(strtotime($node->latestdatecollected), 'custom', 'j F Y'));
  $collection[] = array("Count",$node->count);
  $collection[] = array("Field Notes",$node->fieldnotes);
  $misc = array(
    array("Sex",_darwincore_get_taxonomicname($node->sex)),
    array("Life Stage",$node->lifestage),
    array("Gen Bank Number",$node->genbanknum),
    array("Remarks",$node->remarks),
    array("Other Catalogue Numbers",preg_replace("/[\n\r]+/","<br/>",$node->othercataloguenum))  
  );
  $identification = array(
    array("Basis of Record",l(_darwincore_get_taxonomicname($node->basisofrecord),'taxonomy/term/'.$node->basisofrecord)),
    array("Taxonomic Name",l(_darwincore_get_taxonomicname($node->taxonomicname),'taxonomy/term/'.$node->taxonomicname)),
    array("Identification qualifier",$node->identificationqualifier),
    array("Identified By",$node->identifiedby)
  );
  if($node->dateidentified)
    $identification[] = array("Date Identified",format_date(strtotime($node->dateidentified), 'custom', 'j F Y'));
  $output = '<div class="darwincore">';
  $output .= _theme_darwincore_full_page_help($collection, "Collection");
  $output .= _theme_darwincore_full_page_help($identification, "Identification");
  $location_title='';
  if ($node->location){
    $location_title = l(t('Location'),'node/'.$node->location);
  } else {
    $location_title = 'Location';
  }
  if(is_array($location)){
    $output .= _theme_darwincore_full_page_help($location, $location_title);
  }
  $output .= _theme_darwincore_full_page_help($misc, "Miscellaneous");
  $output .= '</div>';
  return $output;
}
function _theme_darwincore_full_page_help($values, $title){  
  $output = '<div style="border: solid 1px black;padding:0;margin:5px;"><div style="display:block; border: solid 4px #dddddd;">';
  $output .= '<h2 style="display:block; background-color: #dddddd; color: black; padding: 5px;margin:0;">'.$title.'</h2>';
  $i=0;
  foreach($values as $value){
    if($value[1] !=''){
      $output .= "<div style=\"display:block;padding: 0;margin:0; height: 25px;";
      if ($i!=0)
        $output .="border-top: dashed 1px #EDF2F2;";
      $output .='">
  <p><b>'.$value[0].':&nbsp;</b>'.$value[1].'</p></div>';
    }    
    $i++;
  }
  $output .= '</div></div>';
  return $output;
}
