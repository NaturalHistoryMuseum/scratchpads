<?php

function scratchpad_slickgrid_views_data(){
  $data = array();
  $data['scratchpad_slickgrid_nodes']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid', 
      'field' => 'nid'
    )
  );
  $data['scratchpad_slickgrid_batches']['table']['group'] = t('Slickgrid');
  $data['scratchpad_slickgrid_batches']['table']['join'] = array(
    // batches links to node through scratchpad_slickgrid_nodes via bid
    'node' => array(
      'left_table' => 'scratchpad_slickgrid_nodes', 
      'left_field' => 'bid', 
      'field' => 'bid'
    )
  );
  // Batch ID
  $data['scratchpad_slickgrid_batches']['bid'] = array(
    'title' => t('Batch ID'),
    'help' => t('The ID of the batch.'),  
    'argument' => array(
      'handler' => 'views_handler_argument_numeric'
    ), 
    'filter' => array(
      'handler' => 'views_handler_filter_numeric'
    )
  );
  // Batch date created    
  $data['scratchpad_slickgrid_batches']['created'] = array(
    'title' => t('Batch created'), 
    'help' => t('Date batch was created.'), 
    'field' => array(
      'handler' => 'views_handler_field_date', 
      'click sortable' => TRUE
    ), 
    'filter' => array(
      'handler' => 'views_handler_filter_date'
    ), 
    'argument' => array(
      'handler' => 'views_handler_argument_date'
    ), 
    'sort' => array(
      'handler' => 'views_handler_sort'
    )
  );
  return $data;
}
	
	

