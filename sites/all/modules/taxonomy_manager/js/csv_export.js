//global killswitch
if (Drupal.jsEnabled) {
  $(document).ready(function() {
    Drupal.attachCSVExport(Drupal.settings.exportCSV['url']);
  });
}

Drupal.attachCSVExport = function(url) {
  $("#edit-export-submit").click(function() {
    var div = $("#edit-export-csv");
    var param = new Object();
    param['delimiter'] = $("#edit-export-delimiter").val();
    param['option'] = $("#taxonomy_manager_export_options").find("input[@type=radio][@checked]").val();
    param['vid'] = Drupal.getVocId();
    var tid = 0;
    $('.treeview').find("input[@type=checkbox][@checked]").each(function() {
      tid = Drupal.getTermId($(this).parents("li").eq(0));
    });
    param['tid'] = tid;
    
    $.post(url, param, function(data) {
      $(div).val(data);
    });
    return false;
  });
}
