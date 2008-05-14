LIGHTBOX V2 MODULE

------------------

Drupal Lightbox V2 Module:
By: Mark Ashmead
Mailto: bugzie@gmail.com
Co-maintainer: Stella Power (http://drupal.org/user/66894)

Licensed under the GNU/GPL License

Based on Lightbox v2.03.3 by Lokesh Dhakar
<http://www.huddletogether.com/projects/lightbox2/>

Originally written to make use of the Prototype framework, and Script.acalo.us,
now altered to use jQuery.

Permission has been granted to Mark Ashmead & other Drupal Lightbox2 module 
maintainers to distribute the original lightbox.js via Drupal.org under this
license scheme.  This file has been subsequently modified to make use of jQuery
instead of prototype / script.acalo.us.

This module enables the use of lightbox V2 which places images above your 
current page, not within. This frees you from the constraints of the layout, 
particularly column widths.

---------------------------------------------------------------------------------------------------------

Pre-Installation
----------------
* Ensure you have the "jQuery update" module installed.


Installation
------------
1. Copy lightbox2 folder to modules directory
2. At admin/modules enable the module
3. Add rel="lightbox" attribute to any link tag to activate the lightbox. For example:

<a href="images/image-1.jpg" rel="lightbox" title="my caption">image #1</a>

Optional: Use the title attribute if you want to show a caption.

4. If you have a set of related images that you would like to group, follow step one but additionally include a group name between square brackets in the rel attribute. For example: 

<a href="images/image-1.jpg" rel="lightbox[roadtrip]">image #1</a>
<a href="images/image-2.jpg" rel="lightbox[roadtrip]">image #2</a>
<a href="images/image-3.jpg" rel="lightbox[roadtrip]">image #3</a>

No limits to the number of image sets per page or how many images are allowed in each set. Go nuts! 

5. If you wish to turn the caption into a link, format your caption in the following way:

<a href="images/image-1.jpg" rel="lightbox" title='<a href="http://www.yourlink.com">Clicky Visit Link</a>'>image #1</a>


Information
------------

This module will include the lightbox CSS and JS files in your Drupal 
Installation without the need to edit the theme. The module comes with a 
Lightbox2 Lite option which does not use the JQuery library; it is therefore 
less likely to conflict with anything else. 

Known Issues
-------------
There is an issue with Lightbox Lite in IE browsers but only for sites where
Drupal is installed in a subdirectory.  In such instances, the overlay.png image
can not be found.  To overcome this issue you will need to edit the
lightbox2/css/lightbox_lite.css file and change the path to this image.  By
default the line is set to:

filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="../images/overlay.png", sizingMethod="scale");


You will need to change the image path on this line to be the full path, e.g.

filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="/sites/all/modules/lightbox2/images/overlay.png", sizingMethod="scale");


See http://drupal.org/node/185866 for more details.
