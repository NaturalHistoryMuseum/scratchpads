CREATE TABLE mailman_lists (
    lid tinyint(3) unsigned NOT NULL auto_increment,
      name varchar(48) NOT NULL default '',
      command varchar(72) NOT NULL default '',
    admin varchar(48) default '',
    web varchar(255) default '',
    webarch varchar(255) default '',
      PRIMARY KEY  (lid)
  );

CREATE TABLE mailman_users (
      uid int(10) NOT NULL default '0',
      lid int(10) NOT NULL default '0',
      lstatus int(10) NOT NULL default '0',
    lmail varchar(36) NOT NULL default '',
      lpass varchar(36) NOT NULL default '',
    PRIMARY KEY  (uid,lid)
  );
ALTER TABLE mailman_lists ADD COLUMN web varchar(255) DEFAULT ''";
ALTER TABLE mailman_lists ADD COLUMN webarch varchar(255) DEFAULT ''";
ALTER TABLE mailman_lists CHANGE admin admin VARCHAR( 48 ) DEFAULT ''";
