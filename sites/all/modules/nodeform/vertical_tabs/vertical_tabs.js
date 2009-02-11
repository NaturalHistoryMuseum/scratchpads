// $Id: vertical_tabs.js,v 1.3 2008/03/07 17:31:18 ximo Exp $

/**
 * Vertical tabs node form layout (aka drawers).
 * @author Bevan Rudge drupal.geek.nz
 * @author Joakim Stai
 * @license GPL 2
 */

$(document).ready(function(){
  // Add wrapper for vertical tabs.
  $('#nodeform-buttons').before('<div id="nodeform-vertical-tabs"><ul class="ui-tabs-nav"></ul></div>');

  // Turn all marked fieldsets into vertical tabs.
  $('.nodeform-fieldset').each(function(i){
    $('#nodeform-vertical-tabs > ul').append('<li><a href="#' + $(this).attr('id') + '">' + $(this).find('legend').text() + '</a></li>');
    $(this).removeClass('collapsible collapsed').appendTo('#nodeform-vertical-tabs');
  });

  // Activate vertical tabs.
  $('#nodeform-vertical-tabs > ul').tabs();

  // Find the height of the heighest panel.
  var maxHeight = $.map($('#nodeform-vertical-tabs'),function(el){
    return $(el).height();
  }).sort(function(a, b) {
    return b - a;
  })[0];

  // Set the height of the container to a fixed size.
  $('#nodeform-vertical-tabs').height(maxHeight);
});
