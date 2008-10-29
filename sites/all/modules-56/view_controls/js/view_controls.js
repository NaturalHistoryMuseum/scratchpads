$(document).ready(function(){
	
	if ($.tablesorter) {
		$('.tablesorter').tablesorter();	
	}

});
 

function init_configure_form(vid){
	
	if (Drupal.jsEnabled) {
	
		var options = { 
			stop: 	update_positions,
			cursor: 	'move',
			opacity: 	20,	
			start: break_out,
			scroll: true,
			scrollSensitivity: 40,
			connectWith: $("#hidden ul.view-control-form-ul"),
			zIndex: 200
		};
		
		$ui("#visible ul.view-control-form-ul").sortable(options);
			
		options.connectWith = $("#visible ul.view-control-form-ul");
		options.scroll = false;
		
		$ui("#hidden ul.view-control-form-ul").sortable(options);
	
		// attach ajax submit handler
	
		if(ajaxify){
			
			$("#view-controls-configure-form").submit(function(){
			
				var ajax_options = { 
					type: 			"POST",
					url: 			'/view_controls/configure_submit',
					data: 			$ui(this).serialize(),
					beforeSend: 	function(){
									before_send(vid);	
									},
					success: 		function(data, textStatus){
									on_success(data, textStatus, vid);
									}	
				};
			
				$ui.ajax(ajax_options);
				return false;
				
			});
			
			
		}
	
		//attach drag / drop behaviour to tables
		
		var drag_options = {
			helper: 'clone', 
			start: start_table_drag, 
			stop: stop_table_drag,
			drag: table_drag,
			zIndex: 150,
			appendTo: '#helper-table'
		}
		
		$ui('div.view-control-table tbody tr').draggable(drag_options);
	
		if ($.tablesorter) {
			$('div.view-control-table .tablesorter').tablesorter();
		}
		
		// Will be useful to be able to open in lightbox (and test if hotlinking is allowed) 
		if(typeof Lightbox == 'object'){
		
			$("a.lightbox").each(function(){

				$(this).unbind('click');

				$(this).click(function(){
				Lightbox.start(this);
				return false
				});

			})
		}
	
	}
	
}

function init_reset_form(vid){
	
	if(ajaxify){
		
		ajaxify_form("#view-controls-reset-form", '/view_controls/reset_submit', vid);
	
	}
	
}

var view_name;

function ajaxify_form(form_id, target, vid){
	
	view_name = vid;
	
	$ui(form_id).submit(function(){
	
		var ajax_options = { 
			type: 			"POST",
			url: 			target,
			data: 			$ui(this).serialize(),
			beforeSend: 	function(){
							before_send(vid);	
							},
			success: 		function(data, textStatus){
							on_success(data, textStatus, vid);
							}	
		};
	
		$ui.ajax(ajax_options);
		return false;
		
	});
}

function on_success(data, textStatus, vid){
	
	// console.log(view_name)
	
	// nasty hack to get this to work in Opera, but it works for now...
	if(!vid){
		vid = view_name;
	}
	
	if(textStatus != 'success'){
		data = 'Sorry, the view could not be updated at this time';
	}
	
	if($("#view-control-"+vid).parents('.content').length){
		var target = $("#view-control-"+vid).parents('.content');
	}else if($("#view-control-"+vid).parents('.view').length){
		var target = $("#view-control-"+vid).parents('.view');
	}
	
	target.html(data);
	
	add_event_handlers(vid);
	
}

function dropped(e, ui){
	
	var new_id = $(this).parents('.view-control-column').attr('id');
	
	var previous_id = ui.draggable.parents('.view-control-column').attr('id');
	
	if(previous_id != new_id){
		
		$('#'+new_id+' table').append(ui.draggable);

		// let tablesort know the table has been updated
		if ($.tablesorter) {
			if($('#'+new_id+' table.tablesorter').length){
				$('#'+new_id+' table.tablesorter').trigger("update");	
			}
		}	
	}
	
	update_input_name(ui.draggable.attr('id'), new_id, previous_id);
	
}

function update_input_name(id, new_name, old_name){

	var input = $('#'+new_name+' #'+id+' input');
	
	if(input.name().indexOf('outside') !== -1){
		replace_this = 'outside';
	}else{
		replace_this = old_name;
	}
	
	$(input).name(input.name().replace(replace_this, new_name));
	
}

