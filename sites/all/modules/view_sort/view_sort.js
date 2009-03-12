function view_sort_add_draggable(callback, viewname, callback2, callback3){
  view_sort_add_doubleclick(callback3);
  $('.view-sort-drag').sortable({
    stop: function(event, ui){
      view_sort_sorted(callback, viewname, callback2)
    }
  });
}

function view_sort_sorted(callback, viewname, callback2){
  var items ='';
  $('.view-sort-drag > *').each(function(){
    items += " "+$(this).attr('view-sort');
  });
  //alert(items);
  var ajax_options = {
    url:callback+"/"+viewname,
    success:function(data){
      ispecies_callback(callback2,viewname);
    },
    data:"order="+items
  };
  $.ajax(ajax_options); 
}

function view_sort_add_doubleclick(callback){
  $('.sort-div').dblclick(function(){
    var ajax_options = {
      url:callback,
      success:function(data){
        alert(data);
      },
      data:"pin="+$(this).attr('view-sort')
    };
    $.ajax(ajax_options);    
  });
}

$(document).ready(view_sort_add_doubleclick());