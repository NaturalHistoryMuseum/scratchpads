<?php 

function publication_display_xml($node, $vid = FALSE){
  drupal_set_header('Content-Type: application/xml');
  echo publication_get_xml($node->nid, $vid);
}
/**
 * Generate an XML document according to the PubTax schema.
 */
function publication_get_xml($nid, $vid=NULL){
  // Firstly, we'll get the node to be exported.
  
  $node = node_load($nid, NULL, TRUE);

  $xml = '<article dtd-version="3.0" xmlns:tp="http://www.plazi.org/taxpub">  
  <front>
    <article-meta>
      <title-group>
        <article-title>'.$node->field_publication_title[0]['value'].'</article-title>
      </title-group>
      <contrib-group>
        '._publication_get_users_xml($node, $vid).'
      </contrib-group>
      <abstract><label>Abstract</label>'.trim(_filter_autop($node->body)).'</abstract>
      <kwd-group>'._publication_get_keyword_xml($node).'</kwd-group>
    </article-meta>
  </front>
  <body>
    '.trim(_publication_get_sections_xml($node, $vid)) .'
    '. trim(_publication_get_fig_xml($node)).'
  </body>
  <back>
    <ack>
      <title>Acknowledgements</title>
      '.trim(_filter_autop($node->field_publication_ack)).'
    </ack>
    <ref-list>
      '.trim(_publication_get_references($node, $vid)).'
    </ref-list>
    '.trim(_filter_autop($node->field_publication_supplementary)).'      
  </back>
</article>';
  return $xml;
}

function _publication_get_references($node, $vid){
  $xml = '';
  foreach($node->field_publication_biblio as $biblio){
    $biblio_node = _publication_get_child_node($biblio['nid'], $vid);
    $xml .= '
    <ref>
      <element-citation>
        <person-group>';
    $contributors = array_pop($biblio_node->biblio_contributors);
    foreach($contributors as $contributor){
      $xml .= '
      <name>
        <surname>'.$contributor['lastname'].'</surname>
        <given-names>'.$contributor['firstname'].'</given-names>
      </name>';
    }
    $xml .= '
    </person-group>
      <year>'.$biblio_node->biblio_year.'</year>
      <article-title>'.htmlspecialchars($biblio_node->title).'</article-title>
      <source>'.$biblio_node->biblio_secondary_title.'</source>
      <volume>'.$biblio_node->biblio_volume.'</volume>
      <fpage>'.$biblio_node->biblio_section.'</fpage>
    </element-citation>
    </ref>';
  }
  return $xml;
}

function _publication_get_keyword_xml(&$node){
  $keywords = preg_split("/[\n\r]+/", $node->field_publication_keywords[0]['value']);
  $xml = '';
  foreach($keywords as $keyword){
    if($keyword != ''){
      $xml .= "<kwd>$keyword</kwd>";
    }
  }
  return $xml;
}

/**
 * Generate Users XML
 * 
 * NOTE, 
 */
function _publication_get_users_xml(&$node, $vid){
  $user_xml = '';
  foreach($node->field_publication_authors as $user){
    if($user['nid']){
      $user_node = _publication_get_child_node($user['nid'], $vid);
      if($user_node->field_email == ''){
        $user_node_user = user_load(array('uid' => $user_node->uid));
        $user_node->field_email = $user_node_user->mail;
      }
      $user_xml .= '<contrib>
            <name>
              <given-names>'.$user_node->field_givennames[0]['value'].'</given-names>
              <surname>'.$user_node->field_familyname[0]['value'].'</surname>
            </name>
              <aff>'.$user_node->field_institution[0]['value'].'</aff>
              <email>'.$user_node->field_email.'</email>
          </contrib>';
    }
  }
  return $user_xml;
}

/**
 * Get images for embedding at back of document
 */
function _publication_get_fig_xml(&$node){
  $xml = '';
  foreach($node->field_publication_images as $image){
    if(isset($image['nid']) && $image['nid']){
      $image_node = node_load($image['nid']);
      $caption_node = publication_modal_image_caption_node_load($image['nid'], $node->nid);
      $xml .= '
    <fig id="'.$image_node->images['_original'].'">
      <label></label>
      <caption>'.trim(_filter_autop($caption_node->body)).'</caption>
      <graphic href="'.url('/', array('absolute' => TRUE)).$image_node->images['_original'].'"></graphic>
    </fig>';
    }
  }
  return $xml;
}

/**
 * Generate sections XML
 */
