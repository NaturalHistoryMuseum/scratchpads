	$(function() {
		    //relocate the tree
		    var tree_html = $('.classification_tree_panel');
		    $('.node-form').parent().css("position","relative").append(tree_html);
		    //curvecorners settings
		    $('.c').corner();
			$('.inner').corner({
			  tl: { radius: 6 },
			  tr: { radius: 6 },
			  bl: { radius: 6 },
			  br: { radius: 6 }
			});
            $('.expand').show();
		    $('.expand').click(function() {
		      $('.classification_tree_div').animate({
		        "width": "400px"
		      },1500);
		      $('#classification_tree_viewer').animate({
		       "height" : "500px"
		      },1500);
		      $(this).hide();
		      $('.contract').show();
		    });
		    $('.contract').click(function() {
		      $('.classification_tree_div').animate({
		       "width": "230px"
		      },500);
		      $('#classification_tree_viewer').animate({
		       "height" : "260px"
		      },1500);
		      $(this).hide();
		      $('.expand').show();
		    });
		
			TREE = new tree_component();
			var vid = Drupal.settings['classification_vid']['vid'];
			var nid = Drupal.settings['nid'];
			var _url = (!nid) ? false : Drupal.settings.basePath + "classification/js_tree_checked/" + vid + "/" + nid;
			var ancestors = Drupal.settings['ancestry'];
			var checked = Drupal.settings['checked'];
			TREE.init($("#classification_tree_viewer"), {
				data    : { type : "json", async : true, url : Drupal.settings.classification_callback_jstree_elements + vid + "/", json : false },
				dflt    : false,
				ancestry: ancestors, 
				checked : { url : _url, data : checked },
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
					        beforechange	: function(NODE,TREE_OBJ) { },
                			onchange        : function(NODE,TREE_OBJ) { },
                			onrename        : function(NODE,LANG,TREE_OBJ) { },
                			onmove          : function(NODE,REF_NODE,TYPE,TREE_OBJ) { },
                			oncopy          : function(NODE,REF_NODE,TYPE,TREE_OBJ) { },
                			oncreate        : function(NODE,REF_NODE,TYPE,TREE_OBJ) { },
                			ondelete        : function(NODE, TREE_OBJ) { },
                			onopen          : function(NODE, TREE_OBJ) { },
                			onclose         : function(NODE, TREE_OBJ) { },
                			error           : function(TEXT, TREE_OBJ) { },
                			onclk           : function(NODE, TREE_OBJ) { },
                			ondblclk        : function(NODE, TREE_OBJ) { },
                			onrgtclk        : function(NODE, TREE_OBJ, EV) { }
				}
			});
	});