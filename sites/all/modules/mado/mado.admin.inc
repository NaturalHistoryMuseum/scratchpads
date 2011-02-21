<?php

/**
 * Save the theme for all, or just the one page
 */
function mado_apply_to_all(){
  // I'm using this funciton to also delete the ident for a page so that it can
  // use the default.
  $ident_parts = explode("/", $_GET['ident']);
  if(arg(2) == 'deletethis'){
    db_query("DELETE FROM {mado} WHERE identifier = '%s'", $_GET['ident']);
  }else{
    $row = db_fetch_array(db_query("SELECT * FROM {mado} WHERE identifier = '%s'", $_GET['ident']));
    if($row){
      db_query("DELETE FROM {mado} WHERE identifier = '%s'", $ident_parts[0]);
      db_query("INSERT INTO {mado} (identifier, blocks) VALUES ('%s','%s')", $ident_parts[0], $row['blocks']);
      // Also delete the shit for this ident, as then we can update it from any
      // other page.
      db_query("DELETE FROM {mado} WHERE identifier = '%s'", $_GET['ident']);
      // Finally, if we've got override all set, then we want to delete all other
      // set pages.
      if(arg(2) == 'override'){
        db_query("DELETE FROM {mado} WHERE identifier LIKE '%s%%'", $ident_parts[0] . '/');
      }
    }
  }
  // Clear the cache when saving for all.
  cache_clear_all('mado-' . $ident_parts[0] . '-', 'cache', TRUE);
  drupal_goto('taxonomy/term/' . $ident_parts[1]);
}

/**
 * Save the theme - ajax callback
 */
function mado_save_layout(){
  if(isset($_GET['blocks'])){
    module_load_include('theme-functions.inc', 'mado');
    $blocks = array();
    $parts = explode("/", $_GET['identifier']);
    if($parts[0] == 'frontpage'){
      $current_blocks = mado_get_theme_front_page();
      $all_blocks = mado_get_blocks(TRUE);
    }else{
      $current_blocks = mado_get_theme($parts[0], $parts[1]);
      $all_blocks = mado_get_blocks();
    }
    $divider_passed = FALSE;
    foreach($_GET['blocks'] as $block){
      $block = str_replace("-slash-", "/", $block);
      if(isset($current_blocks[$block])){
        $blocks[$block] = $current_blocks[$block];
      }else{
        $blocks[$block] = $all_blocks[$block];
      }
      if(isset($blocks[$block]['#hidden']) && !$divider_passed){
        unset($blocks[$block]['#hidden']);
      }
      if($block == 'divider'){
        $divider_passed = TRUE;
        break;
      }
    }
    db_query("DELETE FROM {mado} WHERE identifier = '%s'", $_GET['identifier']);
    db_query("INSERT INTO {mado} (identifier, blocks) VALUES ('%s', '%s')", $_GET['identifier'], serialize($blocks));
  }else if(isset($_GET['resize'])){
    $_GET['resize'] = str_replace("-slash-", "/", $_GET['resize']);
    module_load_include('theme-functions.inc', 'mado');
    if($_GET['identifier'] == 'frontpage'){
      $identifier = array(
        'identifier' => 'frontpage'
      );
      $blocks = mado_get_theme_front_page();
      // Need to insert something here, as the following query is an update!
      @db_query("INSERT INTO {mado} (identifier, blocks) VALUES ('frontpage','%s')", serialize(array()));
    }else{
      $identifier_parts = explode("/", $_GET['identifier']);
      // Lets find out if the layout is set for nothing, or everything, or just
      // one page
      $identifier = db_fetch_array(db_query("SELECT identifier FROM {mado} WHERE identifier = '%s' OR identifier = '%s' ORDER BY identifier DESC", $identifier_parts[0], $_GET['identifier']));
      $blocks = mado_get_theme($identifier_parts[0], $identifier_parts[1]);
    }
    $blocks[$_GET['resize']]['#scroll'] = $_GET['scroll'];
    $blocks[$_GET['resize']]['#css'] = str_replace(" mado_block", "", $_GET['class']);
    if($identifier){
      db_query("UPDATE {mado} SET blocks = '%s' WHERE identifier = '%s'", serialize($blocks), $identifier['identifier']);
    }else{
      db_query("INSERT INTO {mado} (identifier, blocks) VALUES ('%s', '%s')", $_GET['identifier'], serialize($blocks));
    }
  }
  // Clear the cache as we've started saving the mado theme to the cache!
  cache_clear_all('mado', 'cache', TRUE);
}