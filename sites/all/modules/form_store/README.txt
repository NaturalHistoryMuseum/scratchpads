$Id: README.txt,v 1.1 2007/03/21 21:32:06 heine Exp $

Form store & Form collect.

Copyright (c) 2007 Heine Deelstra (http://heine.familiedeelstra.com).

Released under the GPL v2 (see LICENSE.txt).

Form store is a useless module by itself. It maintains and provides other 
modules with a list of forms. The module was originally designed to work with
MyCaptcha, but since then Captcha (http://drupal.org/project/captcha) relies
on it to add captcha points to arbitrary forms.


Thanks
======

Thanks to Xamox for testing.
Thanks to Sepeck for testing.
Special thanks to Tjeerd Zwaga.


Basic usage
===========

Enable Form store. Upon visiting Administer >> Site configuration >> Form store
(admin/settings/form-store) you'll notice a few default forms are in the list.

To add additional forms enable the module Form collect, visit Administer >> 
Site configuration >> Form store, tab Collect forms (admin/settings/form-store/
collect) and check "Collect forms while browsing the site" before saving the 
configuration.

Any form you visit will be added to the list on admin/settings/form-store. To 
disable collection visit admin/settings/form-store/collect again and uncheck
"Collect forms while browsing the site".


Important note
==============

Before updating the module, make sure that form collection is disabled.