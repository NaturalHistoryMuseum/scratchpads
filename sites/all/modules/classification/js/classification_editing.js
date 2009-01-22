function classification_edit() {
  return {
    // Functions to coordinate editing
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
    check_vern: function(val) {
    	if(val==9) {
    		$('#edit-vern-lang-wrapper').show();
    	}
    	else {
    		$('#edit-vern-lang-wrapper').hide();
    	}
    },
    get_tree: function(vid) {
    	$('.classification_trees').hide();
    	if(TREE[vid] || vid == 0) {
    		$('#classification_tree_' + vid).show();
    		return;
    	}
    	var _tree = '<div id="classification_tree_' + vid + '" class="classification_trees"></div>';
      $('#classification_tree_alternate').append(_tree);
      $('#classification_tree_' + vid).show();
    	TREE[vid] = new tree_component();
    	host = window.location.hostname;
			TREE[vid].init($("#classification_tree_" + vid), {
			  data		: { type : "json", async : true, url : "js_tree2/" + vid + "/", json : false },
				dflt		: false,
				path		: "/sites/" + host + "/modules/classification/jsTree/",
				cookies : { prefix: "tree_" + vid, expires: 7, path: "/" },
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
					drag_copy : "on",
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
                			onmove          : function(NODE,REF_NODE,TYPE,TREE_OBJ) { },
                			oncopy          : function(NODE,REF_NODE,TYPE,TREE_OBJ) { 
                				     EDIT.copy_name_alternate(NODE,REF_NODE);
                			},
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
    open_help: function(shortname) {
    	var base_url = window.location.host;
    	if (shortname) {shortname = '/' + shortname};
    	window.open('http://' + base_url + shortname + '/help/classification/edit?popup=1', 'advanced_help_window', 'width=500,height=500,scrollbars,resizable');
    },
    go_url: function(type) {
    	var tid = $('#edit-tid').val();
      var uri = this.parseUri(window.location.href);
      
      switch(type) {
      	case 'image':
      	  var _page = 'gallery';
      	  break;
      	case 'taxon':
      	  var _page = 'pages';
      	  break;
      	case 'biblio':
      	  var _page = 'biblio';
      	  break;
      }
                				   
      if(uri.directoryPath.length > 0) {
         window.location.href = "http://" + uri.domain + uri.directoryPath.replace("/classification/edit","") + _page + "/" + tid;
      }
      else{
         window.location.href = "http://" + uri.domain + _page + "/" + tid;
      }
    },
    parseUri: function(sourceUri){
       var uriPartNames = ["source","protocol","authority","domain","port","path","directoryPath","fileName","query","anchor"];
       var uriParts = new RegExp("^(?:([^:/?#.]+):)?(?://)?(([^:/?#]*)(?::(\\d*))?)?((/(?:[^?#](?![^?#/]*\\.[^?#/.]+(?:[\\?#]|$)))*/?)?([^?#/]*))?(?:\\?([^#]*))?(?:#(.*))?").exec(sourceUri);
       var uri = {};
    
       for(var i = 0; i < 10; i++){
          uri[uriPartNames[i]] = (uriParts[i] ? uriParts[i] : "");
       }

      if(uri.directoryPath.length > 0){
        uri.directoryPath = uri.directoryPath.replace(/\/?$/, "/");
      }
    
      return uri;
    },
    save_metadata: function() {
    	var tid = $('#edit-tid').val();
    	var parent_tid =	$('#edit-parent-tid').val();
    	var rank = $('#edit-rank').val();
    	var desc = $('#edit-description').val();
    	var relation_id = $('#edit-relation-id').val();
    	var relation_type = $('#edit-relation-type option:selected').val();
    	var vern_lang = $('#edit-vern-lang option:selected').val();
    	var form_build_id = $("input[name=form_build_id]").val();
    	if(tid) {
    	   $.post("update_metadata/", { "tid" : tid, "parent" : parent_tid, "rank" : rank, "description" : desc, "relation_id" : relation_id, "relation_type" : relation_type, "vern_lang" : vern_lang }, function(data) {EDIT.display_message(data);}, "json");
    	   if( parseInt(relation_type) >= 1 || parseInt(relation_id) > 0 ) {
    		    TREE.refresh();
    	   }
    	   $.post("get_metadata/", { "tid" : tid, "form_build_id" : form_build_id }, function(data) {EDIT.build_metadata(data);}, "json");
    	}
    },
  	edit_name: function(NODE) {
  		var tid = this.get_id(NODE);
      var newname = this.get_content(NODE);
      var form_build_id = $("input[name=form_build_id]").val();
      $.post("edit_name/", { "tid" : tid, "value" : newname }, function(data) {EDIT.display_message(data);}, "json");
      this.get_metadata(NODE,form_build_id);
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
    	  $('#edit-vern-lang-wrapper').hide();
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
    move_name: function(NODE,REF_NODE,TYPE) {
    	var orig = this.get_id(NODE);
    	var dest = this.get_id(REF_NODE);
    	switch(TYPE) {
    		case "after":
    		   //must use parent of REF_NODE.id as new parent, so we need another callback to get it & then perform the move
    		   $.post("get_parent/", {"child" : orig, "child_parent" : dest}, function(data) {EDIT.move_name_2(data);}, "json");
    	     break;
    	   case "before":
    	     //must use parent of REF_NODE.id as new parent, so we need another callback to get it and then perform the move
    	     $.post("get_parent/", {"child" : orig, "child_parent" : dest}, function(data) {EDIT.move_name_2(data);}, "json");
    	     break;
    	   case "inside":
    	     //may use REF_NODE.id as new parent
    	     $.post("move_name/", {"child" : orig, "new_parent" : dest }, function(data) {EDIT.display_message(data);}, "json");
    	     break;
    	}
    },
    move_name_2: function(content) {
    	var orig = content.child;
    	var dest = content.dest;
    	$.post("move_name/", {"child" : orig, "new_parent" : dest }, function(data) {EDIT.display_message(data);}, "json");
    },
    copy_name_alternate: function(NODE,REF_NODE) {
    	var orig = this.get_id(NODE);
    	var dest = this.get_id(REF_NODE);
    	$.post("copy_name_alternate/", {"child" : orig, "new_parent" : dest }, function(data) {EDIT.display_message(data);}, "json");
    },
    update_settings: function() {
    	var ranks = $('#edit-ranks-list').val();
    	var verns = $('#edit-vern-list').val();
    	var display_options = $("input[@name='display_options_editor']:checked").val();
    	$.post("update_settings/", { "ranks_list" : ranks, "vern_list" : verns, "display_options_editor" : display_options }, function(data) {EDIT.display_message(data);}, "json");
    	TREE.refresh();
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
	     $('#message').fadeIn("fast").html('<div class="messages ' + message_type + '">' + content.message + '</div>').fadeOut(4000);
    },
    build_metadata: function(content) {
    	 data = content.data;
    	 tid = data.tid;
    	 parent_tid = data.parent_tid;
    	 taxon = data.taxon;
    	 rank = data.rank;
    	 vern_lang = data.vern_lang;
    	 desc = data.description;
    	 relation_id = parseInt(data.relation_id);
    	 relation_type = parseInt(data.relation_type);
    	 media = data.media;
    	    image_count = media.image;
    	    reference_count = media.biblio;
    	    taxon_description = media.taxon_description;
    	    
    	 if (rank == null) { rank = ""; }
    	 if (vern_lang == null) { 
    	 	  vern_lang = "";
    	 	  $('#edit-vern-lang-wrapper').hide();
    	 }
    	 else {
    	 	  $('#edit-vern-lang-wrapper').show();
    	 }
    	 if (isNaN(relation_type)) {
    	 	 relation_type = "";
    	 }
    	 if (isNaN(relation_id)) {
    	 	 relation_id = "";
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
    	 $('#edit-vern-lang').val(vern_lang);
    	 $('#edit-description').val(desc);
    	 
    	 if(parent_tid == 0) {
    	 	$('#edit-relation-type').attr("disabled",true);
    	 }
    	 else {
    	 	$('#edit-relation-type').attr("disabled",false);
    	 }
    	 
    	 // media items
    	 var $metadata = $('#metadata_actions li');
    	 
    	 if (image_count > 0) {
    	 	$metadata.eq(0).removeClass('classification_images');
    	 	$metadata.eq(0).addClass('classification_images_enabled');
    	 	$metadata.eq(0).children().children('span').html('(' + image_count + ')');
    	 }
    	 else {
    	 	$metadata.eq(0).removeClass('classification_images_enabled');
    	 	$metadata.eq(0).addClass('classification_images');
    	 	$metadata.eq(0).children().children('span').html('');
    	 }
    	 if (reference_count > 0) {
    	 	$metadata.eq(1).removeClass('classification_references');
    	 	$metadata.eq(1).addClass('classification_references_enabled');
    	 	$metadata.eq(1).children().children('span').html('(' + reference_count + ')');
    	 }
    	 else {
    	 	$metadata.eq(1).removeClass('classification_references_enabled');
    	 	$metadata.eq(1).addClass('classification_references');
    	 	$metadata.eq(1).children().children('span').html('');
    	 }
    	 if (taxon_description > 0) {
    	 	$metadata.eq(2).removeClass('classification_view');
    	 	$metadata.eq(2).addClass('classification_view_enabled');
    	 }
    	 else {
    	 	$metadata.eq(2).removeClass('classification_view_enabled');
    	 	$metadata.eq(2).addClass('classification_view');
    	 } 	 
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
	$('#message').vCenter();
});