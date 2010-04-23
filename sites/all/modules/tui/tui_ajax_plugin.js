Drupal.Ajax.plugins.tui_ajax_plugin = function(hook, args) {
  if (hook === 'redirect'){
    return false;
  }
  if (hook === 'scrollFind') {
    setTimeout("$('.messages').slideUp(200, Drupal.tui.display_form(false))", 2000);
    Drupal.tui.reload_tree();
    jQuery.each(Drupal.behaviors, function() {
      this('#taxonomy-form-term');
    });
  }
}
Drupal.Ajax.plugins.disable_redirect = function(hook, args) {
  if (hook === 'complete') {
    if (args.options.disable_redirect === true) {
      return false;
    }
  }
}