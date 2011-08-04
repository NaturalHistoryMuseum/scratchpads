<?php

function emonocot_profile_set_blocks(){
  // Set various blocks to be visible
  db_query("DELETE FROM {blocks} WHERE module = 'search' AND delta = 0");
  db_query("DELETE FROM {blocks} WHERE module = 'system' AND delta = 0");
  foreach(list_themes() as $theme){
    // Below here changed for eMonocot profile
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'scratchpadify' AND delta = 1", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight,title) VALUES ('scratchpadify',1,'%s','left',1,-10,'Common Tasks')", $theme->name);
    //About eMonocot Block
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'emonocot' AND delta = 0", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight) VALUES ('emonocot',0,'%s','left',1,20)", $theme->name);
    //eMonocot task guide
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'emonocot' AND delta = 1", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight) VALUES ('emonocot',1,'%s','left',1,-23)", $theme->name);
    //
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'user' AND delta = 1", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight) VALUES ('user', 1, '%s', 'left',1,-11)", $theme->name);
    //Below here not changed for eMonocot profile
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'scratchpadify' AND delta = 4", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status) VALUES ('scratchpadify',4,'%s','header',1)", $theme->name);
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'search' AND delta = 0", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight, title) VALUES ('search',0,'%s','left',1,-200,'<none>')", $theme->name);
    //eMonocot jiggling
    db_query("UPDATE {blocks} SET weight = -30 WHERE theme = '%s' AND module = 'scratchpadify' AND delta = 1 ", $theme->name);
    //Apache Solr
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'apachesolr' AND delta = 'sort'", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight) VALUES ('apachesolr','sort','%s','left',1,-27)", $theme->name);
    db_query("DELETE FROM {blocks} WHERE theme = '%s' AND module = 'apachesolr_search' AND delta = 'type'", $theme->name);
    db_query("INSERT INTO {blocks} (module, delta, theme, region, status, weight) VALUES ('apachesolr_search','type','%s','left',1,-26)", $theme->name);
  }
  //eM Play with blocks
  db_query("INSERT INTO {blocks_roles} (module, delta, rid) VALUES ('user', 1, 2)");
  db_query("UPDATE {blocks} SET title = 'Search' WHERE module = 'search' AND delta = 0");
}

