
var citations = new Array();

function citation_add_citation(key, ref){
	
	citations.push(new Array(key, ref))
	
}


$(document).ready(function(){
	
	if($('#citation').length == 0){
		return;
	}
	
	// if the user doesn't have access, the block won't be displayed
	if(!create_citations){
		return;
	}

	// Call the update_references on timeout so will still run if no ajax	
	var t = setTimeout("update_references()", 100);
	
	$("body").ajaxStart(function () {
		clearTimeout(t);
	}).ajaxStop(function () {
		//give time for content to render
		var t = setTimeout("update_references()", 2000);
		$("body").unbind('ajaxStop');
	});
		
});

function update_references(){

	var data = ' ';

	for(var i=0; i<citations.length;i++){

		if(i>0){
			data += '&';
		}

		data += citations[i][0] +'[]='+citations[i][1];

	}		

	var options = {
		type: 		"POST",
		url: 		"/citation/references",
		data: 		data,
		success: 	function(data, textStatus){
					citation_success(data, textStatus);
					},
		complete:   function(){
					// unbind & rebind the ajax completion event
					$('#citation').unbind('ajaxStop');
					}			
	}

	$.ajax(options);
	
}

function citation_success(data, textStatus){

	$('#citation').html(data);
	
	// add the submission overide once the form has been rendered
	$('#citation-references-form').submit(function(){
		
		var data = $ui(this).serialize();
		
		var page_data = $('html').html();
		
		//any & will break this, so convert to ***amp*** - convert back server side
		page_data = page_data.replace(/&/g,"***amp***");
		
		data += '&page_data='+page_data;
		data += '&title='+$('title').text();
			
		var options = {
			type: 		"POST",
			url: 		"/citation/ajax_callback",
			data:  data,
			success: 	function(data, textStatus){
						citation_added(data, textStatus);
						},
			beforeSend: 	function(data, textStatus){
						citation_started();
						}
	
		}
	
		$.ajax(options);
		
		return false;
		
	})

}

function citation_started(){
	
	//borrow some of the thickbox stuff to show something is working
	if(thickbox){
	$("body").append("<div id='TB_overlay'></div><div id='TB_load'></div>");//add loader to the page
	$('#TB_load').show();//show loader
	}

}

function citation_added(data, textStatus){
	
	if(data){
		
		var url = '/citation/email/'+data;
		
		if(thickbox){
			$('#TB_overlay').remove();
			$('#TB_load').hide();//show loader
			tb_show(null, url, false);			
		}else{
			window.location = url; 
		}
	}
	
}

function init_citation_email_form(){
	
	$('#citation-email-form').submit(function(){

		var email_form = $ui(this).serialize();
		
		if($('#edit-mail').length){
			
			var validation_options = {
				type: 		"POST",
				url: 		'/citation/ajax_email_validation',
				data:       "email="+$('#edit-mail').val(),
				success: 	function(data, textStatus){
							
								// if the email address passes validation
								if(data != 'error'){
									
									var options = {
										type: 		"POST",
										url:  '/citation/ajax_callback',
										data: email_form,
										success: 	function(email_data, textStatus){
													email_sent(email_data, textStatus);
													},
										beforeSend: function(data, status){
													$('#citation-email-form').append('<div class="sending-citation"><img width="32" height="32" alt="Loading data..." src="/'+view_control_directory+'/extras/loading.gif" /></div>');	
													},
										complete: function(data, status){
													$('.sending-citation').remove();	
													}																
									}
									
									$ui.ajax(options);
									
									
								}else{ // addClass citation-error
									$('#edit-mail').addClass('citation-error');
								}
								
							
							}
			};
			
			$.ajax(validation_options);
			
		}
		
		return false;
		
	})
	
}

function email_sent(){
	
	// hide the thickbox
	if (window.tb_remove){
	tb_remove();
	}
	
}

