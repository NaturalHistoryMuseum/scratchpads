$(document).ready(function () {
  $('.tui-term').click(function (){
    var ajax_options = {
      cache:false,
      url:Drupal.settings.tui.callback_url+"/"+$(this).attr('id'),
      success:function(data){
        tui_success(data);
      },
      complete:function(){
        tui_add_event_handlers();
      }
    };
    $.ajax(ajax_options); 
  });
  Drupal.behaviors.tui = function(){
    $("#tabs > ul").tabs();
    $("#tabs > ul > li").bt({
      positions: 'top',
      fill: 'rgba(0, 0, 0, .7)',
      cssStyles: {color: 'white', 'font-size': '10px', width: 'auto'},
      closeWhenOthersOpen: true,
      spikeLength: 10,
      strokeWidth: 0
    });
  };
});
function tui_success(data){
  $('#tui-form-container').html(data);
  jQuery.each(Drupal.behaviors, function() {
    this($('#tui-form-container'));
  });
}
function tui_add_event_handlers(){
  
}