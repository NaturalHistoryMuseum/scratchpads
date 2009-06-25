var currently_sorting = false;

$(document).ready(function() {
  $('#mado_apply').dialog({
    autoOpen: false,
    bgiframe: true,
    draggable: true,
    modal: true,
    width: 'auto',
    height: 'auto',
    title: 'Apply to "This page" or "All"'
  });
  $('.mado_close').click(function(){
    var this_id = $(this).parent().attr('id');
    $('#'+this_id).insertAfter("#divider");
    mado_after_sort();
  });
  $('#mado-start-sort').click( function() {
    if(!currently_sorting){
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
        start: function(event, ui){
          $('.mado_close').css('display','none');
        },
        stop: function(event, ui){
          mado_after_sort();
        }
      });
      $('#mado > *').bind("mouseenter", function(){
          // Mouse over a sort block
          var position = $(this).position();
          var width = $(this).width();
          $(this).children('.mado_close').css('display','block');
          $(this).children('.mado_close').css('top',position.top);
          $(this).children('.mado_close').css('left',position.left+width-30);
        }
      );
      $('#mado > *').bind("mouseleave", function(){
          // Mouse out of a sort block
          $(this).children('.mado_close').css('display','none');
        }
      );
    }
    currently_sorting = true;
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
function mado_after_sort(){
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