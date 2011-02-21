(function ($) {

Drupal.behaviors.modalChild = function(context) {
	
  $('#edit-cancel, #publication-modal-node-delete-confirm a').click(function(){
    
    Drupal.modalFrameChild.triggerParentEvent('childClose');
    return false;
    
  });
  
}

})(jQuery);