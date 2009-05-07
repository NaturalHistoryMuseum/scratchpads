
function initTreeview(id, vid, selected) {
  
  var url = Drupal.settings.taxonomyTreeCallbackPath+'/'+vid+selected;

  $("#"+id).treeview({ 
     url: url
  });
    
  $("#"+id).ajaxSuccess(function(evt, request, settings){
   
     $("#"+id+" input").unbind('mousedown');
     $("#"+id+" input").bind('mousedown', function(){
          
       $(this).attr('checked', ($(this).attr('checked') ? 0 : 1)); 

     });
     
     $("#"+id+" input").click(function(){

       return false;

     });
     
     $("#"+id+" input").click(function(){

       var id = $(this).parents('li').attr('id');
       var split_id = [];
       split_id = id.split('-');
       $(this).parents('li').attr('id',split_id[0]+'-'+($(this).attr('checked') ? 0 : 1));

     });
     
     
   });

}


