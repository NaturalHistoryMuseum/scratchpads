// $Id: editview.js,v 1.3.2.2 2008/01/29 00:21:44 agileware Exp $

$(function(){ if (Drupal.jsEnabled) $("form").each(editviewCaptureForm); });

function editviewCaptureForm() {
    var form_id = $('[@name=form_id]', this).val();
    var delete_form = (form_id == 'editview_delete_form');
    var node_form = form_id && form_id.match(/^editview_node_form_\d+/);
    // This doesn't catch new node forms, we want them to submit normally
    if (node_form || delete_form) {
        var form = this;
        var wrapper = $(form).parent();
        var message = $('#editview_messages');
        var options = {
            url: $('#editview-submit-url').html(),
            dataType: 'json',
            beforeSubmit: function() {
                wrapper.slideUp('slow').hide();
                message.fadeOut('slow').empty();
            },
            success: function(data, status) {
                if (status == 0) {
                    alert('Error submitting form');
                }
                if (data['message']) {
                    message.append(data['message']).fadeIn('slow');
                } 
                //wrapper.hide();
                wrapper.empty();
                wrapper.append(data['form']).slideDown('slow');
                $('form', wrapper).each(editviewCaptureForm);
                $('form', wrapper).each(Drupal.textareaAttach); // TODO: generalise
            }
        }
        // we would just use ajaxForm, except that when you use a
        // file upload field, jquery.form.js has a bug that doesn't
        // send the right 'op', meaning that drupal doesn't know
        // which button was pushed.
        $('input:submit', form).click(function() {
          if ($(this).attr('name') == 'op') {
            $(form).append('<input type="hidden" name="op" value="' + $(this).attr('value') + '" />');
            $(form).ajaxSubmit(options);
            return false;
          }
        });
        // still use this in case the form is submitted without
        // clicking a button (pressing enter in a text field for
        // example)
        $(form).ajaxForm(options);
    }
}

