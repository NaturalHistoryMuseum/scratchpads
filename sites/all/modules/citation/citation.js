var citations_on_page = false;
var additional_citations = new Array();
var citation_nids = Array();
var citation_urls = Array();

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

function citation_email(callback, cid, uid){
  var ajax_options = {
    type:"POST",
    url:callback,
    success:function(data){
      alert(data);
    },
    data:{cid:cid,uid:uid},
    cache:false
  };
  $.ajax(ajax_options);  
}

function citation_got_citations(citations){
  var resultObj = eval('(' + citations + ')');
  $('#citation').html(resultObj['content']);
  citations_on_page = true;
  $.each(resultObj['nids'], function() {
    citation_nids[citation_nids.length] = this;
  });
  citation_add_citation_html(false);
}

function citation_add_citation_url(citations){
  citation_add_citation_html(citations['citation']);
  //$.each(citations['urls'], function() {
  citation_urls[citation_urls.length] = citations['urls']['name']
  //});
}

function citation_add_citation_html(html){
  if(html){
    additional_citations[additional_citations.length] = html;
  }
  if(citations_on_page){
    $.each(additional_citations, function() {
      $('#citation > ul').append('<li>'+this+'</li>');
    });
    additional_citations = new Array();
  }
}

function citation_started(){
  //borrow some of the thickbox stuff to show something is working
  $("body").append('<div id="TB_overlay" class="TB_overlayBG"></div><div id="TB_load"><h1 style="margin:50px 0;color:white;">Please&nbsp;be&nbsp;patient, this&nbsp;can&nbsp;take&nbsp;a&nbsp;little&nbsp;time<h1></div>');//add loader to the page
  $('#TB_load').show();//show loader
}

function citation_success(callback, cid){
  // We've got the data, parse it and work some magic    
  var url = callback+"/"+cid;
  $('#TB_overlay').remove();
  $('#TB_load').hide();//show loader
  tb_show(null, url, false);
}

function citation_create(callback, callback_success){
  // Lets send the whole chuffin' page back to the server, where it'll be parsed
  // returning a cool URL.
  page_data = $('html').html();
  var ajax_options = {
    type:"POST",
    url:callback,
    success:function(data){
      citation_success(callback_success, data);
    },
    beforeSend:function(data, textStatus){
      citation_started();
    },
    data:{url:location.href,"citation_nids[]":citation_nids,"citation_urls[]":citation_urls,page:page_data},
    cache:false
  };
  $.ajax(ajax_options);  
}