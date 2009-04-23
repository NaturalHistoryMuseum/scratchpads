var page_tid;

function ispecies_callback(callback, viewname){
  var ajax_options = {
    cache:false,
    url:callback+"/"+viewname+"/"+page_tid,
    success:function(data){
      ispecies_success(data, viewname);
    },
    complete:function(){
      add_event_handlers($('#'+viewname));
    },
    data:"url="+location.href
  };
  $.ajax(ajax_options); 
}

function ispecies_success(data, viewname){
  var resultObj = eval('(' + data + ')');
  var output = '';
  $.each(resultObj, function() {
    if(this['body']){
      output += this['body'];
    }
  });
  // Replace the output already set
  $('#'+viewname).html(output);
  // Get the citations
  if(resultObj['citation']){
    citation_add_citation_url(resultObj);
  }
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
}