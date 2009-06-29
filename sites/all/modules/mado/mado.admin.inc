<?php

/**
 * Save the theme for all, or just the one page
 */
function mado_apply_to_all(){
  $row = db_fetch_array(db_query("SELECT * FROM {mado} WHERE identifier = '%s'", $_GET['ident']));
  $ident_parts = explode("/", $_GET['ident']);
  if($row){
    db_query("DELETE FROM {mado} WHERE identifier = '%s'", $ident_parts[0]);
    db_query("INSERT INTO {mado} (identifier, blocks) VALUES ('%s','%s')", $ident_parts[0], $row['blocks']);
    // Also delete the shit for this ident, as then we can update it from any
    // other page.
    db_query("DELETE FROM {mado} WHERE identifier = '%s'", $_GET['ident']);
  }
  drupal_goto('taxonomy/term/'.$ident_parts[1]);
}

/**
 * Save the theme - ajax callback
 */
function mado_save_layout(){
  if (isset ( $_GET ['blocks'] )) {
    module_load_include('theme-functions.inc','mado');
    $blocks = array ();
    $all_blocks = mado_get_blocks ();
    $parts = explode("/", $_GET['identifier']);
    $current_blocks = mado_get_theme($parts[0], $parts[1]);
    $divider_passed = FALSE;
    foreach ( $_GET ['blocks'] as $block ) {
      if (isset ( $current_blocks [$block] )) {
        $blocks [$block] = $current_blocks [$block];
      } else {
        $blocks [$block] = $all_blocks [$block];
      }    
      if(isset($blocks[$block]['#hidden']) && !$divider_passed){
        unset($blocks[$block]['#hidden']);
      }
      if ($block == 'divider') {
        $divider_passed = TRUE;
        break;
      }
    }
    db_query ( "DELETE FROM {mado} WHERE identifier = '%s'", $_GET ['identifier'] );
    db_query ( "INSERT INTO {mado} (identifier, blocks) VALUES ('%s', '%s')", $_GET ['identifier'], serialize ( $blocks ) );
  } else if (isset ( $_GET ['resize'] )) {
    $identifier_parts = explode("/", $_GET['identifier']);
    $blocks = mado_get_theme ($identifier_parts[0], $identifier_parts[1]);
    $blocks [$_GET ['resize']] ['#scroll'] = $_GET ['scroll'];
    $blocks [$_GET ['resize']] ['#css'] = str_replace ( " mado_block", "", $_GET ['class'] );
    // Lets find out if the layout is set for nothing, or everything, or just
    // one page
    $identifier = db_fetch_array(db_query("SELECT identifier FROM {mado} WHERE identifier = '%s' OR identifier = '%s' ORDER BY identifier DESC", $identifier_parts[0], $_GET['identifier']));
    if($identifier){
      db_query ( "UPDATE {mado} SET blocks = '%s' WHERE identifier = '%s'", serialize ( $blocks ), $identifier['identifier']);      
    } else {
      db_query ( "INSERT INTO {mado} (identifier, blocks) VALUES ('%s', '%s')", $_GET ['identifier'], serialize ( $blocks ) );
    }
  }
}