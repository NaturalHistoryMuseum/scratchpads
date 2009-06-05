
function treeview(){
  
  var self;
  var elements = {};
  
  return {//
    
    init: function(id, vid, nid, selected, noTree) {
  
      self = this;
      
      elements[id] = {
        vid: vid,
        nid: nid,
        selected: selected       
      }
      
      if(!noTree){
        
        self.displayTree(id);
        
      }
      
    },
    
    displayTree: function(id){

      var element = elements[id];

      $("#"+id).treeview({ 
         url: Drupal.settings.taxonomyTreeCallbackPath+'/'+element['vid']+'?nid='+element['nid']+'&'+element['selected'],
         persist: "cookie"
      });
      
      $("#"+id).ajaxSuccess(function(){
        self.updateEvents(id)
      });
      
    },
    
    markAsSelected: function($obj, checked){
      
      var $li = $obj.parents('li');
      var id = $li.attr('id');
      
      var split_id = [];
      split_id = id.split('-');
      
      $li.attr('id',split_id[0]+'-'+checked);
      
    },
    
    updateEvents: function(element_id){
      
       // Prevent clicking the checkbox from expanding the tree
      $("#"+element_id+" input").unbind('mousedown');
      $("#"+element_id+" input").bind('mousedown', function(){

        $(this).attr('checked', ($(this).attr('checked') ? 0 : 1)); 

      });
      
      $("#"+element_id+" input:checked").each(function(){
        
        self.markAsSelected($(this), 1);
        
      });

      $("#"+element_id+" input").click(function(){

        self.markAsSelected($(this), ($(this).attr('checked') ? 0 : 1));
        return false;

      });
      
      
    }
    
  }
  
}




