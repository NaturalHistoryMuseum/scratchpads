$(document).ready(function () {
 $('.countriesmapcheckbox').change(function (){
  var check_status = $(this).is(':checked');
  $('.' + $(this).attr('id')).each(function(){
   $(this).attr('checked', check_status);
  });
 });
});