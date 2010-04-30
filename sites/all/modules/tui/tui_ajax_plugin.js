Drupal.Ajax.plugins.tui_ajax_plugin = function(hook, args) {
  if (hook == 'redirect'){
    return false;
  }
  if(hook == 'submit'){
    Drupal.tui.submitted = true;
  }
  if(hook == 'complete'){
    Drupal.tui.completed = true;
    // Bug fix for ajax module to try and prevent double submission.
    $('.ajax-trigger').attr('disabled', 'true');
  }
  if(hook == 'scrollFind' && Drupal.tui.completed){
    Drupal.tui.completed = false;
    Drupal.tui.submitted = false;
    setTimeout("$('.messages').slideUp(200, Drupal.tui.display_form(false))", 2000);
    Drupal.tui.reload_tree();
    return false;
  }
}
Drupal.Ajax.plugins.disable_redirect = function(hook, args) {
  if (hook == 'complete') {
    if (args.options.disable_redirect == true) {
      return false;
    }
  }
}
