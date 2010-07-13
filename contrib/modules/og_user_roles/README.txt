$Id: README.txt,v 1.1.2.13.2.4 2009/02/13 19:58:34 somebodysysop Exp $

DESCRIPTION
-----------
This module allows you to, for each group type, specify a list of roles
that group administrators are allowed to assign. In the subscriber list
(og/users/<node id>), a 'configure member roles' tab will appear if both
the group type is allowed to configure roles and the current user is an
admin for the group.

For example, if you created a "trusted user" role, and a new group 
called "official group," you could allow administrators of only "official
group" to add selected other group members of "official group" to the 
"trusted user" role (only within "official group").  These administrators 
would see the "configure member roles" tab.  The group members added to 
the "trusted user" role would only have thse permissions while in the 
"official group" pages or context.

Furthermore, unlike the original og_roles module, this module will restrict
the role you select for the user to the group.  So, if you have a role called
"contributor", and you assign that role to a user and group using this module,
the user will only have the role "contributor" in the specified group, and will
not have it in any other group unless you specifically add the role to the
user in that group.

This module also has it's own access control for optionally integrating the 
taxonomy access control (TAC) and organic groups (OG) modules.
See: http://groups.drupal.org/node/3700.

Supports content_access and acl access control modules.	
See http://groups.drupal.org/node/5392

Works with og_vocab module.	See this issue: http://drupal.org/node/162649

Works with modr8 module.  See this issue: http://drupal.org/node/164092

Issue with Drupal core file upload module.  Documented here: 
http://drupal.org/node/166566

Optional settings and user.module patch to reset user_access() 
permissons cache:  http://drupal.org/node/166566

DOCUMENTATION
-------------
The most current documentation on this module is available online:
http://drupal.org/node/163565

REQUIREMENTS
------------
- Requires the organic groups module.
- Requires the mimemail module (if you wish to send automatic notifications).
- Requires the og_subgroups module (if you wish to create subgroups).
- Requires additional code in your theme's style.css
- Requires Clean URLs.
- Requires base url with no subdirectories. (i.e., your base site url must be
  "www.yoursite.com" and not "www.yoursite.com/subdirectory")
- Requires content_access and acl modules (if you wish content_access functionality)

INSTALLATION
------------
- If you are currently running the "OG Roles" module, please uninstall it.  It
  is incompatible with the "OG User Roles" module.

- Enable the module from administer >> modules.  This should create new table
  "og_users_roles".  If it doesn't then use the og_users_roles.sql file found
  in the distribution directory to create it manually.

- Go to administer >> user management >> access control and assign
  "configure member roles" to the roles you wish to be able to assign group
  roles to users.  Also, assign "administer og_user_roles" permission to user(s) who
  will be allowed to access the OG User Roles administration page.

- This module uses multi column checkbox settings for display of user roles.
  This saves space, particulary when your group has lots of users.
  To activate, you will need to drop the following code into the style.css
  file of your active theme(s):

.checkbox-columns .form-item {
  font-size:75%;
  width: 12em;
  margin-right: 1px;
  float: left;
  display: inline;
}
.checkbox-columns-clear .form-item {
  font-size:75%;
  width: 12em;
  margin-right: 1px;
  clear: left;
  float: left;
  display: inline;
}

- This module supports content_access and acl modules. If you have installed
  these access control modules, please follow integration instructions here:
  http://groups.drupal.org/node/5392

OPTIONAL PATCHES
----------------
There are some patches which are included in the installation tarball.
Please note that these patches are all optional and NOT required
for basic OG User Roles operation.  Detailed information on their 
usage can be found here: http://drupal.org/node/178874

SETTINGS
--------
Only users with role ('administer og_user_roles') may configure this module.

