<?php

/**
 * Help content for classification
 */

function classification_help_pages() {
$module_path = drupal_get_path('module', 'classification') .'/';
drupal_add_css($module_path . 'css/classification.css');
$help_path = $module_path .'/help';

//Import help
$output .= '<h2>Classification Instructions</h2>';
$output .= '<h2>General Instructions</h2>';
$output .= '<p>Your LifeDesk has a single, primary classification and the names it contains are used to tag content such as images, text, biolographic items. When content is tagged in such a manner, it will be presented on taxon pages in your LifeDesk and may also be exported for aggregation on Encyclopedia of Life species pages. Consequently, the classification functions are the most important part of your LifeDesk.</p>';
$output .= '<h2>Importing Names and Classifications</h2>';
$output .= '<div class="messages warning">You can import a classification multiple times into your LifeDesk, but be aware that there are no merge functions built into the import procedure because name resolution will also be dependent on what content you and your members have already generated.</div>';
$output .= '<p>You may have multiple classifications in your LifeDesk, but a single classification is used to organize your content and site presentation. Additional, temporary classifications may be used to help you import individual names or groups of names into your LifeDesk\'s primary classification.</p>';
$output .= '<p>There are three options to import a classification into your LifeDesk:</p>';
$output .= '<ol>';
$output .= '<li>Batch, or "manual" import: entering several names in a text box.</li>';
$output .= '<li>Text file as either: Parent-child, Tabbed List, or a Full Hierarchy format (see below for examples). This file may be produced from a spreadsheet editor like MS Excel or OpenOffice.org Calc.</li>';
$output .= '<li>Programmatic import from EOL as Taxon Concept Schema. This option maintains known relationships among names.</li>';
$output .= '</ol>';
$output .= '<h3>Text File Preparation for Import</h3>';

$output .= '<p>Make a tab-delimited text file in one of the three formats:</p>';

$output .= '<h3>Full Hierarchy (Recommended)</h3>';

$output .= '<pre style="width:460px;">';
$output .= 'taxon	taxon	taxon	taxon	rank	description	synonyms' . "\n";
$output .= 'kingdom1				kingdom' . "\n";
$output .= 'kingdom1	phylum1			phylum' . "\n";
$output .= 'kingdom1	phylum1	class1		classis	descr0' . "\n";
$output .= 'kingdom1	phylum1	class1	order1	order	descr1	syn1,syn2' . "\n";
$output .= 'kingdom1	phylum1	class1	order2	order		syn3,syn4' . "\n";
$output .= 'kingdom1	phylum1	class2	order3	order	descr3	syn5' . "\n";
$output .= '</pre>' . "\n";

$output .= '<p>The example of Full Hierarchy in MS Excel 97/2003 format is available for <a href="/' . $help_path . '/full_hierarchy.xls">download</a>. To import the Excel file, you need to save it from Excel as "Text (Tab delimited)(*.txt)" file.</p>';

$output .= '<p><small>All fields here are delimited with "TAB". First line is the header row, with unlimited number of "taxon" columns, and two columns for "description" and "synonyms". Different synonyms are delimited with <em>commas</em>. Please note that this format supports the use of taxonomic ranks whereas the following two formats do not.</small></p>';

$output .= '<h3>Parent-child</strong></h3>';

$output .= '<pre>' . "\n";
$output .= 'kingdom1	any	descr1	syn1' . "\n";
$output .= 'phylum1	kingdom1	descr2' . "\n";
$output .= 'phylum2	kingdom1' . "\n";
$output .= 'class1	phylum1' . "\n";
$output .= 'order1	class1		syn2,syn3' . "\n";
$output .= 'class2	phylum1' . "\n";
$output .= 'order2	class2	descr3	syn4' . "\n";
$output .= '</pre>' . "\n";

$output .= '<p>The example of Parent-child in MS Excel 97/2003 format is available for <a href="/' . $help_path . '/parent_child.xls">download</a>. To import the Excel file, you need to save it from Excel as "Text (Tab delimited)(*.txt)".</p>';

$output .= '<p><small>Please note that most fields here are delimited with the invisible "TAB" symbol. "any" means any word (just to designate a non-existant parent of the upmost level of the hierarchy). "syn" and "descr" are synomym and description, respectively. Other words correspond with names of the taxa. Note that there are two consecutive "TAB" symbols on the line 5, which means that order1 has only synonyms but no description. Different synonyms are delimited with <em>commas</em>.</small></p>';

$output .= '<h3>Tabbed List</h3>';

$output .= '<pre>' . "\n";
$output .= 'kingdom1;descr1' . "\n";
$output .= '	phylum1;descr2;syn1' . "\n";
$output .= '		class1' . "\n";
$output .= '			order1;;syn2,syn3' . "\n";
$output .= '		class2' . "\n";
$output .= '			order2' . "\n";
$output .= '			order3' . "\n";
$output .= '</pre>' . "\n";

$output .= '<p><small>Here the invisible "TAB" symbol used only to demark the hierarchy. Description and synonyms are delimited with <em>semicolons</em>, whereas different synonyms are delimited with <em>commas</em>. The double semicolon on line 4 designates the case where order1 does not have a description but has synonyms.</small></p>';

$output .= '<p>The example of Tabbed List in MS Excel 97/2003 format is available for <a href="/' . $help_path . '/tabbed_list.xls">download</a>. To import the Excel file, you need to save it from Excel as "Text (Tab delimited)(*.txt)" file.</p>';

$output .= '<p class="classification_help_links">' . l(t('Import your classification'),'classification/import') . '</p>';

//Edit help

$output .= '<h2>Editing Your Classification</h2>';
$output .= '<h3>Capabilities</h3>';
$output .= '<ul><li>Inline editing (double-click a name in your tree)</li><li>Drag & Drop reordering and importing from alternate classifications</li><li>Filtering by parent taxon</li><li>Last open branch is remembered even when you navigate away from the page</li><li>And more to come...</li></ul>';
$output .= '<p>Because the classification editor is a very dynamic environment, you are strongly advised to use a modern browser. Predecessors of Internet Explorer 7 are unfortunately not supported. Similarly, you may experience unusual behavior if you use earlier versions of FireFox than 3.0.0.</p>';
$output .= '<h3>Keyboard Shortcuts</h3>';
$output .= '<ul><li>F2 - rename</li><li>r - refresh</li><li>c - collapse all</li><li>n - new child taxon</li></ul>';

$output .= '<p class="classification_help_links">' . l(t('Edit your classification'),'classification/edit') . '</p>';

//Export help
$output .= '<h2>Exporting Your Classification</h2>';
$output .= '<div class="messages warning">Exporting your classification may take a long time if you have many thousands of taxa.</div>';
$output .= '<p>Select a file format, click the button "Export" and your full classification (along with its synonyms) will be exported as a text file either in a "Parend-child" format or in a compressed (Gzipped) TCS (Taxon Concept Schema) XML format appropriate for many programmatic use. The text file may be opened in any spreadsheet editor like MS Excel or OpenOffice.org Calc. The Gzipped XML file will be saved to the server and an archive retained for you.</p>';

$output .= '<p class="classification_help_links">' . l(t('Export your classification'),'classification/export') . '</p>';

//Content settings
$output .= '<h2>Content Settings</h2>';
$output .= '<p>A names text box is present on the image upload page, on the bibliographic entry page, among other pages in your LifeDesk. Use this configuration page to specify how these text boxes behave for your members. You may also specify how names are presented on the public-facing classification browser on your frontpage.</p>';

$output .= '<p class="classification_help_links">' . l(t('Adjust settings'),'classification/manage') . '</p>';

	return $output;
}
