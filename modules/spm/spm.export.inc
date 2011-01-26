<?php

/**
 * Function to start the export process
 */
function spm_export(){
  // We need to get the list of nodes to export and send it to the 
  // spm_export_nodes function
  $nodes = variable_get('spm_nodes', FALSE);
  if(is_array($nodes)){
    foreach($nodes as $key => $value){
      if(!$value){
        unset($nodes[$key]);
      }
    }
    if(!count($nodes)){
      $nodes = FALSE;
    }
  }
  spm_export_nodes($nodes);
}

/**
 * SPM Settings form
 */
function spm_settings(){
  $results = db_query("SELECT nid, title FROM {node} WHERE type = 'spm' ORDER BY title");
  while($row = db_fetch_array($results)){
    $options[$row['nid']] = l($row['title'], 'node/'.$row['nid']);
  }
  $form['spm_nodes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select the Taxon Pages you would like to export'),
    '#default_value' => variable_get('spm_nodes', array()),
    '#options' => $options,
    '#description' => '<p>' . t('Select each of the Taxon Pages that you would like to be made available as an export to EOL.  By not checking any Taxon Page, all will be exported.') . '</p><p>' . t('If you\'d like your Taxon Pages to be made available to EOL, you must also <a href="!eol_url">register</a> this site\'s endpoint <b>( !endpoint_url )</b> with the Encyclopædia of Life.', array(
      '!eol_url' => 'https://www.eol.org/content_partner/register?method=get',
      '!endpoint_url' => url('spm/export.xml', array(
        'absolute' => TRUE
      ))
    )) . '</p>'
  );
  return system_settings_form($form);
}

/**
 * Take a list of nids, so that we can specify which nodes to export.
 */
function spm_export_nodes($nids = FALSE){
  if(!$nids){
    // We need to get all the SPM nodes, so that we can export the data.
    $results = db_query("SELECT vid FROM {node} WHERE type = 'spm' AND status > 0");
    while($vid = db_result($results)){
      $vids[] = $vid;
    }
  }else{
    // We need to convert  the array of nids to an array of vids
    $results = db_query('SELECT vid FROM {node} WHERE nid IN (%s) AND status > 0', implode(",", $nids));
    while($vid = db_result($results)){
      $vids[] = $vid;
    }
  }
  if(count($vids)){
    //drupal_set_header('Content-Type: application/xml');
    $headers = variable_get('spm_headers', array());
    $fields = variable_get('spm_fields', array());
    foreach($vids as $vid){
      $spm_array = db_fetch_array(db_query('SELECT * FROM {spm} WHERE vid = %d', $vid));
      $node = node_load(array(
        'vid' => $vid
      ));      
      $xml_array_child = array(
        'element_name' => 'taxon',
        'dc:identifier' => 'scratchpad:nid:' . $node->nid,
        'dc:source' => url('node/' . $node->nid, array(
          'absolute' => TRUE
        )),
        'dwc:ScientificName' => $node->title,        
      );
      $references = array();
      // Get the tids that this SPM node is tagged with - if there is more than
      // one, then we can't do anything (not really).
      if(db_result(db_query('SELECT COUNT(*) FROM {term_node} WHERE vid = %d', $vid)) == 1){
        $tid = db_result(db_query('SELECT tid FROM {term_node} WHERE vid = %d', $vid));
        // Get the biblio nodes associated with this taxon
        $results = db_query("SELECT n.vid FROM {node} n, {term_node} t WHERE t.vid = n.vid AND type = 'biblio' AND tid = %d", $tid);
        while($row = db_fetch_array($results)){
          $biblio_node = node_load(array(
            'vid' => $row['vid']
          ));
          $xml_array_child[] = array(
            'element_name' => 'reference',
            'attributes' => array('doi' => $biblio_node->biblio_doi),
            $biblio_node->title
          );
        }
      }
      unset($spm_array['nid']);
      unset($spm_array['vid']);
      foreach($spm_array as $key => $value){
        if($value != ''){
          $field_info = array();
          foreach($headers as $header_key => $header_value){
            foreach($fields[$header_key] as $field_key => $field_value){
              if($field_key == $key){
                $field_info = $field_value;
                break (2);
              }
            }
          }
          $xml_array_child[] = array(
            'element_name' => 'dataObject',
            'dataType' => 'http://purl.org/dc/dcmitype/Text',
            'agent' => array(
              'attributes' => array(
                'role' => 'author'
              ),
              url('user/' . $node->uid, array(
                'absolute' => TRUE
              ))
            ),
            'dcterms:created' => $node->created,
            'dcterms:modified' => $node->changed,
            'dc:title' => $field_info['name'],
            'license' => 'http://creativecommons.org/licenses/by-nc/3.0/',
            'dc:source' => url('node/' . $node->nid, array(
              'absolute' => TRUE
            )),
            'subject' => 'http://rs.tdwg.org/ontology/voc/SPMInfoItems#' . $key,
            'dc:description' => array(
              'attributes' => array(
                'xml:lang' => 'en'
              ),
              htmlentities($value)
            )
          );
        }
      }
      // Get the tids that this SPM node is tagged with - if there is more than
      // one, then we can't do anything (not really).
      if(db_result(db_query('SELECT COUNT(*) FROM {term_node} WHERE vid = %d', $vid)) == 1){
        $tid = db_result(db_query('SELECT tid FROM {term_node} WHERE vid = %d', $vid));
        // Do the image nodes first
        $results = db_query("SELECT n.vid FROM {node} n, {term_node} t WHERE t.vid = n.vid AND type = 'image' AND tid = %d", $tid);
        while($row = db_fetch_array($results)){
          $node = node_load(array(
            'vid' => $row['vid']
          ));
          foreach($node->field_imagefield as $image){
            $xml_array_child[] = array(
              'element_name' => 'dataObject',
              'dataType' => 'http://purl.org/dc/dcmitype/StillImage',

              'agent' => array(
                'attributes' => array(
                  'role' => 'author'
                ),
                url('user/' . $node->uid, array(
                  'absolute' => TRUE
                ))
              ),

              'dcterms:created' => $node->created,

              'dcterms:modified' => $node->changed,

              'license' => 'http://creativecommons.org/licenses/by-nc/3.0/',

              'dc:source' => url('node/' . $node->nid, array(
                'absolute' => TRUE
              ))

              'mediaURL' => url(NULL, array(
                'absolute' => TRUE
              )) . $image['filepath'],

            );
          }
        }
      }
      $xml_array[] = $xml_array_child;
    }
    module_load_include('xml.class.php', 'spm');
    $xml = new EOLXML();
    $xml->fromArray($xml_array);
    $xml->output();
  }
}
