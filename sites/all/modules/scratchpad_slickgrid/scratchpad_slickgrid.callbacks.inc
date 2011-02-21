<?php

function scratchpad_slickgrid_callback_get_bid(){
	
	// Get the last BID created by this user
	// As this is called after the first file upload has completed, this will contain the BID of the upload
	global $user;
	
	$params = array(
		'uid' => $user->uid
	);
	
	$bid = scratchpad_slickgrid_get_batch($params);
	
	 drupal_json(array(
    'status' => TRUE,
    'bid' => $bid
  ));
	
}