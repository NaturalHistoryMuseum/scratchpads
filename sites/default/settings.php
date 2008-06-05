<?php

/* AFTER EDITING THIS FILE, THE BASH SCRIPT linktosettings.bash
 * SHOULD BE RUN.  THIS WILL ENSURE THAT ALL FILES WITHIN THE 
 * SUBDIRECTORIES COPIES OF THIS FILE */

// $Id$

// SDRycroft 9-5-07
// Remove out the www from the domain, and then split it, using the first part
// as the database name
$db_url_part ="";
if (isset($_SERVER['HTTP_HOST'])) {
  $domain = preg_replace('`^www.`', '', $_SERVER['HTTP_HOST']);
  $host_parts = explode(".", $domain);
  $db_url_part = $host_parts[0];
  if (strlen($db_url_part)<4)
    $db_url_part = $host_parts[1];
  // Finally remove "-" due to it not working as a table name
  $db_url_part = str_replace("-","",$db_url_part);
  if($db_url_part == 'monkey'){
    $db_url_part = 'quartz';
  }
}
$user_password = parse_ini_file("/var/www/drupal_db_passwords",true);
ini_set('mysql.default_socket',     '/var/lib/mysql/mysql.sock');
$db_url = 'mysqli://'.$user_password[$db_url_part]['user'].':'.$user_password[$db_url_part]['password'].'@127.0.0.1/'.$db_url_part;
$db_prefix="";
// END SDRycroft

// SDRycroft
// Automatic setting of the "Files" directory.  This is problematic to say the
// least.  It would probably make sense to have these as symbolic links in the
// very root of the filesystem!
$conf = array(
  'file_directory_temp' => '/tmp',  
  'file_directory_path' => substr(dirname(__FILE__).'/files',14),
  // The path to wherever memcache.inc is. The easiest is to simply point it
  // to the copy in your module's directory.
  //'cache_inc' => './sites/all/modules/memcache/memcache.inc',
  // or
  // 'cache_inc' => './sites/all/modules/memcache/memcache.db.inc',
  // Cache bins
  //'memcache_key_prefix' => $db_url_part
);
// END SDRycroft

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
