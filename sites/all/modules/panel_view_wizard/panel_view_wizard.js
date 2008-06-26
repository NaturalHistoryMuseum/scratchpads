$(document).ready(function(){
	
	$("#edit-node-types").change(function(){	
		populate_field_options();
	});

	//show / hide the fields dpending on view type
	$("input[@name='page_type']").change(function(){
		toggle_field_display();
	});
	
	//run once on page load
	toggle_field_display();

});

function populate_field_options(){
	
	$.post('/panels/wizard/get_fields/ajax', {node: $("#edit-node-types").val()},function(txt){
	$('#edit-fields').html(txt);	
	});
}

function toggle_field_display(){
	
	$.post('/panels/wizard/plugin/ajax', {plugin: $("input[@name='page_type']:checked").val()},function(txt){
	
		if(txt==1){
		
			$("#fields-wrapper").show();
		
		}else{
		
			$("#fields-wrapper").hide();
		
		}
		
	});

}
	

