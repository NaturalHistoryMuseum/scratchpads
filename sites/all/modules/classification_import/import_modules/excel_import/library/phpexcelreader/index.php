<?php
require_once 'Excel/reader.php';


// ExcelFile($filename, $encoding);
$data = new Spreadsheet_Excel_Reader();

$data->read('jxlrwtest.xls');
$excel_string = print_r($data->sheets[0]['cells'], 1);
if(!mb_check_encoding($excel_string, "UTF-8")){
  foreach(mb_list_encodings() as $encoding){
    $new_excel_string = mb_convert_encoding($excel_string, 'UTF-8', $encoding);
    if(mb_check_encoding($new_excel_string, 'UTF-8')){
      $excel_string = $new_excel_string;
      break;
    }
  }
}
echo $excel_string;
exit;

for ($i = 1; $i <= $data->sheets[0]['numRows']; $i++) {
	for ($j = 1; $j <= $data->sheets[0]['numCols']; $j++) {
		echo "\"".$data->sheets[0]['cells'][$i][$j]."\",";
	}
	echo "\n";
}
