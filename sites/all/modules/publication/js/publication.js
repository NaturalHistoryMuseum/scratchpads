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

publication.insertTabWarning = function($element) {

  var $tab = $('.ui-tabs-panel:visible');
  
  if(!$('div.warning', $tab).length){
   
     $tab.prepend($(Drupal.theme('publicationTabWarning')).hide().fadeIn('slow'));
    
  }
  
}

Drupal.theme.prototype.publicationChangedWarning = function () {
  
  return '<div class="warning">' + Drupal.theme('tableDragChangedMarker') + ' ' + Drupal.t("Changes made will not be saved until the publication form is submitted.") + '</div>';

}

Drupal.theme.prototype.publicationTabWarning = function () {
  
  return '<div class="warning">' + Drupal.t("This information is used elsewhere in this publication. It is recommended you now save your changes before proceeding.") + '</div>';

}

$(document).ready(function(){
  
  publication.init();
  
});

Drupal.behaviors.publications = function(context) {
  // Add the add more text
  $('a.noderelationships-nodereference-multi-button:not(.publication-processed)', context).addClass('publication-processed').each(function(){
    
    $(this).append($(this).siblings('input').val());
    
  });
  
  $('a.noderelationships-noderef-multiselect-save:not(.publication-processed)', context).addClass('publication-processed').click(function(){
	    
	    alert('clicked');
	    return;
	    
	  });
  
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
  
  $('a.tab-link').click(function(){

    $('#publication-tabs li a[href='+$(this).attr('href')+']').trigger('click');
    return false;
    
  });
  
  $('#default-fields input, #edit-vocabulary, input.taxonomy-tree-checkbox').change(function(){
    
    publication.insertTabWarning();
    
  })
  
  if($(context).eq(0).hasClass('treeview-item')){
    
    publication.insertTabWarning();
    
  }
  
}










/**
 * Rewrite queryString to include nid
 */
Drupal.nodeRelationshipsReferenceButtons.queryString = function() {

  var qs = {
    vocabulary: publication.getVocabulary(),
    vid: publication.getVID(),
    nid: publication.getNID()
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


/**
 * Operation to update a node reference widget with multiple values.
 *
 * Updating multiple values means triggering the AHAH request related to
 * the "Add more items" button, but we need to tell how many items we
 * need. Once we have the correct number of items in the form, we can update
 * them with the new values coming from the modal frame dialog.
 */
Drupal.nodeRelationshipsReferenceButtons.updateMultipleValues = function(selectedItems, fieldOptions, $nodereference, $multiButton) {
	
  var self = this;

  // Hide the multiple selection button while performing the AHAH request.
  $multiButton.hide();

  // Build the list of selected items. Note these values will be applied
  // when the AHAH request triggers Drupal behaviors on the new content.
  self.selectedItems = {};
  self.selectedNids = [];
  self.selectedItems[fieldOptions.fieldName] = [];
  
  for (var nid in selectedItems) {
    self.selectedItems[fieldOptions.fieldName].push(selectedItems[nid]);
    // Add nids to an array ready to be passed server side
    self.selectedNids.push(nid);
  }
  
  var selectedItemsCount = self.selectedItems[fieldOptions.fieldName].length;

  // Perform the AHAH request to rebuild the items list.
  var addMoreSettings = Drupal.settings.ahah[fieldOptions.addMoreBase];
  addMoreSettings.url = fieldOptions.ahahSearchUrl +'/'+ selectedItemsCount;
  addMoreSettings.element = fieldOptions.addMoreElement;
  addMoreSettings.event = 'noderelationships.customClick';  
  // We need to know the NIDS server side, so they can be theme nicely
  addMoreSettings.button = {selectedItems: self.selectedNids};
  var ahah = new Drupal.ahah(fieldOptions.addMoreBase, addMoreSettings);
  ahah.oldSuccess = ahah.success
  ahah.success = function(response, status) {
    ahah.oldSuccess(response, status);
    $(addMoreSettings.element).unbind(addMoreSettings.event);
    $multiButton.show('fast');
    delete ahah;
  };
  ahah.oldError = ahah.error;
  ahah.error = function(response, uri) {
    ahah.oldError(response, uri);
    $(addMoreSettings.element).unbind(addMoreSettings.event);
    delete ahah;
  };
  $(addMoreSettings.element).trigger(addMoreSettings.event);
};
