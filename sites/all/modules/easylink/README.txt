$Id$
EasyLink Module README

Credits
-----------------------------------------------------------------------
This module was developed for the Royal Oak Schools website by The Linuxbox Corporation.  If you have any questions, comments or feedback, please send e-mail to:

techstaff@linuxbox.com


Description
-----------------------------------------------------------------------
The purpose of this module is to make link creation in TinyMCE as easy as possible for non-technical users.  It adds a button to the TinyMCE toolbar that will open a pop-up window with a list of nodes the user can create a link to.  The user then clicks on the title of the node they wish to link to and the link is created.  This allows users to create links without having to know how to create URLs.  The list of pages is defined by giving a view on the configuration page.  Using a view allows the list of nodes to be easily customized through a familiar interface.


Installation
-----------------------------------------------------------------------
NOTE: Installation requires two parts.  
  1 - Install the EasyLink module.
  2 - Install the TinyMCE EasyLink plugin.

Part 1
----------------------------------
1. Unpack the tarball in your modules directory.

Part 2
----------------------------------
1. Inside the easylink module directory is a subdirectory (also called easylink) that contains the TinyMCE plugin.  Copy the subdirectory 'easylink' to the TinyMCE plugins directory (ex: modules/tinymce/tinymce/jscripts/tiny_mce/plugins/easylink)

2. Register the plugin with TinyMCE by adding the following lines to plugin_reg.php

$plugins['easylink'] = array();
$plugins['easylink']['theme_advanced_buttons1'] = array('easylink');

3. Visit the Drupal modules administration page and enable the EasyLink module.

4. Edit your TinyMCE profile to include the EasyLink plugin.

Configuration is now complete.  Goto a page with a TinyMCE editor on it and click the icon with the red 'ez' letters on it to see it in action!


Advanced Configuration
-----------------------------------------------------------------------
Easylink ships with a default view that shows all pages authored by the current user.  If you would like to use a different view go to the easylink configuration page (administer -> settings -> easylink ) and set the view you wish to use.  You may use view with pagers and exposed filters if you like.
