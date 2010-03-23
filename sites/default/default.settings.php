<?php
/**
 * Automatic DB, user, password configuration
 */
$database = preg_replace("/[-\.]/", "", $_SERVER['HTTP_HOST']);
if(array_shift(explode(".", $_SERVER['HTTP_HOST'])) == 'd6'){
  $database = substr($database, 2);
}

$user_password = parse_ini_file("/etc/drupal/6/drupal_db_passwords",true);
if(!isset($user_password[$database])){
  // Likely that this site has either been deleted, or never existed
  header('Location: http://scratchpads.eu/');
  exit;
}

$db_url = 'mysqli://' . $user_password[$database]['user'] . ':'. $user_password[$database]['password'] . '@localhost/'. $database;
$db_prefix = '';
if(
    (
      $_SERVER['REMOTE_ADDR'] == '157.140.4.52' ||
      $_SERVER['REMOTE_ADDR'] == '157.140.4.7' ||
      $_SERVER['REMOTE_ADDR'] == '157.140.4.72' ||
      $_SERVER['REMOTE_ADDR'] == '157.140.2.32' ||
      $_SERVER['REMOTE_ADDR'] == '127.0.0.1'
    ) &&
    $_SERVER['SCRIPT_URL'] != '/install.php'
  ){
  ini_set('display_errors',1);
  // Change following to allow access from above IPs to update.php
  $update_free_access =0;
} else {
  // Change following to allow ALL ips to update.php
  $update_free_access =0;
}

ini_set('arg_separator.output',     '&amp;');
ini_set('magic_quotes_runtime',     0);
ini_set('magic_quotes_sybase',      0);
ini_set('session.cache_expire',     86400); // Changed to one day SDRycroft
ini_set('session.cache_limiter',    'none');
ini_set('session.cookie_lifetime',  864000); // Changed to TEN days SDRycroft
ini_set('session.gc_maxlifetime',   86400); // Changed to one day SDRycroft
ini_set('session.save_handler',     'user');
ini_set('session.use_cookies',      1);
ini_set('session.use_only_cookies', 1);
ini_set('session.use_trans_sid',    0);
ini_set('url_rewriter.tags',        '');
ini_set('mysql.default_socket',     '/var/lib/mysql/mysql.sock');

$conf = array(
  'file_directory_temp' => '/tmp',  
  'file_directory_path' => 'sites/' . $_SERVER['HTTP_HOST'] . '/files',
);
