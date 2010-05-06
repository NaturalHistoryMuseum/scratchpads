<?php




function taxonomy_tree_callback($vid, $nid = null){
  
  $depth = 1;
  $branches_to_expand = array();
  
  // If you're editing, tids can be passed in
  if($_GET['t']){
    
    $selected_tids = explode('-', $_GET['t']);
  
  }
  
  if($_GET['root'] == 'source'){
    
    if(count($selected_tids)){
      
      foreach($selected_tids as $selected_tid){

        $parents = taxonomy_get_parents_all($selected_tid);
      
        $number_of_parents = count($parents) + 1;
      
        if($number_of_parents > $depth){
        
          $depth = $number_of_parents - 1;
        
        }
      
        // We need to know all the parents so we can automatically expand the tree      
        foreach($parents as $parent){
        
          if($selected_tid != $parent->tid){
            $branches_to_expand[$parent->tid] = $parent->tid;
          }
          
        
        }
      
      }

    }elseif(!$nid){
      
    $select_all = true;  
      
    }
    
    $parent = 0;

    
    
  }else{ 
    
    $exploded_root = explode('-', $_GET['root']);
    
    $parent = $exploded_root[0];
    
    if($exploded_root[1]){
      
      $select_all = true;
      
    }
    
  }

  $tree = taxonomy_get_tree($vid, $parent, -1, $depth);

  $json = taxonomy_tree_to_json($tree, $branches_to_expand, $selected_tids, $select_all);

  // We need to store which items have been expanded
  if(count($json)){
    
    if($parent){
    
      $element = array(
        '#type' => 'hidden', 
        '#value' => $parent, 
        '#name' => 'expanded['.$parent.']',
        '#return_value' => $parent
      );
      
      $json[0]['text'] .= drupal_render($element);

    }elseif(count($branches_to_expand)){
    
      foreach($branches_to_expand as $branch_to_expand){
      
        $element = array(
          '#type' => 'hidden', 
          '#value' => $branch_to_expand, 
          '#name' => 'expanded['.$branch_to_expand.']',
          '#return_value' => $branch_to_expand
        );
  
        $json[0]['text'] .=  drupal_render($element);
      
      }
    
    }
  
  }

  return drupal_json($json);
   
}

function taxonomy_tree_to_json($tree, $branches_to_expand, $selected_tids, $select_all = false){

  $tree_array = taxonomy_tree_to_array($tree);
  $json = array();

  foreach($tree_array as $root){

    $item = taxonomy_tree_get_element($root['data']->vid, $root['data']->tid, $root['data']->name, $selected_tids, $select_all);   
        
    if(count($root['children'])){
      
      if($branches_to_expand[$root['data']->tid]){
        
      $item['children'] = taxonomy_tree_add_children($root['children'], $branches_to_expand, $selected_tids);
      $item['expanded'] = true;
      
      }else{
      
      $item['hasChildren'] = true;
      
      }
      
    }else{
      
      $children = taxonomy_get_children($root['data']->tid);
      
      if(count($children)){
        
      $item['hasChildren'] = true;  
        
      }
      
    }

    
    $json[] = $item;
    
  }
  
  return $json;
  
}

/** 
* Recursive; Add all the child terms
*/
function taxonomy_tree_add_children($children, $branches_to_expand, $selected_tids){
    
  $child_items = array();
  
    foreach($children as $child){
      
      $child_item = taxonomy_tree_get_element($child['data']->vid, $child['data']->tid, $child['data']->name, $selected_tids);

      if(count($child['children']) && $branches_to_expand[$child['data']->tid]){

        $child_item['children'] = taxonomy_tree_add_children($child['children'], $branches_to_expand, $selected_tids);
        
        $child_item['expanded'] = true;

        
      }else{

          $c = taxonomy_get_children($child['data']->tid);

          if(count($c)){

          $child_item['hasChildren'] = true;  

          }

      }
      

      
      $child_items[] = $child_item;
      
    }

  return $child_items;
  
}

function taxonomy_tree_get_element($vid, $tid, $name, $selected_tids, $select_all = false){
  
  if($tid && is_array($selected_tids) && in_array($tid, $selected_tids) || $select_all){
    
    $value = 1;
    
  }else{
    
    $value = 0;
    
  }
  
  $element[$tid] = array(
    '#type' => 'checkbox',
    '#title' => $name,
    '#value' => $value,
    '#name' => 'taxonomy['.$vid.']['.$tid.']',
    '#return_value' => $tid
  );
  
  return array(
    'text' => drupal_render($element),
    'id' => ($select_all ? $tid.'-1' : $tid)
  );
  
}

/** 
* Convert a taxonomy tree into a multidimensional array
*/
function &taxonomy_tree_to_array(&$nvocs) {

$map = array();

//go through the tree and find the lowest depth aswell as all available depth levels to scan
$startdepth = false;
$depthlist = array();
foreach($nvocs as $vocindex => &$vocitem) {
if ($startdepth === false || $startdepth > $vocitem->depth) {
//new start depth found
$startdepth = $vocitem->depth;
}

//add this item to the speciffic depth level
$depthlist[$vocitem->depth][$vocitem->tid] = &$vocitem;
}

//sort the depthlist so lowest is first
ksort($depthlist);

//scan all depth levels found
$lastdepth = null;
foreach($depthlist as $depthlevel => &$depths) {
//check for first depth (root) level
$currentdepthdata = array();

if ($depthlevel == $startdepth) {
//add root items to the map
foreach($depths as $itemindex => &$item) {
$mapindex = count($map);
$map[$mapindex] = array("data" => &$item,"children" => array());
$currentdepthdata[$itemindex] = &$map[$mapindex];
}
} else {
//scan through all items within this depth
  foreach($depths as $itemindex => &$item) {
  //find a matching "map" in teh last depth to assign the item too
    foreach($item->parents as $parentindex) {
    $mapindex = count($lastdepthdata[$parentindex]["children"]);
    $lastdepthdata[$parentindex]["children"][$mapindex] = array("data" => &$item,"children" => array());
    $currentdepthdata[$itemindex] = &$lastdepthdata[$parentindex]["children"][$mapindex];
    }
  }
}

$lastdepthdata = $currentdepthdata;

}

return $map;


}


