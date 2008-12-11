Drupal node_import.module README.txt
==============================================================================

This module allows you to import a set of nodes from a Comma Separated
Values (CSV) or Tab Separated Values (TSV) text file.

The module currently supports following types natively:
 * ecommerce tangible product (shippable product),
 * event,
 * page,
 * story,
 * weblink,
 * any node type created with the Content Creation Kit (CCK),
 * any flexinode node type.

Additionally it also has support for importing:
 * event-enabled nodes,
 * location-enabled nodes,
 * taxonomy-, or category-enabled nodes.

Installing node_import (first time installation)
------------------------------------------------------------------------------

1. Backup your database.

2. Copy the complete 'node_import/' directory into the 'modules/' directory of
   your Drupal site.

3. Enable the module from Drupal's admin pages (Administer >> Site building
   >> Modules). The needed tables will be automatically created.

   If the creation of the database tables fail, you can create them manually:

   CREATE TABLE node_import_mappings(
     type VARCHAR(16) NOT NULL DEFAULT '',
     csv_headers TEXT NOT NULL,
     mapping TEXT NOT NULL,
     KEY type(type)
   );

   Do not forget to prefix the table names if you use database table
   prefixing. Note that 'type' may be a reserved word for your database
   version. In this case you need to quote it:

   CREATE TABLE node_import_mappings(
     "type" VARCHAR(16) NOT NULL DEFAULT '',
     csv_headers TEXT NOT NULL,
     mapping TEXT NOT NULL,
     KEY "type"("type")
   );

4. Assign the 'import nodes' permission to the desired roles (Administer >>
   User management >> Access control) that are allowed to import nodes from
   a file. Note that the user will also need the correct permissions to
   create the nodes, eg 'create stories' to import 'story' nodes.

Upgrading node_import (on Drupal 4.7 or later)
------------------------------------------------------------------------------

1. Backup your database.

2. Remove the old 'node_import.module' or old 'node_import/' directory from the
   'modules/' directory of your Drupal site (possible after making a backup
   of it).

3. Copy the complete 'node_import/' directory into the 'modules/' directory of
   your Drupal site.

4. Go to the modules administration page (Administer >> Site building >>
   Modules) and select to run update.php.

   The data from the previous version will automatically be converted to
   the new format if needed.

Configuration
------------------------------------------------------------------------------

Give the roles which are allowed to import files the "import nodes" permission
on the access control administration page (Administer >> User management >>
Access control).

Users will not be able to import nodes of types which they are not allowed
to create, so you may need assign any of the "create XXXs" permissions too.

Node import wizard
------------------------------------------------------------------------------

The module provides a wizard at "Administer >> Content management >> Import
content" that walks the administrator or user through the import:

 1. The user selects the file to upload.

    This can either be a Comma Separated Values (CSV) or a Tab Separated
    Values (TSV) text file. The module will autodetect the format of the
    file if so set.

    It will autodetect a Tab Separated Values file if there is a tab-character
    on the first line.

 2. The user selects the node type she wishes to import this data into (for
    example 'page' or 'story').

    The user will only be able to select content types she has the right to
    create. The administrator will be able to import nodes of all types.

 3. The user maps the fields in the file to fields for the selected node
    type.

    The wizard shows the sample data of the first 5 rows to help her.

 4. The user selects some options for the import.

    The available options depend on the content type imported. See "Creating
    a file for node import" below.

 5. The user can then preview the nodes that would be imported and can make
    any change necessary by going back to previous pages.

 6. Finally, if all is well, the user can import the nodes.

After the import, the user may download a file with the rows from the
original file that failed to import.

Creating a file for node import
------------------------------------------------------------------------------

The module supports two file formats:

 - Comma separated values (CSV) text file,
 - Tab separated values (TSV) text file.

Both formats must contain one row of "column titles" as first row of the file.
These titles will be used in the "field mapping" step of the wizard (step
3 above).

Parsing a CSV file is handled by the "fgetcsv" function. This means that your
CSV file should use double quotes around strings that contain a comma. See
http://www.php.net/fgetcsv for details.

Note that the CSV file requires *comma's* for separating the columns, not
semicolons that Excel exports by default.

Any date you import should be a valid Unix timestamp (number of seconds
since January 1, 1970 00:00:00 GMT) or a string containing a valid US
English date format. See: http://www.php.net/strtotime.

Location options
****************

If the content type you import has locative information enabled on the
content type's workflow (Administer >> Content management >> Content types),
the module will allow you to import all locative information:

 - Location: Name,
 - Location: Street and Location: Additional address line,
 - Location: Postal code,
 - Location: City,
 - Location: State or province,

    This should either be a valid state or province code (such as
    'us-NY' for "New York, USA"), or the full English state or province
    name (such as "New York").

 - Location: Country,

   This should either be a two-letter country code such as 'be' for
   'Belgium' or 'ca' for 'Canada', or the full English country name
   such as 'Belgium' or 'Canada' (case-insensitive).

 - Location: Latititude,
 - Location: Longitude.

If you don't provide the longitude and latitude and location.module
has support for calculating the geocode for that country, then the
module will create the longitude and latitude automatically.

Node options
************

If the user doing the import has "administer nodes" permission, she will
be presented with the options to set the author and creation date as well
as the workflow options.

If the user does not have this permission, the nodes will be created with
the default workflow options, the current date as creation date and the
current user as the owner.

You can specify a user by name, email or uid.

