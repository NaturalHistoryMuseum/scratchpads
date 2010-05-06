function ispecies_callback(callback, viewname){
  var ajax_options = {
    cache:false,
    url:callback+"/"+viewname+"/"+Drupal.settings.ispecies.page_tid,
    success:function(data){
      ispecies_success(data, viewname);
    },
    complete:function(){
      add_event_handlers('#'+viewname);
    },
    data:"url="+location.href
  };
  $.ajax(ajax_options); 
}
function ispecies_success(data, viewname){
  var resultObj = eval('(' + data + ')');
  var output = '';
  var output_added = false;
  $.each(resultObj, function() {
    if(this['body']){
      output_added = true;
      output += this['body'];
    }
  });
  if(!output_added){
    output = Drupal.settings.ispecies.empty_view;
  }
  // Replace the output already set
  $('#'+viewname).html(output);
  // Get the citations
  if(resultObj['citation']){
    citation_add_citation_url(resultObj);
  }
}
function add_event_handlers(context){
  if(typeof Lightbox == 'object'){
    Lightbox.initList(context);
  }
}