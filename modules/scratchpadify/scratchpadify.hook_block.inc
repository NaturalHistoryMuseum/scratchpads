<?php 

/**
 * Helper for hook_block - does all the heavy lifting.
 *
 */
function _scratchpadify_block($op, $delta, $edit){
  switch ($op){
    case 'list':
      return array(
        array('info'=>t('Scratchpad statistics')),
        array('info'=>t('Content')),
        array('info'=>t('About this site')),
        array('info'=>t('Sad trombone')),
        array('info'=>t('Scratchpad notifications')),
        array('info'=>t('Recent edits'))
      );
    case 'view':
      switch($delta){
        case 0:
          $nodes = db_fetch_array(db_query("SELECT COUNT(nid) AS nodes FROM node;"));
          $users = db_fetch_array(db_query("SELECT COUNT(uid) AS users FROM users"));
          $views = db_fetch_array(db_query("SELECT SUM(totalcount) AS totalcount FROM node_counter;"));
          $items = array('Pages: '.$nodes['nodes'],'Users: '.($users['users']-2),'Total Page Views: '.$views['totalcount']);
          return array('subject'=>'Statistics','content'=>theme_item_list($items));
        case 1:
          $links_override = array(
            'biblio' => array(
              'view' => variable_get('biblio_base','biblio'),
              'import' => array(array('<img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/import.gif" alt="Import"/>','biblio/import', array('html' => TRUE, 'attributes' => array('title' => t('Import a bibliography from your reference manager')))))
            ),
            'image' => array(
              'view' => 'image',
              'import' => array(
                array('<img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/import.gif" alt="Import"/>', 'imagex/imagex', array('html' => TRUE, 'attributes' => array('title' => t('Upload multiple images'))))
              )
            ),
            'publication' => array('import' => FALSE),
            'nexus_project' => array('import' => FALSE),
            'forum' => array('view' => 'forum'),
            'blog' => array('view' => 'blog'),
            'group' => array('view' => 'og', 'import' => FALSE)
          );
          $query = array();
          if(arg(0) == 'taxonomy' && arg(1) == 'term' && is_numeric(arg(2))){
            $query = array('tid' => arg(2));
          } elseif(arg(0) == 'node' && is_numeric(arg(1))){
            $node = node_load(array('nid' => arg(1)));
            if(variable_get('og_content_type_usage_'.$node->type, '') == 'group'){
              $viewing_group = $node->type;
              $query = array('gids[]' => arg(1));
            }
          }
          $results = db_query("SELECT type, name FROM node_type, menu_links WHERE (link_path = CONCAT('node/add/', type) OR link_path = CONCAT('node/add/', REPLACE(type, '_', '-'))) AND hidden = 0");
          $sub_menu_item = menu_get_item('admin/content/node_import/add');
          $can_access_import = call_user_func_array($sub_menu_item['access_callback'], unserialize($sub_menu_item['access_arguments']));
            
          while($row = db_fetch_array($results)){
            $link_path = isset($links_override[$row['type']]['view']) ? $links_override[$row['type']]['view'] : 'content/'.$row['type'];
            $items[$row['name']] = l(t($row['name']), $link_path, array('attributes' => array('title' => t('View all "@node_type" nodes', array('@node_type' => $row['name'])))));
            $menu_item = menu_get_item('node/add/'.str_replace('_','-',$row['type']));
            if(call_user_func_array($menu_item['access_callback'], unserialize($menu_item['access_arguments']))){
              $items[$row['name']] .= ' '.l('<img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/add.gif" alt="Add"/>', 'node/add/'.str_replace("_","-",$row['type']), array('query' => $query, 'html' => TRUE, 'attributes' => array('title' => t('Add a "@node_type" node',array('@node_type' =>$row['name'])))));
              // We'll assume that a many permission won't exist when a single
              // doesn't.
              if(isset($links_override[$row['type']]['import']) && is_array($links_override[$row['type']]['import'])){
                foreach($links_override[$row['type']]['import'] as $additional_item){
                  $sub_menu_item = menu_get_item($additional_item[1]);
                  if(function_exists($sub_menu_item['access_callback'])){
                    if(call_user_func_array($sub_menu_item['access_callback'], unserialize($sub_menu_item['access_arguments']))){
                      $items[$row['name']] .= call_user_func_array('l',$additional_item);
                    }
                  }
                }
              } else {
                if($can_access_import){
                  $items[$row['name']] .= l('<img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/import.gif" alt="Import"/>', 'admin/content/node_import/add', array('query' => array('type' => $row['type']), 'html' => TRUE, 'attributes' => array('title' => t('Import from CSV file'))));           
                }
              }
            }
            if($row['type'] == $viewing_group && og_menu_access_picture(arg(1))){
              $items[$row['name']] .= '<br/><span style="font-size:80%">'.l(t('Members'),'og/users/'.arg(1).'/faces') . ' | '.l(t('My membership'), 'og/manage/'.arg(1)).'</span>';
            }
          }
          // Additional item for taxonomy
          $menu_item = menu_get_item('admin/content/taxonomy');
          if(call_user_func_array($menu_item['access_callback'], unserialize($menu_item['access_arguments']))){
            $items['Taxonomy'] = l(t('Taxonomy'), 'admin/content/taxonomy', array('attributes' => array('title' => 'View your site\'s taxonomies'))).' '.l('<img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/import.gif" alt="Import"/>', 'admin/content/taxonomy/classification/import', array('html' => TRUE, 'attributes' => array('title' => t('Import a classification'))));
          }
          uksort($items, "strnatcasecmp");
          return array('subject'=>'Content','content'=>theme_item_list($items , NULL , 'ul' , array('class'=>'scratchpadify-block')));
          break;
        case 2:
          $uid = array_pop(db_fetch_array(db_query("SELECT uid FROM users WHERE uid > 1 AND uid IN (SELECT uid FROM users_roles WHERE rid = 5) ORDER BY uid ASC LIMIT 1")));
          $maintainer = user_load(array('uid'=>$uid));
          return array('subject'=>'About this site','content'=>'<p>This site is moderated by <span style="text-decoration:underline">'.l(str_replace(" ","&nbsp;",$maintainer->name), "user/".$maintainer->uid, array('html' => 1)).'</span> on behalf of the  contributors who retain copyright.</p><p>Content can be used in accordance with a <a href="http://creativecommons.org/licenses/by-nc-sa/3.0/">CC Licence</a>.</p><p>This site uses <a href="http://drupal.org">Drupal</a> and is based on a set of templates and modules defined by the <a href="http://scratchpads.eu/">Scratchpad</a> team at the <a href="http://www.nhm.ac.uk/">Natural History Museum</a>, London.</p>');
        case 3:
          drupal_add_js('var sadtrombone_width = 180;','inline');
          return array('subject'=>'Sad Trombone','content'=>'<div id="widget_sadtrombone"></div><div id="widget_tmp_sadtrombone" style="display:none"><script id="widget_scr_sadtrombone" type="text/javascript" src="http://www.sadtrombone.com/widget/async-sadtrombone.js"></script></div>');
        case 4:
          $scratchpad_notification = variable_get('scratchpad_notification_block', array('time' => 0, 'message' => ''));
          if($scratchpad_notification['time']+3600 < time()){
            $message = file_get_contents(variable_get('scratchpad_notification_block_url','http://scratchpads.eu/notification.txt'));
            $scratchpad_notification = array('time' => time(), 'message' => $message);
            variable_set('scratchpad_notification_block', $scratchpad_notification);
            $hash = md5($scratchpad_notification['message']);
            variable_set('scratchpad_notification_block_message_id', $hash);
          }
          // Check to see if the message has been hidden based on the hash
          if(isset($_COOKIE['scratchpadnotification']) && $_COOKIE['scratchpadnotification'] == variable_get('scratchpad_notification_block_message_id','nohash')){
            return;
          }
          if(trim($scratchpad_notification['message']) != ''){
            $scratchpad_notification['message'] = '<div><div class="left"><p>' . $scratchpad_notification['message'] . '</p></div><div class="right"><p>'.l('<img src="'.base_path().drupal_get_path('module','scratchpadify').'/images/close.png"/>','hidenotification',array('query' => array('destination' => $_GET['q']), 'html' => TRUE)).'</p></div><div style="clear:both"></div></div>';
          }
          return array('subject' => '', 'content' => $scratchpad_notification['message']);
        case 5:
          $results = db_query_range('SELECT nid FROM {node} ORDER BY changed DESC', 0, variable_get('scratchpadify_recent_edits_count',5));
          while($nid = db_result($results)){
            $node = node_load(array('nid' => $nid));
            $nodes[] = l($node->title, 'node/'.$nid);
          }
          return array('subject' => 'Recent edits', 'content' => theme('item_list', $nodes));
      }
      break; 
    case 'configure': // No need to add any extra configuration - Open to suggestions.
      if($delta == 0){
        return array('additional_text' => array(
            '#description' => t('Enter any additional text you would like above the statistics.'),
            '#title' => t('Additional text'),
            '#type' => 'textfield',
            '#weight' => -9,
            '#default_value' => variable_get('scratchpadify-stats-block-text','')
          )
        );
      } elseif($delta = 5){        
        return array('recent_count' => array(
            '#title' => t('Number of recent edits to show'),
            '#type' => 'select',
            '#weight' => -9,
            '#options' => array(1=>1,2=>2,3=>3,4=>4,5=>5,6=>6,7=>7,8=>8,9=>9,10=>10,15=>15,20=>20,30=>30,50=>50),
            '#default_value' => variable_get('scratchpadify_recent_edits_count',5)
          )
        );
      }
    case 'save':
      if($delta == 0){
        variable_set('scratchpadify-stats-block-text', $edit['additional_text']);
      } elseif($delta == 5){
        variable_set('scratchpadify_recent_edits_count', $edit['recent_count']);        
      }
      break;
  }
}
