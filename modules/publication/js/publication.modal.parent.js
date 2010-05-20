(function ($) {

Drupal.behaviors.modalNodeForm = function(context) {

  $('a.modal:not(.modalframe-processed)', context).addClass('modalframe-processed').unbind('click').click(function(){
    
    var element = this;
    var $statusMessagesDiv = $(element).parents('div.publication-field').find('div.status-messages');

    // Called when a caption image is added / edited or deleted
    function onSubmitCallback(args, statusMessages) {
      // Display status messages generated during submit processing.
      if (statusMessages) {
        $statusMessagesDiv.hide().html(statusMessages).show('fast');
      }
      
      if(typeof(args) != 'undefined'){

        switch(args.type)
        {
         
         case 'publication_section':
         
          if(args.op == 'Delete'){
           
           publication.nodeDeleted(args.type, args.nid);
           
           $('tr.draggable-section-'+args.nid).remove();
           
           // Tidy up orphaned elements
           $('#draggable-section input[value='+args.nid+'].parent-group').val(0).parents('tr').find('div.indentation').remove();
           
            if(!Drupal.tableDrag['publication-draggable-section'].changed){
              Drupal.tableDrag['publication-draggable-section'].changed = true;
              publication.insertChangedWarning($('#publication-draggable-section'));
            }
           
          }else if(args.op == 'Updated' || args.op == 'Reset'){   
           
             $titleSpan = $('tr.draggable-section-'+args.nid+' span.title');
             $titleSpan.html(args.title);
             
             if(args.op == 'Updated' && args.original_title){

               $titleSpan.addClass('manuscript-name');
               $titleSpan.attr('title', args.original_title);
               
             }else{
               
               $titleSpan.removeClass('manuscript-name');
               $titleSpan.removeAttr('title');
               
             }
            
          }else if(args.op == 'Created'){

            var tr = $(args.output).find('tr.draggable');
        
            $('td', tr).eq(1).hide();
            Drupal.tableDrag['publication-draggable-section'].makeDraggable(tr.get(0));        
            tr.appendTo('#publication-draggable-section');
            Drupal.attachBehaviors(tr);
            
          } 
          
         break;

         case 'publication_image_caption':

           if(args.op == 'Delete'){
       
             publication.nodeDeleted(args.type, args.nid);
             
             $(element).parent().removeClass('has-caption')
       
           }else if(args.op == 'Created' || args.op == 'Updated'){
       
             $(element).parent().addClass('has-caption')
       
           }
         
         break;
         
         case 'publication_taxon_treatment':

          $titleSpan = $('tr.draggable-section-'+args.nid+' span.title');

           if(args.op == 'Reset'){
       
             $titleSpan.removeClass('term-field-override');
       
           }else{
             
              $titleSpan.addClass('term-field-override');
             
           }
         
         break;                  
          
        }
        
      }

    } // End of onSubmitCallback()
    

    // Build modal frame options.
    var modalOptions = {
      url: $(element).attr('href') + '&modal=true&publication_nid=' + publication.getNID() + '&publication_vid=' + publication.getVID() + '&publication_form_build_id=' + publication.getBuildID(),
      autoFit: true,
      onSubmit: onSubmitCallback,
      width: 500,
      height: 400
    };

    // Open the modal frame dialog.
    Drupal.modalFrame.open(modalOptions);
    
    return false;
    
  });
  
}

})(jQuery);

