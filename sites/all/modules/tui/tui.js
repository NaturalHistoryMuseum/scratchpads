Drupal.tui = new Object;

Drupal.tui.init = function(context) {
  $('.tui-term').click(function (){
    Drupal.tui.display_form($(this).attr('id'));
  });
}

Drupal.tui.ajax_success = function(data){
  $('#tui-form-container').html(data);
  jQuery.each(Drupal.behaviors, function() {
    this($('#tui-form-container'));
  });
}

Drupal.tui.display_form = function(term_id){
  if(term_id){
    Drupal.tui.term_id = term_id;
  }
  var ajax_options = {
    cache:false,
    url:Drupal.settings.tui.callback_url+"/"+Drupal.tui.term_id,
    success:function(data){
      Drupal.tui.ajax_success(data);
    }
  };
  $.ajax(ajax_options); 
}

Drupal.tui.reload_tree = function(){
  
}

Drupal.behaviors.tui = function(context){
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

$(document).ready(function () {
  Drupal.tui.init();
});