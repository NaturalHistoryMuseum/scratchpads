// $id$
function taxtabchangeform(){
  var taxtabReturn = function (data) {
    var returnHtml = Drupal.parseJson(data);
      $('#search-block-form').parent().html(returnHtml['html']);
      $('#taxtab-create-form').parent().html(returnHtml['html']);
      $('a.taxtablink').click(taxtabchangeform);
      Drupal.autocompleteAutoAttach();
    }
  $.get(this.href, null, taxtabReturn);
  return false
}
if (Drupal.jsEnabled) {
  $(document).ready(function(){
    $('a.taxtablink').click(taxtabchangeform);
  });
}