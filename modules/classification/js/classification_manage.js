function deleteClassification(vid) {
	var classification_element = $('#classification-'+vid);
	var classification_name = classification_element.text();
	
	if(confirm("Are you sure you want to delete " + classification_name + "?")) {
	  $.post("delete_classification/", {"vid" : vid }, function(data) {displayMessage(data);}, "json");
	  classification_element.parent().parent().parent().fadeOut("slow");
  }
  else {
  	return false;
  }
}

function displayMessage(content) {
    	 var message_type = "";
    	 switch (content.status) {
    	 	  case "deleted":
    	 	    message_type = "status";
    	 	    break;
    	 	  case "failed":
    	 	    message_type = "error";
    	 	    break;
    	 	  default:
    	 	    message_type = "";
    	 }
}