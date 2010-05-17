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

  $xml = '<?xml version="1.0" encoding="utf-8" standalone="yes"?>
<?xml-stylesheet type="text/xsl" href="/'.drupal_get_path('module','publication').'/publication.xsl"?>
<Root>
  <article xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:mml="http://www.w3.org/1998/Math/MathML" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" dtd-version="3.0">
    <front>
      <article-meta>
        <title-group>
          <article-title>'.$node->field_publication_title[0]['value'].'</article-title>
        </title-group>
        <contrib-group>
          '._publication_get_users_xml($node, $vid).'
        </contrib-group>
        <abstract>'.htmlspecialchars($node->body).'</abstract>
        <kwd-group>'._publication_get_keyword_xml($node).'</kwd-group>
      </article-meta>
    </front>
    <body>'._publication_get_sections_xml($node, $vid) . _publication_get_fig_xml($node).'
    </body>
    <back>'._publication_get_references($node, $vid).'      
    </back>
  </article>
</Root>';
  return $xml;
}

function _publication_get_references($node, $vid){
  $xml = '';
  foreach($node->field_publication_biblio as $biblio){
    $biblio_node = _publication_get_child_node($biblio['nid'], $vid);
    $xml .= '<ref><element-citation><person-group>';
    $contributors = array_pop($biblio_node->biblio_contributors);
    foreach($contributors as $contributor){
      $xml .= '<name><surname>'.$contributor['lastname'].'</surname><given-names>'.$contributor['firstname'].'</given-names></name>';
    }
    $xml .= '</person-group><year>'.$biblio_node->biblio_year.'</year><article-title>'.$biblio_node->title.
      '</article-title><source>'.$biblio_node->biblio_secondary_title.'</source><volume>'.$biblio_node->biblio_volume.'</volume><fpage>'.$biblio_node->biblio_section.'</fpage></element-citation></ref>';
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
  foreach($node->field_publication_contrib as $user){
    if($user['nid']){
      $user_node = _publication_get_child_node($user['nid'], $vid);
      $user_node_user = user_load(array('uid' => $user_node->uid));
      $user_xml .= '<name>
            <given-names>'.$user_node->field_givennames[0]['value'].'</given-names>
            <surname>'.$user_node->field_familyname[0]['value'].'</surname>
            <aff>'.$user_node->field_institution[0]['value'].'</aff>
            <email>'.$user_node_user->mail.'</email>
          </name>';
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
      $xml .= '<fig-group><caption>'.htmlspecialchars($caption_node->body).'</caption><graphic href="'.url('/', array('absolute' => TRUE)).$image_node->images['_original'].'"></graphic></fig-group>';
    }
  }
  return $xml;
}

/**
 * Generate sections XML
 */
function _publication_get_sections_xml(&$publication_node, $vid, $section_node = FALSE){
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
        $section_xml .= '
      <sec><title>'.$child_node->title.'</title><p>'.htmlspecialchars($child_node->body).'</p>'._publication_get_sections_xml($publication_node, $vid, $child_node).'
      </sec>';
      }
    }
  }
  return $section_xml;  
}

/**
 * Generate XML from an SPM content type
 */
function _publication_get_treatment_sections(&$node, $vid, $tid){
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
    $child_node = _publication_get_child_node($nid, $vid);
    $xml = '';
    $term_fields = $node->term_field_defaults;
    if(isset($node->term_fields[$tid])){
      $term_fields = $node->term_fields[$tid];
    }
    $labels = variable_get('spm_fields', array());
    foreach($term_fields as $key => $term_field){
      $labels_children = $labels[$key];
      foreach($term_field as $term_field_child){
        if($term_field_child && $child_node->$term_field_child){
          $xml .= '<sec><title>'.$labels_children[$term_field_child]['name'].'</title><p>'.$child_node->$term_field_child.'</p></sec>';
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
  $term = array_pop($section_node->taxonomy);
  $term->name = $section_node->title;
  if($section_node->body){
    $section_node->body = '<p>'.htmlentities($section_node->body).'</p>';    
  }
  $treatment_xml .= '
    <taxon-treatment>
      <nomenclature>
        <taxon-name>'.$term->name.'</taxon-name>
        <taxon-author>
          <string-name>'.$term->taxon_author.'</string-name>
        </taxon-author>
        <taxon-status>'.$term->usage.'</taxon-status>
      </nomenclature>
      '.$section_node->body.'
      '._publication_get_treatment_sections($node, $vid, $term->tid).
      _publication_get_sections_xml($node, $vid, $section_node).'
    </taxon-treatment>';
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