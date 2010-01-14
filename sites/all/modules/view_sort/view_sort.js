// On clicking Sort
function view_sort_add_draggable(viewname){
  // Lock/Hide
  $('.view-sort-toggle > img').click(function(){
    var children = $(this).parent().parent().children();
    if($(this).attr("id") == 'view-sort-hide'){
      // Hiding
      var hide_me = $(this).parent().parent().parent();
      var ajax_options = {
        type: "POST",
        url:Drupal.settings.view_sort.callbacks.hide,
        data:{hide:hide_me.attr('view-sort'),html:$(children[1]).html()},
        data:{pin:hide_me.attr('view-sort'),html:$(children[1]).html()},
        success:function(data){
          hide_me.hide();
        }
      };
      $.ajax(ajax_options);
    } else if ($(this).attr("id") == 'view-sort-pin'){
      // Pin/Lock
      var pin_me = $(this).parent().parent().parent();
      var ajax_options = {
        type: "POST",
        url:Drupal.settings.view_sort.callbacks.pin,
        data:{pin:pin_me.attr('view-sort'),html:$(children[1]).html()},
        success:function(data){
          // We can move this block up to the top
          var something = pin_me.clone();
          something.prependTo(".view-sort-top"); 
          pin_me.remove();
        }
      };
      $.ajax(ajax_options);      
    }  
  });
  // Sortable block
  $('.view-sort-drag').sortable({
    cancel: '.notsortable',
    stop: function(event, ui){
      var items ='';
      $('.view-sort-drag > *').each(function(){
        items += " "+$(this).attr('view-sort');
      });
      var ajax_options = {
        type:"POST",
        url:Drupal.settings.view_sort.callbacks.sorted+"/"+viewname+"/"+Drupal.settings.ispecies.page_tid,
        success:function(data){
          if(data.length){
            $('#view-sort-error').html(data);
            $('#view-sort-error').dialog('open');
            $('#view-sort-error').css('width','250px');
          }
          //ispecies_callback(Drupal.settings.view_sort.callbacks.ispecies,viewname);
        },
        data:"order="+items
      };
      $.ajax(ajax_options);
    }
  });
  view_sort_add_mouseover();
  // Change the number of items to display
  $('#view-sort-select').change(function(){
    // Change the number to show
    var dropdown_number = $('#view-sort-select').val();
    var ajax_options = {
      type:"POST",
      url:Drupal.settings.view_sort.callbacks.number,
      data:{view:viewname, number:dropdown_number}
    };
    $.ajax(ajax_options);
    // Count the number of blocks in the top
    var top_div_sortables_count=0;
    $('.view-sort-top > .sort-div').each(function(){
      top_div_sortables_count++;
    });
    if(top_div_sortables_count == dropdown_number){return;}
    else if(top_div_sortables_count > dropdown_number){
      // Too many on top, move some down.
      var number_to_move = top_div_sortables_count - dropdown_number;
      var number_of_div = 1;
      $('.view-sort-top > .sort-div').each(function(){
        if(number_of_div > dropdown_number){
          var something = $(this).clone();
          something.prependTo(".view-sort-bottom");
          $(this).remove();
        }
        number_of_div ++;
      });
    } else {
      var number_to_move = dropdown_number - top_div_sortables_count;
      var number_of_div = 1;
      $('.view-sort-bottom > .sort-div').each(function(){
        if(number_of_div <= number_to_move){
          var something = $(this).clone();
          something.appendTo(".view-sort-top"); 
          $(this).remove();
          view_sort_add_mouseover(); 
        }
        number_of_div ++;
      });
    }
  });
  $('#view-sort-error').dialog({
    autoOpen: false,
    bgiframe: true,
    draggable: true,
    modal: true,
    height: 150
  });
}

function view_sort_add_mouseover(){
  // Show the Lock/Hide images
  $('.view-sort-bottom > div > *').mouseover(function(){
    $(this).children().each(function(){
      if($(this).hasClass('view-sort-toggle')){
        $(this).show();
      }
    })
  });  
  // Hide the Lock/Hide images
  $('.view-sort-bottom > div > *').mouseout(function(){
    $(this).children().each(function(){
      if($(this).hasClass('view-sort-toggle')){
        $(this).hide();
      }
    })
  });
}