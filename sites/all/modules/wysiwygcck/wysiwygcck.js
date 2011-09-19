// $Id: wysiwygcck.js $

/**
 * This module proposes that we add another method to the API for the editors used
 * in the wysiwyg module (wysiwyg/editors/js/*.js). Until we can count on those js 
 * files implementing it, we place some default implementations here.
 */
Drupal.wysiwyg.editor.triggerSave.tinymce = Drupal.wysiwyg.editor.triggerSave.tinymce || function(context, params) {
  tinyMCE.triggerSave();
}
Drupal.wysiwyg.editor.triggerSave.fckeditor = Drupal.wysiwyg.editor.triggerSave.fckeditor || function(context, params) {
  var instance = FCKeditorAPI.GetInstance(params.field);
  instance.UpdateLinkedField();
}

/**
 * Attach behavior to forms, so that prior to serializing during AJAX submit,
 * there's an opportunity to save editor content to the target element.
 * The form-pre-serialize event is only available with jquery.form.js 2.16 or later,
 * which is why this module has jquery_form_update as a dependency.
 */
Drupal.behaviors.wysiwygcck = function(context) {
  $('form:not(.wysiwygcck-processed)').each(function() {
    $(this)
      .bind('form-pre-serialize', function() {Drupal.wysiwygcck.triggerSave(this);})
      .addClass('wysiwygcck-processed');
  });
}

/**
 * Let all the editor instances save their content to the field, but
 * not necessarily detach. Called during AJAX/AHAH form submits.
 */
Drupal.wysiwygcck.triggerSave = function(context) {
  var instances = Drupal.wysiwygcck.getInstances(context);
  for (var id in instances) {
    var params = instances[id];
    var editor = params.editor;
    if (jQuery.isFunction(Drupal.wysiwyg.editor.triggerSave[editor])) {
      Drupal.wysiwyg.editor.triggerSave[editor](context, params);
    }
  }
}

/**
 * Return the instances (id => params) for elements within the context that
 * have an attached editor.
 */
Drupal.wysiwygcck.getInstances = function(context) {
  var result = null;
  
  if (Drupal.wysiwyg && Drupal.wysiwyg.instances) {
    for (var id in Drupal.wysiwyg.instances) {
      var instance = Drupal.wysiwyg.instances[id];
      if (instance.status && jQuery('#' + id, context).length) {
        if (!result) {
          result = {};
        }      
        result[id] = {};
        // Params should not include complex data types.
        for (var i in instance) {
          if (jQuery.inArray(typeof(instance[i]), ['object', 'array', 'function']) == -1) { 
            result[id][i] = instance[i];
          }
        }
      }
    }
  }
  
  return result;
}

/**
 * Extend tabledrag.js's default row swapping behavior to take into account
 * that wysiwyg editors are usually in an iframe, and iframes get reloaded when moved
 * in the dom.
 */
if (Drupal.tableDrag) {
  jQuery.aop.around({target: Drupal.tableDrag.prototype.row.prototype, method: 'swap'}, function(invocation) {
    // TODO: What's the proper way to handle situation where this.group has
    // multiple items?
    if (this.group.length > 1) {
      return invocation.proceed();
    }
    
    // Get the editor instances in the row being dragged. If there aren't any, let tabledrag's
    // default implementation proceed.
    var position = invocation.arguments[0];
    var referenceRow = invocation.arguments[1];
    var thisRow = this.group[0];
    var instances = Drupal.wysiwygcck.getInstances(thisRow);
    if (!instances) {
      return invocation.proceed();
    }
    
    // If thisRow is the same as referenceRow, then nothing will happen during a swap.
    // However, letting the default swap behavior run will result in the editors' iframes
    // being reloaded and messed up, so instead, we just force nothing to happen.
    if (thisRow == referenceRow) {
      return;
    }
    
    // If we reached here, it means we need to let tabledrag swap the rows. However,
    // if the editors have iframes, they'll probably get messed up by the swap, 
    // so we want to detach the editors before the swap, and reattach them after the swap.
    for (var id in instances) {
      Drupal.wysiwygDetach(thisRow, instances[id]);
    };
    result = invocation.proceed();
    for (var id in instances) {
      Drupal.wysiwygAttach(thisRow, instances[id]);
    };
    
    return result;
  });
}
