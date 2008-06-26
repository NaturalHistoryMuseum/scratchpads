"How to Make TAC and OG Work Together"

http://groups.drupal.org/node/3700 (please read carefully)

Also read "Multiple Node Access logic patch": http://drupal.org/node/196922

The OG User Roles module is required for TAC/OG access
control.  It can be downloaded here: 

	http://www.drupal.org/project/og_user_roles

Once OG User Roles module is downloaded and installed, you must:

a. Install the patch described below. 
b. Go to OG User Roles settings and place check in "TAC/OG Access 
   Control Integration" ("Integrate TAC and OG Access Control").
c. Go to OG User Roles settings and click on the "Configure multinode UI"
   tab.  You will see the "multinode_access" table listing of node 
   grant realms.  You need to make the following changes to this table:

   ogr_access
   ==========
   Place check in checkbox next to "ogr_access" Realm name.
   Enter "ogr" in Group column.
   Select "OR" as Logic.
   Select "1" as Weight.
   Select "0" for Check.

   term_access
   ===========
   Place check in checkbox next to "term_access" Realm name.
   Enter "tac" in Group column.
   Select "AND" as Logic.
   Select "0" as Weight.
   Select "0" for Check.

   All other realms should remain unchecked.

   See the screenshot of what your table modification should look
   like: http://drupal.org/files/OGRConfigureMultinodeUI.jpg
   
   Click on "Save changes" button to save your changes.

Patches:
	
	node.module.multinode.patch

This patch is also available from here: 

   http://cvs.drupal.org/viewcvs/drupal/contributions/sandbox/somebodysysop/TAC_OG/
   http://ftp.scbbs.com/pub/drupal/TAC_OG/

How to apply patches: http://drupal.org/node/60108

Download and place this patch file into your Drupal web site "/modules/node"
directory.

Apply Patch
===========
patch -p0 < node.module.multinode.patch

Note: These instructions are for OG User Roles release 3.0 and higher.