Another option is "Titles must be unique for this node type". If this
option is checked, the import of a row will fail when there is already
another node of the same type with the same title.

Taxonomy options
****************

It is possible to assign taxonomy terms to nodes. For single select
vocabularies you need to put the name of the term in the column of the
file. For multiple select and free tagging vocabularies you can
specify multiple terms to be assigned to a node by separating them by
'|' (vertical bar).

So for example, if "multiple select" is a multiple select vocabulary and
"single select" is a single select vocabulary, you can put the following
in your (for example) CSV file:

  "title","body","single select","multiple select"
  "a title","some body text","term","term1|term2|term3"

This will create a node with 4 terms assigned to it:
 - the single select term "term"
 - three multiple select terms: "term1", "term2", "term3".

For a free tagging vocabulary you can use either '|' (vertical bar) or
',' (comma).

During import you will be presented with the option to add missing terms
to a vocabulary on the fly if you wish to do that. For example, consider
following Tab Separated Values file (here with spaces instead of tabs so
the columns are clear):

  title     body             single select   multiple select
  a title   some body text   term            term1|term2|term3

where "term3" does not exist in the "multiple select" vocabulary before
the import. If you select the option to "Add non-existing terms to the
vocabulary", "term3" will be added to "multiple select" vocabulary
before doing the import.

The options are:

 - "Add non-existing terms to the vocabulary" : if a term does not
   exist, the wizard will inform you during the preview that it will
   be created and it will create the term before importing the row.
   Note that if you have a single or multiple hierarchy vocabulary
   this term may not appear at the right spot in the hierarchy.

 - "Ignore non-existing terms" : if a term does not exist, the wizard
   will not warn you and the term will not be assigned to the node.

 - "Warn me about non-existing terms before the import" : if a term
   does not exist, the wizard will warn you about this during
   preview, but it will not consider non-existing terms an error.

 - "Do not import the node if there are non-existing terms" : if a
   term does not exist, the wizard will consider this an error and
   will not import that row.

If unsure, select "Do not import the node if there are non-existing
terms".

Category options
****************

If you use the category module instead of the taxonomy module for assigning
terms to nodes, you will need to have the taxonomy wrapper of the category
module installed and enabled.

In that case, category module will work the same as taxonomy module (so
see "Taxonomy options" above).

Event options
*************

You can import the start and end dates of event-enabled content types.
There are two ways for this:

 - either you specify in one column of the file the whole date,

 - or you specify a seperate column for the day, the month, the year,
   the hour and the minutes of as well the start and end dates.

Image support
*************

Added support for image module.

 * You need to upload the images to files/images before doing the import.

 * The content of the column you map to Image: Image must be the filename
   relative to files/images (or whatever you set the image directory
   path to).

For example:

 * Upload image1.jpg to files/images.

 * Import a CSV file like:

   "Title","Body","Image"
   "The title", "The body", "image1.jpg"

 * Map the Title column to the Title field, the Body column to the Body
   field and the Image column to the Image: Image field.

The image_gallery module is supported as well because it works with
taxonomy module. This means that you can create image galleries by
importing a CSV file like:

"Title","Body","Image","Image gallery"
"The title", "The body", "image1.jpg","The gallery"

Here you would map the Image gallery column to the Taxonomy: Image
galleries field.

Just like with taxonomy terms, you can assign a default image gallery
in the options or let Drupal create non-existing image galleries on
the fly (select Add non-existing terms to the vocabulary).

Upload (attachments) support
****************************

Added support for upload module.

 * Only one attachment can be specified.

 * You need to upload the file to files/ before doing the import.

 * The content of the column you map to Upload: File attachments must
   be the filename relative to files/ (or whatever you set the file
   directory path to).

For example:

 * Upload file1.txt to files/

 * Import a CSV file like:

   "Title","Body","File"
   "The title", "The body", "file1.txt"

 * Map the Title column to the Title field, the Body column to the Body
   field and the File column to the Upload: File attachments field.

Extending node_import
------------------------------------------------------------------------------

This module provides an easy API to permit any node type to be imported.  To
make your node type importable via this module, you must create 2 or more
functions, to tell which fields exist, etc.

See docs/node_import_hook_docs.php.

Bugs and shortcomings
------------------------------------------------------------------------------

 * NOT ALL FEATURES ARE FULLY TESTED.

 * See http://drupal.org/project/issues/node_import for a complete list of
   bugs and other problems.

 * The content types, users, taxonomy vocabularies, ... need to exist before
   starting the import. This module does not create content types!

Credits / Contact
------------------------------------------------------------------------------

The original author (version 4.5) of this module is Moshe Weitzman (weitzman).
Neil Drumm (drumm) rewrote the module for 4.6 (which he never released as
such).

The port of the module to 4.7 and some new features were provided by:
 - David Donohue (dado),
 - Nic Ivy (njivy).

The port to Drupal 5.x was made possible thanks to Daniel F. Kudwien (sun)
and Henrique Recidive (recidive).

Robrecht Jacques (robrechtj) is the current active maintainer of the module.

Best way to contact the developers to report bugs or feature requests is by
visiting the node_import project page at http://drupal.org/project/node_import.

If you have a problem with a specific file (CSV or TSV), it helps the authors
if you attach a (small) file that shows the problem. When you do, mention how
you configured the type you are trying to import, eg what vocabularies you
have defined, whether the content type is event- or location-enabled, etc.

$Id: README.txt,v 1.15.2.1.2.5 2008/06/28 09:40:58 robrechtj Exp $