- Go to administer >> Organic Groups >> Organic groups user roles:

  Assignable roles
  ================
  Check the box next to each role you wish group admins to be able to
  manage.  Make sure you have assigned the appropriate privileges to
  roles selected here. Note that because these will be assignable by
  non-site admins, you should be conservative in what permissions you
  give (maybe 'create' permissions on a special node type, etc.)

  Again, whatever roles you select here, when they are assigned to
  a user in a group, that user will only have the permissions of that
  role while he is within the context of the group in which he is
  assigned the role.  When he is outside of that group's context,
  he will no longer have this role.

  Allow Group Admins to define Registration Codes for new group 
  subscribers to moderated groups?
  =============================================================
  Allows you to allow Group Administrators to define Registration Codes 
  to allow users to subscribe to moderated groups without administrator 
  approval. Your Group Admins must have the "manage registration codes" 
  permission to use this setting.  See details here: 
  http://drupal.org/node/217229

  Allow group admins to approve new signups?
  ==========================================
  If user registration to your site requires administrator approval 
  and you allow users to subscribe to groups at registration, then 
  you can optionally allow the administrator(s) of the group(s) to 
  which the user is subscribing to approve the signup request. 
  This will require that the administrator of each group that you 
  wish to give this privilege have a role which includes the 
  administer users permission.

    Allow group admins to approve new signups?

  If your site requires administrator approval for signups: When a 
  user registers and elects to subscribe to a group (group is on 
  registration form), then the admin of the group will receive an 
  email notification of the pending signup and be allowed to 
  approve it. Note that this requires that the group admin have 
  a group role which includes the administer users permission.
  Requires the mimemail.module: 
     
     http://www.drupal.org/project/mimemail

  See more details here: http://drupal.org/node/163567 

  Default Non-Group Role for new users
  ====================================
  If you wish to assign a default site-wide role to every new user that
  signs up to your web-site, place a check in:

    "Set default global (site-wide) role for new signups?"

  and, select the role to which you want all new signups assigned from
  the pulldown menu:

    "Selectable roles:"

  Default Basic Group Role for new group subscribers
  ==================================================
  If you wish to assign a default group specific role to every new subscriber
  to groups on your your web-site, place a check in:

    "Set default basic group (group limited) role for users who join groups?"

  and, select the role to which you want all new group subscribers assigned from
  the pulldown menu:

    "Selectable roles:"

  Allow Group Admins to define Default Basic Group Role for new group subscribers
  ===============================================================================
  Allows you to allow Group Administrators to define a default group role to 
  automatically assign to users who join their groups. The role is specific to 
  the group to which the user is subscribing. That is, the user will only have 
  the privileges of the role in the group he is subscribed to.
  
  Allow Group Admins to set default basic group (group limited) role for 
  users who join their groups?
  
  Do you wish to allow Group Admins to define a specific "basic group role" for 
  every new subscriber to their group at the time he subscribes to the group? 
  The role is limited to the group that he is subscribed to. This role 
  assignment can be be removed by the groups' admin(s). The Group Admin will 
  be able to define the default group role when he edits (not creates) the 
  group node.  The Group Admin must have the "configure member roles" permission
  to be able to define default group roles for his group.  See
  http://drupal.org/node/177414 for more information.

  Default Founder Role for users who create groups
  ================================================
  If you wish to assign a default group specific role to a user who creates
  a group, then place a check in:

    "Set default group founder (group limited) role for users who create groups?"

  and, select the group role to which you want the group "founder" user
  assigned from the pulldown:

    "Role to use as founder role:"

  Default Group Role for new group administrator
  ==============================================
  Allows you to select a group role to automatically assign to users who 
  are elevated to group administrator. The role is specific to the group(s) 
  in which the user is a group administrator. That is, the user will only 
  have the privileges of the role in the group he is the administrator for.

  If you wish to assign a default group specific role to every user who is
  newly elevated to group administrator status, then place a check in:

    "Set default group (group limited) administrator role for users who 
     are elevated to group administrator?"

  and, select the group role to which you want the new group administrator 
  user assigned from the pulldown:

    "Role to use as group administrator role:"

  Default Group Admin Notification for new subscribers
  ====================================================
  If you wish to automatically send an email notification to the group
  administrator when a new subscriber is added to a group, then place
  a check in:

    "Send email notification to group admin when new subscriber
    is added to group?"

  Note that this feature requires mimemail.

  Nodes to automatically place into all available groups
  ======================================================
  If you wish to have certain nodes automatically placed into all
  available groups each time they are modified, place the comma
  separated node id numbers into this box:

    "Nodes to automatically place into all available groups"

  Note: This is really a specialized customization for my own site.
  Use it only if you need it.

  TAC / OG Access Control Integration
  ===================================
  This is a very specialized feature that you should leave unchecked
  by default. We have been working on an access control project
  which allows TAC and OG to work in unison.  Basic details of this
  project are here: http://groups.drupal.org/node/3700

  In order to use this feature, you will need a special patch
  installed: http://groups.drupal.org/node/4026

  Also, to use this feature, you must install the OG Forum
  module: http://www.drupal.org/project/og_forum

  Again, unless you understand this project and know what you are
  doing, please leave this unchecked.

  Create Subgroups
  ================
    Allows you to select a group type to use for displaying a
    "Create Subgroup" link on your group menus. Users will need to
    have the "create og_subgroups" permission in order to access this
    link on their group menus. Note that this functionality requires
    the og_subgroups.module

  To use this, you must first have og_subgroups.module installed.
  Next, you place a check in this checkbox:

    Create link to "Create Subgroup" in group menu?

  Finally, you select the group type to use for creating subgroups
  from this pulldown menu:

    Group type to use for subgroup creation:

  Clear the Cache?
  ================
    We found that there are some modules which will return 
    "Access denied" message even when OG User Roles returns 
    permissions which allow the user access. This is due to cached 
    permissions being used instead of the group permissions. This 
    settiing allows you to reset these permissions for a user on 
    an ongoing basis if you are having this problem.

    Clear the user_access() and cache_menu caches for user?

    Do you wish to automatically clear the cache_menu item and 
    reset the user_access() cached permissions for this user? 
    If you select this option, you will need to apply the 
    og_user_roles.user.module.5.2.patch to the user.module. 
    See OG User Roles and File Upload http://drupal.org/node/166566 
    for details. Please note that this setting creates a heavy 
    overload as permissions must be re-created on each page load. 
    Use this setting only if absolutely necessary.

    Place a check in this checkbox if you wish to use this option. 
    If you select this option, you will need to apply the 
    og_user_roles.user.module.5.2.patch to the user.module for it 
    to be effective.

  Test/Debug
  ==========
  This option allows you to output debug info to a table.  You must first 
  follow these instructions here to create the table: 
  http://drupal.org/node/164038
  
  Once the table is created, place a check in this checkbox:

    Output debug data to og_user_test table?
    
  Every time user_access() is called, it will call og_user_all_roles() 
  which adds group roles to $user->roles. This option allows you to
  see the output from this function. (Note that this feature is for 
  testing/debug purposes, and could create a very large output file. 
  This feature also requires that the table og_user_test already exist.) 

  Don't forget to click on "Save configuration" button to save your choices.

  See http://drupal.org/node/164038 for more details.

