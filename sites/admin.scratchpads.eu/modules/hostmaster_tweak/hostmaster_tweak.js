Drupal.hostmaster_tweak = new Object;

Drupal.hostmaster_tweak.do_search = function(copy){
  if(copy){
    var domain = $('#edit-title-copy').val();    
  }else{
    var domain = $('#edit-title').val();
  }
  if(domain.length>10){
    if(domain.substring(domain.length-10) == 'taxon.name'){
      $('#edit-profile-3475').attr('checked', 'checked');
    }    
  }
  return false;
}

Drupal.behaviors.hostmaster_tweak = function(context){
  $('#edit-profile-3054', context).attr('checked', 'checked');
  $('#edit-platform-3914').attr('checked', 'checked');
  $('#edit-title', context).keyup(function(){
    Drupal.hostmaster_tweak.do_search(false);
    return false;
  });
  $('#edit-title-copy', context).keyup(function(){
    Drupal.hostmaster_tweak.do_search(true);
    return false;
  });
};