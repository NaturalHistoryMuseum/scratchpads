function classification_biblio() {

	var tid;
	var nid;
	var changed;
	
	return {
		
	// Constructor	
	init: function() {
		
 		this.add_events();

    },

	// Add the events
    add_events: function() {
	
		$(document).ajaxComplete(function(){

			$('div#classification_tree li a').click(CITE.get_citation);
			
		});
		
		$('#classification_name_metadata button.save').click(CITE.update_citation);
		
		$('#classification_name_metadata').change(function(){
			
			CITE.changed = true;
			
		})
		
		$('#tree_actions li.delete a').click(function(){
		  
		  // Test to see if there's a TID. If theres not, the term has been deleted 
			if(!$('#edit-tid').val()){
			  
			  $('#edit-bibliographic-citation').val('');
			  
			}
			
		});
			
		
    },

	// Get the citation data for the selected item
    get_citation: function() {

		CITE.tid = $(this).parent('LI').attr('id').replace('n', '');
		
		// Update the add link with the new tid
		
		$.post(Drupal.settings.classification_biblio_callback_base_url+"/get_citation/", { "tid" : CITE.tid}, function(data) {CITE.build_citation(data);}, "json");
    	
    },

	make_link: function(link_obj, tid, nid){
		
		var a = document.createElement('a');
		t = document.createTextNode(link_obj.text);
		a.appendChild(t);
		
		link_obj.href = link_obj.href.replace('%nid', nid);
		link_obj.href = link_obj.href.replace('%tid', tid);
		
		a.setAttribute('href', link_obj.href);
		
		a.onclick = function(){
			
			if(CITE.changed){
				if(!confirm('The information on this form has been changed. If you continue you will lose these changes.\rIf you want to keep them, select \'Cancel\' and save the form before proceeding')){
					return false;
				}
			}
			
		}
		
		return a;
		
	},

	// Add the citation data to the form
    build_citation: function(data) {

		$('#edit-bibliographic-citation').val(data.citation);
		
		var link_settings;
		
		if(data.nid){
			link_settings = Drupal.settings.classification_biblio_edit_link;
		}else{
			link_settings = Drupal.settings.classification_biblio_add_link;
		}
		
		var link = CITE.make_link(link_settings, CITE.tid, data.nid);
		
		$('#citation-link').html(link);
		
		
		$('#citation-link').attr('href', '/node/add/biblio/classification/'+this.tid);
    },

    update_citation: function() {

		$.post(Drupal.settings.classification_biblio_callback_base_url+"/update_citation/", { "tid" : CITE.tid, "citation" : $('#edit-bibliographic-citation').val()});
		
    }

  };
}



$(document).ready(function() {
	
	CITE = new classification_biblio();
	CITE.init();

});