$(document).ready(function(){
 $('.scratchpadify-block li').bind("mouseenter", function(){
  $(this).children().children('img').show(); 
 });
 $('.scratchpadify-block li').bind("mouseleave", function(){
   $(this).children().children('img').hide(); 
 });
});