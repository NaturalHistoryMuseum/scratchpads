Drupal.behaviors.dragon = function(context){
  Drupal.dragon.init(context);
};

Drupal.dragon = new Object;
Drupal.dragon.original_form = false;
Drupal.dragon.original_drop_event = false;

Drupal.dragon.init = function(context) {
  // Register listeners.
  var dragon = $('.dragon').get(0);
  if(dragon){
    dragons = $.each($('.dragon').get(), function(indexInArray, valueOfElement){
      valueOfElement.addEventListener('drop', Drupal.dragon.upload, false);
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
}

Drupal.dragon.single_file_upload = function(file_num){
  if(file_num == Drupal.dragon.original_drop_event.dataTransfer.files.length){
    return;
  }
  var other_form_bits = $(Drupal.dragon.original_form).serializeArray();
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
  
  for(var j = 0; j < other_form_bits.length; j++){
  
    /* Generate headers. */            
    builder += 'Content-Disposition: form-data; name="'+other_form_bits[j]['name']+'"';
    builder += crlf;
    builder += crlf;
    
    // Data
    builder += other_form_bits[j]['value'];
    builder += crlf;
    
    /* Write boundary. */
    builder += dashdash;
    builder += boundary;
    builder += crlf;
    
  }
  
  var file = Drupal.dragon.original_drop_event.dataTransfer.files[file_num];

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
  
  /* Mark end of the request. */
  builder += dashdash;
  builder += boundary;
  builder += dashdash;
  builder += crlf;
  
  xhr.open("POST", Drupal.settings.dragon.callbacks.upload, true);
  xhr.setRequestHeader('content-type', 'multipart/form-data; boundary=' + boundary);
  xhr.sendAsBinary(builder);        
  
  xhr.onload = function(event) {
    if (xhr.responseText) {
      var response = eval('('+ xhr.responseText +')');
      if(response['status']){
        $('#attach-wrapper').html(response['data']);
        $.each(Drupal.behaviors, function() {
          this();
        });
        file_num ++;
        Drupal.dragon.single_file_upload(file_num);
      }
    }
  };  
}

Drupal.dragon.upload = function(event) {
  Drupal.dragon.original_drop_event = event;
  Drupal.dragon.original_form = $(event.currentTarget).parents('form')
  Drupal.dragon.single_file_upload(0);     
  /* Prevent FireFox opening the dragged file. */
  event.stopPropagation();
}