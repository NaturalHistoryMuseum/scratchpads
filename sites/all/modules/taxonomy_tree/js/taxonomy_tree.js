
function treeview(){
  
  var self;
  
  return {//
    
    init: function(id, vid, selected) {
  
      self = this;

      $("#"+id).treeview({ 
         url: Drupal.settings.taxonomyTreeCallbackPath+'/'+vid+'?'+selected,
         persist: "cookie"
      });
      
      $("#"+id).ajaxSuccess(function(){
        self.updateEvents(id)
      });
      
    },
    
    updateEvents: function(id){
      
       // Prevent cleciking the checkbox from expanding the tree
      $("#"+id+" input").unbind('mousedown');
      $("#"+id+" input").bind('mousedown', function(){

        $(this).attr('checked', ($(this).attr('checked') ? 0 : 1)); 

      });

      $("#"+id+" input").click(function(){

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




