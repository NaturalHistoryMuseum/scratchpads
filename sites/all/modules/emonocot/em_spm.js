$(document).ready(function() {
  //Hide those pesky fieldsets
  $('fieldset').css('display', 'none');
  $('.wysiwyg-toggle-wrapper').css('display', 'none');
  //Actually, the taxonomy one is pretty useful
  $('#taxonomy-fieldset').find('fieldset').css('display', 'block');
});
