var mado_callback_url;

$(document).ready(function() {
  $('#mado-start-sort').click( function() {
    $('#divider').css('display','block');
    $('#mado .mado_content').each( function (){
      if($(this).height()>150){
        $(this).height($(this).height()/2);
        $(this).css('overflow','hidden');        
      }
    });
    $('#mado').sortable({
      stop: function(event, ui){
        var items = Array();
        $('.mado_block').each(function(){
          if($(this).attr('id')!=''){
            items[items.length] = $(this).attr('id');
          }
        });
        var ajax_options = {
          url:mado_callback_url,
          data:{"blocks[]":items,"identifier":mado_ident}
        };
        $.ajax(ajax_options); 
      }
    });
  });
});