Drupal.tui = new Object;

Drupal.tui.init = function(context) {
  $('.tui-term', context).click(function (){$('.tui-added-original').removeClass('tui-added-original');Drupal.tui.display_form(this);});
  $('#tabs > ul', context).tabs();
  $('#tabs > ul > li, #tui-tree-links img', context).bt({positions: 'top',fill: 'rgba(0, 0, 0, .7)',cssStyles: {color: 'white', 'font-size': '14px', width: 'auto'},closeWhenOthersOpen: true,spikeLength: 10,strokeWidth: 0});
  $('.tui-node-closed', context).mouseup(function(){Drupal.tui.click_closed($(this).parent().attr('id'));});
  $('.tui-node-open', context).mouseup(function(){Drupal.tui.click_open($(this).parent().attr('id'));});
  $('#tui-tree-subcontainer li').draggable({helper:'clone',cursorAt:{left:1, top:1},handle:'> .tui-term',opacity:0.8,delay:200,distance:10,start:function(event, ui){Drupal.tui.drag_start(event, ui);}});
  $(window).resize(function(){Drupal.tui.resize_frame();});
  $('#tui-tree-links img', context).mouseup(function(){Drupal.tui.click_buttonclick($(this).attr('id'));});
  Drupal.tui.resize_frame();
}

Drupal.tui.click_buttonclick = function(img_clicked){
  switch(img_clicked){
    case 'tui-add':
      $('#'+Drupal.tui.term_id).addClass('tui-added-original');
      Drupal.tui.term_id = 'new-'+Drupal.settings.tui.vocabulary+'-'+Drupal.tui.term_id;
      Drupal.tui.display_form(false);
      break;
    case 'tui-delete':
      if(Drupal.tui.term_id){
        $('#tui-tree-links').append('<div id="dialog" title=""></div>');
        $('#dialog').attr('title', Drupal.settings.tui.dialog.delete.title);
        $('#dialog').html(Drupal.settings.tui.dialog.delete.content);
        $('#tui-dialog-term-name').html($('.tui-term.active').html());
        $('#dialog').dialog({modal:true,buttons:{"Cancel":function(){$(this).dialog("close");},"OK":function(){Drupal.tui.do_delete(Drupal.tui.term_id);$(this).dialog("close");}},width:'300px',height:'200px'});
      }
      break;
    case 'tui-next':
    case 'tui-previous': 
      $.ajax({cache:false,url:Drupal.settings.tui.callbacks.nextorprevious+"/"+img_clicked+"/"+Drupal.settings.tui.vocabulary+"/"+Drupal.tui.term_id,success:function(data){Drupal.tui.term_id = "tid-"+data;if(!Drupal.settings.tui.opentids[data]){Drupal.settings.tui.opentids[data] = data;Drupal.tui.update_link();Drupal.tui.reload_tree();Drupal.tui.show_form_after_tree_rebuild = true;}else{Drupal.tui.display_form($('#' + Drupal.tui.term_id));}}});
  }
}

Drupal.tui.do_delete = function(term_id){
  $.ajax({cache:false,url:Drupal.settings.tui.callbacks.delete+"/"+term_id,success:function(data){Drupal.tui.reload_tree();}});
}

Drupal.tui.resize_frame = function(){
  if($('#tui-tree-container').height() > ($(window).height()-50)){
    $('#tui-tree-container').css('height', ($(window).height()-50)+'px');
    $('#tui-tree-container').css('overflow-y', 'scroll');
  } else {
    $('#tui-tree-container').css('height', 'auto');
    $('#tui-tree-container').css('overflow-y', 'visible');
  }
}

Drupal.tui.update_link = function(){
  var tids_string = '';
  for(var i in Drupal.settings.tui.opentids){
    tids_string += Drupal.settings.tui.opentids[i]+"%2C";
  }
  $('#tui-link-back').attr('href', Drupal.settings.tui.callbacks.page+'/'+tids_string.substring(0, tids_string.length-3));
}

Drupal.tui.drag_start = function(event, ui){
  $('.tui-term.active').removeClass('active');
  delete Drupal.tui.term_id;
  $(event.currentTarget).addClass("tui-added-original");
  $('#tui-tree-subcontainer .tui-nodeleaf, #tui-tree-subcontainer .tui-term').droppable({tolerance:'pointer',greedy:true,over:function(event, ui){Drupal.tui.drop_over(event, ui);},deactivate:function(event, ui){Drupal.tui.drop_deactivate(event, ui);}});
}

