$Id$

DESCRIPTION
-----------

XML Sitemap automatically creates a site map that conforms to the sitemaps.org
specification. This helps search engines keep their search results up to date.
XML Sitemap replaces the Google Sitemap module written by Matthew Loar as part
of Google Summer of Code 2005.

You can view your site map at http://www.example.com/?q=sitemap.xml.

INSTALLING XML SITEMAP
----------------------

See INSTALL.txt in this directory.

SETTING UP YOUR SITE MAP
------------------------

Go to http://www.example.com/?q=admin/settings/xmlsitemap to configure general
settings.

Chunk size: The maximum number of links to include a single site map. If the
total number of links exceeds the chunk size, multiple site maps will be
generated, and http://www.example.com/?q=sitemap.xml will be an index of your
site maps.

Links may be assigned a priority between 0.0 and 1.0. The default priority is
0.5. A priority of "Not in site map" excludes a link from the site map.

Front page priority: This setting determines the priority of the front page.

By itself, the XML Sitemap module adds only the front page of your site to the
site map. Other types of links are handled by supporting modules.

XML Sitemap: Menu
Allows menu items to be added to the site map. You can choose the menus to
include at http://www.example.com/?q=admin/settings/xmlsitemap and can add and
remove menu items at http://www.example.com/?q=admin/build/menu. The priority of
a menu item is determined by its weight.

XML Sitemap: Node
Adds nodes to the site map and provides Views style plugins for creating
specialized site maps with Views. The default priority of a node is determined
by a combination of its content type priority, whether it appears on the front
page of your site, and the number of comments it has received. You can set
content type priorities at http://www.example.com/?q=admin/content/types, and
you can override the default priority for individual nodes when you add or edit
a node.

XML Sitemap: Term
Adds taxonomy terms to the site map. You can change the default priority when
you add or edit a vocabulary, and you can override the default priority when you
add or edit individual terms.

XML Sitemap: User
Adds user profiles to the site map. The anonymous user role must be given
permission to access user profiles at
http://www.example.com/?q=admin/user/access. You can change the default user
priority at http://www.example.com/?q=admin/user/settings. The priorities set
for user roles at http://www.example.com/?q=admin/user/roles will override the
default user priority. You can override both the default priority and the role
priority when you add or edit a user.

SUBMITTING YOUR SITE MAP TO SEARCH ENGINES
------------------------------------------

XML Sitemap can notify search engines when your site map changes. Which search
engines you can notify depends on the optional modules you have enabled. The
included "XML Sitemap: Engines" module supports Google, Yahoo!, Ask, and Windows
Live.

Go to http://www.example.com/?q=admin/settings/xmlsitemap and choose the Search
engines tab to configure search engine notifications.

Submit site map when updated: Notify search engines every time your site is
updated. You should check this box only if it’s very important that search
engines be notified of changes immediately or you cannot schedule notifications
with cron.

Submit site map on cron run: This is the preferred way to notify search engines
of updates. XML Sitemap will wait until the next cron run to notify search
engines of updates.

Log access: Record each time the site map is retrieved.

MORE INFORMATION
----------------

XML Sitemap documentation: http://drupal.org/handbook/modules/gsitemap

The Sitemap protocol: http://sitemaps.org.

Search engines:
http://www.google.com/support/webmasters/bin/topic.py?topic=8476 (Google)
http://developer.yahoo.com/search/siteexplorer/V1/ping.html (Yahoo!)
http://asksupport.custhelp.com/cgi-bin/ask.cfg/php/enduser/std_adp.php?p_faqid=2729 (Ask)
http://webmaster.live.com/ (Windows Live)

