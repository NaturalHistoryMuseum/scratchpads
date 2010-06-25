$(document).ready(function(){
  // Register listeners.
  var dragon = $('.dragon').get(0);
  if(dragon){
    dragons = $.each($('.dragon').get(), function(indexInArray, valueOfElement){
      valueOfElement.addEventListener('drop', upload, false);
    });
    $('body').get(0).addEventListener('dragenter', function(event){
      $('.dragon').fadeIn(500);
    }, false);
    $('body').get(0).addEventListener('dragexit', function(event){
      if(event.clientX == 0 && event.clientY == 0){
        $('.dragon').fadeOut(500);
      }
    }, false);
    $('body').get(0).addEventListener('dragover', function(event){ 
      event.preventDefault(); 
    }, false);
  }
  
  // Upload function.
  function upload(event) {
    var other_form_bits = $(event.currentTarget).parents('form').serializeArray();
    var data = event.dataTransfer;
    /* Show spinner for each dropped file. 
    
    for (var i = 0; i < data.files.length; i++) {
      alert('file');
      $('#dropzone').append($('<img src="spinner.gif" width="16" height="16" />').css("padding", "48px"));
    }
    */
  
    var boundary = '------multipartformboundary' + (new Date).getTime();
    var dashdash = '--';
    var crlf     = '\r\n';
    
    /* Build RFC2388 string. */
    var builder = '';
    
    builder += dashdash;
    builder += boundary;
    builder += crlf;
    
    var xhr = new XMLHttpRequest();
    
    for(var i = 0; i < other_form_bits.length; i++){
    
      /* Generate headers. */            
      builder += 'Content-Disposition: form-data; name="'+other_form_bits[i]['name']+'"';
      builder += crlf;
      builder += crlf;
      
      // Data
      builder += other_form_bits[i]['value'];
      builder += crlf;
      
      /* Write boundary. */
      builder += dashdash;
      builder += boundary;
      builder += crlf;
      
    }
    
    for (var i = 0; i < data.files.length; i++) {
      var file = data.files[i];
    
      /* Generate headers. */            
      builder += 'Content-Disposition: form-data; name="files[upload]"';
      if (file.fileName) {
        builder += '; filename="' + file.fileName + '"';
      }
      builder += crlf;
    
      builder += 'Content-Type: application/octet-stream';
      builder += crlf;
      builder += crlf; 
      
      /* Append binary data. */
      builder += file.getAsBinary();
      builder += crlf;
      
      /* Write boundary. */
      builder += dashdash;
      builder += boundary;
      builder += crlf;
    }
    
    /* Mark end of the request. */
    builder += dashdash;
    builder += boundary;
    builder += dashdash;
    builder += crlf;
    
    xhr.open("POST", Drupal.settings.dragon.callbacks.upload, true);
    xhr.setRequestHeader('content-type', 'multipart/form-data; boundary=' + boundary);
    xhr.sendAsBinary(builder);        
    
    xhr.onload = function(event) { 
    /* If we got an error display it. */
    if (xhr.responseText) {
      alert(xhr.responseText);
    }
      $(".dragon").load(Drupal.settings.dragon.callbacks.upload);
    };
    
    /* Prevent FireFox opening the dragged file. */
    event.stopPropagation();
            
  }
});
