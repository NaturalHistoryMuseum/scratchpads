$Id: README.txt,v 1.1.4.1 2008/06/02 08:02:45 allisterbeharry Exp $

Views Datasource README
---------------------------------------

Current Version
---------------
5.x-1.0-ALPHA1

Release Notes
-------------
CVS module created and intial code uploaded to repository. This is a 
proof-of-concept release with working views_json and views_xml for Drupal 5. 
In the Views interface simply select the view type as Views JSON: MIT 
Simile/Exhibit JSON data document, Views XML: Raw XML data document, or 
Views XML: OPML XML data document. For an existing view you can add arguments
with the same names as above to get an icon on your view page to access the XML
or JSON rendering of your view, similar to how the RSS feed argument works.

About
-----
Views Datasource is a set of plugins for Views for rendering node content in a 
set of shareable, reusable data formats based on XML, JSON, and XHTML. These 
formats allow content in a Drupal site to be easily used as data sources for 
Semantic Web clients and web mash-ups. Views Datasource plugins output content 
from node lists created in Drupal Views as:
  1)XML data documents using schemas like OPML and Atom;
  2)RDF/XML and RDF/N3 data documents using a vocabulary like FOAF;
  3)JSON data documents in a format like MIT Simile/Exhibit JSON;
  4)XHTML data documents using a microformat like hCard
  
The project consists of 4 Views style plugins:
  1)views_xml - Output as raw XML, OPML, and Atom;
  2)views_json - Output as Simile/Exhibit JSON, canonical JSON, JSONP;
  3)views_rdf - Output as FOAF, SIOC and DOAP;
  4)views_xhtml - Output as hCard, hCalendar, and Geo. 