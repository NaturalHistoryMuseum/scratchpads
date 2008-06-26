function init_dblclick(dom){
	
	var selector;
	
	if(dom){
		selector = dom+" .dblclick";
	}else{
		selector = ".dblclick";
	}
	
    $(selector).each(function(){
		
		$(this).hover(
			function(){
				$(this).addClass('dblcick-hover');
			},	
		
			function(){
				$(this).removeClass('dblcick-hover');
			}
		);
		
		$(this).unbind('dblclick');
		
		$(this).dblclick(dblclicked);
		
	})
	
}

function dblclicked(){
	
	var edit_node = $(this).attr('class').match(/node-\d+/);

	if(edit_node){
		$.post('/dblclick2edit', {op: edit_node}, goto_edit);
	}
	
}

function goto_edit(url){
	
	if(url){
	window.location = url;	
	}
	
}




if (Drupal.jsEnabled) {

  $(document).ready(function(){
	
	init_dblclick(null);

  });

}
