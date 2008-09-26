// $Id: README.txt,v 1.1 2007/05/30 21:26:10 ngroot Exp $

Remove Nonviewable Menu Items
------------------------
Drupal does not check to ensure that the user has access to view 
a node referenced by a custom menu item before it displays the 
menu item.  This module searches through the {menu} table, looks 
for nodes, and checks to see which nodes the user is unable to view.
Those nodes are added to the the menu tree with the access attribute
set to FALSE, ensuring they do not appear in menus.

This was motivated by my use of the category_menu and cac_lite 
modules on a site I was building.  Menu items which were not accessible
to users in certain roles would appear even though they did not
have access to them, creating usability and (minor) security problems.

If you're wondering where the configuration page is for this module,
you will not find it.  This module does one thing.  I hope it does it 
well. :)  Feel free to let me know if it doesn't, or even better,
fix it!

- Neal Groothuis
neal [at] twotonetech.com
