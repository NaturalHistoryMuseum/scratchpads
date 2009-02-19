function ispecies_callback(callback, viewname){
  var ajax_options = {
    url:callback,
    success:function(data){
      ispecies_success(data, viewname);
    },
    complete:function(){
      add_event_handlers($('#'+viewname));
    }
  };
  $.ajax(ajax_options); 
}

function ispecies_success(data, viewname){
  var resultObj = eval('(' + data + ')');
  var output = '';
  $.each(resultObj, function() {
    output += this['body'];
  });
  // Replace the output already set
  $('#'+viewname).html(output);
}

function add_event_handlers(alteredobject){
  
  alteredobject.parents('.content').find("a.thickbox").each(function(){

    $(this).unbind('click');
    tb_init($(this));

  })
  
  if(typeof Lightbox == 'object'){
    
    alteredobject.parents('.content').find("a.lightbox").each(function(){
  
      $(this).unbind('click');
        
      $(this).click(function(){
      Lightbox.start(this);
      return false
      });
  
    })
  }
  // add tablesorter
  if ($.tablesorter) {
    $("#view-control-"+vid+" .tablesorter").tablesorter();  
  }
  
}