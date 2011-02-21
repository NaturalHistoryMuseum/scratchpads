-------------------------------
Comment Mail
-------------------------------

Originally written by Jeff Robbins (jeff /attt\ lullabot /dott\ com)
with additonal work done by Konstantin KŠfer (http://drupal.org/user/14572).

The Comment Mail module allows an e-mail to be sent to the site
administrator(s) when new comments are posted. A link in the
email allows quick approval, editing, deletion, and/or banning
of the poster's IP address (good for spam).

Installation:
=============
1. Place the commentmail folder in your module directory (normally sites/all/modules directory).
2. Enable the module at Administer > Site building > Modules (admin/build/modules).
3. Configure it at Administer > Site configuration > Comment mail (admin/settings/commentmail).

You can set the address to which the emails will be sent, determine under what
circumstances to send an email, as well as setting the text of the emails.

There are two templates for email. One is for those sites that
have comment moderation active and therefor need to approve comments to
publish them. The other is for comments that are automatically published
and so only need deletion actions.
