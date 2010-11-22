( function($) {
  if (Drupal.jsEnabled) {
    $(document).ready(
        function() {
          if (Drupal.ahah != undefined) {
            /*
             * Override the Drupal.ahah.prototype.beforeSubmit
             * 
             * The only difference here, is that we allow a function to be called 
             * which is set by Drupal.settings
             */
            Drupal.ahah.prototype.beforeSubmit = function (form_values, element, options) {
              // Disable the element that received the change.
              $(this.element).addClass('progress-disabled').attr('disabled', true);
              
              // ADDITIONAL AHAH_ACTION CODE
              var selectorName = this.selector.substring(1);                 
              if(typeof Drupal.settings.ahah[selectorName] == 'object' && typeof window[Drupal.settings.ahah[selectorName]['submitextra']] == 'function'){
                window[Drupal.settings.ahah[selectorName]['submitextra']](form_values, element, options);
              }
              // END ADDITIONAL AHAH_ACTION CODE
              
              // Insert progressbar or throbber.
              if (this.progress.type == 'bar') {
                var progressBar = new Drupal.progressBar('ahah-progress-' + this.element.id, eval(this.progress.update_callback), this.progress.method, eval(this.progress.error_callback));
                if (this.progress.message) {
                  progressBar.setProgress(-1, this.progress.message);
                }
                if (this.progress.url) {
                  progressBar.startMonitoring(this.progress.url, this.progress.interval || 1500);
                }
                this.progress.element = $(progressBar.element).addClass('ahah-progress ahah-progress-bar');
                this.progress.object = progressBar;
                $(this.element).after(this.progress.element);
              }
              else if (this.progress.type == 'throbber') {
                this.progress.element = $('<div class="ahah-progress ahah-progress-throbber"><div class="throbber">&nbsp;</div></div>');
                if (this.progress.message) {
                  $('.throbber', this.progress.element).after('<div class="message">' + this.progress.message + '</div>')
                }
                $(this.element).after(this.progress.element);
              }
            }
            /*
             * Override of Drupal.ahah.prototype.success. The only difference is that we
             * allow for new Drupal.settings.
             */
            Drupal.ahah.prototype.success = function(response, status) {
              var wrapper = $(this.wrapper);
              var form = $(this.element).parents('form');
              // Manually insert HTML into the jQuery object, using $() directly
              // crashes
              // Safari with long string lengths.
              // http://dev.jquery.com/ticket/1152
              var new_content = $('<div></div>').html(response.data);

              // Restore the previous action and target to the form.
              form.attr('action', this.form_action);
              this.form_target ? form.attr('target', this.form_target) : form
                  .removeAttr('target');
              this.form_encattr ? form.attr('target', this.form_encattr) : form
                  .removeAttr('encattr');

              // Remove the progress element.
              if (this.progress.element) {
                $(this.progress.element).remove();
              }
              if (this.progress.object) {
                this.progress.object.stopMonitoring();
              }
              $(this.element).removeClass('progress-disabled').attr('disabled',
                  false);
              // Add the new content to the page.
              Drupal.freezeHeight();
              if (this.method == 'replace') {
                wrapper.empty().append(new_content);
              } else {
                wrapper[this.method](new_content);
              }
              // Immediately hide the new content if we're using any effects.
              if (this.showEffect != 'show') {
                new_content.hide();
              }
              // Determine what effect use and what content will receive the effect, then
              // show the new content. For browser compatibility, Safari is
              // excluded from
              // using effects on table rows.
              if (($.browser.safari && $("tr.ahah-new-content", new_content)
                  .size() > 0)) {
                new_content.show();
              } else if ($('.ahah-new-content', new_content).size() > 0) {
                $('.ahah-new-content', new_content).hide();
                new_content.show();
                $(".ahah-new-content", new_content)[this.showEffect]
                    (this.showSpeed);
              } else if (this.showEffect != 'show') {
                new_content[this.showEffect](this.showSpeed);
              }
              // Merge in new and changed settings, if any.
              if (response.settings) {
                $.extend(Drupal.settings, response.settings);
              }
              // If there's a response action, call it
              if (response.action) {
                // Allow for function and string references
                if (typeof response.action == 'function') {
                  response.action(response);
                } else {
                  // Test if its an array.
                  var func = eval(response.action);
                  if (typeof func == 'function') {
                    func(response);
                  }
                }
              }
              // Attach all javascript behaviors to the new content, if it was successfully
              // added to the page, this if statement allows #ahah[wrapper] to
              // be optional.
              if (new_content.parents('html').length > 0) {
                Drupal.attachBehaviors(new_content);
              }
              Drupal.unfreezeHeight();
            };
          }
        });
  }
})(jQuery);
