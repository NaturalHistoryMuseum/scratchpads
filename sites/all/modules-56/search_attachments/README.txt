search_attachments.module (Version 2006-07-11)
----------------------------------------------

Author: Mark Jordan, mjordan@sfu.ca
Module homepage: http://interoperating.info/mark/search_attachments

License
-------

GPL version 2. See the file LICENSE.txt or http://www.gnu.org/licenses/gpl.txt
for the complete license.

Purpose
-------

search_attachments.module appends the text of PDF, MS Word, and plain text attachments
to their parent node's search_dataset database table. The node text and the 
attachements' text are therefore considered a single document for the purposes of 
Drupal's search engine.

In order to extract the text from attached files, this module calls 'helper apps'. 
The module is not limited to using specific helpers described below -- Drupal 
administrators can configure any helpers they like for PDF, Microsoft Word, or plain text.

Currently, search_attachments.module only works in Drupal 4.7, and has only been 
tested on Linux and Mac OS X. The module itself should work on any platform that
Drupal runs on, but is limited to platforms that support suitable helper apps,
as documented below.


Recommended helper apps
-----------------------

In order to use search_attachments.module, you will need the appropriate helper
apps on the same computer that Drupal is running on. The three recommended helper
apps are pdftotext, catdoc, and cat:

'pdftotext' for PDF files - Source code and binaries for Linux, Solaris, DOS/Win32 
are available at http://www.foolabs.com/xpdf/download.html; an installer for Mac OS X is 
available at http://www.bluem.net/downloads/pdftotext_en/

'catdoc' for MS Word files - Source code for *nix is available at http://www.45.free.net/~vitus/software/catdoc/; 
an RPM is available at http://ftp.belnet.be/packages/dries.ulyssis.org/fedora/fc3/i386/RPMS.dries/catdoc-0.94-1.1.fc3.rf.i386.rpm

'cat' for plain text files - This utility is a standard on most *nix systems, so a 
simple 'which cat'command should tell you where it is. A version for Windows appears 
to be available as part of the utilities from http://unxutils.sourceforge.net/.

Other helper apps
-----------------

To demonstrate that any helper app that can be called by PHP's shell_exec() and
return the entire text of a file, I provide the following Perl script (in this case
written for Perl on Windows) that can be used instead of the *nix 'cat' command:

#!c:\perl\bin\perl.exe
use strict;
my $file = $ARGV[0];
chomp $file;

open (INPUT, $file);
while  (<INPUT>) {
  print $_;
}

It is possible to write a similar helper using Perl's Win32::OLE module to extract 
the text of Word files,as documented at http://www.wellho.net/resources/ex.php4?item=p257/msword 
and elsewhere.


Installation and usage of search_attachments.module
---------------------------------------------------

1) Install helper apps such as catdoc and pdftotext. Unix cat command is sufficient 
   to test .txt attachements.

2) Place search_attachments.module in your Drupal modules directory.

3) Log into Drupal as admin and go to administer > modules and activate the module.

4) Go to administer > settings and enter the paths to the helpers you want to use. 
   If you save the settings and the module can't find the indicated helper apps, 
   it will tell you. If you don't want to use a helper, remove all information from 
   the path settings for that helper.

5) Attach some files to nodes (attachements must end in .txt, .pdf, or .doc). Change the
   main node content so the node will be reindexed.

6) Run cron.php on your site to reindex (e.g., http://yoursite.org/cron.php)

7) Search for words contained in your attachements. A link to the parent node should 
   appear in your results.
   
This module uses PHP's shell_exec() function, so you should restrict 'administer'
access to trusted users -- normal end users would never need to configure site-wide
search settings anyway so this should be an obvious precaution. Just thought I'd point
it out. Regular end users can have 'search' access without introducing security 
vulnerabilities.
   
If you are sure that the pdftotext and catdoc are installed (i.e., 'which catdoc'
returns a valid path), and search_attachements.module still complains that it can't
find the helpers, chances are that PHP is configured using safe mode 
(http://php.net/features.safe-mode).


Known limitations / To do
-------------------------

This is the first public version of search_attachments.module, so it's pretty 
basic at this point. If there is interest some features that could be added
include:

-The ability to allow administrators to associate helper apps with attachments
in formats other than PDF, MS Word, and plain text. This would only require a
settings interface that allowed the addition of more form elements easily.

-The ability to index attachments separately from the node text. Using the current
architecture, I don't see how this can be done since the attachment's identity is
lost by virtue of its being appended to its parent node's text. Any suggestions
are welcome.

-The ability to format the search results such that hits in attachements are
identified. If we can get the former 'to do' to work then this might be possible
as well. 


Thank you
---------

Yuri McPhedran for testing, suggestions, and pointers to downloads of helper apps for
various platforms.

