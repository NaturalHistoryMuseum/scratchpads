// $Id: fckeditor-2.6.js,v 1.8 2008/12/01 14:14:41 sun Exp $

/**
 * Attach this editor to a target element.
 *
 * See Drupal.wysiwyg.editor.attach.none() for a full desciption of this hook.
 */
Drupal.wysiwyg.editor.attach.fckeditor = function(context, params, settings) {
  var FCKinstance = new FCKeditor(params.field, settings['Width'], settings['Height']);
  // Configure editor settings for this input format.
  FCKinstance.BasePath = settings.BasePath;
  for (var setting in settings) {
    FCKinstance.Config[setting] = settings[setting];
  }
  // Attach editor.
  FCKinstance.ReplaceTextarea();
};

/**
 * Detach a single or all editors.
 *
 * See Drupal.wysiwyg.editor.detach.none() for a full desciption of this hook.
 */
Drupal.wysiwyg.editor.detach.fckeditor = function(context, params) {
  if (typeof params != 'undefined' && typeof FCKeditorAPI != 'undefined') {
    var editor = FCKeditorAPI.GetInstance(params.field);
    if (editor) {
      $('#' + params.field).val(editor.GetXHTML()).show();
      $('#' + params.field + '___Config').remove();
      $('#' + params.field + '___Frame').remove();
      delete FCKeditorAPI.__Instances[params.field];
    }
  }
//  else {
//    tinyMCE.triggerSave();
//    tinyMCE.remove();
//  }
};

