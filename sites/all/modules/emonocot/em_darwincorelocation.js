//Bind some actions to the lat/long fields
$(document).ready( function() {
  $('#gmap-auto1map-locpick_latitude0').bind('keyup', function() {process_lat();});
  $('#gmap-auto1map-locpick_longitude0').bind('keyup', function() {process_lon();});
});

//Extend the Number object to convert degrees to radians
Number.prototype.degreesToRadians = function() {
  return this * Math.PI /180;
}

//Regular expression defining valid input format
var dms_re = /(\d{1,3})\D+([0-5][0-9]|[0-9])\D+([0-5][0-9]|[0-9])\D+ ?(e(ast)?$|w(est)?$|n(orth)?$|s(outh)?$)/i
var lon_re = /[EW]/i
var lat_re = /[NS]/i
var neg_re = /[WS]/i
var precision = 6;  // maximum numbers after decimal point
var ten_to_n = Math.pow(10,precision);
  
function process_lat() {
  var field = document.getElementById('gmap-auto1map-locpick_latitude0');
  if (field.value.match(dms_re)) {
      dms = convertDMStoDec(field.value);
      //Is this the correct field?
      if (dms['lat_lon'] == 'latitude') {
        field.value = dms['decimal'].degreesToRadians();
      } else {
    	field.value = '';
        field = document.getElementById('gmap-auto1map-locpick_longitude0');
        field.value = dms['decimal'].degreesToRadians();
      }
  }
}
  
function process_lon() {
  var field = document.getElementById('gmap-auto1map-locpick_longitude0');
  if (field.value.match(dms_re)) {
	  dms = convertDMStoDec(field.value);
      //Is this the correct field?
      if (dms['lat_lon'] == 'longitude') {
        field.value = dms['decimal'].degreesToRadians();
      } else {
    	field.value = '';
        field = document.getElementById('gmap-auto1map-locpick_latitude0');
        field.value = dms['decimal'].degreesToRadians();
      }
  }
}
 
// converts coordinate in degrees, minutes, seconds (with direction) to decimal degrees
function convertDMStoDec(evt) {
  var deg = parseInt(RegExp.$1, 10);
  var min = parseInt(RegExp.$2, 10);
  var sec = parseInt(RegExp.$3, 10);
  var dir = RegExp.$4;
  var dec = deg + min / 60.0 + sec / 3600.00;
    
  var return_text = '';
  // dec = dec.toPrecision(precision); // not quite what I wanted. so using ...
  dec = Math.round(dec * ten_to_n)/ten_to_n;
  if (dir.substr(0,1).match(neg_re)) {
    dec = -dec;
  }
  
  var lon_or_lat = "latitude";
  if (dir.substr(0,1).match(lon_re))
  {
    lon_or_lat = "longitude";
  }
  var return_values = new Array();
  return_values['decimal'] = dec;
  return_values['lat_lon'] = lon_or_lat;
  
  return return_values;
}

