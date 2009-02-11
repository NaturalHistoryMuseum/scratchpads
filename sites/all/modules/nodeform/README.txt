// $Id: README.txt,v 1.5 2008/03/07 19:09:53 ximo Exp $

ABOUT THIS MODULE
-----------------

The node form has a lot of room for improvement in terms of usability. Based
on work from the Usability group, this module was made to present different
alternatives to how the form is structured. Instead of merely looking at
mockups and screenshots, we may now trial the suggestions to get a better
understanding of how they work.

So far, vertical tabs and an accordion for fieldsets have been implemented.

Note: It only works with the Garland and Minelli themes for now.

For background to this module, please see these discussions:
http://groups.drupal.org/node/8365
http://groups.drupal.org/node/8305
http://drupal.org/node/190946


INSTALLATION
------------

1. Move the nodeform folder to your modules folder (often sites/all/modules)

2. Copy the following PHP function and paste it into the template.php file of
your theme (if using Garland, you will find it in themes/garland):

/**
* Implementation of theme_fieldset(), used to achieve custom styling of
* fieldsets on the node form.
*/
function phptemplate_fieldset($element) {
  // If we're currently at a node form, prepare all fieldsets (except
  // input formats) for further manipulation by jQuery and CSS.
  if (arg(0) == 'node' && (arg(1) == 'add' && arg(2)) || (is_numeric(arg(1)) && arg(2) == 'edit')) {
    if ($element['#parents'][0] != 'format') {
      $element['#attributes']['id'] = form_clean_id('edit-'. implode('-', $element['#parents']) .'-fieldset');
      $element['#attributes']['class'] = 'nodeform-fieldset';
    }
  }

  // Pass the element on to the original theme function for theming.
  return theme_fieldset($element);
}

3. Enable the module from Administer > Site building > Modules

4. Go to Administer > Content management > Post settings to select the
layout of the node form


CREDITS
-------

Development of this module by Joakim Stai. Vertical tabs implementation by
Bevan Rudge and Joakim Stai, based on mockups by SteveJB and others. Accordion
JS/CSS by Joakim Stai, based on mockups by couzinhub and others.

The alternatives presented here are results of numerous mockups and ideas
discussed on http://groups.drupal.org/usability.

jQuery UI Tabs 3 by Klaus Hartl.
jQuery Accordion by Jörn Zaefferer and Frank Marcia.
