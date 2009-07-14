<?php

/**
 * Cache an ispecies object
 */
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
  global $user;
  $node->uid = $user->uid;
  $node->taxonomy = array($data[1]=> taxonomy_get_term($data[1]));
  node_save($node);
  db_query("INSERT INTO {ispecies} (nid, url, ispeciestype) VALUES (%d, '%s', '%s');", $node->nid, $node->title, $data[0]);
}

/**
 * Hide an ispecies object
 */
function views_sort_hide(){
  $data = explode("---", $_POST['pin']);
  db_query("INSERT INTO {view_sort_hide} (tid, hide, ispeciestype) VALUES (%d, '%s', '%s')", $data[1], $data[2], $data[0]);
}

function views_sort_number(){
  if(isset($_POST['view']) && isset($_POST['number'])){  
    $sort_orders = variable_get('view_sort-limits', array());
    $sort_orders[$_POST['view']] = $_POST['number'];
    variable_set('view_sort-limits', $sort_orders);
  }
}

function views_sort_sorted($viewname=false){
  // We keep things simple, and use the variable table to save limits
  if($viewname){
    $order = split(" ",trim($_POST['order']));
    //print_r($order);exit;
    $weight = -90;
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
          $node->node_weight = $weight;
          $weight ++;
        }
        if($node->sticky){
          $sticky_node = TRUE;
        }
        node_save($node);
      }
    }
    if($sticky_node){
      echo "<p>".t("It is not possible to sort Sticky nodes with Non-Sticky nodes.  Sticky nodes will always be at the top of a list.")."</p>";
    }
  } else {
    watchdog('view_sort', 'ERROR sorting views, no viewname set (Referer: '.referer_uri().') : '.print_r($_REQUEST));
  }
}

function views_sort_popup_html($view_name = "", $view_args = ""){
  $view_args = json_decode(urldecode($view_args));
  $items = view_sort_get_items($view_name, $view_args);
  //print_r($items);exit;
  echo '<div id="view-sort-select-number"><form><label for="view-sort-select">Number to display:</label><select id="view-sort-select">';
  $sort_orders = variable_get('view_sort-limits', array());
  if(!isset($sort_orders[$view_name])){
    $sort_orders[$view_name] = 20;
    $divider_position = false;
  } else {
    $divider_position = $sort_orders[$view_name];
  }
  $num_local_results = view_sort_get_local_items_count($view_name, $view_args);
  if($num_local_results < $sort_orders[$view_name]){
    $sort_orders[$view_name] = $num_local_results;
  }
  for($i=1; $i<=20; $i++){
    echo '<option value="',$i,'"';
    if($sort_orders[$view_name] == $i){
      echo ' SELECTED';
    }
    echo '>',$i,'</option>';
  }
  echo '</select></form></div>';
  $i = 0;
  echo '<div id="',$view_name,'-popup" class="view-sort-top view-sort-drag class-',$view_name,'">';
  $divider_output = FALSE;
  //echo $sort_orders[$view_name];exit;
  foreach($items as $id => $item){
    // Add the divider
    if(((!isset($item['local']) || !$item['local']) || $i == $sort_orders[$view_name]) && !$divider_output){
      echo '<div class="view-sort-clear"></div></div><div class="class-',$view_name,' view-sort-bottom">';
      $divider_output = TRUE;
    }
    $class = (isset($item['local']) && $item['local']) ? "sortable" : "notsortable";
    echo '<div view-sort="',$view_name,'---',$view_args[0],'---',$id,'" class="sort-div ',$class,'"><div style="position:relative;clear:both;">';    
    if(!isset($item['local']) || !$item['local']){
      echo '<div class="view-sort-toggle"><img src="',url(drupal_get_path('module','view_sort')),'/close.png" id="view-sort-hide"/><img src="',url(drupal_get_path('module','view_sort')),'/pin.png" id="view-sort-pin"/></div>';
    }
    echo $item['body'],'</div></div>';
    $i ++;
  }
  if(!$divider_position){
    echo '<div class="view-sort-clear"></div></div><div class="class-',$view_name,' view-sort-bottom">';
  }
  echo '<div class="view-sort-clear"></div></div><div id="view-sort-error"></div><script type="text/javascript">
    view_sort_add_draggable(\'',$view_name,'\');
  </script>';
}

function view_sort_get_local_items_count($view_name, $view_args){
  // Get the view
  $view = views_get_view($view_name);
  $view->args = $view_args;
  $view->execute();
  return count($view->result);
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