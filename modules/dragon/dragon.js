Drupal.behaviors.dragon = function(context){
  Drupal.dragon.init(context);
};
Drupal.dragon = new Object;
Drupal.dragon.original_form = false;
Drupal.dragon.original_drop_event = false;
Drupal.dragon.upload_path = false;
Drupal.dragon.field_name = false;
Drupal.dragon.wrapper = false;
Drupal.dragon.id_parts = false;

Drupal.dragon.init = function(context) {
  // Register listeners.
  var dragon = $('.dragon').get(0);
  if(dragon){
    dragons = $.each($('.dragon').get(), function(indexInArray, valueOfElement){
      valueOfElement.addEventListener('drop', Drupal.dragon.upload, false);
    });
    $('body').get(0).addEventListener('dragenter', function(event){
      $('.dragon').animate({height:'100px',width:'99%'}, 1000);
      $('.dragon p').css({color:'#000'});
      $('.dragon').css({backgroundColor : '#f5f5b5'});
    }, false);
    $('body').get(0).addEventListener('dragexit', function(event){
      if(event.clientX == 0 && event.clientY == 0){
        $('.dragon').animate({height:'50px',width:'200px'}, 1000);
        $('.dragon p').css({color:'#ccc'});
        $('.dragon').css({backgroundColor : '#fff'});
      }
    }, false);
    $('body').get(0).addEventListener('dragover', function(event){ 
      event.preventDefault(); 
    }, false);
  }
}

Drupal.dragon.single_file_upload = function(file_num){
  var boundary = '------multipartformboundary' + (new Date).getTime();
  var dashdash = '--';
  var crlf     = '\r\n';
  
  if(file_num == Drupal.dragon.original_drop_event.dataTransfer.files.length){
    return;
  }
  var other_form_bits = $(Drupal.dragon.original_form).serializeArray();
  /* Build RFC2388 string. */
  var builder = dashdash + boundary + crlf;
  var xhr = new XMLHttpRequest();
  for(var j = 0; j < other_form_bits.length; j++){
    builder += 'Content-Disposition: form-data; name="'+other_form_bits[j]['name']+'"' + crlf + crlf + other_form_bits[j]['value'] + crlf + dashdash + boundary + crlf;
  }
  var file = Drupal.dragon.original_drop_event.dataTransfer.files[file_num];
  /* Generate headers. */
  temp_field_name = Drupal.dragon.field_name;
  temp_wrapper = Drupal.dragon.wrapper;
  temp_upload_path = Drupal.dragon.upload_path;
  field_parts = Drupal.dragon.field_name.split('_');  
  if(field_parts[field_parts.length-1].substring(0, field_parts[field_parts.length-1].length-1) == parseInt(field_parts[field_parts.length-1].substring(0, field_parts[field_parts.length-1].length-1))){
    temp_field_name = temp_field_name.replace('_'+field_parts[field_parts.length-1], '_'+(parseInt(field_parts[field_parts.length-1].substring(0, field_parts[field_parts.length-1].length-1))+parseInt(file_num))+']');
    temp_upload_path = temp_upload_path.replace('/'+field_parts[field_parts.length-1].substring(0, field_parts[field_parts.length-1].length-1), '/'+(parseInt(field_parts[field_parts.length-1].substring(0, field_parts[field_parts.length-1].length-1))+parseInt(file_num)));
    temp_wrapper = temp_wrapper.replace('-'+field_parts[field_parts.length-1].substring(0, field_parts[field_parts.length-1].length-1)+'-', '-'+(parseInt(field_parts[field_parts.length-1].substring(0, field_parts[field_parts.length-1].length-1))+parseInt(file_num))+'-');
    if(!$('#'+temp_wrapper).html()){
      // We haven't got this form item actually on the page, we need to mimic
      // the pressing of the "Add another item" button
      var shah = new Drupal.shah(Drupal.dragon.id_parts[1], Drupal.settings.ahah[Drupal.dragon.id_parts[1]]);
    }
  }
  builder += 'Content-Disposition: form-data; name="'+temp_field_name+'"';
  if (file.fileName) {
    builder += '; filename="' + file.fileName + '"';
  }
  builder += crlf + 'Content-Type: application/octet-stream' + crlf + crlf + file.getAsBinary() + crlf + dashdash + boundary + crlf + dashdash + boundary + dashdash + crlf;
  xhr.open("POST", temp_upload_path, true);
  xhr.setRequestHeader('content-type', 'multipart/form-data; boundary=' + boundary);
  xhr.sendAsBinary(builder);
  xhr.onload = function(event){
    if (xhr.responseText){
      var response = eval('('+ xhr.responseText +')');
      if(response['status']){
        $('#'+temp_wrapper).html(response['data']);
        $.each(Drupal.behaviors, function(){
          this();
        });
        file_num ++;
        Drupal.dragon.single_file_upload(file_num);
      }
    }
  };  
}
Drupal.dragon.upload = function(event) {
  $('.dragon').animate({height:'50px',width:'200px'}, 1000);
  $('.dragon p').css({color:'#ccc'});
  $('.dragon').css({backgroundColor : '#fff'});
  Drupal.dragon.original_drop_event = event;
  Drupal.dragon.original_form = $(event.currentTarget).parents('form');
  Drupal.dragon.id_parts = event.currentTarget.id.split('__dragon__');
  Drupal.dragon.upload_path = Drupal.settings.ahah[Drupal.dragon.id_parts[0]].url;
  Drupal.dragon.field_name = $('#'+Drupal.settings.ahah[Drupal.dragon.id_parts[0]].wrapper+" input[type=file]").attr('name');
  Drupal.dragon.wrapper = Drupal.settings.ahah[Drupal.dragon.id_parts[0]].wrapper;
  Drupal.dragon.single_file_upload(0);
  event.stopPropagation();
}