// JS for everything - nexus.js is just the nexus Obj

$(document).ready(function() {
 
 $('#nexus-dynamic-option-'+$('#edit-taxa-vid').val()).show();
 
 $('#edit-taxa-vid').change(function(){
   
   $('.nexus-dynamic-options').hide();
   $('#nexus-dynamic-option-'+$(this).val()).show('fast');
     
 }); 
  
});

