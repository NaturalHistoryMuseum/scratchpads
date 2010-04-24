Drupal.tui = new Object;

Drupal.tui.init = function(context) {
  $('.tui-term', context).click(function (){
    Drupal.tui.display_form($(this).attr('id'));
  });
  $("#tabs > ul", context).tabs();
  $("#tabs > ul > li", context).bt({
    positions: 'top',
    fill: 'rgba(0, 0, 0, .7)',
    cssStyles: {color: 'white', 'font-size': '10px', width: 'auto'},
    closeWhenOthersOpen: true,
    spikeLength: 10,
    strokeWidth: 0
  });
  $('.tui-node-closed', context).mouseup(function(){
    Drupal.tui.click_closed($(this).parent().attr('id'));
  });
  $('.tui-node-open', context).mouseup(function(){
    Drupal.tui.click_open($(this).parent().attr('id'));
  });
  $('#tui-tree-container li').sortable({
    axis:'y',
    handle:'span.tui-term'
  });
}

Drupal.tui.click_closed = function(vid_and_tid){  
  $('#'+vid_and_tid+' > span.tui-nodeleaf').removeClass('tui-node-closed');
  $('#'+vid_and_tid+' > span.tui-nodeleaf').addClass('tui-node-open');
  $('#'+vid_and_tid+' > span.tui-nodeleaf').unbind('mouseup');
  var ajax_options = {
    cache:false,
    url:Drupal.settings.tui.callbacks.tree+"/"+vid_and_tid,
    success:function(data){
      Drupal.tui.tree_success($('#'+vid_and_tid), data);
    }
  };
  $.ajax(ajax_options);
}

Drupal.tui.remove_tid = function(vid_and_tid){
  jQuery.each($('#'+vid_and_tid+' > ul .tui-term'), function(idx, obj){
    tid_to_remove = $(obj).attr('id').substring(4);
    delete Drupal.settings.tui.opentids[tid_to_remove];
  });
}

Drupal.tui.add_tid = function(vid_and_tid){
  jQuery.each($('#'+vid_and_tid+' > ul .tui-term'), function(idx, obj){
    tid_to_add = $(obj).attr('id').substring(4);
    Drupal.settings.tui.opentids[tid_to_add] = tid_to_add;
  });
}

Drupal.tui.click_open = function(vid_and_tid){
  $('#'+vid_and_tid+' > span.tui-nodeleaf').removeClass('tui-node-open');
  $('#'+vid_and_tid+' > span.tui-nodeleaf').addClass('tui-node-closed');
  Drupal.tui.remove_tid(vid_and_tid);
  $('#'+vid_and_tid).children('ul').remove();
  jQuery.each(Drupal.behaviors, function() {
    this($('#'+vid_and_tid));
  });
}

Drupal.tui.tree_success = function(html_object, data){
  $(html_object).append(data);
  jQuery.each(Drupal.behaviors, function() {
    this(html_object);
  });
  Drupal.tui.add_tid($(html_object).attr('id'));
}

Drupal.tui.full_tree_success = function(data){
  $('#tui-tree-container').html(data);
  jQuery.each(Drupal.behaviors, function() {
    this('#tui-tree-container');
  });
}

Drupal.tui.form_success = function(data){
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
    url:Drupal.settings.tui.callbacks.form+"/"+Drupal.tui.term_id,
    success:function(data){
      Drupal.tui.form_success(data);
    }
  };
  $.ajax(ajax_options); 
}

Drupal.tui.reload_tree = function(){
  var ajax_options = {
    type:'POST',
    cache:false,
    url:Drupal.settings.tui.callbacks.full_tree,
    success:function(data){
      Drupal.tui.full_tree_success(data);
    },
    data: Drupal.settings.tui.opentids
  };
  $.ajax(ajax_options);
}

Drupal.behaviors.tui = function(context){
  Drupal.tui.init(context);
};