function _publication_get_sections_xml(&$publication_node, $vid, $section_node = FALSE, $tag = 'sec', $title = 'title'){
  if($section_node){
    $this_node = $section_node;
  } else {
    $this_node = $publication_node;
  }
  foreach($this_node->field_publication_section as $section){
    if($section['nid']){
      $child_node = _publication_get_child_node($section['nid'], $vid);
      if(isset($publication_node->taxon_treatment_sections[$section['nid']])){
        $section_xml .= _publication_get_treatment_xml($child_node, $publication_node, $vid);
      } else {
        if($child_node->type == 'publication_taxon_description'){
          foreach($this_node->taxonomy as $this_tid => $value){
            $section_xml .= _publication_get_treatment_sections($publication_node, $child_node, $vid, $this_tid);
            break;
          }
        } else if ($child_node->type == 'publication_section') {
        $section_xml .= '
      <'.$tag.'>
        <'.$title.'>'.$child_node->title.'</'.$title.'>'.trim(_filter_autop($child_node->body))._publication_get_sections_xml($publication_node, $vid, $child_node, $tag).'
      </'.$tag.'>';
        }
      }
    }
  }
  return $section_xml;  
}

/**
 * Generate XML from an SPM content type
 */
function _publication_get_treatment_sections(&$node, $section_node, $vid, $tid){
  $results = db_query("SELECT n.nid FROM {node} n, {term_node} t WHERE type = 'spm' AND t.nid = n.nid AND tid = %d", $tid);
  $nids = array();
  while($row = db_fetch_array($results)){
    $nids[] = $row['nid'];
  }
  if($nids){
    $child_nids = og_group_child_nids($node->nid);
    $intersect_nids = array_intersect($nids, $child_nids);
    if($intersect_nids){
      $nid = array_pop($intersect_nids);
    } else {
      $nid = array_pop($nids);
    }
    $term_fields = $node->default_term_fields;
    if($section_node->term_fields){
      $term_fields = $section_node->term_fields;
    }
    $child_node = _publication_get_child_node($nid, $vid);
    $xml = '';
    $labels = variable_get('spm_fields', array());
    foreach($term_fields as $key => $term_field){
      $labels_children = $labels[$key];
      foreach($term_field as $term_field_child){
        if($term_field_child && $child_node->$term_field_child){
          $xml .= '
      <tp:treatment-sec>
        <label>'.$labels_children[$term_field_child]['name'].'</label>
        '.trim(_filter_autop($child_node->$term_field_child)).'
      </tp:treatment-sec>';
        }
      }
    }
    return $xml;
  }
}

/**
 * Generate the special treatment sections - Note, this is currently hardcoded 
 * to use the ITIS content type.
 */
function _publication_get_treatment_xml($section_node, &$node, $vid){
  module_invoke_all('taxonomy_term_load', $section_node->taxonomy);
  foreach($section_node->taxonomy as $value){
    $term = $value;
    break;
  }
  $term->name = $section_node->title;
  $materials = '';
  if($section_node->field_publication_taxon_material[0]['value']){
    $materials = '<treatment-sec type="type-material-guess">
        <label>Type material</label>
        '.trim(_filter_autop($section_node->field_publication_taxon_material[0]['value'])).'
      </treatment-sec>';
  }
  $treatment_xml .= '
    <tp:taxon-treatment>
      <tp:nomenclature>
        <tp:taxon-name>'.$section_node->title.'</tp:taxon-name>
        <tp:taxon-authority>'.$term->taxon_author.$term->description.'</tp:taxon-authority>
        <tp:taxon-status>'.$term->usage.'</tp:taxon-status>
        <tp:nomenclature-citation-list>
          <tp:nomenclature-citation>
            <comment>'.$section_node->field_publication_taxon_citation[0]['value'].'</comment>
          </tp:nomenclature-citation>
        </tp:nomenclature-citation-list>
      </tp:nomenclature>
      '.$materials.'
      '._publication_get_sections_xml($node, $vid, $section_node, 'tp:treatment-sec', 'label').'
    </tp:taxon-treatment>';
  return $treatment_xml;
}

/**
 * Get a specific node version, depending on the version of the main publication
 * node.
 */
function _publication_get_child_node($nid, $publication_vid){
  if($publication_vid){
    // Lets find the vid
    $result = db_query("SELECT vid FROM {node_revisions} WHERE nid = %d AND timestamp <= (SELECT timestamp FROM {node_revisions} WHERE vid = %d) ORDER BY timestamp DESC LIMIT 1", $nid, $publication_vid);
    $vid = db_result($result);
    if($vid){
      return node_load($nid, $vid);
    }
    watchdog('publication', 'Child node revision is missing ('.$nid.' - '.$publication_vid.')', WATCHDOG_ERROR);
  }
  return node_load(array('nid'=>$nid));
}