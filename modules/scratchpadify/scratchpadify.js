$(document).ready(function(){
 $('.scratchpadify-block li').bind("mouseenter", function(){
   $(this).children('.add-links').show();
 });
 $('.scratchpadify-block li').bind("mouseleave", function(){
   $(this).children('.add-links').hide(); 
 });
 $('.vertical-tabs').hide();
 $('.vertical-tabs').before('<p class="scratchpadify-show-advanced"><a class="scratchpadify-show-advanced" href="#">'+Drupal.settings.scratchpadify.show_advanced+'</a></p>');
 $('p.scratchpadify-show-advanced').bind("click", function(e){
   $('.vertical-tabs').slideDown('slow');
   $(this).remove();
   return false;
 });
});