Drupal.tui.drop_deactivate = function(event, ui){
  if(!Drupal.tui.waiting_for_reply){
    $('#tui-tree-subcontainer .tui-nodeleaf, #tui-tree-subcontainer .tui-term').droppable("destroy");
    Drupal.tui.waiting_for_reply = true;
    $.ajax({cache:false,url:Drupal.settings.tui.callbacks.move+"/"+Drupal.tui.parentorsibling+"/"+Drupal.tui.this_id+"/"+Drupal.tui.parent_or_sibling_id,success:function(data){Drupal.tui.reload_tree();}});
  }
}

Drupal.tui.drop_over = function(event, ui){
  Drupal.tui.parent_or_sibling_id = $(ui.element).parent().attr('id');
  Drupal.tui.this_id = $(ui.draggable).attr('id');
  Drupal.tui.parentorsibling = 'child';
  if($(ui.element).attr('id') == ''){
    Drupal.tui.parentorsibling = 'sibling';
  }  
  $('.tui-added').remove();
  if($(ui.element).hasClass('tui-term')){
    $(ui.element).append('<ul class="tui-added"><li>'+$(ui.draggable).html()+'</li></ul>');    
  } else {
    $(ui.element).parent().after('<li class="tui-added">'+$(ui.draggable).html()+'</li>');    
  }
}

Drupal.tui.click_closed = function(vid_and_tid){  
  $('#'+vid_and_tid+' > span.tui-nodeleaf').removeClass('tui-node-closed');
  $('#'+vid_and_tid+' > span.tui-nodeleaf').addClass('tui-node-open');
  $('#'+vid_and_tid+' > span.tui-nodeleaf').unbind('mouseup');
  $.ajax({cache:false,url:Drupal.settings.tui.callbacks.tree+"/"+vid_and_tid,success:function(data){Drupal.tui.tree_success($('#'+vid_and_tid), data);}});
}

Drupal.tui.remove_tid = function(vid_and_tid){
  jQuery.each($('#'+vid_and_tid+' > ul .tui-term'), function(idx, obj){
    tid_to_remove = $(obj).attr('id').substring(4);
    delete Drupal.settings.tui.opentids[tid_to_remove];
  });
  Drupal.tui.update_link();
}

Drupal.tui.add_tid = function(vid_and_tid){
  jQuery.each($('#'+vid_and_tid+' > ul .tui-term'), function(idx, obj){
    tid_to_add = $(obj).attr('id').substring(4);
    Drupal.settings.tui.opentids[tid_to_add] = tid_to_add;
  });
  Drupal.tui.update_link();
}

Drupal.tui.click_open = function(vid_and_tid){
  $('#'+vid_and_tid+' > span.tui-nodeleaf').removeClass('tui-node-open');
  $('#'+vid_and_tid+' > span.tui-nodeleaf').addClass('tui-node-closed');
  Drupal.tui.remove_tid(vid_and_tid);
  $('#'+vid_and_tid).children('ul').remove();
  Drupal.tui.resize_frame();
  jQuery.each(Drupal.behaviors, function() {
    this($('#'+vid_and_tid));
  });
}

Drupal.tui.tree_success = function(html_object, data){
  $(html_object).append(data);
  jQuery.each(Drupal.behaviors, function() {
    this(html_object);
  });
  Drupal.tui.resize_frame();
  Drupal.tui.add_tid($(html_object).attr('id'));
}

Drupal.tui.full_tree_success = function(data){
  //alert(data);
  Drupal.tui.waiting_for_reply = false;
  $('#tui-tree-subcontainer').html(data);
  jQuery.each(Drupal.behaviors, function() {
    this('#tui-tree-subcontainer');
  });
  if(Drupal.tui.show_form_after_tree_rebuild){
    Drupal.tui.show_form_after_tree_rebuild = false;
    Drupal.tui.display_form($('#'+Drupal.tui.term_id));
  }
}

Drupal.tui.form_success = function(data){
  $('#tui-form-container').html(data);
  jQuery.each(Drupal.behaviors, function() {
    this($('#tui-form-container'));
  });
  $('.loading').removeClass('loading');
}

Drupal.tui.display_form = function(element){
  $('.tui-term').removeClass('active');
  if(element){
    $(element).addClass('active');
    $(element).addClass('loading');
    if($(element).attr('id')){
      Drupal.tui.term_id = $(element).attr('id');
    }
  }
  $.ajax({cache:false,url:Drupal.settings.tui.callbacks.form+"/"+Drupal.tui.term_id,success:function(data){Drupal.tui.form_success(data);}}); 
}

Drupal.tui.reload_tree = function(){
  $.ajax({type:'POST',cache:false,url:Drupal.settings.tui.callbacks.full_tree+"/"+Drupal.settings.tui.vocabulary,success:function(data){Drupal.tui.full_tree_success(data);},data:Drupal.settings.tui.opentids});
}

Drupal.behaviors.tui = function(context){
  Drupal.tui.init(context);
};