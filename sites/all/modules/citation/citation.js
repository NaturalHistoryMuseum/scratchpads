function citation_get_citations(callback){
  var ajax_options = {
    url:callback,
    success:function(data){
      citation_got_citations(data);
    },
    cache:false
  };
  $.ajax(ajax_options); 
}

function citation_got_citations(citations){
  var resultObj = eval('(' + citations + ')');
  $('#citation').html(resultObj['content']);
}