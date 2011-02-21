// JS for everything - nexus.js is just the nexus Obj

$(document).ready(function() {
 
 var select = $("#node-form select[name='taxa_vid']");
 
 $('#nexus-dynamic-option-'+select.val()).show();
 
 $(select).change(function(){
   
   $('.nexus-dynamic-options').hide();
   $('#nexus-dynamic-option-'+$(this).val()).show('fast');
     
 }); 
  
});

