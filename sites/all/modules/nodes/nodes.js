var lastform = false;
function nodes(id, url){
  // Hide other forms
  if(lastform != id){
    $(".nodes-form").remove();
    // Get the HTML from server, and add it
    var nodesGetForm = function (data) {
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
  $.post(url,{formvalue: $('#'+id).val(), formid: id, field: field, updated: $('#'+id+'-updated').val()},nodesSubmitForm);
}