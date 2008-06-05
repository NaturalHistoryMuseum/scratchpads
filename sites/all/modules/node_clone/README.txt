$Id$

README file for the clone module for Drupal 5.x.

5.x-2.x branch.

The clone module allows users to make a copy of an existing node and then edit 
that copy. The authorship is set to the current user, the menu and url aliases 
are reset, and the (localized) words "Clone of" are inserted into the title to 
remind you that you are not editing the original node.

Users with the "clone node" permission can utilize this functionality. A new
tab will appear on node pages with the word "clone".  The -2.x branch of this
module works by pre-populating the node form, rather than immediately saving
a copy of the original node to the database.  Thus, your node will not be
saved until you hit "Sumit" (just like if you went to node/add/x).

This module makes reasonable checks on access permissions.  A user cannot clone 
a node unless they can use the input format of that node, and unless they have
permission to create new nodes of that type based on a call to node_access().

Settings can be accessed at admin/settings/clone.  On this page you can
set whether the publishing options are reset when making a clone of a node.  
This is set for each node type individually.

This module seems to work with common node types, however YMMV, especially with
nodes that have any sort of image or file  attachments.   In all cases, but 
especially if you are using a complex (CCK) or custom node type, you should 
evaluate this module on a test site with a copy of your database before 
attempting to use it on a live site.

To install this module, copy the folder with the .info and .module files to the 
/sites/all/modules  OR /modules directory of your Drupal installation and enable
it at /admin/build/modules.  A new permission is available, but there are no 
changes to the database structure.

Note: this module originally derived from code posted by Steve Ringwood 
(nevets@drupal) at http://drupal.org/node/73381#comment-137714

