<?php
// $Id: apachesolr_multisitesearch.admin.inc,v 1.7 2010/09/03 02:27:00 pwolanin Exp $

/**
 * @file
 *   Provides a multi-site search admin pages and functionality
 */

function apachesolr_multisitesearch_metadata() {
  $document = new Apache_Solr_Document();
  $document->id = apachesolr_document_id(0, 'multisite_metadata');
  $document->site = url(NULL, array('absolute' => TRUE));
  $document->hash = apachesolr_site_hash();
  $document->entity = 'multisite_meta';
  $document->ss_multisite_meta_sitename = variable_get('site_name', 'Drupal');
  module_load_include('inc', 'apachesolr', 'apachesolr.index');
  if (module_exists('taxonomy')) {
    $vocabs = taxonomy_get_vocabularies();
    foreach ($vocabs as $vid => $vocab) {
          // We index each name as a string for cross-site faceting
        // using the vocab name rather than vid in field construction.
        $document->setMultiValue('sm_multisite_meta_taxonomy', apachesolr_vocab_name($vid));
    }
  }
  drupal_alter('apachesolr_multisitesearch_metadata', $document);
  return $document;
}

function apachesolr_multisitesearch_update_metadata() {
  try {
    // Get the $solr object
    $solr = apachesolr_get_solr();
    $metadata_doc = apachesolr_multisitesearch_metadata();
    $solr->addDocuments(array($metadata_doc));
    watchdog('Apache Solr Multisite', 'Updated site meta data');
    return TRUE;
  }
  catch (Exception $e) {
    watchdog('Apache Solr Multisite', 'Indexing failed for meta data <br /> !message',
             array('!message' => nl2br(strip_tags($e->getMessage()))), WATCHDOG_ERROR);
  }
  return FALSE;
}

function apachesolr_multisitesearch_get_metadata() {
  try {
    // Get the $solr object
    $solr = apachesolr_get_solr();
    $solr->setCollapseSingleValueArrays(FALSE);
    $params['qt'] = 'standard';
    $params['fl'] = '*';
    $response = $solr->search('entity:multisite_meta', 0, 1000, $params);
    foreach ($response->response->docs as $doc) {
      // Convert doc into a simple array.
      if (isset($doc->hash)) {
        foreach ($doc as $k => $v) {
          $data[$doc->hash][$k] = $v;
        }
        if (empty($data[$doc->hash]['sm_multisite_meta_taxonomy'])) {
          $data[$doc->hash]['sm_multisite_meta_taxonomy'] = array();
        }
      }
    }
    watchdog('Apache Solr Multisite', 'Fetched site meta data');
    variable_set('apachesolr_multisitesearch_metadata', $data);
  }
  catch (Exception $e) {
    watchdog('Apache Solr Multisite', 'Failed to fetch meta data <br /> !message',
             array('!message' => nl2br(strip_tags($e->getMessage()))), WATCHDOG_ERROR);
  }
}

function apachesolr_multisitesearch_get_site_hashes() {
  try {
    // Get the $solr object
    $solr = apachesolr_get_solr();
    $solr->setCollapseSingleValueArrays(FALSE);
    $params['qt'] = 'standard';
    $params['fl'] = '';
    $params['facet'] = 'true';
    $params['facet.field'][] = 'hash';
    $params['facet.mincount'] = 1;
    $params['facet.limit'] = '1000';
    $response = $solr->search('*:*', 0, 0, $params);
    $results = (array)$response->facet_counts->facet_fields->hash;
    return $results;
  }
  catch (Exception $e) {
    watchdog('Apache Solr Multisite', 'Failed to fetch hash facet count <br /> !message',
             array('!message' => nl2br(strip_tags($e->getMessage()))), WATCHDOG_ERROR);
  }
}

/**
 * This is the submit handler for the active facets form.
 *
 * The form values for each module are array filtereed to remove non-enabled items and
 * stored in the variable table with the name 'apachesolr_enabled_facets'.
 *
 * @see apachesolr_multisitesearch_enabled_facets_form()
 */
function apachesolr_multisitesearch_enabled_facets_form_submit($form, &$form_state) {
  $enabled = array();
  foreach ($form_state['values']['apachesolr_multisitesearch_enabled_facets'] as $module => $facets) {
    $enabled[$module] = array_filter($facets);
  }
  variable_set('apachesolr_multisitesearch_enabled_facets', $enabled);
  drupal_set_message($form_state['values']['submit_message'], 'warning');
}

/**
 * Indicates what order the specified facets should be listed in.  This function is used in a usort
 * invocation.
 * @param $a
 *   The first facet.
 * @param $b
 *   The second facet.
 * @return
 *   A signed integer that indicates which of the specified facets should come first.
 */
