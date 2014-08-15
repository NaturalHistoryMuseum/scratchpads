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
  $additional_data = '
    RewriteEngine on
    RewriteCond %{REQUEST_URI} ^/files/
    RewriteRule (.*) http://%{HTTP_HOST}/sites/%{HTTP_HOST}$1 [L]
';
  switch($uri){
    // This may no longer be required, but it certainly isn't causing harm being
    // here, so it will remain for now.
    case 'scratchpads.eu':
      return $additional_data . '    ProxyPass /support/issues http://web-scratchpad-solr.nhm.ac.uk:3000/
    ProxyPassReverse /support/issues http://web-scratchpad-solr.nhm.ac.uk:3000/
';
    // Password protected sites with google given access to /google*.html
    // Useful for development sites.  Note, passwords will need to be added to
    // the digest password file
    case 'macrostomorpha.info.myspecies.info':
      return $additional_data . '    <Location />
      Order deny,allow
      Allow from all
      AuthType Digest
      AuthName "Hidden Upgrade Site"
      AuthDigestProvider file
      AuthUserFile /var/www/digest_passwd
      Require valid-user
    </Location>
    <LocationMatch "/google[0-9a-f]*.html">
      Order allow,deny
      Allow from all
    </LocationMatch>
';
    // Display the old publications crap.
    case 'phthiraptera.info':
      return $additional_data . '    RewriteCond %{REQUEST_URI} /Publications/
    RewriteRule (.*) /var/www/phthiraptera.org$1
    RewriteCond %{REQUEST_URI} /phthiraptera.org
    RewriteRule /phthiraptera\.org(.*) /var/www/phthiraptera.org$1
';
    // Any NHM only sites
    case 'edit.nhm.ac.uk':
    case 'zooemu.nhm.ac.uk':
      return $additional_data . '    <Location />
      Order deny,allow
      Deny from all
      Allow from 157.140.0.0/255.255.0.0
    </Location>
';
    default:
      return $additional_data;
  }
}
