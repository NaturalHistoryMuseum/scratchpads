<?php


function views_sort_pin(){
  $data = explode("---", $_POST['pin']);
  $items = view_sort_get_items($data[0], array($data[1]));
  $item = $items[$data[2]];
  // We've got our item, now we need to save it to the database.
  $node = new stdClass();
  $node->title = $data[2];
  $node->body = substr($_POST['html'], strpos($_POST['html'], "<div"));
  $node->type = 'ispecies';
  $node->format = 2;
  $node->taxonomy = array($data[1]=> taxonomy_get_term($data[1]));
  node_save($node);
  db_query("INSERT INTO {ispecies} (nid, url, ispeciestype) VALUES (%d, '%s', '%s');", $node->nid, $node->title, $data[0]);
  exit;
}

function views_sort_sorted($viewname=false, $tid=false){
  // We keep things simple, and use the variable table to save limits
  if($viewname){
    // First we set the limit according to where the divider was
    $sort_orders = variable_get('view_sort-limits', array());
    // Split up the order variable that we've been sent
    $order = split(" ",trim($_POST['order']));
    // Find the divider
    $divider_position = array_search("divider", $order);
    // if the divider is at the end, we don't set it
    if($divider_position < count($order)-1 && $divider_position){
      $sort_orders[$viewname] = $divider_position;
    } else {
      unset($sort_orders[$viewname]);
    }
    variable_set('view_sort-limits', $sort_orders);
    
    // Next we set the weights if the view is of nodes
    $items = view_sort_get_items($viewname, array($tid));
    $new_order = array();
    $weight = -99;
    $set_weight_as_zero = false;
    foreach($order as $item){
      if($item == 'divider'){
        $set_weight_as_zero = true;
        continue;      
      }
      $node = node_load(array_pop(explode("---", $item)));
      if($node){
        if($set_weight_as_zero){
          $node->node_weight = 0;
        } else {
          $node->sticky = 1;
          $node->node_weight = $weight;
          $weight ++;
        }
        node_save($node);
      }
    }
  } else {
    watchdog('view_sort', 'ERROR sorting views, no viewname set (Referer: '.referer_uri().') : '.print_r($_REQUEST));
  }
  exit;
}

function views_sort_popup_html($view_name = "", $view_args = ""){
  $view_args = json_decode(urldecode($view_args));
  $items = view_sort_get_items($view_name, $view_args);
  //print_r($items);exit;
  $output = '<div id="view-sort-select-number"><form><label for="view-sort-select">Number to display:</label><select id="view-sort-select">';
  for($i=0; $i<=50; $i++){
    $output .= '<option value="'.$i.'">'.$i.'</option>';
  }
  $output .='</select></form></div>';
  $i = 0;
  $sort_orders = variable_get('view_sort-limits', array());
  $sort_orders = array('flickr' => 10);
  if (isset($sort_orders[$view_name])){
    $divider_position = $sort_orders[$view_name];
  } else {
    $divider_position = false;
  }
  $output .= '<div id="'.$view_name.'" class="view-sort-top view-sort-drag class-'.$view_name.'">';
  foreach($items as $id => $item){
    // FIXME - This needs looking at
    // Remove links from the output (we don't want users clicking, just dragging
    
    $output .= '<div view-sort="'.$view_name.'---'.$view_args[0].'---'.$id.'" class="sort-div';
    if(isset($item['local']) && $item['local']){
      $output .= ' sortable';
    } else {
      $output .= ' notsortable';
    }
    $output .= '"><div style="position:relative;clear:both;">';
    if(!isset($item['local']) || !$item['local']){
      $output .='<div class="view-sort-toggle"><img src="'.url(drupal_get_path('module','view_sort')).'/close.png" id="view-sort-hide"/><img src="'.url(drupal_get_path('module','view_sort')).'/pin.png" id="view-sort-pin"/></div>';
    }
    $output .= $item['body'].'
  </div></div>';
    $i ++;
    if($divider_position && $divider_position == $i){
      $output .= '<div class="view-sort-clear"></div></div><div class="class-'.$view_name.' view-sort-bottom">';
    }
  }
  if(!$divider_position){
    $output .= $divider;
  }
  $output .= '<div class="view-sort-clear"></div></div><script type="text/javascript">
    '."view_sort_add_draggable('$view_name');
  </script>";
  print $output;
  exit;
}

function view_sort_get_items($view_name, $view_args){
  // Get the view
  $view = views_get_view($view_name);
  $view->args = $view_args;
  $view->execute();
  $items = array();
  if(count($view->result)){
    foreach($view->result as $item){
      $items[$item->nid] = array('body' => node_view(node_load($item->nid), true), 'local'=> 1);
    }
  }
  if(function_exists('ispecies_ajax_callback')){
    if(isset($view->display['default']->display_options['filters']['ispeciestype']['value'])){
      //echo array_pop($view->args);exit;
      $items_temp = ispecies_get_data($view->display['default']->display_options['filters']['ispeciestype']['value'] , $view->args[0]);
      foreach($items_temp as $item){
        if(is_array($item) && isset($item['body'])){
          $items[$item['page_url']] = array('body' => $item['body']);
        } 
      }
    }
  }
  return $items;
}