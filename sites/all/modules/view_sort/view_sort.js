// On clicking Sort
function view_sort_add_draggable(callback, viewname, callback2, callback3){
  view_sort_add_click(callback3);
  $('.view-sort-drag').sortable({
    stop: function(event, ui){
      view_sort_sorted(callback, viewname, callback2)
    }
  });
  $('.view-sort-drag > div > *').mouseover(function(){
    $(this).children().each(function(){
      if($(this).hasClass('view-sort-toggle')){
        $(this).show();
      }
    })
  });
  $('.view-sort-drag > div > *').mouseout(function(){
    $(this).children().each(function(){
      if($(this).hasClass('view-sort-toggle')){
        $(this).hide();
      }
    })
  });
}

// On sort, do stuff
function view_sort_sorted(callback, viewname, callback2){
  var items ='';
  $('.view-sort-drag > *').each(function(){
    items += " "+$(this).attr('view-sort');
  });
  //alert(items);
  var ajax_options = {
    type:"POST",
    url:callback+"/"+viewname+"/"+Drupal.settings.ispecies.page_tid,
    success:function(data){
      //alert(data);
      ispecies_callback(callback2,viewname);
    },
    data:"order="+items
  };
  $.ajax(ajax_options); 
}

// Function for pinning and removing
function view_sort_add_click(callback){  
  $('.view-sort-toggle > img').click(function(){
    var children = $(this).parent().parent().children();
    var ajax_options = {
      type: "POST",
      url:callback,
      data:{pin:$(this).parent().parent().parent().attr('view-sort'),html:$(children[1]).html()}
    };
    $.ajax(ajax_options);  
  });
}

$(document).ready(
  function(){
    view_sort_add_click();
  }
);
