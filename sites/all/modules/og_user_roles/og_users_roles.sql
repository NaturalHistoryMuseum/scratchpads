-- Drupal dba.module database dump
--
-- Database: drupal03
-- Date: Sunday, February 25, 2007 - 14:44

--
-- Table structure for table 'og_users_roles'
--

CREATE TABLE og_users_roles (
  uid int(10) unsigned NOT NULL default '0',
  rid int(10) unsigned NOT NULL default '0',
  gid int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (uid, rid, gid)
) TYPE=MyISAM;

