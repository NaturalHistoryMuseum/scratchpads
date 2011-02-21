Drupal.behaviors.dragon = function(context){
  Drupal.dragon.init(context);
};
Drupal.dragon = new Object;
Drupal.dragon.original_form = false;
Drupal.dragon.original_drop_event = false;
Drupal.dragon.upload_path = false;
Drupal.dragon.field_name = false;
Drupal.dragon.field_name_array = new Array();
Drupal.dragon.file_num_array = new Array();
Drupal.dragon.wrapper = false;
Drupal.dragon.id_parts = false;
Drupal.dragon.original_drop_box_id = false;

Drupal.dragon.init = function(context) {
  // Register listeners.
  var dragon = $('.dragon').get(0);
  if(dragon){
    dragons = $.each($('.dragon').get(), function(indexInArray, valueOfElement){
      valueOfElement.addEventListener('drop', Drupal.dragon.upload, false);
    });
    $('body').get(0).addEventListener('dragenter', function(event){
      $('.dragon').css({backgroundColor : '#f5f5b5'});
      $('.dragon').css({border : 'solid 1px #ccc'});
    }, false);
    $('body').get(0).addEventListener('dragexit', function(event){
      if(event.clientX == 0 && event.clientY == 0){
        $('.dragon').css({backgroundColor : '#fff'});
        $('.dragon').css({border : 'none'});
      }
    }, false);
    $('body').get(0).addEventListener('dragover', function(event){ 
      event.preventDefault(); 
    }, false);
  }
}

Drupal.dragon.single_file_upload = function(file_num, offset){
  var boundary = '------multipartformboundary' + (new Date).getTime();
  var dashdash = '--';
  var crlf     = '\r\n';
  
  if(file_num - offset == Drupal.dragon.original_drop_event.dataTransfer.files.length){
    $('.dragon').css({backgroundColor : '#fff'});
    $('.dragon').css({border : 'none'});
  }
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
      Drupal.shah_create(Drupal.dragon.id_parts[1]);
      $('#'+Drupal.dragon.id_parts[1]).trigger('mousedown');
    }
  }
  var other_form_bits = $(Drupal.dragon.original_form).serializeArray();
  /* Build RFC2388 string. */
  var builder = dashdash + boundary + crlf;
  var xhr = new XMLHttpRequest();
  for(var j = 0; j < other_form_bits.length; j++){
    builder += 'Content-Disposition: form-data; name="'+other_form_bits[j]['name']+'"' + crlf + crlf + other_form_bits[j]['value'] + crlf + dashdash + boundary + crlf;
  }
  var file = Drupal.dragon.original_drop_event.dataTransfer.files[file_num - offset];
  if(file){
    /* Generate headers. */
    builder += 'Content-Disposition: form-data; name="'+temp_field_name+'"';
    if (file.fileName) {
      $('#'+Drupal.dragon.original_drop_box_id+' .dragon-message').html(Drupal.t('Uploading file: ')+file.fileName);
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
          Drupal.dragon.file_num_array[Drupal.dragon.original_drop_box_id] = file_num;
          $('#'+Drupal.dragon.original_drop_box_id+' .dragon-message').html('');
          Drupal.dragon.single_file_upload(file_num, offset);
        }
      }
    };
  }
}
Drupal.dragon.upload = function(event) {
  Drupal.dragon.original_drop_event = event;
  Drupal.dragon.original_form = $(event.currentTarget).parents('form');
  Drupal.dragon.original_drop_box_id = event.currentTarget.id;
  Drupal.dragon.id_parts = event.currentTarget.id.split('__dragon__');
  Drupal.dragon.upload_path = Drupal.settings.ahah[Drupal.dragon.id_parts[0]].url;
  if(!Drupal.dragon.field_name_array[Drupal.dragon.id_parts[0]]){
    Drupal.dragon.field_name_array[Drupal.dragon.id_parts[0]] = $('#'+Drupal.settings.ahah[Drupal.dragon.id_parts[0]].wrapper+" input[type=file]").attr('name');
  }
  Drupal.dragon.field_name = Drupal.dragon.field_name_array[Drupal.dragon.id_parts[0]];
  Drupal.dragon.wrapper = Drupal.settings.ahah[Drupal.dragon.id_parts[0]].wrapper;
  file_num = 0;
  if(Drupal.dragon.file_num_array[Drupal.dragon.original_drop_box_id]){
    file_num = Drupal.dragon.file_num_array[Drupal.dragon.original_drop_box_id];
  }
  Drupal.dragon.single_file_upload(file_num, file_num);
  event.stopPropagation();
}