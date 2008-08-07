var lastform = false;
function nodes(id, url){
  // Hide other forms
  if(lastform != id){
    $(".nodes-form").remove();
    // Get the HTML from server, and add it
    var nodesGetForm = function (data) {
      var returnHtml = Drupal.parseJson(data);
      $('#'+id).html($('#'+id).html()+returnHtml['html']);
    }
    $.get(url, null, nodesGetForm);
    lastform = id;
  }
}
function nodescancel(){
  $(".nodes-form").remove();
  return false;
}
function nodessubmit(url,id,field){
  var nodesSubmitForm = function (data) {
    var returnHtml = Drupal.parseJson(data);
    $('#'+field+'_'+returnHtml['nid']).html(returnHtml['value']);
  }
  $.post(url,{formvalue: $('#'+id).val(), formid: id, field: field, updated: $('#'+id+'-updated').val()},nodesSubmitForm);
}