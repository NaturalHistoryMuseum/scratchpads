pagearray

README

Description
------------

Page Array is a small helper module that can be used whenever there is a need
to load the output of any Drupal path in data form. Based on the code in
index.php, Page Array bypasses the theme('page') call, instead returning
the page's output in data form. This can then be used for tasks like
outputting in Javascript form, theming for printing, etc.

Key features:
* Loads data for any path, not just the current one.
* Respects all access restrictions.
