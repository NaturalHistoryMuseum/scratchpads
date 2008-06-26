// $Id$
function tinytaxalterroot(url){
  var tinytaxReturn = function (data) {
    var returnHtml = Drupal.parseJson(data);
    $('#tinytaxroot-'+returnHtml['vid']).html(returnHtml['html']);
    $('a.tinytaxlink').click(tinytaxalterroot);
  }
  $.get(url, null, tinytaxReturn);
  return false
}