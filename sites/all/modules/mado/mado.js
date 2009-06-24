$(document).ready(function() {
  $('#mado-start-sort').click( function() {
    $('#mado-start-sort').html(Drupal.settings.mado.finished_sorting);
    $('#divider').css('display','block');
    $('.mado-hidden').removeClass('mado-hidden');
    $('#mado > *').css('background-color','white');
    $('.mado-title-links').css('display','none');
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
          url:Drupal.settings.mado.callback_url,
          data:{"blocks[]":items,"identifier":Drupal.settings.mado.ident}
        };
        $.ajax(ajax_options); 
      }
    });
    tb_init('.thickbox');
  });
  $('.resize-x').click( function(){
    $(this).parent().parent().parent().parent().parent().parent().toggleClass("mado_block_2");
    mado_update_resizes(this);
  });
  $('.resize-y').click( function(){
    $(this).parent().parent().parent().parent().children('.subboxcontent').children('.mado_content').toggleClass("mado_scroll");
    mado_update_resizes(this);
  });
});
function mado_update_resizes(element){
  var scroll = 0;
  if($(element).parent().parent().parent().parent().children('.subboxcontent').children('.mado_content').hasClass("mado_scroll")){
    scroll = 1;
  }
  var newclass = $(element).parent().parent().parent().parent().parent().parent().attr("class");
  var resizeid = $(element).parent().parent().parent().parent().parent().parent().attr("id");
  var ajax_options = {
    url:Drupal.settings.mado.callback_url,
    data:{"resize":resizeid,"identifier":Drupal.settings.mado.ident,"class":newclass,"scroll":scroll}
  };
  $.ajax(ajax_options);  
}