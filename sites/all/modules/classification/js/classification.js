	$(function() {
			TREE = new tree_component();
			EDIT = new classification_edit();
			host = window.location.hostname;
			TREE.init($("#classification_tree"), {
				data		: { type : "json", async : true, url : "js_tree/", json : false },
				dflt		: false,
				path		: "/sites/" + host + "/modules/classification/jsTree/",
				cookies : { prefix: "tree", expires: 7, path: "/" },
				ui		  : {dots : true, rtl : false, animation : 10, hover_mode : true},
        lang    : {new_node : "Taxon", loading : "&nbsp;&nbsp;&nbsp;&nbsp;"},
				rules		: {
					type_attr	: "rel",
					createat	: "top",
					multitree	: true,
					metadata	: false,
					use_inline: false,
					clickable	: "all",
					renameable: "all",
					draggable	: "all",
					deletable	: "all",
					createable: "all",
					dragrules	: "all"
				},
				callback	: {
                			beforechange    : function(NODE,TREE_OBJ) { return true; },
                			beforemove      : function(NODE,REF_NODE,TYPE,TREE_OBJ) { return true; },
                			beforecreate    : function(NODE,REF_NODE,TYPE,TREE_OBJ) { 
                				     EDIT.add_name(REF_NODE);
                				     return true;
                		  },
                			beforerename    : function(NODE,LANG,TREE_OBJ) { return true; },
                			beforedelete    : function(NODE,TREE_OBJ) { 
                			       var del = EDIT.delete_name(NODE);
                			       if(del) { return true; }
                			}, 
					            beforechange	  : function(NODE,TREE_OBJ) { },
                			onchange        : function(NODE,TREE_OBJ) { },
                			onrename        : function(NODE,LANG,TREE_OBJ) {
                				     EDIT.edit_name(NODE);
                		  },
                			onmove          : function(NODE,REF_NODE,TYPE,TREE_OBJ) {
                				     EDIT.move_name(NODE,REF_NODE,TYPE);
                			},
                			oncopy          : function(NODE,REF_NODE,TYPE,TREE_OBJ) { },
                			oncreate        : function(NODE,REF_NODE,TYPE,TREE_OBJ) { },
                			ondelete        : function(NODE, TREE_OBJ) { },
                			onopen          : function(NODE, TREE_OBJ) { },
                			onclose         : function(NODE, TREE_OBJ) { },
                			error           : function(TEXT, TREE_OBJ) { },
                			// onclk callback added by David Shorthouse
                			onclk           : function(NODE, TREE_OBJ) {
                				     var form_build_id = $("input[name=form_build_id]").val();
                				     EDIT.get_metadata(NODE,form_build_id);
                				     EDIT.tab_selector(1);
                		  },
                			ondblclk        : function(NODE, TREE_OBJ) {
						                 TREE_OBJ.rename.call(TREE_OBJ,NODE);
                			},
                			onrgtclk        : function(NODE, TREE_OBJ, EV) { }
				}
			});

			$.hotkeys.add('f2',		{ disableInInput: true },	function() { TREE.rename(); });
			$.hotkeys.add('c',    { disableInInput: true }, function() { TREE.close_all(); });
			$.hotkeys.add('n',		{ disableInInput: true },	function() { TREE.create(); });
			$.hotkeys.add('r',		{ disableInInput: true },	function() { TREE.refresh(); });
			$.hotkeys.add('up',		{ disableInInput: true },	function() { TREE.get_prev(); });
			$.hotkeys.add('down',	{ disableInInput: true },	function() { TREE.get_next(); });
			$.hotkeys.add('left',	{ disableInInput: true },	function() { TREE.get_left(); });
			$.hotkeys.add('right',{ disableInInput: true },	function() { TREE.get_right(); });
	});