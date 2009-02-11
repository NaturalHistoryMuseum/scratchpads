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
		  $.post("import_classification/", {"tid" : result[0], "vid" : vid, "tot_count" : result[1]});
  		$.post("import_status/",{ "tot_count" : result[1] },function(data) {IMPORT.import_status(data);}, "json");
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
    import_message: function(content) {
    	var message_type = "";
    	switch (content.status) {
    		case "success":
    		  message_type = "status";
    	    break;
    	  case "fail":
    	    message_type = "error";
    	    break;
    	  default:
    	    message_type = "";
     }
     $("#import_status").fadeIn("fast").html('<div class="messages ' + message_type + '">' + content.message + '</div>').fadeOut(3000);
    },
    import_status: function(content) {
      switch(content.status) {
    	  case "success":
          $("#import_status").html('<div class="messages status">Count: ' + content.curr_count + ' of ' + content.tot_count + ' (' + parseInt((parseInt(content.curr_count)/parseInt(content.tot_count))*100) + '%) Name: ' + content.name + '</div>');
          if(parseInt(content.curr_count) < parseInt(content.tot_count)) {
     	       $.post("import_status/",{ "tot_count" : content.tot_count },function(data) {IMPORT.import_status(data);}, "json");
          }
          else if (parseInt(content.curr_count)+1 >= parseInt(content.tot_count)) {
     	       $("#import_status").html('<div class="messages status">Import complete. <a href="../classification/edit">View and Edit</a> your classification.</div>');
          }
          else {}
          break;
        case "fail":
             $("#import_status").html('<div class="messages status">Import completed in a flash (or is <i>almost</i> done). <a href="../classification/edit">View and Edit</a> your classification.</div>');
          break;
        default:
     }
    }
  }
}