function stop_table_drag(e, ui){
	
	// $uid.stop();
	$ui('#drop-box-overlay').droppable("disable");
	$ui('#drop-box-overlay').remove();
	$ui("#helper-table").css('visibility', 'hidden');
	
	var previous_id = ui.instance.element.parents('.view-control-column').attr('id');
	
	//add the dummy empty row	
	if($("#"+previous_id+" tr").size() < 1){
		$("#"+previous_id+" table").append('<tr class="dummy-empty"><td></td></tr>');
	}else{
		$("tr.dummy-empty").remove();
	}
	
	
}	

function start_table_drag(e, ui){
	
	//ensure the dimensions of the object being dragged are respected
	ui.helper.width(ui.instance.element.width());

	var previous_id = ui.instance.element.parents('.view-control-column').attr('id');
	
	if(previous_id == 'hidden'){		
		var active_id = 'visible';		
	}else{
		var active_id = 'hidden';		
	}
	
	$('#'+active_id).append('<div id="drop-box-overlay"></div>');
	
	// make the overlay droppable
	var drop_options = {
		accept:		'#'+active_id+' tr',
		drop: 		dropped
	}	
		
	// use the outer div, not the table - less buggy
	$ui("#drop-box-overlay").droppable(drop_options);

}

// IE needs the tr to be in a table - but appending causes flash of the row un-positoned - show on drag
function table_drag(){

	$ui("#helper-table").css('visibility', 'visible');
	
}

// allow the dragged list elements to move beyond confines of parent with overflow:auto
function break_out(e, ui){

	var previous_id = $(ui.instance.helper).parents('div.view-control-column').attr('id');
	
	var scroll = $ui('#'+previous_id+' ul.view-control-form-ul').scrollTop();
	
	$("li.ui-sortable-helper").css('margin-top', (0 - scroll));
	
	//ensure the dimensions of the object being dragged are respected
	ui.helper.width(ui.instance.currentItem.width());
	ui.helper.height(ui.instance.currentItem.height());
	
	ui.instance.helper.appendTo($(this).parents('.view-control-column'));
	
}



// Change the form elements to reflect repositioned content
function update_positions(e, ui){
	
	var previous_id = $(ui.instance.helper).parents('div.view-control-column').attr('id'); // the helper will still have the old id
	
	var new_id = $(ui.instance.currentItem).parents('div.view-control-column').attr('id'); 
	
	if(new_id != previous_id){ //item has been moved
		
		update_input_name($ui(ui.instance.currentItem).attr('id'), new_id, previous_id);
		
		//if the old list is now empty, add a dummy list item (bug fix)
		//otherwise, remove the dummy list items	
		if($("#"+previous_id+" li:not(.ui-sortable-helper)").size() < 1){
			$("#"+previous_id+" ul.view-control-form-ul").append('<li class="dummy-empty"></li>');
		}else{
			$("#"+previous_id+" li.dummy-empty").remove();
			$("#"+new_id+" li.dummy-empty").remove();
		}
		
	}
	
}



function before_send(vid){

	// hide the thickbox
	if (window.tb_remove){
	tb_remove();
	}
	
	// add spinning wheel
	$("#view-control-"+vid).html('<div class="ajax-loading"><img width="32" height="32" alt="Loading data..." src="/'+view_control_directory+'/extras/loading.gif" /></div>');
	
}



// Re-add the event handlers
function add_event_handlers(vid){

	$("#view-control-"+vid).parents('.content').find("a.thickbox").each(function(){

		$(this).unbind('click');
		tb_init($(this));

	})
	
	if(typeof Lightbox == 'object'){
		
		$("#view-control-"+vid).parents('.content').find("a.lightbox").each(function(){
	
			$(this).unbind('click');
				
			$(this).click(function(){
			Lightbox.start(this);
			return false
			});
	
		})
	}
	
	if ($.init_dblClicker == 'function') {
	   init_dblClicker("#view-control-"+vid);
	}
	
	// add tablesorter
	if ($.tablesorter) {
	$("#view-control-"+vid+" .tablesorter").tablesorter();	
	}
	
}
		

