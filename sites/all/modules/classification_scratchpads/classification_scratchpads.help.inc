<?php

/**
 * Help content for classification
 */

function classification_scratchpads_help_pages() {
  $module_path = drupal_get_path('module', 'classification') .'/';
  drupal_add_css($module_path . 'css/classification.css');
  $help_path = $module_path .'help/';
  $url = url($help_path);
  $import_url = l('Import a new taxonomy.','admin/content/taxonomy/classification/import');
  
  //Import help
  
  $output = <<<ENDOFSTRING
  <h2>General Instructions</h2>
  <p>Your Scratchpad should have a single, <em>primary</em> classification.  The names it contains are used to tag content such as images, text, biolographic items etc. When content is tagged in such a manner, it will be presented on taxon pages in your Scratchpad.</p>
  <h2>Importing Names and Classifications</h2>
  <div class="messages warning">
    <p>You can import a classification multiple times into your Scratchpad, but be aware that there are no merge functions built into the import procedure because name resolution will also be dependent on what content you and your members have already generated.</p>
  </div>
  <p>There are three options to import a classification into your Scratchpad:</p>
  <ol>
  <li>Batch, or "manual" import: entering several names in a text box.</li>
  <li>Text file as either: Parent-child, Tabbed List, or a Full Hierarchy format (see below for examples). This file may be produced from a spreadsheet editor like MS Excel or OpenOffice.org Calc.</li>
  <li>Programmatic import from EOL as Taxon Concept Schema. This option maintains known relationships among names.</li>
  </ol>
  <h3>Text File Preparation for Import</h3>
  <p>Make a tab-delimited text file in one of the three formats:</p>
  <h3>Full Hierarchy (Recommended)</h3>
  <img src="$url/full_hierarchy.png">
  <p>The example of Full Hierarchy in MS Excel 97/2003 format is available for <a href="$url/full_hierarchy.xls">download</a>. To import the Excel file, you need to save it from Excel as "Text (Tab delimited)(*.txt)" file.</p>
  <p><small>All fields here are delimited with "TAB". First line is the header row, with unlimited number of "taxon" columns, and two columns for "description" (taxonomic notes) and "synonyms". Different synonyms are delimited with <em>pipes</em> (|). Species names should be in a full format (with full genus name). One immediate parent could be skipped in case where, for example, one genus has subgenera but others do not. Please note that this format supports the use of taxonomic ranks whereas the following two formats do not.</small></p>
  <h3>Parent-child</strong></h3>
  <img src="$url/parent_child.png">
  <p>The example of Parent-child in MS Excel 97/2003 format is available for <a href="$url/parent_child.xls">download</a>. To import the Excel file, you need to save it from Excel as "Text (Tab delimited)(*.txt)".</p>
  <p><small>Please note that most fields here are delimited with the invisible "TAB" symbol. "any" means any word (just to designate a non-existant parent of the upmost level of the hierarchy). Other words correspond with names of the taxa. Species names should be in a full format (with full genus name). Note that there are two consecutive "TAB" symbols on the line 5, which means that order1 has only synonyms but no taxonomic note. Different synonyms are delimited with <em>pipes</em> (|).</small></p>
  <h3>Tabbed List</h3>
  <img src="$url/tabbed_list.png">
  <p><small>Here the invisible "TAB" symbol used only to demark the hierarchy. Taxonomic notes and synonyms are delimited with <em>semicolons</em>, whereas different synonyms are delimited with <em>pipes</em> (|). Species names should be in a full format (with full genus name). The double semicolon on line 4 designates the case where order1 does not have a taxonomic note but has synonyms.</small></p>
  <p>The example of Tabbed List in MS Excel 97/2003 format is available for <a href="$url/tabbed_list.xls">download</a>. To import the Excel file, you need to save it from Excel as "Text (Tab delimited)(*.txt)" file.</p>
  <p>The step-by-step guide (draft PDF slideshow, 1 Mb) for classification import is available for <a href="$url/stepguide_import.pdf">download</a>. There addressed some typical problems: copying classification from on-line resource, editing in MS Excel, exporting from MS Excel, importing to Scratchpad and initial editing.</p>
  <p class="classification_help_links">$import_url</p>
  <h2>Editing Your Classification</h2>
  <div class="messages warning">All valid scientific names should have a rank; otherwise, many of Scratchpad instances will not work properly. Please also note that when you finish the import of alternative classification, new ranks will not be stored in the right way until you update the classification settings.</div>
  <h3>Capabilities</h3>
  <ul><li>Inline editing (double-click a name in your tree)</li><li>Drag & Drop reordering and importing from alternate classifications</li><li>Multiple select (Ctrl key + mouse click)</li><li>Filtering by parent taxon</li><li>Last open branch is remembered even when you navigate away from the page</li><li>And more to come...</li></ul>
  <p>Because the classification editor is a very dynamic environment, you are strongly advised to use a modern browser. Predecessors of Internet Explorer 7 are unfortunately not supported. Similarly, you may experience unusual behavior if you use earlier versions of FireFox than 3.0.0.</p>
  <h3>Keyboard Shortcuts</h3>
  <ul><li>F2 - rename</li><li>r - refresh</li><li>c - collapse all</li><li>n - new child taxon</li></ul>
ENDOFSTRING;
  return $output;
}