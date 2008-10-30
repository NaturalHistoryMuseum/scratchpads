<?php
$s = "<hello world";
/*strip illegal XML characters*/
preg_replace('/[\x{80}-\x{A0}'. // Non-printable ISO-8859-1 + NBSP
'\x{01}-\x{1F}'. //Non-printable ASCII characters
'\x{AD}'. // Soft-hyphen
'\x{2000}-\x{200F}'. // Various space characters
'\x{2028}-\x{202F}'. // Bidirectional text overrides
'\x{205F}-\x{206F}'. // Various text hinting characters
'\x{FEFF}'. // Byte order mark
'\x{FF01}-\x{FF60}'. // Full-width latin
'\x{FFF9}-\x{FFFD}'. // Replacement characters
'\x{0}]/u', // NULL byte
'',$s);
str_replace('"', '&quot;', $s); //encode quote
str_replace('&', '&amp;', $s); //encode ampersand
str_replace("'", '&pos;', $s); //encode apostrophe
$s = str_replace('<', '&lt;', $s); //encode left-angled bracket
str_replace('>', '&rt;', $s); //encode right-angled bracket

echo $s;