function _apachesolr_multisitesearch_sort_facets($a, $b) {
  return strcasecmp($a['info'], $b['info']);
}

/**
 * Creates the form that allows the user to select which facets will be enabled.
 *
 * Only enabled facets are sent to solr.  Fewer enabled facets can reduce the
 * load on the search server.  Blocks are only offered for enabled facets, so
 * this also reduces the clutter on the blocks admin page.
 */
function apachesolr_multisitesearch_enabled_facets_form() {
  $form = array();
  $facets = array();
  $module_facets = array();
  $module_list = array();

  foreach (module_implements('apachesolr_multisitesearch_facets') as $module) {
    $module_facets[$module] = module_invoke($module, 'apachesolr_multisitesearch_facets', TRUE);
    uasort($module_facets[$module], '_apachesolr_multisitesearch_sort_facets');
    $module_list[$module] = $module;
  }

  $enabled_facets = apachesolr_multisitesearch_enabled_facets();
  $form = array();
  $form['apachesolr_multisitesearch_enabled_facets']['help'] = array (
    '#type' => 'item',
    '#value' => t('You can use this screen to select which search filter blocks should be created by enabling the corresponding filters on this page. For performance reasons, you should only enable filters that you intend to have available to users on the search page.  After selecting which filter blocks to create, you will be sent to the blocks page where you can choose which of those blocks should be enabled when your users search by placing each block in a region.'),
  );
  if ($module_list) {
    $placeholders = implode(', ', array_fill(0, count($module_list), "'%s'"));
    $result = db_query("SELECT name, info FROM {system} WHERE name IN (". $placeholders .") AND type = 'module'", $module_list);
    while ($item = db_fetch_array($result)) {
      $module_list[$item['name']] = unserialize($item['info']);
    }
  }
  foreach($module_facets as $module => $facets) {
    $form['apachesolr_multisitesearch_enabled_facets'][$module] = array(
      '#type' => 'fieldset',
      '#title' => check_plain($module_list[$module]['name']),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    // We must use module + delta as the keys since that combination is
    // guaranteed to be unique.  A single module could, for example, have
    // two different blocks that expose different faceting on the same
    // field in the index.
    foreach($facets as $delta => $data) {
      $form['apachesolr_multisitesearch_enabled_facets'][$module][$delta] = array(
        '#type' => 'checkbox',
        '#title' => $data['info'],
        '#return_value' => $data['facet_field'],
        '#default_value' => isset($enabled_facets[$module][$delta]) ? $data['facet_field'] : 0,
      );
    }
  }

  $has_facets = (bool)$module_facets;

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#access' => $has_facets,
  );
  $form['no-facets-message'] = array(
    '#value' => t('<em>No filters are available from your currently enabled modules</em>'),
    '#access' => !$has_facets,
  );


  $form['#tree'] = TRUE;
  $form['submit_message'] = array(
    '#type' => 'value',
    '#value' => t('The Apache Solr Multisite Search filter settings were changed.  To arrange the blocks for your enabled filters, visit the <a href="@url">blocks administration page</a>.', array('@url' => url('admin/build/block'))),
  );
  $form['admin'] = array(
    '#type' => 'fieldset',
    '#title' => t('Administrative functions'),
  );
  $form['admin']['refresh'] = array(
    '#type' => 'submit',
    '#value' => t('Refresh metadata now'),
    '#prefix' => '<p>' . t('Multisite metadata is used to communicate between all of the sites in a multisite setup. If site names are not showing properly in the search results and facet blocks try refreshing the metadata. Metadata is also refreshed periodically on cron runs.') . '</p>',
    '#submit' => array('apachesolr_multisitesearch_refresh_metadata_now'),
  );
  
  // Use the metadata and a list of all the hashes in the index
  // to build up checkboxes for deleting site indexes.
  // This is only necessary because sometimes hashes get
  // stranded in the index and deleting the index from the normal
  // admin screen doesn't rectify the problem.
  $metadata = variable_get('apachesolr_multisitesearch_metadata', array());
  $hashes = apachesolr_multisitesearch_get_site_hashes();
  foreach ($hashes as $hash => $count) {
    if ($hash == apachesolr_site_hash()) {
      $options[$hash] = t('This site (!site, !count documents)', array('!site' => variable_get('site_name', 'Drupal'), '!count' => $count));
    }
    elseif (!empty($metadata[$hash])) {
      $options[$hash] = $metadata[$hash]['site'] . ' ' . t('(!count documents)', array('!count' => $count));
    }
    else {
      $options[$hash] = $hash . ' ' . t('(!count documents)', array('!count' => $count));
    }
  }
  
  if (count($options) > 0) {
    $form['admin']['delete']['hashes'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Delete data from sites using this index'),
      '#options' => $options,
      '#description' => t('If you end up in a situation where the index has hashes that no longer map to real active sites, use this option to delete the outdated data. If you delete another site\'s index from this site, that site will have to first trigger a re-index, and then run cron until the index is rebuilt.'),
    );
    $form['admin']['delete']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Delete selected indexes'),
      '#submit' => array('apachesolr_multisitesearch_delete_indexes'),
    );
  }
  return $form;
}

