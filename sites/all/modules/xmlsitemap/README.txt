$Id$
XML Sitemap Module
Author: Matthew Loar <matthew at loar dot name>
This module was originally written as part of Google Summer of Code 2005.

DESCRIPTION
-----------
The XML Sitemap module creates an XML site map at ?q=sitemap.xml that
conforms to the sitemaps.org specification. It provides many options for
customizing the data reported in the site map.

INSTALLATION
------------
See INSTALL.txt in this directory.

KNOWN ISSUES
------------
Users who have not enabled clean URLs have reported receiving an
"Unsupported file format" error from Google. The solution is to replace
"?q=sitemap.xml" at the end of the submission URL with
"index.php?q=sitemap.xml". Submission URLs for each search engine can be
configured at Administer -> Site configuration -> XML Sitemap.

MORE INFORMATION
----------------
The Sitemap protocol: http://sitemaps.org.

Search engines:
http://www.google.com/webmasters/sitemap
http://developer.yahoo.com/search/siteexplorer/V1/ping.html

Site maps may be manually submitted to MSN by visiting
http://search.msn.com/docs/submit.aspx and submitting the URL of the site
map.

