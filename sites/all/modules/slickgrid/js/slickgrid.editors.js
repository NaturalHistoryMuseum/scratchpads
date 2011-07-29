
(function($) {

    var SlickEditor = {

        TextCellEditor : function(args) {
            var $input;
            var defaultValue;
            var scope = this;

            this.init = function() {

                $input = $("<INPUT type=text class='editor-text' />")
                    .appendTo(args.container)
                    .bind("keydown.nav", function(e) {
                        if (e.keyCode === $.ui.keyCode.LEFT || e.keyCode === $.ui.keyCode.RIGHT) {
                            e.stopImmediatePropagation();
                        }
                    })
                    .focus()
                    .select();
            };

            this.destroy = function() {
                $input.remove();
            };

            this.focus = function() {
                $input.focus();
            };

            this.getValue = function() {
                return $input.val();
            };

            this.setValue = function(val) {
                $input.val(val);
            };

            this.loadValue = function(item) {
                defaultValue = item[args.column.field] || "";
                $input.val(defaultValue);
                $input[0].defaultValue = defaultValue;
                $input.select();
            };

            this.serializeValue = function() {
                return $input.val();
            };

            this.applyValue = function(item,state) {
                item[args.column.field] = state;
                delete item.serialised_data;
            };

            this.isValueChanged = function() {
                return (!($input.val() == "" && defaultValue == null)) && ($input.val() != defaultValue);
            };

            this.validate = function() {
                if (args.column.validator) {
                    var validationResults = args.column.validator($input.val(), $input);
                    if (!validationResults.valid)
                        return validationResults;
                }

                return {
                    valid: true,
                    msg: null
                };
            };

            this.init();
        },

        /*
         * An example of a "detached" editor.
         * The UI is added onto document BODY and .position(), .show() and .hide() are implemented.
         * KeyDown events are also handled to provide handling for Tab, Shift-Tab, Esc and Ctrl-Enter.
         */
        LongTextCellEditor : function (args) {
            var $input, $wrapper;
            var defaultValue;
            var scope = this;

            this.init = function() {
                var $container = $("body");

                $wrapper = $("<DIV style='z-index:10000;position:absolute;background:white;padding:5px;border:3px solid gray; -moz-border-radius:10px; border-radius:10px;'/>")
                    .appendTo($container);

                $input = $("<TEXTAREA hidefocus rows=5 style='backround:white;width:250px;height:80px;border:0;outline:0'>")
                    .appendTo($wrapper);

                $("<DIV style='text-align:right'><BUTTON>Save</BUTTON><BUTTON>Cancel</BUTTON></DIV>")
                    .appendTo($wrapper);

                $wrapper.find("button:first").bind("click", this.save);
                $wrapper.find("button:last").bind("click", this.cancel);
                $input.bind("keydown", this.handleKeyDown);

                scope.position(args.position);
                $input.focus().select();
            };

            this.handleKeyDown = function(e) {
                if (e.which == $.ui.keyCode.ENTER && e.ctrlKey) {
                    scope.save();
                }
                else if (e.which == $.ui.keyCode.ESCAPE) {
                    e.preventDefault();
                    scope.cancel();
                }
                else if (e.which == $.ui.keyCode.TAB && e.shiftKey) {
                    e.preventDefault();
                    grid.navigatePrev();
                }
                else if (e.which == $.ui.keyCode.TAB) {
                    e.preventDefault();
                    grid.navigateNext();
                }
            };

            this.save = function() {
                args.commitChanges();
            };

            this.cancel = function() {
                $input.val(defaultValue);
                args.cancelChanges();
            };

            this.hide = function() {
                $wrapper.hide();
            };

            this.show = function() {
                $wrapper.show();
            };

            this.position = function(position) {
                $wrapper
                    .css("top", position.top - 5)
                    .css("left", position.left - 5)
            };

            this.destroy = function() {
                $wrapper.remove();
            };

            this.focus = function() {
                $input.focus();
            };

            this.loadValue = function(item) {
                $input.val(defaultValue = item[args.column.field]);
                $input.select();
            };

            this.serializeValue = function() {
                return $input.val();
            };

            this.applyValue = function(item,state) {
                item[args.column.field] = state;
            };

            this.isValueChanged = function() {
                return (!($input.val() == "" && defaultValue == null)) && ($input.val() != defaultValue);
            };

            this.validate = function() {
                return {
                    valid: true,
                    msg: null
                };
            };

            this.init();
        },
        
        /*
         * This opens the field from the node form in a popup
         * This *should* work for all field types
         * If it doesn't work for a particular field, raise an issue
         */
        nodeFormEditor : function (args) {
            var $form, $wrapper, $input, $loadingForm;
            var defaultValue;
            var scope = this;
            var serialised_data;
            var isChanged = false;

            function toggle(e) {
                if (e.type == "keydown" && e.which != 32) return;

                if ($input.css("opacity") == "1")
                    $input.css("opacity", 0.5);
                else
                    $input.css("opacity", 1);

                e.preventDefault();
                e.stopPropagation();
                return false;
            }

            this.init = function() {
              
              // Initalise the container for the form
              var $container = $("body");
              
              $wrapper = $("<div class='node-form-editor' style='z-index:10000;position:absolute;'/>")
                  .appendTo($container);

              $loadingForm = $("<div class='loading-form'>Loading the form...</div>").appendTo($wrapper);      

              // Get the form 
              var data = {
                nid: args.item.id,
                field_name: args.column.field,
              };
              
              // Is there a VID? If there is this is a taxonomy field
              if(args.column.vid){
                data.vid = args.column.vid;
              }
              
              // Set the ajax success function to add the form when callback is complete
              slickgrid.setAjaxOption('successFunctions', this.ajaxSuccess); 
              slickgrid.callback('get_form', data);
                
            }
            
             this.ajaxSuccess = function(args) {
              

                scope.buildForm(args[0]);
              
              
      
             }
             
             this.buildForm = function(response){

               if(response['error']){
                 scope.cancel();
                 return;
               }  
               
               $form = $(response.data.content);

               $loadingForm.remove();
               
               $form.appendTo($wrapper);
               
               // Add the buttons;
               $buttons = $("<div style='text-align:right'>");
               
               $('<button>Save</button>').click(function(){
                 
                 var fieldID = $form.find('textarea').attr('id');
                 
                 // on some wysiwyg fields, input is used
                 if(typeof fieldID == 'undefined'){
                   var fieldID = $form.find('input.form-text').attr('id');
                 }
                
                 // Is this a wysiwyg textarea?
                 if(typeof Drupal.wysiwyg != 'undefined' &&  typeof fieldID != 'undefined' && typeof Drupal.wysiwyg.instances[fieldID] == 'object'){
                   // detach the wysiwyg editor (and apply the value to the form)
                   Drupal.wysiwygDetach($form, {field: fieldID});
                 }
                 
                 scope.save();
                 
               }).appendTo($buttons);
               
               $('<button>Cancel</button>').click(function(){
                 scope.cancel();
               }).appendTo($buttons);
               
               $buttons.appendTo($wrapper);

               if (response.data.__callbacks) {
                 $.each(response.data.__callbacks, function(i, callback) {
                   eval(callback)($form, response.data);
                 });
               }  

               $("<input type='hidden' name='reload_from_view' value='1' />").appendTo($form);
               
               $form.bind("keydown", scope.handleKeyDown);
               
               $input = $('textarea, input[type="text"], select', $form).eq(0);
               
               Drupal.attachBehaviors($form);               
               
             }

            this.handleKeyDown = function(e) {

                if (e.which == $.ui.keyCode.ENTER &! $form.find('textarea').length) {
                  e.preventDefault();
                  scope.save();
                }
                else if (e.which == $.ui.keyCode.ESCAPE) {
                    e.preventDefault();
                    scope.cancel();
                }
                else if (e.which == $.ui.keyCode.TAB && e.shiftKey) {
                    e.preventDefault();
                    grid.navigatePrev();
                }
                else if (e.which == $.ui.keyCode.TAB) {
                    e.preventDefault();
                    grid.navigateNext();
                }
            };

            this.save = function() {
             
             isChanged = true;       
              // Commit the changes to the grid       
              args.commitChanges();
            };

            this.cancel = function() {
                args.cancelChanges();
                return false;
            };

            this.hide = function() {
                
                $wrapper.hide();
            };

            this.show = function() {
                $wrapper.show();
            };

            this.position = function() {
              
              var left;
              
              if(args.position.left + $wrapper.width() > args.gridPosition.right){
                left = (args.gridPosition.width + args.gridPosition.left) - $wrapper.width() - 10;
              }else{
                left = args.position.left - 5;
              }
              
                $wrapper
                    .css("top", args.position.top - 5)
                    .css("left", left)
            };

            this.destroy = function() {
                $wrapper.remove();
            };

            this.loadValue = function(item) {
              defaultValue = item[args.column.field]
            };

            this.serializeValue = function() {
                // Keep a copy of the old value
                return defaultValue;
                
            };

            this.applyValue = function(item,state) {            
                
                item[args.column.field] = state;

                item.serialised_data = $form.serialize();
  
            };

            this.isValueChanged = function() {
                return isChanged;
            };
            
            this.focus = function() {
              $input.focus();
            }

            this.validate = function() {
              if (args.column.validator) {                
                  var validationResults = args.column.validator($input.val(), $input);
                  if (!validationResults.valid)
                      return validationResults;
              }

              return {
                  valid: true,
                  msg: null
              };
            };

            this.init();
        }

    };

    $.extend(window, SlickEditor);

})(jQuery);