/**
 * Submit handler for the "Delete selected indexes" button.
 */
function apachesolr_multisitesearch_delete_indexes($form, &$form_state) {
  // Instantiate a new Solr object.
  $solr = apachesolr_get_solr();
  foreach ($form_state['values']['admin']['delete']['hashes'] as $hash) {
    if ($hash) {
      $query = "hash:$hash";
      $solr->deleteByQuery($query);
      drupal_set_message(t('The index for !hash has been deleted.', array('!hash' => $hash)));
      if (apachesolr_site_hash() == $hash) {
        // Rebuild our node-tracking table.
        apachesolr_rebuild_index_table();
        apachesolr_index_updated(time());
      }
    }
  }
  $solr->commit();
  apachesolr_multisitesearch_get_metadata();
}

/**
 * Submit handler for the "Refresh metadata now" button.
 */
function apachesolr_multisitesearch_refresh_metadata_now() {
  variable_del('apachesolr_multisitesearch_last_metadata_update');
  variable_del('apachesolr_multisitesearch_last_metadata_fetch');
  apachesolr_multisitesearch_refresh_metadata();
  drupal_set_message(t('The metadata has been refreshed.'));
}

function apachesolr_multisitesearch_rebuild_facets() {
  $facets = array();

  $facets['type'] = array(
    'info' => t('Apache Solr Multisite Search: Filter by content type'),
    'facet_field' => 'type',
  );

  $facets['name'] = array(
    'info' => t('Apache Solr Multisite Search: Filter by author'),
    'facet_field' => 'name',
  );

  $facets['changed'] = array(
    'info' => t('Apache Solr Multisite Search: Filter by updated date'),
    'facet_field' => 'changed',
  );
  $facets['created'] = array(
    'info' => t('Apache Solr Multisite Search: Filter by post date'),
    'facet_field' => 'created',
  );
  $facets['hash'] = array(
    'info' => t('Apache Solr Multisite Search: Filter by site'),
    'facet_field' => 'hash',
  );
  $this_site_hash = apachesolr_site_hash();
  // Get taxonomy vocabulary facets.
  // @todo - use meta data also/instead.
  if (module_exists('taxonomy')) {
    module_load_include('inc', 'apachesolr', 'apachesolr.index');
    $data = variable_get('apachesolr_multisitesearch_metadata', array());
    $taxo = array();
    foreach ($data as $hash => $site) {
      if ($hash == $this_site_hash) {
        continue;
      }
      foreach ($site['sm_multisite_meta_taxonomy'] as $name) {
        $delta = 'sm_vid_'. $name;
        $taxo[$delta]['name'] = $name;
        $taxo[$delta]['sites'][] = $site['ss_multisite_meta_sitename'];
      }
    }
    $vocabs = taxonomy_get_vocabularies();
    foreach ($vocabs as $vid => $vocab) {
      // In this case the delta and facet field are the same.
      $delta = 'sm_vid_'. apachesolr_vocab_name($vid);
      $name = $vocab->name;
      if (isset($taxo[$delta])) {
        $name .= t(' (this site and !sites)', array('!sites' => implode(', ', $taxo[$delta]['sites'])));
        unset($taxo[$delta]);
      }
      $facets[$delta] = array(
        'info' => t('Apache Solr Multisite Search: Filter by taxonomy @name', array('@name' => $name)),
        'facet_field' => $delta,
      );
    }
    if (variable_get('apachesolr_multisitesearch_include_remote_taxonomies', TRUE)) {
      // Handle taxonomies only present at other sites.
      foreach ($taxo as $delta => $data) {
        $name = $data['name'] . ' (' . implode(', ', $data['sites']) . ')';
        $facets[$delta] = array(
          'info' => t('Apache Solr Multisite Search: Filter by taxonomy @name', array('@name' => $name)),
          'facet_field' => $delta,
        );
      }
    }
  }
  cache_set('apachesolr_multisitesearch:facets', $facets, 'cache_apachesolr');
  return $facets;
}
