
var printed = false;

$(document).ready(function(){

	if(typeof( window[ 'printable' ] ) != "undefined"){

	
		// Call the update_references on timeout so will still run if no ajax, but is cancelled if there is
		var t = setTimeout("print_page()", 3000);

		$("html").ajaxStart(function () {
			clearTimeout(t);
		}).ajaxStop(function () {
			//give time for content to render
			var t = setTimeout("print_page()", 3500);
		});
		
	}else{
		
			
		$('.nhm-printable a').click(function(){
			
			openWin = window.open($(this).href());
			
			var exists = false;
		    
			if (openWin && !openWin.closed){
			
			exists = true;
			
			try {
			        openWin.focus();
			        }
			        catch (e) {
			            exists = false
			        }
			
			if(exists){
			return false;
			}
			
			}
			
		})
			
		
	}
	
	


	
});



function print_page(){
	
	if(!printed){
	window.print();
	printed = true;
	}
	
}