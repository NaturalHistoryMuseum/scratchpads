// $Id: hideForm.js,v 1.2.2.1 2007/08/15 17:10:22 mh86 Exp $

/**
 * @file shows / hides form elements
 */
 
// global killswitch
if (Drupal.jsEnabled) {
  $(document).ready(function() {
    var settings = Drupal.settings.hideForm || [];
    
    if (settings['div']) {
      if (!(settings['div'] instanceof Array)) {
        Drupal.attachHideForm(settings['div'], settings['show_button'], settings['hide_button']);
      }
      else {
        for (var i=0; i<settings['div'].length; i++) {
          Drupal.attachHideForm(settings['div'][i], settings['show_button'][i], settings['hide_button'][i]); 
        }
      }
    }
     
  })
}

/**
 * adds click events to show / hide button
 */
Drupal.attachHideForm = function(div, show_button, hide_button) {
  var hide = true;
  div = "#"+ div;
  show_button = "#"+ show_button;
  hide_button = "#"+ hide_button;
  
  //don't hide if there is an error in the form
  $(div).find("input").each(function() {
    if($.className.has(this, "error")) {
      hide = false;
    }
  });
  
  if (hide) { 
    $(div).hide();
  }
  
  $(show_button).click(function() {
    $(div).show();
  });
  
  $(hide_button).click(function() {
    $(div).hide();
  });
}
