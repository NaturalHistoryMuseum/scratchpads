function citation_get_citations(callback){
  var ajax_options = {
    url:callback,
    success:function(data){
      citations_got_citations(data);
    }
  };
  $.ajax(ajax_options); 
}

function citation_got_citations(citations){
  
}