function view_sort_add_draggable(callback, viewname, callback2){
  view_sort_add_doubleclick();
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

function view_sort_add_doubleclick(){
  $('.sort-div').dblclick(function(){
    alert($(this).html());
  });
}

$(document).ready(view_sort_add_doubleclick());