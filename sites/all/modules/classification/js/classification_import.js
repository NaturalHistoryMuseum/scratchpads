function classification_import() {
  return {
  	// Functions to coordinate import
  	import_classification: function() {
  		$('#edit-search').hide();
  		$('#search_results').hide();
  		var vid = $('#edit-vid').val();
  		var result = $("input[@name='results[ancestry]']:checked").val();
  		result = result.split("|");
  		$("#import_status").fadeIn("fast").html('<div class="messages status">Import is under way...</div>');
		$.post(Drupal.settings.classification_callback_base_url+"/import_classification/", {"tid" : result[0], "vid" : vid, "tot_count" : result[1]});
		$.post(Drupal.settings.classification_callback_base_url+"/import_status/",{ "tot_count" : result[1] },function(data) {IMPORT.import_status(data);}, "json");
    },
    get_total: function(content) {
    	result = content.split("|");
    	if(result[1] > 15000) {
  			alert("Sorry, that import will exceed 15,000 names. Please focus your search.");
  			$('#import_classification_button').hide();
  		}
  		else {
  			$('#import_classification_button').show();
  		}
    },
    import_status: function(content) {
      switch(content.status) {
    	  case "success":
          $("#import_status").html('<div class="messages status">Count: ' + content.curr_count + ' of ' + content.tot_count + ' (' + parseInt((parseInt(content.curr_count)/parseInt(content.tot_count))*100) + '%) Name: ' + content.name + '</div>');
          if(parseInt(content.curr_count) < parseInt(content.tot_count)) {
               $.post(Drupal.settings.classification_callback_base_url+"/import_status/",{ "tot_count" : content.tot_count },function(data) {IMPORT.import_status(data);}, "json");
          }
          else if (parseInt(content.curr_count)+1 >= parseInt(content.tot_count)) {
               $.post(Drupal.settings.classification_callback_base_url+"/import_message/",{ "tot_count" : content.tot_count, "vid" : $('#edit-vid').val(), "status" : content.status},function(data) {IMPORT.import_message(data);}, "json");
          }
          else {}
          break;
        case "fail":
             $.post(Drupal.settings.classification_callback_base_url+"/import_message/",{ "tot_count" : content.tot_count, "vid" : $('#edit-vid').val(), "status" : content.status},function(data) {IMPORT.import_message(data);}, "json");
          break;
        default:
     }
    },
	import_message: function(content){      
      $("#import_status").html(content.message);     
	}
  }
}