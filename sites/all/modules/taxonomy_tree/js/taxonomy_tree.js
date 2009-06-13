Drupal.behaviors.taxonomyTree = function(context) {

  $(context).find('.taxonomy-tree-field ul').each(function(){
  
    // getTaxonomyTree();
    var vid = $(this).attr('id');
  
    var url = Drupal.settings.taxonomyTreeCallbackPath+'/'+vid;
    
    var nid = $(context).find("input[name='taxonomy_tree_nid["+vid+"]']").val();
    var selected = $(context).find("input[name='taxonomy_tree_selected["+vid+"]']").val();
        
    if(typeof nid != 'undefined'){
      
      url += '/'+nid;
      
    }
    
    if(selected){
    
      url += '?t='+selected;
    
    }
    
    $(this).treeview({ 
       url: url,
       persist: "location",
    });
    
    // updateEvents($(this));
    
    $(this).ajaxSuccess(function(){
      updateEvents($(this))
    });
  
  });
  


}

updateEvents = function($context){
  
   // Prevent clicking the checkbox from expanding the tree
   
  $context.find('input').each(function(){
    
    $(this).unbind('mousedown');
    $(this).bind('mousedown', function(){
    
      $(this).attr('checked', ($(this).attr('checked') ? 0 : 1)); 
    
    });
    
    if($(this).is(':checked')){
      
      markAsSelected($(this), 1);
      
    }
    
    $(this).click(function(){
    
      self.markAsSelected($(this), ($(this).attr('checked') ? 0 : 1));
      return false;
    
    });
    
  })

}

markAsSelected = function($obj, checked){
  
  var $li = $obj.parents('li');
  var id = $li.attr('id');
  
  var split_id = [];
  split_id = id.split('-');
  
  $li.attr('id',split_id[0]+'-'+checked);
  
};