function emonocot_profile_set_conditions(){
  // Insert conditions into the Scratchpad
  $conditions = '<ol>
<li><b>ACCEPTANCE OF TERMS</b> This agreement is between eMonocot, and you and your agents (collectively “you”) regarding the use of this website (the "Site"). By using the Site, you agree to the Terms and Conditions in this document.</li>
<li><b>OWNERSHIP OF SITE</b> The text, graphics, sound and software (collectively "Content") on this Site is owned by you and your agents and you bare sole and ultimate responsibility for this Content. eMonocot supports the computer hardware infrastructure and software content management system that provides access to this Content.</li>
<li><b>ACCESS TO SERVICES AND TERMINATION OF ACCESS</b> You are responsible for all activity logged through your user account and for the activity of other persons or entity you grant access to this Site. You agree to notify eMonocot immediately if you become aware of any unauthorised use and you agree that eMonocot may terminate your access privileges and remove Content without notice if eMonocot believes you have violated any provision of this Agreement. You agree that termination of your access to the Site shall not result in any liability or other obligation of eMonocot to you or any third party in connection with such termination.  An archive copy of your content at the time of termination will be kept and made available to you on request.</li>
<li><b>CONTENT</b> You agree to be bound by the Joint Academic Network (JANET) Acceptable Use Guidelines (<a href="http://www.ja.net/company/policies/aup.html">http://www.ja.net/company/policies/aup.html</a>). In summary this document states that all Content placed on the Site must be legal, decent and truthful. Through you or your agent’s use of the Site, you represent and warrant that you have all the rights necessary to receive, use, transmit and disclose all data that you use in any way with the Site. You agree and acknowledge that you are solely responsible for any liabilities, fines, or penalties occasioned by any such violations or lack of rights and that you are solely responsible for the accuracy and adequacy of information and data furnished on the Site.</li>
<li><b>TAKE DOWN POLICY</b> If you are a rights owner and are concerned that you have found material on a Site and have not given permission for its use, please contact us in writing (scratchpad@nhm.ac.uk) providing:
  <ul>
    <li>Your contact details</li>
    <li>The full bibliographic details of the material</li>
    <li>The Site address where you found the material</li>
    <li>A statement that, under penalty of perjury, you are the rights owner or are authorised to act for the rights owner</li>
  </ul>
</li>
<li><b>DISCLAIMER OF WARRANTIES</b> The use of the Site is solely at your own risk. The site is provided on an "as is" and "as available" basis and eMonocot expressly disclaims all warranties of any kind with respect to the site, whether express or implied. eMonocot makes no warranty that the access to the site and/or Content therein will be uninterrupted or secure. Your sole and exclusive remedy with respect to any defect in or dissatisfaction with the Site is to cease using the Site.</li>
<li><b>LIMITATION OF LIABILITY</b> You understand and agree that eMonocot shall not be liable for any direct, indirect, incidental, special, consequential, or exemplary damages resulting from any matter related to your or other persons use of the site.</li>
<li><b>DISCLAIMER OF CONTENT</b> You understand and acknowledge that eMonocot assumes no responsibility to screen or review Content and that eMonocot shall have the right, but not the obligation, in its sole discretion to review, refuse, monitor, or remove any Content. ViBRANT expressly disclaims all responsibility or liability to you or any other person or entity for the Content and you acknowledge and agree that you assume all risk associated with the use of any and all Content.</li>
</ol>';
  $extras = array(
    'extras-1' => '',
    'extras-2' => '',
    'extras-3' => '',
    'extras-4' => '',
    'extras-5' => ''
  );
  db_query("INSERT INTO {legal_conditions} (conditions, date, extras) VALUES ('%s',NOW(),'%s')", $conditions, serialize($extras));
  variable_set('legal_display', 2);
}

function emonocot_profile_tweak_menu(){
  // Remove the "Biblio" & "Taskguide" links from the navigation menu - they
  // look ugly.    
  $links = array(
    'biblio' => array(
      'menu_name' => 'primary-links',
      'link_title' => st('Bibliography'),
      'module' => 'system',
      'weight' => -50
    ),
    'contact' => array(
      'menu_name' => 'primary-links',
      'link_title' => st('Contact us'),
      'module' => 'system',
      'hidden' => 0,
      'weight' => -48
    ),
    'forum' => array(
      'menu_name' => 'primary-links',
      'module' => 'system',
      'weight' => -49
    ),
    'logout' => array(
      'menu_name' => 'primary-links',
      'link_title' => st('Log Out'),
      'module' => 'system'
    ),
    'map/node' => array(
      'hidden' => 1,
      'module' => 'system'
    ),
    'map/node/load/%/%' => array(
      'hidden' => 1,
      'module' => 'system'
    ),
    'map/user' => array(
      'hidden' => 1,
      'module' => 'system'
    ),
    'map/user/load' => array(
      'hidden' => 1,
      'module' => 'system'
    ),
    'node/add' => array(
      'module' => 'system',
      'hidden' => 1
    ),
    'admin' => array(
      'plid' => array_pop(db_fetch_array(db_query("SELECT mlid FROM {menu_links} WHERE link_path = 'basicadmin'"))),
      'module' => 'system',
      'link_title' => st('Advanced')
    )
  );
  foreach($links as $path => $changes){
    $item = menu_get_item($path);
    $item['link_path'] = $path;
    $item['link_title'] = $item['title'];
    // FFS, Drupal doesn't have a function to return the MLID for an item. WTF!
    $item['mlid'] = array_pop(db_fetch_array(db_query("SELECT mlid FROM {menu_links} WHERE link_path = '%s'", $path)));
    foreach($changes as $key => $value){
      $item[$key] = $value;
    }
    $item['customized'] = 1;
    menu_link_save($item);
  }
  // Create a link to help
  $link = array(
    'menu_name' => 'primary-links',
    'weight' => -45,
    'link_path' => 'admin/advanced_help/scratchpadify_help',
    'link_title' => 'Help'
  );
  menu_link_save($link);
  //eM link
  $em_link = array(
    'menu_name' => 'primary-links',
    'link_title' => st('Log In'),
    'weight' => 40,
    'link_path' => 'user/login'
  );
  menu_link_save($em_link);
}

function emonocot_profile_apache_solr() {
  $blocks = array( '');
  $blocks['apachesolr_search'] = array('type' => 'type');
  variable_set('apachesolr_enabled_facets', $blocks);
  apachesolr_clear_cache();
}