// On initial load, add hover to all li dragable items
$(document).ready(function () {
  // Prevent duplicate attachment of the collapsible behavior.
  $('.femail-signature > p').bind('click', function(e){
    $(this).parent().children('div').toggle('slow');
  });
});