USAGE
-----
- As a group manager or administrator, go to the Group Home Page. In the
  group navigation menu you should see an link titled:

    "# subscribers".

  Click on this link.  You should next see a menu tab which says:

    "Configure member roles"

  Click on this tab.  You should next see the list of subscribers for this
  group.  Next to each subscriber should be a list of the assignable
  roles you selected in "SETTINGS" step above.

  Check on each role that each user in the group should have, then don't
  forget to click on the "Save Changes" button at the bottom of the page.

  Each user will now have the privileges of the role(s) you have checked
  for him while he is within the context of this group.

  Using Content Access and ACL with OG User Roles: http://groups.drupal.org/node/5392

NOTES
-----
- Please note that these roles and their permissions will only take effect within
  the specified group.  If you want a user to have the permissions of a role
  sitewide, then you must apply the role to the user

- Group Admin.  When a user who is not the group creator is assigned to be a
  "Group Admin" for the group, we have noticed that the "Add subscribers" tab
  does not appear for this user when "og user roles" is installed.
  The fix for this is to:

  a.  Create a "GroupAdmin" role in "access control", and give this role the
    "edit group content" permission.
  b.  Go into "configure member roles" for the group, and give the Group Admin
    user the "GroupAdmin" role in this group.

- Noted incompatibility with Auto Assign module.  http://drupal.org/node/149483
  Can't use both modules to assign the same role to users on signup.

- comment 'Done - note to others' by Ricco

  I followed the instructions, and all went smooth as silk. To save others
  time, here are some notes;

  *) you must have the og_forum module installed first. I hadn't, you can
  find it here; http://drupal.org/project/og_forum [1]

  *) it seems that you must disable the og_roles module before you try to
  enable the og_user_roles module. I hadn't and when I tried to enable the
  module, i got the happy message;


  Fatal error: Cannot redeclare _user_roles() (previously declared in
  /...mypathinfo.../modules/og_roles/og_roles.module:199) in
  /...mypathinfo.../modules/og_user_roles/og_user_roles.module on line 244


  that seems to lock me up for a few seconds... and would not let me even
  view my site... so use FTP and delete the og_user module (bring it down
  to your site of course first for backup) and then refresh your browser,
  and you can get back in, and continue.


CREDITS
-------
The original og_roles module was Authored and maintained by Farsheed Hamidi-Toosi
and Angela Byron of CivicSpace Labs Sponsored by Raven Brooks of BuyBlue.org

Code for registration code functionality inspired by "Registration Code" module
by Drupal user "colan". That module owes it's original code to Drupal user
"nevets".

This version was modified from their original version.

