$(document).ready(function(){
  add_mouse_listeners();
  $('div.scratchpad_thumbs_header a#nodes').bind("click", function(e){
    var nodes = new Array();
    var i = 0;
    $('.scratchpad_thumbs>li>a').each(function(i){
      nodes[i] = $(this).attr('nodes');
      i ++;
    });
    function sortNumber(a,b){
      return b-a;
    }
    nodes.sort(sortNumber);
    for(i=0;i<nodes.length;i++){
      var html_to_append = '<li>'+$("a[nodes='"+nodes[i]+"']").parent().html()+'</li>';
      $("a[nodes='"+nodes[i]+"']").parent().remove();
      $('.scratchpad_thumbs').append(html_to_append);
    }
    add_mouse_listeners();
  });
  $('div.scratchpad_thumbs_header a#views').bind("click", function(e){
    var views = new Array();
    var i = 0;
    $('.scratchpad_thumbs>li>a').each(function(i){
      views[i] = $(this).attr('views');
      i ++;
    });
    function sortNumber(a,b){
      return b-a;
    }
    views.sort(sortNumber);
    for(i=0;i<views.length;i++){
      var html_to_append = '<li>'+$("a[views='"+views[i]+"']").parent().html()+'</li>';
      $("a[views='"+views[i]+"']").parent().remove();
      $('.scratchpad_thumbs').append(html_to_append);
    }
    add_mouse_listeners();
  });
  $('div.scratchpad_thumbs_header a#users').bind("click", function(e){
    var users = new Array();
    var i = 0;
    $('.scratchpad_thumbs>li>a').each(function(i){
      users[i] = $(this).attr('users');
      i ++;
    });
    function sortNumber(a,b){
      return b-a;
    }
    users.sort(sortNumber);
    for(i=0;i<users.length;i++){
      var html_to_append = '<li>'+$("a[users='"+users[i]+"']").parent().html()+'</li>';
      $("a[users='"+users[i]+"']").parent().remove();
      $('.scratchpad_thumbs').append(html_to_append);
    }
    add_mouse_listeners();
  });
});
function add_mouse_listeners(){
  $('.scratchpad_thumbs>li>a>img').bind("mouseenter", function(e){
    $(this).attr('src',$(this).attr('src').replace('small.drop.png','small.drop.annotated.png'));
  });
  $('.scratchpad_thumbs>li>a>img').bind("mouseleave", function(e){
    $(this).attr('src',$(this).attr('src').replace('small.drop.annotated.png','small.drop.png'));
  });
}