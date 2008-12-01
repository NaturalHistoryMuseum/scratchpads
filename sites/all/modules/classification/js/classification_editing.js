function classification_edit() {
  return {
  	import_classification: function() {
  		var vid = $('#edit-vid').val();
  		var result = $("input[@name='results[ancestry]']:checked").val();
  		result = result.split("|");
  		if(result[1] > 15000) {
  			alert("Sorry, that import will exceed 15,000 names. Please focus your search.");
  		}
  		else {
  			$("#import_status").fadeIn("fast").html('<div class="messages status">Import is under way...</div>');
  		  $.post("import_classification/", {"tid" : result[0], "vid" : vid}, function(data) {EDIT.import_message(data);}, "json");
  		}
    },
  	get_id: function(NODE) {
  		var node = NODE.id;
  		return node.replace("n","");
    },
    get_content: function(NODE) {
    	var node = NODE.id;
    	return $('#' + node + ' a:first').text();
    },
    filter: function() {
    	$('#reroot').fadeIn("fast");
    },
    apply_filter: function() {
    	$('#reroot').fadeOut("fast");
    	TREE.refresh();
    },
    remove_filter: function() {
    	$('#edit-root').val('');
    	$('#reroot_tid div:first').text('');
    	TREE.refresh();
    },
    close_filter: function() {
    	$('#reroot').fadeOut("fast");
    },
    get_metadata: function(NODE,FORM_ID) {
    	var tid = this.get_id(NODE);
    	$.post("get_metadata/", { "tid" : tid, "form_build_id" : FORM_ID }, function(data) {EDIT.build_metadata(data);}, "json");
    },
    set_vid: function() {
    	var vid = $("select[@name='alternate_classifications'] option:selected").val();
    	$("#edit-alternate-vid").val(vid);
    	this.get_tree();
    },
    get_tree: function() {
    	var vid = $("#edit-alternate-vid").val();
    	$("#classification_tree_alternate").html('');
    	TREE2 = new tree_component();
    	host = window.location.hostname;
			TREE2.init($("#classification_tree_alternate"), {
				data		: { type : "json", async : true, url : "js_tree2/" + vid + "/", json : false },
				dflt		: false,
				path		: "/sites/" + host + "/modules/classification/jsTree/",
				cookies : { prefix: "tree2", expires: 7, path: "/" },
				ui		  : {dots : true, rtl : false, animation : 10, hover_mode : true},
        lang    : {new_node : "Taxon", loading : "&nbsp;&nbsp;&nbsp;&nbsp;"},
				rules		: {
					type_attr	: "rel",
					createat	: "top",
					multitree	: true,
					metadata	: false,
					use_inline: false,
					clickable	: "all",
					renameable: "none",
					draggable	: "all",
					deletable	: "none",
					createable: "none",
					dragrules	: "all"
				},
				callback	: {
                			beforechange    : function(NODE,TREE_OBJ) { return true; },
                			beforemove      : function(NODE,REF_NODE,TYPE,TREE_OBJ) { return true; },
                			beforecreate    : function(NODE,REF_NODE,TYPE,TREE_OBJ) { return true; },
                			beforerename    : function(NODE,LANG,TREE_OBJ) { return true; },
                			beforedelete    : function(NODE,TREE_OBJ) { }, 
					            beforechange	  : function(NODE,TREE_OBJ) { },
                			onchange        : function(NODE,TREE_OBJ) { },
                			onrename        : function(NODE,LANG,TREE_OBJ) { },
                			onmove          : function(NODE,REF_NODE,TYPE,TREE_OBJ) {	
                				     EDIT.move_name_alternate(NODE,REF_NODE);
                			},
                			oncopy          : function(NODE,REF_NODE,TYPE,TREE_OBJ) { },
                			oncreate        : function(NODE,REF_NODE,TYPE,TREE_OBJ) { },
                			ondelete        : function(NODE, TREE_OBJ) { },
                			onopen          : function(NODE, TREE_OBJ) { },
                			onclose         : function(NODE, TREE_OBJ) { },
                			error           : function(TEXT, TREE_OBJ) { },
                			// onclk callback added by David Shorthouse
                			onclk           : function(NODE, TREE_OBJ) {
                				     return false;
                		  },
                			ondblclk        : function(NODE, TREE_OBJ) { },
                			onrgtclk        : function(NODE, TREE_OBJ, EV) { }
				}
			});
    },
    go_url: function(type) {
    	var tid = $('#edit-tid').val();
    	if (tid) {
    	  switch(type) {
    		  case 0:
    		    alert("id: " + tid + " type: images");
    		    break;
    		  case 1:
    		    alert("id: " + tid + " type: references");
    		    break;
    		  case 2:
    		    alert("id: " + tid + " type: taxon page");
    		    break;
    		  default:
    		    alert("Nothing")
    	  }
    	}
    },
    save_metadata: function() {
    	var tid = $('#edit-tid').val();
    	var parent_tid =	$('#edit-parent-tid').val();
    	var rank = $('#edit-rank').val();
    	var desc = $('#edit-description').val();
    	var relation_id = $('#edit-relation-id').val();
    	var relation_type = $('#edit-relation-type').val();
    	if(tid) {
    	   $.post("update_metadata/", { "tid" : tid, "parent" : parent_tid, "rank" : rank, "description" : desc, "relation_id" : relation_id, "relation_type" : relation_type }, function(data) {EDIT.display_message(data);}, "json");
    	}		
    },
  	edit_name: function(NODE) {
  		var tid = this.get_id(NODE);
      var newname = this.get_content(NODE);
      $.post("edit_name/", { "tid" : tid, "value" : newname }, function(data) {EDIT.display_message(data);}, "json");
    },
    adjust_id: function(content) {
    	var new_node_id = "n" + content.maxtid;
    	return new_node_id;
    },
    add_name: function(REF_NODE) {
    	var parent_tid = this.get_id(REF_NODE);
      $.post("add_name/", { "parent_tid" : parent_tid }, function(data) {EDIT.display_message(data); EDIT.update_tid(REF_NODE.id,data.tid);}, "json");
    },
    adjust_name: function() {
    	var name = $('#input_name').val();
    	$('#metadata_taxon_title').html(name);
    },
    update_tid: function(REF_NODE,tid) {
    	$('#' + REF_NODE + ' ul li:first-child').attr('id', 'n' + tid);
    	$('#edit-tid').val(tid);
    	$('#edit-parent-tid').val('');
    	$('#metadata_taxon_title').html('');
    	$('#edit-rank').val('');
    	$('#edit-relation-id').val('');
    	$('#edit-relation-type').val('');
    	$('#edit-description').val('');
    },
    delete_name: function(NODE) {
    	var tid = this.get_id(NODE);
    	var name = this.get_content(NODE);
    	if(confirm("Are you sure you want to delete:\n\n" + name + " \n\nAND all of its children?\n\nYou may have content associated with the identifiers behind these names.")) {
    	  $.post("delete_name/", {"tid" : tid }, function(data) {EDIT.display_message(data);}, "json");
    	  TREE.get_next();
    	  $('#edit-tid').val('');
    	  $('#edit-parent-tid').val('');
    	  $('#metadata_taxon_title').html('');
    	  $('#edit-rank').val('');
    	  $('#edit-relation-id').val('');
    	  $('#edit-relation-type').val('');
    	  $('#edit-description').val('');
    	  return true;
    	}
    	else {
    		return false; 
      }
    },
    move_name: function(NODE,REF_NODE) {
    	var orig = this.get_id(NODE);
    	var dest = this.get_id(REF_NODE);
    	$.post("move_name/", {"child" : orig, "new_parent" : dest }, function(data) {EDIT.display_message(data);}, "json");
    },
    move_name_alternate: function(NODE,REF_NODE) {
    	var orig = this.get_id(NODE);
    	var dest = this.get_id(REF_NODE);
    	$.post("move_name_alternate/", {"child" : orig, "new_parent" : dest }, function(data) {EDIT.display_message(data);}, "json");
    },
    update_settings: function() {
    	var ranks = $('#edit-ranks-list').val();
    	var display_options = $("input[@name='display_options_editor']:checked").val();
    	$.post("update_settings/", { "ranks_list" : ranks, "display_options_editor" : display_options }, function(data) {EDIT.display_message(data);}, "json");
    	TREE.refresh();
    	TREE.close_all();
    },
    display_message: function(content) {
    	 var message_type = "";
    	 switch (content.status) {
    	 	  case "edited":
    	 	    message_type = "status";
    	 	    break;
    	 	  case "added":
    	 	    $('#n' + content.tid_parent + ' li:first-child').attr('id','n' + content.tid_child);
    	 	    message_type = "status";
    	 	    break;
    	 	  case "deleted":
    	 	    message_type = "error";
    	 	    break;
    	 	  case "moved":
    	 	    message_type = "status";
    	 	    break;
    	 	  case "updated":
    	 	    message_type = "status";
    	 	    break;
    	 	  case "failed":
    	 	    message_type = "error";
    	 	    break;
    	 	  default:
    	 	    message_type = "";
    	 }
	     $('#message').fadeIn("fast").html('<div class="messages ' + message_type + '">' + content.message + '</div>').fadeOut(3000);
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
//  	 $.post("import_status/", function(data) {EDIT.import_message(data);}, "json");
    },
    build_metadata: function(content) {
    	 data = content.data;
    	 tid = data.tid;
    	 parent_tid = data.parent_tid;
    	 taxon = data.taxon;
    	 rank = data.rank;
    	 desc = data.description;
    	 relation_id = parseInt(data.relation_id);
    	 relation_type = parseInt(data.relation_type);
    	 if (rank == null) { rank = ""; }
    	 if (isNaN(relation_type)) {
    	 	 relation_type = "";
    	 }
    	 /* synonyms and other names */
    	 else if (relation_type <= 8 || relation_type > 9) {
    	 	 taxon = "<span style='color:#FF0000'>" + taxon + "</span>";
    	 }
    	 /* vernaculars */
    	 else {
    	 	 taxon = "<span style='color:#228B22'>" + taxon + "</span>";
    	 }
    	 $('#edit-tid').val(tid);
    	 $('#edit-parent-tid').val(parent_tid);
    	 $('#metadata_taxon_title').html(taxon);
    	 $('#edit-rank').val(rank);
    	 $('#edit-relation-id').val(relation_id);
    	 $('#edit-relation-type').val(relation_type);
    	 $('#edit-description').val(desc);
	  },
    tab_selector: function(tab) {
        $('#tabs-wrapper ul li:nth-child(' + tab + ')').addClass('active');
        $('#tabs-wrapper ul li:not(:nth-child(' + tab + '))').removeClass('active');
        $('#alternate_wrapper div.alternate_content:nth-child(' + tab + ')').show();
        $('#alternate_wrapper div.alternate_content:not(:nth-child(' + tab + '))').hide();
    }
  };
}

$(document).ready(function() {
	var menu = $('#metadata_actions');
		$(menu).find('A').mouseover( function() {
	  $(menu).find('LI.hover').removeClass('hover');
	  $(this).parent().addClass('hover');
	  }).mouseout( function() {
		$(menu).find('LI.hover').removeClass('hover');
	  });
	$('#edit-root-save').appendTo('#edit-root-wrapper');
});