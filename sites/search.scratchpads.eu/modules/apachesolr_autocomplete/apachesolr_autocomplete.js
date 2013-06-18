// $Id: apachesolr_autocomplete.js,v 1.1 2010/10/05 13:32:01 janusman Exp $

/**
 * Adds the custom autocomplete widget behavior.
 */
Drupal.behaviors.apachesolr_autocomplete = function(context) {
  $(".apachesolr-autocomplete.unprocessed", context).autocomplete(Drupal.settings.apachesolr_autocomplete.path,
  {
    // Classnames for the widget.
    inputClass: "",
    loadingClass: "throbbing",
    // Do not select first suggestion by default.
    selectFirst: false,
    // Specify no matching as it wil be done on server-side.
    matchContains: false,
    matchSubset: false,
    // Maximum number of items to show in widget.
    max: 50,
    width: 300,
    scroll: true,
    scrollHeight: 360,
    // Data returned from server is JSON-encoded.
    dataType: "json",
    // Function to parse returned json into elements.
    parse: function(data) {
      return $.map(data, function(item) {
        return {
          data: item,          // Echo the input data.
          value: item.display, // This will be shown in the options widget.
          result: item.key     // The actual value to put into the form element.
        }
      });
    },
    // Return the HTML to display in the options widget.
    formatItem: function(item) {
      return item.display;
    }
  }).result(function(item, element) {
    // Handle selection of an element in the autocomplete widget.
    // We should submit the widget's parent form.
    $(this).get(0).form.submit();
  }).addClass('form-autocomplete'); // Add Drupal autocomplete widget's style.
};
