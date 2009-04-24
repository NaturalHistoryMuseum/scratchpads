// On clicking Sort
function view_sort_add_draggable(callback, viewname, callback2, callback3){
  view_sort_add_click(callback3);
  $('.view-sort-drag').sortable({
    stop: function(event, ui){
      view_sort_sorted(callback, viewname, callback2)
    }
  });
}

// On sort, do stuff
function view_sort_sorted(callback, viewname, callback2){
  var items ='';
  $('.view-sort-drag > *').each(function(){
    items += " "+$(this).attr('view-sort');
  });
  var ajax_options = {
    type:"POST",
    url:callback+"/"+viewname+"/"+page_tid,
    success:function(data){
      ispecies_callback(callback2,viewname);
    },
    data:"order="+items
  };
  $.ajax(ajax_options); 
}

// Function for pinning and removing
function view_sort_add_click(callback){  
  $('.sort-div > h1').click(function(){
    var ajax_options = {
      type: "POST",
      url:callback,
      data:{pin:$(this).parent().attr('view-sort'),html:$(this).parent().html()}
    };
    $.ajax(ajax_options);  
  });
}

$(document).ready(
  function(){
    view_sort_add_click();
  }
);