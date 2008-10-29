------------------------
NOTIFY MODULE README
------------------------

This is a simple notification module. It provides e-mail notifications to
members about updates and changes to the Drupal web site.

Send comments to the new maintainer Rob Barreca <rob@electronicinsight.com>.

------------------------
REQUIREMENTS
------------------------

This module requires Drupal 5 and cron to be running.

------------------------
INSTALLATION
------------------------

1. Extract the notify module directory, including all its subdirectories, into
   your sites/all/modules directory.

2. Enable the notify module on the Administer >> Site building >> Modules page.
   The database tables will be created automagically for you at this point.

3. Modify permissions on the Administer >> User management >> Access control page.

4. Go to Administer >> Site configuration >> Notification settings and modify the
   settings to your liking.
   Note: e-mail updates can only happen as frequently as the cron is setup to.
   Check your cron settings.

5. To enable your notification preferences, click on the "My notification
   settings" on the "My account" page. Or, similarly go to another user's
   account page at user/<user_id_here> to modify his or her personal settings.

6. Additional options can be set at Administer >> User management >> Users by
   clicking the "Notification settings" tab.

------------------------
AUTHOR / MAINTAINER
------------------------

Kjartan Mannes <kjartan@drop.org> is the original author.

Rob Barreca <rob@electronicinsight.com> is the new maintainer.

------------------------
WISH LIST
------------------------

This is in no particular order.

- Filters on what to notify about.
- Options to get full text in mail.
- Some way of detecting mail bounces.
