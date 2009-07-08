// On clicking Sort
function view_sort_add_draggable(viewname){
  // Lock/Hide
  $('.view-sort-toggle > img').click(function(){
    var children = $(this).parent().parent().children();
    var ajax_options = {
      type: "POST",
      url:callback,
      data:{pin:$(this).parent().parent().parent().attr('view-sort'),html:$(children[1]).html()}
    };
    $.ajax(ajax_options);  
  });
  // Sortable block
  $('.view-sort-drag').sortable({
    cancel: '.notsortable',
    stop: function(event, ui){
      var items ='';
      $('.view-sort-drag > *').each(function(){
        items += " "+$(this).attr('view-sort');
      });
      //alert(items);
      var ajax_options = {
        type:"POST",
        url:Drupal.settings.view_sort.callbacks.sorted+"/"+viewname+"/"+Drupal.settings.ispecies.page_tid,
        success:function(data){
          if(data.length){
            alert(data);
            $('#view-sort-error').html(data);
            $('#view-sort-error').dialog('open');
            $('#view-sort-error').css('width','250px');
          }
          ispecies_callback(Drupal.settings.view_sort.callbacks.ispecies,viewname);
        },
        data:"order="+items
      };
      $.ajax(ajax_options);
    }
  });
  // Show the Lock/Hide images
  $('.view-sort-drag > div > *').mouseover(function(){
    $(this).children().each(function(){
      if($(this).hasClass('view-sort-toggle')){
        $(this).show();
      }
    })
  });  
  // Hide the Lock/Hide images
  $('.view-sort-drag > div > *').mouseout(function(){
    $(this).children().each(function(){
      if($(this).hasClass('view-sort-toggle')){
        $(this).hide();
      }
    })
  });
  // Change the number of items to display
  $('#view-sort-select').change(function(){
    // Change the number to show
    var ajax_options = {
      type:"POST",
      url:Drupal.settings.view_sort.callbacks.number,
      data:{view:viewname, number:$('#view-sort-select').val()}
    };
    $.ajax(ajax_options);
  });
  $('#view-sort-error').dialog({
    autoOpen: false,
    bgiframe: true,
    draggable: true,
    modal: true,
    height: 150
  });
}