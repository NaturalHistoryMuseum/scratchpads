<?php

/**
 * Form to view the classification editing logs
 */
function classification_logs_form(&$form_state) {
  $feed_title = variable_get('site_name', 'LifeDesk') . ' Classification Activity';
  drupal_add_feed(url('classification/logs/rss'), $feed_title);

  $header = array(
    array('data' => t('Activity'), 'field' => 'l.action'),
    array('data' => t('Editor'), 'field' => 'l.name'),
    array('data' => t('Time'), 'field' => 'l.changed', 'sort' => 'desc'),
  );
  $sql = "
    SELECT
      l.lid,
      l.action,
      l.changed,
      u.uid,
      u.name
    FROM 
      {classification_log} l 
    INNER JOIN 
      {users} u ON (l.uid = u.uid)";
  $sql .= tablesort_sql($header);
  $query_count = "
    SELECT 
      COUNT(l.lid) 
    FROM 
      {classification_log} l";
  $result = pager_query($sql, 50, 0, $query_count);

  while ($log = db_fetch_object($result)) {
	$form['action'][$log->lid] = array('#value' => $log->action);
    $form['editor'][$log->lid] = array('#value' => theme('username', $log));
    $form['modified'][$log->lid] = array('#value' => $log->changed ? t('@time ago', array('@time' => format_interval(time() - $log->changed))) : t('never'));
    $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));
  }

  return $form;
}

function theme_classification_logs_form($form) {
  $header = array(
    array('data' => t('Activity'), 'field' => 'l.action', 'width' => '450px' ),
    array('data' => t('Editor'), 'field' => 'u.name'),
    array('data' => t('Time'), 'field' => 'l.changed', 'sort' => 'desc'),
  );

  if (isset($form['editor']) && is_array($form['editor'])) {
    foreach (element_children($form['editor']) as $key) {
      $rows[] = array(
        drupal_render($form['action'][$key]),
        drupal_render($form['editor'][$key]),
        drupal_render($form['modified'][$key]),
      );
    }
  }
  else {
    $rows[] = array(array('data' => t('No logs have been recorded.'), 'colspan' => '3'));
  }

  $output .= theme('table', $header, $rows);
  if ($form['pager']['#value']) {
    $output .= drupal_render($form['pager']);
  }

  $output .= drupal_render($form);

  return $output;
}

/**
 * RSS feed for classification logs
 */
function classificataion_logs_rss() {

  $output = '<?xml version="1.0" encoding="utf-8" standalone="yes"?>
    <rss version="2.0" xmlns:atom="http://www.w3.org/2005/Atom">
      <channel>
      <title>' . variable_get('site_name', 'LifeDesk') . ' Classification Activity</title>
      <link>' . url('classification/logs', array('absolute' => TRUE)) . '</link>
      <description>' . t('Activity of names management in') . ' ' . variable_get('site_name', 'LifeDesk') . '</description>';

    $query = "SELECT
      l.lid,
      l.action,
      l.changed,
      u.uid,
      u.name
    FROM 
      {classification_log} l 
    INNER JOIN 
      {users} u ON (l.uid = u.uid)";
    
    $query_pager = "SELECT 
      COUNT(l.lid) 
    FROM 
      {classification_log} l";
  
    $result = pager_query($query,6, 0, $query_pager);

    while ($row = db_fetch_object($result)) {
	//Tue, 03 Jun 2003 09:39:21 GMT
	  $item = array(
		'title' => $row->action,
		'pubDate' => date("r",$row->changed),
		'link' => url('classification/logs', array('absolute' => TRUE)),
		'description' => $row->action . ' executed by ' . theme('username', $row) . ' on ' . date("r", $row->changed),
	  );
	  $output .= classification_logs_rss_item($item);
    }

    $output .=  '
    </channel>
    </rss>';

  drupal_set_header('Content-Type: application/rss+xml; charset=utf-8');
  echo $output;
}

function classification_logs_rss_item($item) {
  $rssitem = '
    <item>
      <title>'. check_plain($item['title']) .'</title>
      <link>'. $item['link']  .'</link>
      <description><![CDATA[' . $item['description'] . ']]></description>
      <pubDate>' . $item['pubDate'] . '</pubDate>
    </item>';
  return $rssitem;
}