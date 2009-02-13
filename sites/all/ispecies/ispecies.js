citations.ispecies = new Array();
var ispecies_update_id;

$(document).ready(function(){
	
	ispecies_update_id = 'bhl';
	
	$(document).ajaxStop(function(){

		ispecies_complete(ispecies_update_id);
		ispecies_update_id = '';
				
	});
	
});

function init_ispecies(view_name){
	
	var ajax_options = { 
		type: 			"POST",
		url: 			'/ispecies/ajax_callback',
		data: 			{view_name: view_name, tid: tid},
		success: 		function(data, textStatus){
						ispecies_success(data, textStatus, view_name);
						},
		complete:		function(){
						ispecies_complete(view_name);	
						}			
	};

	$ui.ajax(ajax_options);	
	
}

function init_bhl(obj){
	
	$(obj+' li a:not(.thickbox)').unbind('click');
	$(obj+' li a:not(.thickbox)').click(function() {
		
		$(this).parents('li').find("ul").slideToggle("fast");
		return false;

	});
	
	// Can only handle one thickbox so open in new window
	if(obj==='form.view-control-form-bhl'){
		
		$(obj+' li a.thickbox').each(function(){
			
			$(this).unbind('click');
			$(this).removeClass('thickbox');
			$(this).attr({target: '_blank'});
		
		});
		
	}
	
	
}

function ispecies_success(json, textStatus, element_id){

	if(textStatus != 'success'){
		resultObj.output = 'Sorry, the view could not be updated at this time';
	}else{
		var resultObj = eval('(' + json + ')');
	}

	// update the panel content
	$("#"+element_id).html(resultObj.output);
	
	//update citations array
	
	if(resultObj.references instanceof Object){
	
		for(var i=0; i<resultObj.references.length;i++){	
		citation_add_citation(resultObj.view_name, resultObj.references[i]);
		}
		
	}else if(resultObj.references){
		
		citation_add_citation(resultObj.view_name, resultObj.references);
		
	}
	
}

function ispecies_set_update(element_id){
	
	ispecies_update_id = element_id;
	
	if(element_id == 'bhl'){
	init_bhl('form.view-control-form-bhl');		
	}
	
}


function ispecies_complete(element_id){

	// $ui("#"+element_id+" a.lightbox").unbind('click');
	// $ui("#"+element_id+" a.lightbox").click(function(){
	// Lightbox.start(this);
	// return false;	
	// });
	
	
	if(element_id == 'bhl'){
	init_bhl('#view-control-bhl');		
	}
	

	// This is necessary as iSpecies looks up external data when the form is reset, so the 
	// events need to be readded once this has completed
	add_event_handlers(element_id);

}