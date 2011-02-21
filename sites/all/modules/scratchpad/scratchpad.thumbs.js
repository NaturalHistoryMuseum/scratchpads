$(document).ready(function(){
  add_mouse_listeners();
  $('div.scratchpad_thumbs_header a#nodes').bind("click", function(e){
    sort_the_thumbs('nodes');
  });
  $('div.scratchpad_thumbs_header a#views').bind("click", function(e){
    sort_the_thumbs('views');
  });
  $('div.scratchpad_thumbs_header a#users').bind("click", function(e){
    sort_the_thumbs('users');
  });
  $('div.scratchpad_thumbs_header a#domain').bind("click", function(e){
    sort_the_thumbs('domain');
  });
});
function sortNumber(a,b){
  return b-a;
}
function sort_the_thumbs(sortby){
  var nodes = new Array();
  var i = 0;
  $('.scratchpad_thumbs>li>a').each(function(i){
    nodes[i] = $(this).attr(sortby);
    i ++;
  });
  nodes.sort(sortNumber);
  for(i=0;i<nodes.length;i++){
    $("a["+sortby+"='"+nodes[i]+"']").parent().insertAfter('.scratchpad_thumbs>li:last');
  }
  add_mouse_listeners();
}
function add_mouse_listeners(){
  $('.scratchpad_thumbs>li>a>img').bind("mouseenter", function(e){
    $(this).attr('src',$(this).attr('src').replace('small.drop.png','small.drop.annotated.png'));
  });
  $('.scratchpad_thumbs>li>a>img').bind("mouseleave", function(e){
    $(this).attr('src',$(this).attr('src').replace('small.drop.annotated.png','small.drop.png'));
  });
}