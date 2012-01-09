
Drupal.behaviors.projectAuto = function (context) {
  // The initially selected term, if any.
  var tid;
  $('div.project-taxonomy-element input:not(.projectAuto-processed)', context).addClass('projectAuto-processed').each(function () {
    if (this.checked) {
      tid = this.value;
    }
    Drupal.projectMoveElement(this.value);
    $(this).click(function () {
      Drupal.projectSetTaxonomy(this.value);
    });
  });
  // Only reset taxonomy selectors when initially attaching on edit forms.
  if (tid) {
    Drupal.projectSetTaxonomy(tid);
  }
}

Drupal.projectMoveElement = function(tid) {
  // move all elements with a class linked to this tid into the
  // project taxonomy fieldset (similar to module sub-terms)
  $('.related-tid-' + tid).each(function() {
    $('#edit-tid-' + tid + '-wrapper').append($(this).parent().remove());
  });
}

Drupal.projectSetTaxonomy = function (tid) {
  $('div.project-taxonomy-element select').each(function () {
    // If this is the selector for the currently selected term or a
    // related element, show it (in case it was previously hidden).
    if (this.id == 'edit-tid-' + tid || $(this).hasClass('related-tid-' + tid)) {
      // Hide not the select but its containing div (which also contains
      // the label).
      $(this).parents('div.form-item').show();
    }
    // Otherwise, hide it.
    else {
      $(this).parents('div.form-item').hide();
    }
  });
}

Drupal.behaviors.projectSandboxShortname = function (context) {

  $('div#edit-project-sandbox-wrapper input:not(.projectSandboxShortname-processed)', context).addClass('projectSandboxShortname-processed').each(function() {
    // Add required markup to field label
    Drupal.projectMarkUriRequired();
    // Only toggle the field and required label if numeric short name is turned on
    if (Drupal.settings.project.project_sandbox_numeric_shortname) {
      $(this).click(function () {
        if (this.checked) {
          $('div#edit-project-uri-wrapper').hide();
        }
        else {
          $('div#edit-project-uri-wrapper').show();
        }
      });
      // Set the default value when loading the page
      if (this.checked) {
        $('div#edit-project-uri-wrapper').hide();
      }
    }
  });
}

Drupal.projectMarkUriRequired = function() {
  var required = Drupal.t('This field is required.')
  $('div#edit-project-uri-wrapper label').append('<span title="' + required + '" class="form-required">*</span>');
}
