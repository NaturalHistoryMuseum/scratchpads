<?php
// $Id: nodewords.inc,v 1.2 2006/09/15 12:24:35 robrechtj Exp $

/**
 * Implementation of hook_node_import_fields().
 */
function nodewords_node_import_fields($type) {
  if (user_access('administer meta tags')) {
    return array(
      'nodewords_abstract' => t('Meta tags: abstract'),
      'nodewords_copyright' => t('Meta tags: copyright'),
      'nodewords_description' => t('Meta tags: description'),
      'nodewords_geourl' => t('Meta tags: geourl'),
      'nodewords_keywords' => t('Meta tags: keywords'),
      'nodewords_robots' => t('Meta tags: robots'),
    );
  }
}

/**
 * Implementation of hook_node_import_prepare().
 */
function nodewords_node_import_prepare(&$node, $preview = FALSE) {
  //TODO: validate
  $nodewords = array();
  foreach (array('abstract', 'copyright', 'description', 'geourl', 'keywords', 'robots') as $field) {
    $fieldname = 'nodewords_'. $field;
    if (isset($node->$fieldname)) {
      $nodewords[$field] = $node->$fieldname;
      unset($node->$fieldname);
    }
  }
  if (!empty($nodewords)) {
    $node->nodewords = $nodewords;
  }
}
