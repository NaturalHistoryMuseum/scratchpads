
function treeview(){
  
  var self;
  var elements = {};
  
  return {//
    
    init: function(id, vid, selected, noTree) {
  
      self = this;
      
      elements[id] = {
        vid: vid,
        selected: selected       
      }
      
      if(!noTree){
        
        self.displayTree(id);
        
      }
      
    },
    
    displayTree: function(id){

      var element = elements[id];

      $("#"+id).treeview({ 
         url: Drupal.settings.taxonomyTreeCallbackPath+'/'+element['vid']+'?'+element['selected'],
         persist: "cookie"
      });
      
      $("#"+id).ajaxSuccess(function(){
        self.updateEvents(id)
      });
      
    },
    
    updateEvents: function(element_id){
      
       // Prevent cleciking the checkbox from expanding the tree
      $("#"+element_id+" input").unbind('mousedown');
      $("#"+element_id+" input").bind('mousedown', function(){

        $(this).attr('checked', ($(this).attr('checked') ? 0 : 1)); 

      });

      $("#"+element_id+" input").click(function(){

        // Change the ID so child items are checked same as parent
        var id = $(this).parents('li').attr('id');
        var split_id = [];
        split_id = id.split('-');
        $(this).parents('li').attr('id',split_id[0]+'-'+($(this).attr('checked') ? 0 : 1));
        return false;

      });
      
      
    }
    
  }
  
}




