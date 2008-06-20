$Id$

DESCRIPTION
-----------

This module sends html or plain text newsletters to the subscription list. In
the newsletter footer an unsubscribe link is provided. Subscription and
unsubscription are managed through a block, a form or by the newsletter
administrator on the module's admin pages.

Individual newsletters are grouped in newsletters by a newsletter taxonomy term.
Newsletters can have a block with the ability of (un)subscription, listing of
recent newsletters and an associated rss-feed. 

Send newsletters and not-sent newsletters are listed separately. The
subscription list can be managed. 

Sending of large mailings can be managed by cron.

REQUIREMENTS
------------

 * Drupal 5
 * Taxonomy module
 * For large mailing lists, cron is required.
 * HTML-format newsletters and/or newsletters with file attachments require the
   mimemail module.


INSTALLATION
------------

 1. CREATE DIRECTORY

    Create a new directory "simplenews" in the sites/all/modules directory and
    place the entire contents of this simplenews folder in it.

 2. ENABLE THE MODULE

    Enable the module on the Modules admin page:
      Administer > Site building > Modules

 3. ACCESS PERMISSION

    Grant the proper access to user accounts at the Access control page:
      Administer > User management > Access control. 
    To enable users to (un)subscribe to a newsletter use the "subscribe to
    newsletters" permission. This will enable the Simplenews block where the
    user can (un)subscribe to a newsletter. 
    Use the "view links in block" permission to enable the display of previous
    newsletters in the Simplenews block.

 4. ENABLE SIMPLENEWS BLOCK

    Enable the Simplenews block on the Administer blocks page:
      Administer > Site building > Blocks.
    One block is available for each newsletter you have on the website. Note
    that multiple newsletter blocks with subscription form does not work. This
    is a known bug in Drupal 5 version of Simplenews.
    See http://drupal.org/node/121479

 5. CONFIGURE SIMPLENEWS

    Configure Simplenews on the Simplenews admin pages:
      Administer > Content Management > Newsletters > Settings.

 6. CONFIGURE SIMPLENEWS BLOCK

    Configure the Simplenews block on the Block configuration page. You reach
    this page from Block admin page (Administer > Site building > Blocks). Click
    the 'Configure' link of the appropriate simplenews block.
 
    Permission "subscribe to newsletters" is required to view the subscription
    form in the simplenews block or to view the link to the subscription form.
    Links in the simplenews block (to previous issues, previous issues and
    RSS-feed) are only displayed to users who have "view links in block"
    privileges.

 7. SIMPLENEWS BLOCK THEMING

    More control over the content of simplenews blocks is possible using the  
    block theming. Additional variables are available for custom features:
      $block['subscribed'] is TRUE if user is subscribed to the newsletter
      $block['user'] is TRUE if the user is logged in (authenticated)
      $block['tid'] is the term id number of the newsletter

 8. TIPS

    A subscription page is available at: /newsletter/subscriptions

SEND NEWSLETTERS WITH CRON
--------------------------

Cron jobs are required to send large mailing lists. Cron jobs can be triggered
by Poormanscron or any other cron mechanisme such as crontab.
If you have a medium or large size mailinglist (i.e. more than 500
subscribers) always use cron to send the newsletters.
  
When you use cron:
 * Set the Initial send time to zero seconds.
   Failure may lead to sending duplicate newsletters.
 * Make sure the number of newsletters send per cron run (Cron throttle) is not
   too high.
   Too high values will lead to the warning message 'Attempting to re-run cron
   while it is already running'
    
When you do not use cron:
 * Set the Initial send time high enough to send all newsletters.
   Failure may lead to receipients not receiving their newsletter.
    
These settings are found on the Newsletter Settings page at:
  Administer > Content Management > Newsletters > Settings > General

COLLABORATION WITH OTHER MODULES
--------------------------------

 * Taxonomy
   The taxonomy module is required by Simplenews. Simplenews creates a
   'Newsletter' vocabulary which contains terms for each series of newsletters.
   Each newsletter node is tagged with one of the terms to group it into one of
   the newsletter series.

 * Mimemail
   By using Mimemail module simplenews can send HTML emails. Mime Mail takes care
   of the MIME-encoding of the email. 
   Mime Mail is also required to be able to send emails with attachments, both
   plain text and HTML emails.

 * Simplenews_template
   Simplenews Template provides a themable template with configurable header,
   footer and style. Header, footer and style are configurable for each
   newsletter independently.

 * Simplenews_roles
   A helper module for the Simplenews module which automatically populates a
   newsletter subscription list with users from specified roles.

 * Category
   Simplenews and Category module are currently NOT COMPATIBLE.
   See http://drupal.org/node/115693

 * Poormanscron
   Use Poormanscron if you don't have access to cron systems such as crontab.
   Read the 'Send newsletters with Cron' remarks above.

CREDITS
-------

Originally written by Dries Knapen.
Currently maintained by Sutharsan and RobRoy.
