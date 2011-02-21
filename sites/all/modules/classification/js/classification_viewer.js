	$(function() {//
			TREE = new tree_component();
			var vid = Drupal.settings['classification_vid']['vid'];
			var tid = Drupal.settings['tid'];
			var ancestors = Drupal.settings['ancestry'];
			TREE.init($("#classification_tree_viewer"), {
				data	: { type : "json", async : true, url : Drupal.settings.classification_callback_jstree_viewer + vid + "/", json : false },
				dflt	: {
					value : tid,
					url   : false
				},
				ancestry: ancestors,
				cookies : false,
				root    : 0,
				path	: Drupal.settings.classification_module_path + "/jsTree/",
				ui		: {dots : true, rtl : false, animation : 10, hover_mode : true},
                lang    : {new_node : "Taxon", loading : "&nbsp;&nbsp;&nbsp;&nbsp;"},
				rules		: {
					type_attr	: "rel",
					createat	: "top",
					multitree	: false,
					metadata	: false,
					use_inline  : false,
					clickable	: "all",
					renameable  : "none",
					draggable	: "none",
					deletable	: "none",
					createable  : "none",
					dragrules	: "none"
				},
				callback	: {
                			beforechange    : function(NODE,TREE_OBJ) { return true; },
                			beforemove      : function(NODE,REF_NODE,TYPE,TREE_OBJ) { return true; },
                			beforecreate    : function(NODE,REF_NODE,TYPE,TREE_OBJ) { 
                				     return true;
                		    },
                			beforerename    : function(NODE,LANG,TREE_OBJ) { return true; },
                			beforedelete    : function(NODE,TREE_OBJ) { 
                			     	 return true;
                			}, 
					            beforechange	  : function(NODE,TREE_OBJ) { },
                			onchange        : function(NODE,TREE_OBJ) { },
                			onrename        : function(NODE,LANG,TREE_OBJ) {
                		    },
                			onmove          : function(NODE,REF_NODE,TYPE,TREE_OBJ) {
                			},
                			oncopy          : function(NODE,REF_NODE,TYPE,TREE_OBJ) { },
                			oncreate        : function(NODE,REF_NODE,TYPE,TREE_OBJ) { },
                			ondelete        : function(NODE, TREE_OBJ) { },
                			onopen          : function(NODE, TREE_OBJ) { },
                			onclose         : function(NODE, TREE_OBJ) { },
                			error           : function(TEXT, TREE_OBJ) { },
                			// onclk callback added by David Shorthouse to browse elsewhere
                			onclk           : function(NODE, TREE_OBJ) {
                				   var taxon = NODE.id.replace("n","");
                				   window.location.href = Drupal.settings.basePath + "pages/" + taxon;
                		    },
                			ondblclk        : function(NODE, TREE_OBJ) {
                			},
                			onrgtclk        : function(NODE, TREE_OBJ, EV) {
                      }
				}
			});
	});