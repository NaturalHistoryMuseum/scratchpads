$Id: README.txt,v 1.4.2.1 2008/01/29 00:21:44 agileware Exp $

The Editview Drupal Module

   Editview is a plugin for the [1]Views module. It allows you to create a view
   in which the nodes are editable, and new nodes can be created. Editview uses
   AJAX to speed things up, but will still work on browsers with javascript
   disabled. Editview is only known to work under drupal 4.7, and has not yet
   been extensively tested.

Installation

   To install Editview:
    1. Ensure that [2]the Views module is installed and enabled.
    2. Unpack [3]the latest editview tarball in your drupal modules directory.
    3. Enable the editview module on your drupal admin/modules page.

Usage

   Editviews are created just like any other type of view, with a few caveats.

  Select the View Type

   Normal views allow you to choose whether they are one of:
     * Full Nodes
     * Teaser List
     * Table View
     * List View

   Installing the Editview module adds another option:
     * Edit View

   If you select that option then your view is an Editview.

  Caveat 1

   Editview assumes that your view is filtered to display only one node type.
   This is so that the Editview knows what type to make any nodes it adds.

  Caveat 2

   You should include any 'required' fields in the view, otherwise adding new
   nodes will be impossible, since any nodes you try to add will fail their
   validation.

Cookbook

  Make a view that allows you to edit and create pages on your site

   This Editview will show an editable table of page nodes. You can edit their
   title and body fields, save your changes, delete pages (with confirmation),
   and create new pages which are then added to the view.
   Steps:
    1. Create a new view, giving it a name and limiting access to roles who are
       able to create and edit pages.
    2. Check  the  'Provide  Page  View'  box,  and  give  your  view  an
       easy-to-remember url.
    3. In 'View Type', select 'Edit View'.
    4. In the 'Fields' section, add 'Node: Title', and 'Node: Body'
    5. In the 'Filters' section, add a 'Node: Type' filter, and select 'page'
       as the value.
    6. Save the view and visit its url.

  View, edit, and add 'child' nodes on a 'parent' node page

   This view uses cck, views, and editview to add the ability to see and edit a
   table of 'child' nodes on a 'parent' node page. We'll relate children to
   parents using a cck node reference field to the child node type.
   Steps:
    1. Create the parent and child node types. In this example we'll make
       'organisation' the parent type, and 'employee' the child type.
         1. Create 'organisation' as a cck node type, giving it whatever fields
            you like.
         2. Create 'employee' as a ckk node type. One of its fields should be a
            node reference which you should limit to only be able to access
            nodes of type 'organisation'. Make sure the node reference is a
            select list, not an auto-complete text field. We'll be relying on
            not having to enter the reference manually.
         3. At this point you may want to create a few test organisations and
            assign each one a few employees.
    2. Make  a  new table view listing employees, that will appear on the
       organisation view page:
         1. Create the view as a block of type 'Table View'.
         2. The view's fields should be the employee type's fields, without the
            organisation node reference.
         3. Add  the  organisation node reference as an argument. Put the
            following  as  the  argument  code:
			if  (arg(0) == 'node' && is_numeric(arg(1)))  {
				$organisation = node_load(arg(1));
				if ($organisation->nid) {
					return array($organisation->nid);
				}
			}
			This will  make  sure the view only shows employees of the current
			organisation being viewed.
         4. Save the view, and enable it as a block on the admin/block page.
         5. Configure  the block, putting the following code in the 'Page
            specific visibility settings':
			<?php
			if (arg(0) == 'node' && is_numeric(arg(1)) && arg(2) == '') {
				$node = node_load(arg(1));
				if ($node->type == 'content_organisation') {
					return true;
				}
			}
			return false;
			?>
			This will make sure that the view is only displayed on the view
			tab of organisation nodes.
    3. Create our edit view, which will appear on the edit tab of organisation
       nodes.
          + Go back to the views page and clone the view you just made. Change
            the type of the new view to 'Edit View' and save it.
          + Go to the blocks admin page and enable the edit view as a block,
            putting the following as its code (note the similarity (and minor
            (but important) difference) to above):
			<?php
			if (arg(0) == 'node' && is_numeric(arg(1)) && arg(2) == 'edit') {
				$node = node_load(arg(1));
				if ($node->type == 'content_organisation') {
					return true;
				}
			}
			return false;
			?>

   Now when you go to an organisation node, you will see a table listing all of
   that organisation's employees, and when you click on its 'edit' tab, the
   list of employees will become editable as well.

  More Cookbook Examples?

   If you find another useful way to use an Editview, write a quick description
   and I'll add it here.

Todo

     * Access control - display a table view when users can't edit nodes.
       Currently unprivileged users can't edit nodes using editview, but the
       node editing forms are still loaded and displayed.
     * Javascript libraries loaded alongside editview.js may be loaded more
       than once. For instance, the jscalendar library that comes with the
       jstools module will load a new jscalendar button every time you add or
       edit a node, and you end up with multiple buttons per row instead of
       just  one. The best fix would be to make sure all js libraries are
       idempotent.

Changelog

   29th January 2008
          - File uploads now work
          - Updated to the latest jquery.form.js (version 2.03)
          - Added hook_field_form_render so that other modules can define a
            way of rendering certain fields, especially when their views
            fieldname doesn't directly correspond to a node fieldname.
   2nd November 2006
          - An Editview now appears even when empty, so you can add nodes to an
          empty view.

About Editview

   Editview was developed  by Agileware Pty Ltd (http://www.agileware.net) and
   co-sponsored by Xu Media Solutions (http://xumedia.org) and Agileware..

References

   1. http://drupal.org/project/views
   2. http://drupal.org/project/views
   3. http://ftp.osuosl.org/pub/drupal/files/projects/editview-4.7.0.tar.gz
