Drupal thickbox module:
------------------------
Author - Fredrik Jonsson fredrik at combonet dot se
Requires - Drupal 5
License - GPL (see LICENSE)


Overview:
--------
The Thickbox module is a wrapper for the jQuery plugin ThickBox.
The jQuery library is a part of Drupal since version 5.

Thanks to user contributions the module provides login forms, automated integration
with the image module and CCK imagefields.

* jQuery - http://jquery.com/
* ThickBox - http://jquery.com/demo/thickbox/

From the ThickBox homepage:
"ThickBox is a webpage UI dialog widget written in JavaScript on top of
the jQuery library. Its function is to show a single image, multiple
images, inline content, iframed content, or content served through AJAX
in a hybrid modal."


Installation:
------------
Installation is as simple as copying the module into your 'modules'
directory, then enabling the module at 'administer >> modules'.


Configuration:
-------------
If you use the image module there are a setting that will automaticly 
activate Thickbox for all image nodes.


Contributions:
-------------
* imagecache+thickbox formatters to imagefields (CCK)
  by Jšrg Hermsdorf (yojoe)
* thickbox_auto.js contributed by Johannes Dillmann (kleingeist).
  Provides automated integration with the image module.
* thickbox_login.js contributed by Jeff Miccolis (jmiccolis).
  Provides Thickbox login forms.

Last updated:
------------
$Id: README.txt,v 1.4.2.2 2007/04/05 08:11:49 frjo Exp $