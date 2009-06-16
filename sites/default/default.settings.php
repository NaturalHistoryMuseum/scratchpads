<?php
/**
 * Automatic DB, user, password configuration
 */
$database = preg_replace("/[-\.]/", "", $_SERVER['HTTP_HOST']);
if(array_shift(explode(".", $_SERVER['HTTP_HOST'])) == 'd6'){
  $database = substr($database, 2);
}

$user_password = parse_ini_file("/var/www/drupal_db_passwords",true);
if(!isset($user_password[$database])){
  // Likely that this site has either been deleted, or never existed
  header('Location: http://scratchpads.eu/');
  exit;
}

$database_user = $user_password[$database]['user'];
$database_pass = $user_password[$database]['pass'];

$db_url = 'mysqli://' . $database_user . ':'. $database_pass . '@localhost/'. $database;
$db_prefix = '';
/* if(
    $_SERVER['REMOTE_ADDR'] == '157.140.4.52' ||
    $_SERVER['REMOTE_ADDR'] == '157.140.2.32' ||
    $_SERVER['REMOTE_ADDR'] == '127.0.0.1'
  ){
  $update_free_access = 1;
} else {*/
  $update_free_access = 0;
/* } */

ini_set('arg_separator.output',     '&amp;');
ini_set('magic_quotes_runtime',     0);
ini_set('magic_quotes_sybase',      0);
ini_set('session.cache_expire',     200000);
ini_set('session.cache_limiter',    'none');
ini_set('session.cookie_lifetime',  2000000);
ini_set('session.gc_maxlifetime',   200000);
ini_set('session.save_handler',     'user');
ini_set('session.use_only_cookies', 1);
ini_set('session.use_trans_sid',    0);
ini_set('url_rewriter.tags',        '');
ini_set('mysql.default_socket',     '/var/lib/mysql/mysql.sock');

$conf = array(
  'file_directory_temp' => '/tmp',  
  'file_directory_path' => 'sites/' . $_SERVER['HTTP_HOST'] . '/files',
);
