*******************************************************
    README.txt for userplus.module for Drupal
*******************************************************

This module was developed by FunnyMonkey to make the user administration
process more efficient for administrators.

The userplus module supplements Drupal's user administration with the
following features:

1. "add multiple users" on a single form
2. "assign user roles" to multiple users on a single form
3. "assign user groups" works in conjunction with Organic
     Groups to provide group subscription for multiple users
     on a single form

These features can be found alongside Drupal's user management capabilities
on the "Userplus" menu item at admin/user/userplus.  Note that "assign user
groups" will only appear as a task if the Organic Groups module is enabled.

INSTALLATION:

1. Put the userplus directory in the modules directory that makes the
   most sense for your site.  See http://drupal.org/node/70151 for tips
   on where to install contributed modules.
2. Enable userplus via admin/build/modules.
3. [optional] Copy the contents of userplus.css and paste it into your
   theme's style.css.

CONFIGURATION:

It is not necessary to do any configuration, however you may customize the
number of rows that will appear on each of the userplus administration screens
by visiting admin/settings/userplus.

TECHNICAL NOTES:
During "add multiple users" user validation bypasses user.module because
errors that occur are reported through form_set_error().  Since our form
contains different fields than the standard "add user" form, it doesn't make
sense to raise errors on the standard fields.  Also, and this is the main
reason, if multiple errors occur in either 'name' or 'mail' when using the
standard user validation process, we will lose errors past the first one
because they get ignored in form_set_error().  To get around this, we do
our own validation in _userplus_validate_user(), however, since we don't
call hook_user('validate', ...) there is a good chance that this module
will not respect validation done by modules that implement and depend on
hook_user('validate', ...).

TO DO:

1. Think about how to possibly separate out core functionality from UI
in user.module so we can reuse more code there.

DRUPAL 5.x:

During the port to Drupal 5.x, some functionality was removed from
Userplus.  Since Drupal's core now provides a means to delete multiple
users, Userplus no longer needs to do this.  Also, it has become much
easier to switch users' roles in a bulk manner with the core functionality
provided by Drupal, so that has also been removed from Userplus.

Please use the issue queue -- http://drupal.org/project/issues/userplus -- to
request additional functionality, report bugs, provide patches, complain,
etc...
