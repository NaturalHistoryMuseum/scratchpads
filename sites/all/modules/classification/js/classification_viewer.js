	$(function() {
			TREE = new tree_component();
			var host = window.location.hostname;
			var vid = Drupal.settings['classification_vid']['vid'];
			TREE.init($("#classification_tree_viewer"), {
				data		: { type : "json", async : true, url : "classification/js_tree_viewer/" + vid + "/", json : false },
				dflt		: false,
				root    : 0,
				path		: "/sites/" + host + "/modules/classification/jsTree/",
				cookies : { prefix: "tree_viewer", expires: 7, path: "/" },
				ui		  : {dots : true, rtl : false, animation : 10, hover_mode : true},
        lang    : {new_node : "Taxon", loading : "&nbsp;&nbsp;&nbsp;&nbsp;"},
				rules		: {
					type_attr	: "rel",
					createat	: "top",
					multitree	: false,
					metadata	: false,
					use_inline: false,
					clickable	: "all",
					renameable: "none",
					draggable	: "none",
					deletable	: "none",
					createable: "none",
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
                				   var uri = parseUri(window.location.href);
                				   var taxon = NODE.id.replace("n","");
                				   
                				   if(uri.directoryPath.length > 0) {
                				   	  window.location.href = "http://" + uri.domain + uri.directoryPath.replace("front_page/","") + "pages/" + taxon;
                				   }
                				   else{
                				      window.location.href = "http://" + uri.domain + "pages/" + taxon;
                				   }
                		  },
                			ondblclk        : function(NODE, TREE_OBJ) {
                			},
                			onrgtclk        : function(NODE, TREE_OBJ, EV) {
                      }
				}
			});
	});
	
function parseUri(sourceUri){
    var uriPartNames = ["source","protocol","authority","domain","port","path","directoryPath","fileName","query","anchor"];
    var uriParts = new RegExp("^(?:([^:/?#.]+):)?(?://)?(([^:/?#]*)(?::(\\d*))?)?((/(?:[^?#](?![^?#/]*\\.[^?#/.]+(?:[\\?#]|$)))*/?)?([^?#/]*))?(?:\\?([^#]*))?(?:#(.*))?").exec(sourceUri);
    var uri = {};
    
    for(var i = 0; i < 10; i++){
        uri[uriPartNames[i]] = (uriParts[i] ? uriParts[i] : "");
    }
    
    // Always end directoryPath with a trailing backslash if a path was present in the source URI
    // Note that a trailing backslash is NOT automatically inserted within or appended to the "path" key
    if(uri.directoryPath.length > 0){
        uri.directoryPath = uri.directoryPath.replace(/\/?$/, "/");
    }
    
    return uri;
}