var citations_on_page = false;
var additional_citations = new Array();

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
  citations_on_page = true;
  citation_add_citation_html(false);
}

function citation_add_citation_html(html){
  if(html){
    additional_citations[additional_citations.length] = html;
  }
  if(citations_on_page){
    $.each(additional_citations, function() {
      $('#citation > ul').append('<li>'+this+'</li>');
    });
  }
}

function citation_create(callback){
  alert(callback);return false;
  // Lets send the whole chuffin' page back to the server, where it'll be parsed
  // returning a cool URL.
  page_data = $('html').html();
  var ajax_options = {
    url:callback,
    success:function(data){
      alert(data);
    },
    cache:false
  };
  $.ajax(ajax_options);  
}