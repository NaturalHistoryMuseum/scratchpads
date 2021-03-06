<?php

// Automatic DB, user, password configuration
$database = preg_replace("/[-\.]/", "", $_SERVER['HTTP_HOST']);
$conf = array(
  'file_directory_temp' => '/tmp',
  'file_directory_path' => 'sites/' . $_SERVER['HTTP_HOST'] . '/files',
  'preprocess_css' => 1,
  'preprocess_js' => 1
);
$user_password = parse_ini_file("/etc/drupal/6/drupal_db_passwords", true);
if(!isset($user_password[$database])){
  // Likely that this site has either been deleted, or never existed
  header('Location: http://scratchpads.eu/');
  exit();
}
$db_url = 'mysqli://' . $user_password[$database]['user'] . ':' . $user_password[$database]['password'] . '@localhost/' . $database;
$db_prefix = '';
// Not everybody can run updates.
$update_free_access = 0;
// Set some PHP settings
ini_set('pcre.backtrack_limit', 10000000);
ini_set('pcre.recursion_limit', 10000000);
ini_set('arg_separator.output', '&amp;');
ini_set('magic_quotes_runtime', 0);
ini_set('magic_quotes_sybase', 0);
ini_set('session.cache_expire', 86400); // Changed to one day SDRycroft
ini_set('session.cache_limiter', 'none');
ini_set('session.cookie_lifetime', 864000); // Changed to TEN days SDRycroft
ini_set('session.gc_maxlifetime', 86400); // Changed to one day SDRycroft
ini_set('session.save_handler', 'user');
ini_set('session.use_cookies', 1);
ini_set('session.use_only_cookies', 1);
ini_set('session.use_trans_sid', 0);
ini_set('url_rewriter.tags', '');
ini_set('mysql.default_socket', '/var/lib/mysql/mysql.sock');