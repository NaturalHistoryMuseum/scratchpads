// Grid & dataView need to be globals so they can be accessed from formatters etc.,
var grid;
var dataView;

(function($) {

    // register namespace
    $.extend(true, window, {
        Slickgrid: Slickgrid
    });

    // Slickgrid class implementation
    function Slickgrid(container, viewName, callbackPath) {


        var columnFilters = {};
        var objHttpDataRequest;
        var timeout;
        var $status; // $status container for result messages / working icon

        function init() {

            $status = $('#slickgrid-status');
            
            // Are row checkboxes enabled? If they are, add a checkbox selector column
            if (options['select_row_checkbox']) {

                initRowCheckboxes();

            }

            // Initialise the dataview & slickgrid
            dataView = new Slick.Data.DataView();
            grid = new Slick.Grid(container, dataView, columns, options);

            // Is pager enabled?
            if (options['pager']) {
                var pager = new Slick.Controls.Pager(dataView, grid, $("#pager"));
            }
            
            // Is delete enabled? If it is, add the context menu
            if (options['row_delete']) {
              
                initRowDelete();
                
            }
            
            // Are sortable columns enabled?
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


            // If row checkboxes are enabled, add row selection to the grid & register the plugin
            if (options['select_row_checkbox']) {
                grid.setSelectionModel(new Slick.RowSelectionModel({
                    selectActiveRow: false
                }));
                grid.registerPlugin(checkboxSelector);
            }            

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

                initGrouping();   

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
        		maxWidth : $(container).width(), // Only allow vertical resizing
        		minWidth : $(container).width(),
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

          $('input[id^=columnpicker]', '.slick-columnpicker').change(onColumnsChanged);
  
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
        
        function initRowCheckboxes(){
          
          var checkboxSelector = new Slick.CheckboxSelectColumn({
              cssClass: "slick-cell-checkboxsel"
          });

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

                if (columns[i].id !== "selector") {
                  
                    var header = grid.getHeaderRowColumn(columns[i].id);
                    $(header).empty();
                    
                    if (options['columns'][columns[i].id] && options['columns'][columns[i].id]['filter']) {
                        
                      columns[i].filter = eval('new Slick.Filters.' + options['columns'][columns[i].id]['filter'] + '()');
                       
                       // Does this filter have an input function? 
                       // If it does, add the input html
                       if(typeof columns[i].filter.input === 'function'){
                         columns[i].filter.input()
                          .data("columnId", columns[i].id)
                          .width($(header).width() - 4)
                          .val(columnFilters[columns[i].id])
                          .appendTo(header);
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
          
                        var groupsUI = new Slick.Controls.GroupsUI(dataView, grid, $("#controls"));

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
              
              var offset = $('.view-slickgrid').offset();
              
              $("#contextMenu")
                      .data("row", cell.row)
                      .css("top", e.pageY  - offset.top)
                      .css("left", e.pageX - offset.left)
                      .show();

              $("body").one("click", function() {
                  $("#contextMenu").hide();
              });
          });
          
          // Register event for user clicking the context menu
          $("#contextMenu").click(function(e) {
      			if (!$(e.target).is("li"))
      				return;

      			var row = $(this).data("row");
      			
      			deleteRow(row);

      		});
          
        }
        
        // Delete a row from the dataset
        function deleteRow(row) {
           
           var item = dataView.getItem(row);
           dataView.deleteItem(item.id);
           callback('delete', {'nid': item.id});
           
        }
        
        function initSortableColumns(){
          
          grid.onSort.subscribe(function(e, data) {
      
              var sortCol = data.sortCol;
              var sortAsc = data.sortAsc;
              sortdir = sortAsc ? 1: -1;
              sortcol = sortCol.field;
      
              // Set which function to use to sort the column - presently just uses a basic comparer
              dataView.sort(comparer, sortAsc);
      
          });
          
        }
        
        function initHiddenColumns(){
            
             var visibleColumns = [];
             
             $(columns).each(function(i,e) {
           
               if(jQuery.inArray(columns[i]['id'], options['settings']['hidden_columns']) === -1){
                 visibleColumns.push(columns[i]);
               }
               
             });
             
             grid.setColumns(visibleColumns);
             
        }
        
        // All callbacks should be routed through this function
        function callback(op, data){

          // Check to see if there is an AJAX request already in
          // progress that needs to be stopped.
          if (objHttpDataRequest){

          // Abort the AJAX request.
          objHttpDataRequest.abort();

          }
          
          // Clear the timeout for removing the result
          clearTimeout(timeout);
          
          objHttpDataRequest = $.ajax({
            type: 'POST',
            url: callbackPath + '/' + op,
            data: data,
            success: callbackSuccess,
            dataType: "json"
          });

        }
        
        function updateSettings(setting, value){
          
          updateStatus(true);
          
          data = {
            'view': viewName,
            'setting': setting,
            'value': value
          }
          
          callback('settings', data);
          
        }
        
        function callbackSuccess(response){
          
          updateStatus(false);
          
          if(response && response.result){
            $result.append(response.result);
            timeout = setTimeout("$('#result').empty();",3000);
          }
          
        }
        
        function updateStatus(loading){
          
          if(loading){
            $status.empty();
            $status.addClass('loading');
          }else{
            $status.removeClass('loading');
          }
          

        }

        init();

    }

})(jQuery);


