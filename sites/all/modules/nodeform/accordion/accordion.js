// $Id: accordion.js,v 1.1 2008/03/07 17:33:25 ximo Exp $

/**
 * Accordion node form layout.
 * @author Joakim Stai
 * @license GPL 2
 */

$(document).ready(function(){
  // Add wrapper for accordion.
  $('#nodeform-buttons').before('<div id="nodeform-accordion"></div>');

  // Turn all marked fieldsets into accordion panels.
  $('.nodeform-fieldset').each(function(i){
    var legend = $(this).find('legend').html(); // Store the legend's content before we lose it
    $(this).removeClass('collapsible collapsed').appendTo('#nodeform-accordion').find('legend').insertBefore($(this)).replaceWith('<h3 class="nodeform-accordion-header">' + legend + '</h3>');
  });

  // Activate accordion.
  $('#nodeform-accordion').Accordion({
    header: '.nodeform-accordion-header',
    active: false,
    alwaysOpen: false,
    animated: false
  });
});
