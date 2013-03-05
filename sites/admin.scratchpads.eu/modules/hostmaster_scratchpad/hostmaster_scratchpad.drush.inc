<?php

/**
 * Implementation of hook_pre_provision_install
 */
function drush_hostmaster_scratchpad_pre_provision_install($url = NULL){
  drush_set_option('site_title', drush_get_option('site_title', FALSE), 'site');
  drush_set_option('client_title', drush_get_option('client_title', FALSE), 'site');
  drush_set_option('country', drush_get_option('country', FALSE), 'site');
  drush_set_option('client_email', drush_get_option('client_email', FALSE), 'site');
  drush_set_option('fullname', drush_get_option('fullname', FALSE), 'site');
  drush_set_option('institution', drush_get_option('institution', FALSE), 'site');
  drush_set_option('taxonomic_scope', drush_get_option('taxonomic_scope', FALSE), 'site');
}

/**
 * Implements hook_provision_apache_vhost_config
 */
function hostmaster_scratchpad_provision_apache_vhost_config($uri, $data){
  switch($uri){
    case 'scratchpads.eu':
      return '
    ProxyPass /support/issues http://web-scratchpad-solr.nhm.ac.uk:3000/
    ProxyPassReverse /support/issues http://web-scratchpad-solr.nhm.ac.uk:3000/
';
      break;
    case 'phthiraptera.info':
      return '
    RewriteEngine on
    RewriteCond %{REQUEST_URI} /Publications/
    RewriteRule (.*) /var/www/phthiraptera.org$1
    RewriteCond %{REQUEST_URI} /phthiraptera.org
    RewriteRule /phthiraptera\.org(.*) /var/www/phthiraptera.org$1
';
      break;
    case 'mosquito-taxonomic-inventory.info.myspecies.info':
      return '
    <Location />
      AuthType Digest
      AuthName "Hidden Upgrade Site"
      AuthDigestProvider file
      AuthUserFile /var/www/digest_passwd
      Require valid-user
      Order allow,deny
      Allow from 157.140.0.0/255.255.0.0
      Satisfy any
    </Location>
';
      break;
  }
}