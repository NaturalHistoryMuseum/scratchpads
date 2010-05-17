var publication = publication || {};

publication.init = function(){
  
  // Add the tabs
  $("#publication-tabs ul.ui-tabs-nav").tabs({
    show: function(event, ui) { $('#edit-current-tab').val($(ui.tab).attr('href')) }
  });
  
  // Prevent jumping to #tab when staying on same tab
  scroll(0,0);
    
}

publication.getNID = function(){
  
  return $('#edit-publication-nid').val();
  
}

publication.getVID = function(){
  
  return $('#edit-publication-vid').val();
  
}

publication.getVocabulary = function(){
  
  return $('#edit-vocabulary').val();
  
}

publication.getBuildID = function(){
  
  return $('#node-form input[name=form_build_id]').val();
  
}

publication.nodeDeleted = function(type, nid){
  
  $('#node-form').append('<input type="hidden" name="deleted_node['+type+'][]" value="'+nid+'">');
  
}

/**
 * Add an asterisk or other marker to the changed row.
 */
publication.markChanged = function($element) {
  var marker = Drupal.theme('tableDragChangedMarker');
  if ($('span.tabledrag-changed', $element).length == 0) {
    $element.append(marker);
  }
};

publication.insertChangedWarning = function($element) {

  $(Drupal.theme('publicationChangedWarning')).insertAfter($element).hide().fadeIn('slow');
  
}

Drupal.theme.prototype.publicationChangedWarning = function () {
  
  return '<div class="warning">' + Drupal.theme('tableDragChangedMarker') + ' ' + Drupal.t("Changes made will not be saved until the publication form is submitted.") + '</div>';

}

$(document).ready(function(){
  
  publication.init();
  
});

Drupal.behaviors.publications = function(context) {

  // Add the add more text
  $('a.noderelationships-nodereference-multi-button:not(.publication-processed)', context).addClass('publication-processed').each(function(){
    
    $(this).append($(this).siblings('input').val());
    
  })
  
  $('ul div.edit-manuscript-name input', context).click(function(event){
    event.stopPropagation();
  });
  
  
  $('#edit-vocabulary').change(function(){
    $('#classification-names').html('');
  });
  
  var $fieldset = $(context).parents('fieldset.publication-noderef:not(.changed)');
  
  if($fieldset.length){
    publication.markChanged($fieldset.find('legend'));
    $fieldset.addClass('changed');
    publication.insertChangedWarning($fieldset);
  }
  
}







/**
 * Rewrite queryString to include nid
 */
Drupal.nodeRelationshipsReferenceButtons.queryString = function() {

  var qs = {
    vocabulary: publication.getVocabulary(),
    vid: publication.getVID(),
    nid: publication.getNID(),
  };
  
  var excludeArgs = ['q', 'destination', 'pass', 'translation', 'language'];
  if (window.location.search && window.location.search.length) {
    // Remove leading question mark and splits the string into query arguments.
    var q = window.location.search.substring(1).split('&');
    for (var i = 0; i < q.length; i++) {
      var pair = q[i].split('=');
      if (pair.length == 2) {
        var name = decodeURIComponent(pair[0]);
        if ($.inArray(name, excludeArgs) == -1) {
          qs[name] = decodeURIComponent(pair[1]);
        }
      }
    }
  }

  return qs;
  
};