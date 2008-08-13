var lastform = false;
function nodes(id, url){
  // Hide other forms
  if(lastform != id){
    $(".nodes-form").remove();
    // Get the HTML from server, and add it
    var nodesGetForm = function (data) {
      //alert("***"+data);
      //alert($('#id'+id).html());
      var returnHtml = Drupal.parseJson(data);
      $('#'+id).html($('#'+id).html()+returnHtml['html']);
      
      // Ensure fieldsets are collapsible - Christ, this is shite!
      $('fieldset.collapsible > legend').each(function() {
        var fieldset = $(this.parentNode);
        // Expand if there are errors inside
        if ($('input.error, textarea.error, select.error', fieldset).size() > 0) {
          fieldset.removeClass('collapsed');
        }
        // Turn the legend into a clickable link and wrap the contents of the fieldset
        // in a div for easier animation
        var text = this.innerHTML;
        $(this).empty().append($('<a href="#">'+ text +'</a>').click(function() {
          var fieldset = $(this).parents('fieldset:first')[0];
          // Don't animate multiple times
          if (!fieldset.animating) {
            fieldset.animating = true;
            Drupal.toggleFieldset(fieldset);
          }
          return false;
        })).after($('<div class="fieldset-wrapper"></div>').append(fieldset.children(':not(legend)')));
      });
      // End of the shite
      
      // Lets try moving this frame around a little
      var right = parseInt($('.nodes-form').width()) + parseInt($('.nodes-form').css('left').replace('px','')) + 50;
      if (typeof window.innerWidth != 'undefined'){
        viewportwidth = window.innerWidth;
      } 
      else if (typeof document.documentElement != 'undefined' && typeof document.documentElement.clientWidth != 'undefined' && document.documentElement.clientWidth != 0){
        viewportwidth = document.documentElement.clientWidth;
      } else {
        viewportwidth = document.getElementsByTagName('body')[0].clientWidth;
      }
      if(right > viewportwidth){
        var moveleft = right - viewportwidth;
        if(moveleft<0){
          moveleft = 0;
        }
        $('.nodes-form').css('left',parseInt($('.nodes-form').css('left').replace('px',''))-parseInt(moveleft)+'px');
      }
    }
    $.get(url, null, nodesGetForm);
    lastform = id;    
  }
}
function nodescancel(){
  $(".nodes-form").remove();
  lastform = false;
  return false;
}
function nodessubmit(url,id,field){
  var nodesSubmitForm = function (data) {
    var returnHtml = Drupal.parseJson(data);
    $('#'+field+'_'+returnHtml['nid']).html(returnHtml['value']);
  }
  var valueToSend = new Array();
  $('.'+id).each(function(i){
    valueToSend[valueToSend.length] = $(this).val();
  });
  $.post(url,{'formvalue[]':valueToSend,'formid':id,'field':field,'updated':$('#'+id+'-updated').val()},nodesSubmitForm);
}
var allselected = false;
function nodesselectallrows(){
  if(!allselected){
    $('.selectable').each(function(){
      var nidparts = $(this).attr('id').split("-");
      var nid = nidparts[2];
      $('#nodes-row-'+nid).addClass('nodes-selected');
      $('#nodes-row-checkbox-'+nid).attr('checked','checked');
    });
    allselected = true;
    $('#selected').toggle();
    $('#deselected').toggle();
  } else {
    $('.selectable').each(function(){
      var nidparts = $(this).attr('id').split("-");
      var nid = nidparts[2];
      $('#nodes-row-'+nid).removeClass('nodes-selected');
      $('#nodes-row-checkbox-'+nid).removeAttr('checked');
    });
    allselected = false;
    $('#deselected').toggle();
    $('#selected').toggle();
  }
}
function nodesselectrow(nid){
  if($('#nodes-row-checkbox-'+nid).attr('checked')){
    $('#nodes-row-checkbox-'+nid).removeAttr('checked');
  } else {
    $('#nodes-row-checkbox-'+nid).attr('checked','checked');
  }
  $('#nodes-row-'+nid).toggleClass('nodes-selected');
  return false;
}