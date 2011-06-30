// Grid & dataView need to be globals so they can be accessed from formatters etc.,
var grid;
var dataView;

(function($) {

    // register namespace
    $.extend(true, window, {
        Slickgrid: Slickgrid
    });

    // Slickgrid class implementation
    function Slickgrid(container, viewName, viewDisplayID, callbackPath) {


        var columnFilters = {};
        var objHttpDataRequest;
        var timeout;
        var checkboxSelector;
        var $status; // $status container for result icons & messages
        var $loadingBar; // $loadingBar container for loading bar
        var $undo; // $undo div
        var activeRow; // The row currently being edited
        var commandQueue = [];
        var ajaxOptions; // default options for ajax callback
        var locked; // Flag denoting log status
        
        function init() {

            $status = $('#slickgrid-status');
            $loadingBar = $('#slickgrid-loading-bar');
            $undo = $('#slickgrid-undo');
                        
            resetAjaxOptions();
            
            // Are row checkboxes enabled? If they are, add a checkbox selector column
            if (options['multi_edit']) {

                // Init row checkboxes - needs to be done before the grid is initiated as a column needs to be added
                initRowCheckboxes();

            }
            
            // Is undo enabled? If it is, add an editCommandHandler
            if (options['undo']) {
                
                initUndo();
                
            }
            
            // Initialise the dataview & slickgrid
            dataView = new Slick.Data.DataView();
            grid = new Slick.Grid(container, dataView, columns, options);

            // Is pager enabled?
            if (options['pager']) {
                var pager = new Slick.Controls.Pager(dataView, grid, $("#slickgrid-pager"));
            }
            
            // Is delete enabled? If it is, add the context menu
            if (options['delete_nodes']) {
              
                initRowDelete();
                
            }
            
            // Are sortable columns enabled?
            // Sortable columns won't work with collapsible taxonomy fields
            if (options['sortable_columns']) {
            
                initSortableColumns();

            }

            // Users can show / hide columns
            if (options['select_columns']) {
      
              var columnpicker = new Slick.Controls.ColumnPicker(columns, grid, options);
              
            }

            // Are there hidden columns?
            if(options['settings']['hidden_columns']){
              
              initHiddenColumns();   
              
            }
            
            // Does the grid have filters that need adding?
            if (options['has_filters']) {
                initFilters();
            }

            grid.setSelectionModel(new Slick.RowSelectionModel({
                selectActiveRow: false  // Do not select active row 
                                        // Going to handle this ourselves (making it selected) so user can select & edit multiple items
                                        // Otherwise all rows will be deselected on edit
            }));

            // If row checkboxes are enabled, add row selection to the grid & register the plugin
            if (options['multi_edit']) {

                grid.registerPlugin(checkboxSelector);

            }
            
            // Register events for my handling of active rows
            grid.onBeforeEditCell.subscribe(onBeforeEditCell);
            grid.onBeforeCellEditorDestroy.subscribe(onBeforeCellEditorDestroy);
            
            // Cell has been changed - need to submit the changes
            grid.onCellChange.subscribe(onCellChange);

            dataView.onRowCountChanged.subscribe(function(e, args) {
                grid.updateRowCount();
                grid.render();
            });
            
            dataView.onRowsChanged.subscribe(function(e, args) {
                grid.invalidateRows(args.rows);
                grid.render();
            });

            dataView.beginUpdate();

            // Add the data to the dataView
            dataView.setItems(data);

            // If a grouping field has been chosen, group the data
            // NB: needs to come after the data has been added to the dataView
            if (options['grouping_field']) {

                initGroups();   

            }
          
            // Add the collapsible taxonomy field
            if (options['collapsible_taxonomy_field']) {
            
              initCollapsibleTaxonomyField(options['collapsible_taxonomy_field']);
            
            }

            // If has_filter is true, there are header filters being used
            // Apply the filter to the dataView
            if (options['has_filters']) {
                dataView.setFilter(filter);
            }
            
            
            dataView.endUpdate();
            
            initCallbackEvents();

        }
        
        // Add callbacks to grid events so the info can be saved
        function initCallbackEvents(){
          
          grid.onColumnsReordered.subscribe(onColumnsReordered);
          
          // There isn't a grid event when a column is shown / hidden - tag one onto onHeaderContextMenu()
          grid.onHeaderContextMenu.subscribe(onHeaderContextMenu);
          
          grid.onColumnsResized.subscribe(onColumnsResized);
          
          // Add resizable callback event
          $('#slickgrid').resizable({
            handles: 's',
        		stop : function(e, ui) {

        			if (ui.originalSize.height != ui.size.height) {
        				onViewportResized(ui.size.height);
        			}
        			
        		}	
          });
          
        }
        
        // User has reodered the columns - save it to the backend
        function onColumnsReordered(e, ui){

          var orderedColumns = [];
          // This event is firing when columns have been dragged slightly but returned to same position
          // grid.getColumns() has the new order, while the global columns retains the old order
          // set orderActuallyChanged to true when looping through if the order really has changed
          var orderActuallyChanged = false;
          
          $(grid.getColumns()).each(function(i, col) {
            
            orderedColumns.push(col['id']);
            
            if(!orderActuallyChanged && col['id'] != columns[i]['id']){
              orderActuallyChanged = true;
            }
            
          });
          
          if(orderActuallyChanged){
            updateSettings('ordered_columns', orderedColumns);
          }   
          
        }
        
        // User has shown / hidden a column - save it to the backend
        function onColumnsChanged(){
          
          var hiddenColumns = [];
          
          $('input[id^=columnpicker]', '.slick-columnpicker').each(function(i,e) {
              if (!$(this).is(":checked")) {
                  hiddenColumns.push(columns[i]['id']);
              }
          });

          updateSettings('hidden_columns', hiddenColumns);
          
          // Add column filters back into the grid
          if (options['has_filters']) {
              initFilters();
          }          
          
        }
        
        // The context menu (choosing which columns to display) has been opened
        function onHeaderContextMenu(e, ui){
          
          // If multi edit is anebled we want to hide the first column from the user as its the checkbox select one
          if (options['multi_edit']) {
            $('.slick-columnpicker li').eq(0).hide()
          }  
          
          // User has changed the columns
          $('input[id^=columnpicker]', '.slick-columnpicker').change(onColumnsChanged);
          // Auto resize does not fire column resize - so need to call it manually
          $('#autoresize').change(function(){
            onAutoResize($(this).is(':checked') ? 1 : 0);
          });
        }
        
        // Columns have been resized
        function onColumnsResized(e, ui){
          
          // Need to save width of ALL changed columns - if auto resize is on it won't just be the resized column that has changed
          var resizedColumns = {};
          
          $(grid.getColumns()).each(function(i, col) {

              resizedColumns[col['id']] = col.width;
              
          });
          
          updateSettings('column_width', resizedColumns);
  
        }
        
        // Viewport has been resized
        function onViewportResized(height){
          
          updateSettings('viewport_height', height);
          
        }
        
        function onAutoResize(value){
          
          updateSettings('forceFitColumns', value);
          
        }
        
        // User has started editing a cell
        // Need to add row to the selected rows
        function onBeforeEditCell(e, ui){
          
          setActiveRow(ui.row); 

          // BUG fix: need to turn off & remove tooltips prior to re-editing
          cellNode = grid.getCellNode(ui.row, ui.cell);  

          if(typeof $(cellNode).attr('bt-xTitle') != 'undefined'){
            $(cellNode).btOff();  
            $(cellNode).bt({removeTip: 1});            
          }          

          $(cellNode).removeClass('invalid');
          
        }
        
        // User has stopped editing a cell
        // Deselect the row being actively edited
        function onBeforeCellEditorDestroy(editor){

          unsetActiveRow();     
          
        }
        
        function setActiveRow(row){
          
          $('div[row="'+row+'"]').addClass('active-row');
          
        }
        
        function unsetActiveRow(){
          
          var cell = grid.getActiveCell();
          $('div[row="'+cell.row+'"]').removeClass('active-row');
          
        }
        
        function onCellChange(e, ui){
          
          nids  = [ui.item.id];
          
          $.each(grid.getSelectedRows(), function(i, row) { 

            // Retrieve the data item of the selected row
            var item = dataView.getItem(row);

            if(item.id != ui.item.id){
              
              // Add the NID to the edited nids object
              nids.push(item.id);

            }
          
          });
          
          
          
          update(ui.item, ui.cell, nids);
          
        }
        
        function update(item, cell, nids){
          
          var c = grid.getColumns()[cell];

          // Array of data to be passed to the backend
          data = {
            'view': viewName,
            'display_id': viewDisplayID,
            'nids': nids,
            'field_id': c.id,
            'field_name': c.field,
            'revision': options['undo']
          }
          
        // Is this a serialised form? If it is serliase the data array
         if(typeof item.serialised_data != 'undefined'){
            
            data = $.param(data) + '&' + item.serialised_data;
              
          }else{ // If not serialised pass data as an object

            data[c.id] = item[c.id];

          }
          
          // Perform the update
          callback('update', data, true);
          
        }
      
        
        function initRowCheckboxes(){
          
          checkboxSelector = new Slick.CheckboxSelectColumn({
              cssClass: "slick-cell-checkboxsel"
          });

          // Add the selector column
          columns.unshift(checkboxSelector.getColumnDefinition());
          
        }

        function initFilters() {

            updateFilters();

            // Apply filters to the input kep up event
            $(grid.getHeaderRow()).delegate(":input", "change keyup",
            function(e) {
                columnFilters[$(this).data("columnId")] = $.trim($(this).val());             
                dataView.refresh();
            });
            
            // Register events for the header inputs
            grid.onColumnsReordered.subscribe(function(e, args) {
                updateFilters();
            });

            grid.onColumnsResized.subscribe(function(e, args) {
                updateFilters();
            });
            

        }

        function updateFilters() {

            // add the header inputs
            for (var i = 0; i < columns.length; i++) {

                if (columns[i].id !== "_checkbox_selector") {
                  
                    var header = grid.getHeaderRowColumn(columns[i].id);
                    $(header).empty();
                    
                    if (options['columns'][columns[i].id] && options['columns'][columns[i].id]['filter']) {
                      
                      
                      var c = grid.getColumns()[grid.getColumnIndex(columns[i].id)];
                      
                      if(typeof c != 'undefined'){
                        c.filter = eval('new Slick.Filters.' + options['columns'][columns[i].id]['filter'] + '()');
                          // Does this filter have an input function? 
                         // If it does, add the input html
                         if(typeof c.filter.input === 'function'){
                           var $input = c.filter.input()
                            .data("columnId", c.id)
                            .val(columnFilters[c.id]);
                            Drupal.theme('slickgridFilter', $input, options['columns'][c.id]['filter']).appendTo(header);
                         }                        
                      }
                      

                   
 
                    }

                }
            }

        }

        // Generic filter function, passes filtering to the appropriate filter function
        function filter(item) {          
            for (var columnId in columnFilters) {
                if (columnId !== undefined && columnFilters[columnId] !== "") {
                  
                    var c = grid.getColumns()[grid.getColumnIndex(columnId)];
                      // Pass the filtering to the doFilter function of whatever filter object is being used  
                      if(c.filter.doFilter(item, c.field, columnFilters[columnId]) === false){
                        return false; // only return false at this point so ALL filters get a chance to run
                      } 
                }
            }
            return true;
        }

        // Basic comparison function used in sorting columns
        function comparer(a, b) {
            var x = a[sortcol],
            y = b[sortcol];
            return (x == y ? 0: (x > y ? 1: -1));
        }
        
        function initGroups(){
          
                        // var groupsUI = new Slick.Controls.GroupsUI(dataView, grid, $("#controls"));

                        var groupingFieldLabel = options['columns'][options['grouping_field']]['label'];

                        // Set the grouping field
                        dataView.groupBy(
                        options['grouping_field'],
                        function(g) {
                            return groupingFieldLabel + ":  " + g.value + "  <span class='grouping-field-count'>(" + g.count + " items)</span>";
                        },
                        function(a, b) {
                            return a.value - b.value;
                        }
                        );

                        // Should all groups be collapsed
                        if (options['collapse_groups_by_default']) {
                            // Refresh the dataView so we have access to the groups
                            // Use endUpdate() rather than refresh() to reset suspend
                            dataView.endUpdate();
                            for (var i = 0; i < dataView.getGroups().length; i++) {
                                dataView.collapseGroup(dataView.getGroups()[i].value);
                            }
                        } 

                        // Add event to expand / collapse groups
                        grid.onClick.subscribe(function(e, args) {
                            var item = this.getDataItem(args.row);
                            if (item && item instanceof Slick.Group && $(e.target).hasClass("slick-group-toggle")) {
                                if (item.collapsed) {
                                    this.getData().expandGroup(item.value);
                                }
                                else {
                                    this.getData().collapseGroup(item.value);
                                }

                                e.stopImmediatePropagation();
                                e.preventDefault();
                            }
                        });
          
        }
        
        
        // Add events to manage the collapsible toggle
        function initCollapsibleTaxonomyField(field){
          
          grid.onClick.subscribe(function(e,args) {
            
    			                if ($(e.target).hasClass("toggle")) {
    			                  
    			                    // ensure the filters know this column is selected
    			                    columnFilters[field] = true;   
    			                  
    			                    var item = dataView.getItem(args.row);
    			                    
    			                    if (item) {
    			                        if (!item._collapsed)
    			                            item._collapsed = true;
    			                        else
    			                            item._collapsed = false;

    			                        dataView.updateItem(item.id, item);
    			                    }
    			                    
    			                    // Ensure this doesn't screw with the header filters
    			                    delete columnFilters[field];
    			                    
    			                    e.stopImmediatePropagation();
    			                    
    			                }
    			            });
          
        }
        
        function initRowDelete(){
          
          grid.onContextMenu.subscribe(function (e)
          {
              e.preventDefault();
              var cell = grid.getCellFromEvent(e);

              var offset = $("#slickgrid").offset();
              
              $('div[row="'+cell.row+'"]').addClass('delete-row');
              
              $("#slickgrid-delete")
                      .data("row", cell.row)
                      .css("top", e.pageY  - offset.top)
                      .css("left", e.pageX - offset.left)
                      .show();

              $("body").one("click", function() {
                  cancelDelete(cell);
              });
              
              $('.slick-viewport').scroll(function(){
                cancelDelete(cell);
              })
          });
          
          // Register event for user clicking the context menu
          $("#slickgrid-delete-button").click(function(e) {
      			var row = $('#slickgrid-delete').data("row");
            deleteRow(row);
      		});
          
        }
        
        function cancelDelete(cell){
          $("#slickgrid-delete").hide();
          $('div[row="'+cell.row+'"]').removeClass('delete-row');
        }
        
        // Delete a row from the dataset
        function deleteRow(row) {
           
           var item = dataView.getItem(row);
           dataView.deleteItem(item.id);
           callback('delete', {'nid': item.id}, true);
           
        }
        
        function initSortableColumns(){
          
          grid.onSort.subscribe(function(e, data) {
      
              var sortCol = data.sortCol;
              var sortAsc = data.sortAsc;
              sortdir = sortAsc ? 1: -1;
              sortcol = sortCol.id;
      
              // Set which function to use to sort the column - presently just uses a basic comparer
              dataView.sort(comparer, sortAsc);
      
          });
          
        }
        
        function initHiddenColumns(){
            
             var visibleColumns = [];
             
             $(columns).each(function(i,e) {
           
               if($.inArray(columns[i]['id'], options['settings']['hidden_columns']) === -1){
                 visibleColumns.push(columns[i]);
               }
               
             });
             
             grid.setColumns(visibleColumns);
             
        }
        
        // All callbacks should be routed through this function
        // Pass in array of overideFunctions to prevent default
        function callback(op, data, lock){

          // Should this be locked for the duration of the callback.
          // Default is true - should be locked for all edits & undos
          if(lock){
            editorLock(true);
          }
          
          updateLoadingBar(true);
          
          // Check to see if there is an AJAX request already in
          // progress that needs to be stopped.
          if (objHttpDataRequest){

          // Abort the AJAX request.
          objHttpDataRequest.abort();

          }

          ajaxOptions['url'] = callbackPath + '/' + op;
          ajaxOptions['data'] = data;

          objHttpDataRequest = $.ajax(ajaxOptions);

        }
        
        function updateSettings(setting, value){
          
          data = {
            'view': viewName,
            'setting': setting,
            'value': value
          }
          
          callback('settings', data);
          
        }
        
        // Error handling function
        function callbackError(args){
                    
          grid.getEditController().cancelCurrentEdit();
          
          x = args[0];
          e = args[1];

          var errorMessage = []; // Error message for user
          var errorLog; // Error message for log
          
          if(x.status==0){
            
            errorMessage.push({type : 'error', message: 'Could not connect to server. Your website may be unavailable!'});
            
          }else{
          
            errorMessage.push({type : 'error', message: 'Sorry there was an error - please reload this page and try again.'});
          
            if(x.status==404){
            errorLog =  '404: Requested URL not found.';
            }else if(x.status==500){
            errorLog =  '500: Internel Server Error.';
            }else if(e=='parsererror'){
            errorLog =  'Error parsing JSON Request.';
            }else if(e=='timeout'){
            errorLog =  'Request Time out.';
            }else {
            errorLog =  'Unknown Error';
            }
            
            // turn off error function to prevent everlasting loop if there is an error
            setAjaxOption('errorFunctions'); 
            setAjaxOption('completeFunctions'); 
            setAjaxOption('errorFunctions', function(){resetAjaxOptions()});
            setAjaxOption('successFunctions'); 
            
            // Pass the error to the callback function so we can try and fix any errors.
            callback('log', {error : errorLog});
          
          }
          
          updateLoadingBar(false);
          updateStatus(true, errorMessage);
          
        }
        
        function callbackSuccess(args){
          
          // Our ajax function caller just passes in an array of all arguments
          response = args[0];
          
          updateLoadingBar(false);

          if(response){
            
            // Are there any update nodes
            if(response.updated){
                           
                $.each(response.updated, function(nid, node) { 
                
                  // Get the row denoted by the nid
                  row = dataView.getRowById(nid);
                
                  // Get the data item for the row
                  var item = dataView.getItem(row);
                
                  // Update the item with the new value (if necessary)
                  if(item[response.field_name] != node.value){

                      // Change the value
                      item[response.field_name] = node.value;

                      // Update the dataView
                      dataView.updateItem(item.id, item);

                  }           
                
                
                });

              // Are we allowing undoing content (there will be a command queue if we are)
              if(options['undo'] && response.op == 'update'){

                if(!$undo.hasClass('undo-enabled')){

                  enableUndo();
                
                }
                
                // Store all of the updated items so we know we can undo them
                commandQueue[commandQueue.length  - 1]['updated'] =  response.updated;
              }

            }
            // Were there any errors?
            if(response.errors){  
              
              response.error = true;

              $.each(response.errors, function(nid, errorMessage) { 
                
                if(errorMessage[response.field_name]){
                  
                  row = dataView.getRowById(nid);

                  cell = grid.getColumnIndex(response.field_id);
                
                  cellNode = grid.getCellNode(row, cell);
                
                  $(cellNode).addClass('invalid');
                  
                   $(cellNode).bt(errorMessage[response.field_name], {
                    positions : 'right',
                    fill : 'rgba(0, 0, 0, .7)',
                    strokeWidth : 0,
                    spikeLength : 10,
                    cssStyles : {
                      color : 'white',
                      'font-size' : '10px'
                    },
                    width: 150,
                    closeWhenOthersOpen : true
                  });
                
                }
                
                                         
              });
              
            }

            if(response.messages){
            updateStatus(response.error, response.messages);
            }
            
          }
          
          // Reset ajax options after successful request
          resetAjaxOptions(); 
          
        }
        
        function callbackComplete(args){

          // Ajax has completed - so turn off the lock (IF it's on)
          if(locked){
            editorLock(false);
          }
          
        }
        
        function updateStatus(error, statusMessages){
          
           $status.attr('class', '');
           
           if(error){
             $status.addClass('slickgrid-error').html('Warning');
           }else{
             $status.addClass('slickgrid-no-error').empty();
           }

           $status.bt({
            contentSelector: Drupal.theme('slickgridMessages', statusMessages), 
            positions : 'left',
            fill : 'rgba(0, 0, 0, .7)',
            strokeWidth : 0,
            spikeLength : 10,
            cssStyles : {
              color : 'white',
              'font-size' : '10px'
            },
            width: 250,
            closeWhenOthersOpen : true,
            trigger : 'hover'
          });        
          
        }
        
        function updateLoadingBar(loading){
                    
          if(loading){
            $loadingBar.addClass('loading');
          }else{
            $loadingBar.removeClass('loading');
          }
          
        }
        
        // Undo
        function initUndo(){
          
          options['editCommandHandler'] = queueAndExecuteCommand;
          
        }
        
        // Only enable undo when there are items that can be undone
        function enableUndo(){

          $undo.addClass('undo-enabled');
          $undo.click(undo);
          
        }
        
        function disableUndo(){

          $undo.removeClass('undo-enabled');
          $undo.unbind('click');
          
        }
        
        function queueAndExecuteCommand(item,column,editCommand) {
        
            commandQueue.push(editCommand);
            editCommand.execute();
        }

        function undo() {

            var command = commandQueue.pop();
            if (command && Slick.GlobalEditorLock.cancelCurrentEdit()) {
                
                // Undo is handled via node revisions so post to backend all nodes updated as part of this command
                
                // Get the column
                var c = grid.getColumns()[command.cell];
                var data = {
                  'view': viewName,
                  'display_id': viewDisplayID,
                  'field_name': c.field,
                  'field_id': c.id,
                  'updated': command.updated
                }
                
                callback('undo', data, true);
                grid.gotoCell(command.row, command.cell, false);
                
            }
            
            if(commandQueue.length == 0){
              disableUndo();
            }
            
        }
        
        // Default ajax options
        function defaultAjaxOptions(){
          
          ajaxOptions = {
            type: 'POST',
            dataType: "json",
            successFunctions: ['callbackSuccess'],
            errorFunctions: ['callbackError'],
            completeFunctions: ['callbackComplete'],
            lock: true  
          };
          
          // This allows us have an array of success, error functions etc.,
          $.each(['success', 'error', 'complete'], function(i, type){
            
            ajaxOptions[type] = function(){            
              ajaxFunctionCaller(type, arguments);
            };
            
          })

        }
        
        function ajaxFunctionCaller(type, args){

          if(ajaxOptions[type+'Functions']){
            $.each(ajaxOptions[type+'Functions'], function(i, func){
              // TODO - Insecure. Change this.
              eval(func)(args);

            });
          }

          
        }
        
        // Default ajax options
        // Type successFunctions
        function setAjaxOption(type, value){
          if(typeof value == 'function'){
               if(typeof ajaxOptions[type] == 'undefined'){
                 ajaxOptions[type] = [value]; 
               }else{
                 ajaxOptions[type].push(value); 
               }     
          }else if(typeof value != 'undefined'){
              ajaxOptions[type] = value; 
          }
          else{ // Not a function so remove the option
            delete ajaxOptions[type];
          }
 
        }
        
        function resetAjaxOptions(){
          defaultAjaxOptions();
        }
        
        function getViewName(){
          return viewName;
        }
        
        function getViewDisplayID(){
          return viewDisplayID;
        }
        
        // Function for locking grid edits globally
        function editorLock(status){
          grid.setOptions({editable: !status});
          locked = status;

        }
        
        ///////////////////////////////////////////// Public API /////////////////////////////////////////////
        $.extend(this, {
           // Methods
           "callback":               callback,
           "setAjaxOption":          setAjaxOption,
           "resetAjaxOptions":       resetAjaxOptions,
           "getViewName":            getViewName,
           "getViewDisplayID":       getViewDisplayID,
           "editorLock":             editorLock
        });

        init();

    }

})(jQuery);

// Theme functions 

// Theme a slickgrid filter
Drupal.theme.prototype.slickgridFilter = function ($input, type) {
  
  return $('<span class="slickgrid-filter slickgrid-filter-'+type+'"></span>')
  .click(function(){$input.focus()}) // Make it a bit more uable - click anywhere on the span to set the focus on the input
  .append($('<span></span>').append($input));
  
};

// Theme the messages
Drupal.theme.prototype.slickgridMessages = function (messages) {
  
  var $ul = $('<ul class="slickgrid-messages">');

  $.each(messages, function(i, message) { 
    $('<li class="'+message.type+'">'+message.message+'</li>').appendTo($ul);
  });

  return $ul;
  
};




