<?php
/**
 * Following function is barely changed from the original, but just to make things simpler, it's recreated here.
 */
function leftandright_overview_terms($vid){
  // Lets make sure this vocabulary wants to use leftandright
  if(!leftandright_use_leftandright($vid)){
    $vocabulary = taxonomy_get_vocabularies();
    module_load_include("admin.inc", "taxonomy");
    return drupal_get_form('taxonomy_overview_terms',$vocabulary[$vid]);
  }
  $header = array(t('Name'), t('Operations'));
  $vocabulary = taxonomy_vocabulary_load($vid);
  if (!$vocabulary) {
    return drupal_not_found();
  }
  
  $destination = drupal_get_destination();

  drupal_set_title(check_plain($vocabulary->name));
  $start_from      = $_GET['page'] ? $_GET['page'] : 0;
  $total_entries   = 0;  // total count for pager
  $page_increment  = 25; // number of tids per page
  $displayed_count = 0;  // number of tids shown

  if ($vocabulary->tags) {
    // We are not calling taxonomy_get_tree because that might fail with a big
    // number of tags in the freetagging vocabulary.
    $results = pager_query(db_rewrite_sql('SELECT t.*, h.parent FROM {term_data} t INNER JOIN  {term_hierarchy} h ON t.tid = h.tid WHERE t.vid = %d ORDER BY weight, name', 't', 'tid'), $page_increment, 0, NULL, $vocabulary->vid);
    while ($term = db_fetch_object($results)) {
      $rows[] = array(
        l($term->name, "taxonomy/term/$term->tid"),
        l(t('edit'), "admin/content/taxonomy/edit/term/$term->tid", array('query' => $destination)),
      );
    }
  }
  else {
    $total_entries = leftandright_get_tree_count($vocabulary->vid);
    //$tree = taxonomy_get_tree($vocabulary->vid);
    $tree = leftandright_get_tree($vocabulary->vid, 0,NULL,$start_from*$page_increment,$page_increment);
    foreach ($tree as $term) {
      $total_entries++; // we're counting all-totals, not displayed
      if (($start_from && ($start_from * $page_increment) >= $total_entries) || ($displayed_count == $page_increment)) {
        continue;
      }
      $rows[] = array(str_repeat('&nbsp;&nbsp;&nbsp;&nbsp;', $term->depth) .' '. l($term->name, "taxonomy/term/$term->tid"), l(t('edit'), "admin/content/taxonomy/edit/term/$term->tid", array('query' => $destination)));
      $displayed_count++; // we're counting tids displayed
    }
    if (!$total_entries) {
      $rows[] = array(array('data' => t('No terms available.'), 'colspan' => '2'));
    }
    $GLOBALS['pager_page_array'][] = $start_from;
    $GLOBALS['pager_total'][] = intval($total_entries / $page_increment);
  }
  $output .= theme('table', $header, $rows, array('id' => 'taxonomy'));
  if ($vocabulary->tags || $total_entries >= $page_increment) {
    $output .= theme('pager', NULL, $page_increment);
  }
  return $output;
  
}
/**
 * autotag settings page
 */
function leftandright_admin_settings() {
  $form['leftandright_minimum_terms'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum number of terms required to use Leftandright module'),
    '#default_value' => variable_get('leftandright_minimum_terms', 1000)
  );
  return system_settings_form($form);
}
/**
 * List and manage vocabularies.
 */
function leftandright_overview_vocabularies() {
  $rebuilding = leftandright_trees_rebuilding();
  $above_table = false;
  if(count($rebuilding)){
    $processes = variable_get('leftandright_rebuild_pids',array());
    $current_pids = array();
    foreach($rebuilding as $pid=>$vid){
      // Check that this process was started by this site
      if(in_array($pid,$processes)){
        if(!$above_table){
          $above_table = '<p><strong>'.t('Currently rebuilding the following vocabularies:').'</strong></p><ul>';
        }
        $vocabulary = taxonomy_vocabulary_load($vid);
        $above_table .='<li>'.check_plain($vocabulary->name).'</li>';
        $current_pids[] = $pid;
        $display_above_table = true;
      }
    }
    variable_set('leftandright_rebuild_pids',$current_pids);
    $above_table .= '</ul>';
  }
  $vocabularies = taxonomy_get_vocabularies();
  $rows = array();
  foreach ($vocabularies as $vocabulary) {
    if(array_search($vocabulary->vid,$rebuilding)){
      $rebuild = t('This tree is currently being rebuilt');
    } else {
      if(leftandright_use_leftandright($vocabulary->vid)){
        $rebuild = l(t('rebuild tree'), 'leftandright/update/'.$vocabulary->vid);
      } else {
        $rebuild = t('This vocabulary has fewer terms than is required (@leftandright_number)', array('@leftandright_number'=>variable_get('leftandright_minimum_terms',1000)));
      }
    }
    $rows[] = array(
      'name' => check_plain($vocabulary->name),
      'rebuild' => $rebuild
    );
  }
  if (empty($rows)) {
    $rows[] = array(array('data' => t('No categories available.'), 'colspan' => '5'));
  }
  $header = array(t('Name'),t('Rebuild Tree'));

  return $above_table . theme('table', $header, $rows, array('id' => 'taxonomy'));
}