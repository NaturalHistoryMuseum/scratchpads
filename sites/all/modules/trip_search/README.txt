Readme
------

Trip search is a search module that does not use Drupal search
indexing.  Instead, it uses native MySQL full text index searching.
Some code is there to allow searching with regular expressions,
if you are using PostgreSQL. However this has not yet been tested
and probably does not work.

I explained here why I still prefer SQL Search to the standard
Drupal search: http://drupal.org/node/87579

The module is not fully tested in this 4.7 release.  Please use
the drupal.org issues system to identify issues--and post fixes!

Trip search offers advanced "Google-like" search operators:

* phrases (enclose search string in double quotes)
* exclude terms (preceed term with minus sign)
* OR operator

These operators can be entered directly or can be entered
into an "advanced search" page.

Additional features:

* filter results by category, by user and by content type
* paging of results
* ranking of results by multiple parameters
  (number of matches, location of matches (title vs. body),
  number of hits, 'sticky' flag, etc.)
* logs search actions to watchdog
* search block
* admin settings including tweaking of search ranking
* node access security control

Dependencies
============
By itself, trip_search uses an old version of jscalendar. For
nicer, DHTML popup calendars, you should install the Javascript tools
module and enable jstools and jscalendar. You can modify the jscalendar
look by changing its skin in the admin >> settings >> jscalendar menu.
You should also change the jscalendar css to remove or change the width setting
for form-text.jscalendar: the width setting is handled by the trip_search 
form size


Usage
------

If you wish to have search terms highlighted on resulting
pages, install and enable highlight.module.


ToDo
====
This is hopefully a "production" release, though there still 
a few things to finish off:

1) handling statistics
2) Update the translations (French is done)
errr.... that's it I think

And more ambitious stuff...
a) Getting the module to use MySQL ranking
b) making ot possible to call trip_search from other modules

Release information : DRUPAL-4-7--1-1
=====================================
1) Updated to support 4.7.4 forms security release
2) Search results are logged
3) Fixed problem displaying teasers for anonymous users

Release information : DRUPAL-4-7--1-3
=====================================
1) Fixed XSS weakness in the log entries and SQL queries
2) Integrated jstools jscalendar for a nicer calendar
3) Content types and users are supported as selection criteria
