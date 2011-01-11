(function($) {

    // register namespace
    $.extend(true, window, {
        Slickgrid: Slickgrid
    });

    // Slickgrid class implementation
    function Slickgrid(container) {

        var grid;
        var dataView;
        var columnFilters = {};
        var objHttpDataRequest;
        var timeout;

        function init() {

            // Are row checkboxes enabled? If they are, add a checkbox selector column
            if (options['select_row_checkbox']) {

                var checkboxSelector = new Slick.CheckboxSelectColumn({
                    cssClass: "slick-cell-checkboxsel"
                });

                columns.unshift(checkboxSelector.getColumnDefinition());

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
            
            // Are sortable columns enabled?
            if (options['sortable_columns']) {
            
                grid.onSort.subscribe(function(e, data) {
            
                    var sortCol = data.sortCol;
                    var sortAsc = data.sortAsc;
                    sortdir = sortAsc ? 1: -1;
                    sortcol = sortCol.field;
            
                    // Set which function to use to sort the column - presently just uses a basic comparer
                    dataView.sort(comparer, sortAsc);
            
                });
            
            }

            // Users can show / hide columns
            if (options['select_columns']) {
                var columnpicker = new Slick.Controls.ColumnPicker(columns, grid, options);
            }

            // If showHeaderRow is true, there are header filters which need to be added
            if (options['showHeaderRow']) {
                addHeaderFilters();
            }

            // Are there hidden columns?
            if(options['settings']['hidden_columns']){
              
              var visibleColumns = [];
              
              $(columns).each(function(i,e) {

                if(jQuery.inArray(columns[i]['id'], options['settings']['hidden_columns']) === -1){
                  visibleColumns.push(columns[i]);
                }
                
              });
              
              grid.setColumns(visibleColumns);
              
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

                // Should all groups be collapsed
                if (options['collapse_groups_by_default']) {
                    // Refresh the dataView so we have access to the groups
                    // Use endUpdate() rather than refresh() to reset suspend
                    dataView.endUpdate();
                    for (var i = 0; i < dataView.getGroups().length; i++) {
                        dataView.collapseGroup(dataView.getGroups()[i].value);
                    }
                }      

            }

            // If showHeaderRow is true, there are header filters being used
            // Apply the filter to the dataView
            if (options['showHeaderRow']) {
                dataView.setFilter(filter);
            }
            
            dataView.endUpdate();
            
            addCallbackEvents();

        }
        
        // Add callbacks to grid events so the info can be saved
        function addCallbackEvents(){
          
          grid.onColumnsReordered.subscribe(onColumnsReordered);
          
          // There isn't a grid event when a column is shown / hidden - tag one onto onHeaderContextMenu()
          grid.onHeaderContextMenu.subscribe(onHeaderContextMenu);
          
          
        }
        
        // User has reodered the columns - save it to the backend
        function onColumnsReordered(e, ui){
          
          var orderedColumns = [];
          
          $(grid.getColumns()).each(function(i, col) {
            
            orderedColumns.push(col['id']);
            
          });
          
          updateSettings('ordered_columns', {'columns': orderedColumns});
          
        }
        
        // User has shown / hidden a column - save it to the backend
        function onColumnsChanged(){
          
          var hiddenColumns = [];
          
          $('input[id^=columnpicker]', '.slick-columnpicker').each(function(i,e) {
              if (!$(this).is(":checked")) {
                  hiddenColumns.push(columns[i]['id']);
              }
          });

          updateSettings('hidden_columns', {'columns': hiddenColumns});
          
        }
        
        // The context menu (choosing which columns to display) has been opened
        function onHeaderContextMenu(e, ui){

          $('input[id^=columnpicker]', '.slick-columnpicker').change(onColumnsChanged);
  
        }

        function addHeaderFilters() {

            updateHeaderFilters();

            $(grid.getHeaderRow()).delegate(":input", "change keyup",
            function(e) {
                columnFilters[$(this).data("columnId")] = $.trim($(this).val());
                dataView.refresh();
            });

            // Register events for the header inputs
            grid.onColumnsReordered.subscribe(function(e, args) {
                updateHeaderFilters();
            });

            grid.onColumnsResized.subscribe(function(e, args) {
                updateHeaderFilters();
            });

        }

        function updateHeaderFilters() {

            // add the header inputs
            for (var i = 0; i < columns.length; i++) {

                if (columns[i].id !== "selector") {
                    var header = grid.getHeaderRowColumn(columns[i].id);
                    $(header).empty();
                    if (options['columns'][columns[i].id] && options['columns'][columns[i].id]['filter']) {
                        $("<input type='text'>")
                        .data("columnId", columns[i].id)
                        .width($(header).width() - 4)
                        .val(columnFilters[columns[i].id])
                        .appendTo(header);
                    }

                }
            }

        }

        // Basic filter matching characters in beginning of string
        function filter(item) {
            for (var columnId in columnFilters) {
                if (columnId !== undefined && columnFilters[columnId] !== "") {
                    var c = grid.getColumns()[grid.getColumnIndex(columnId)];
                    if (item[c.field].indexOf(columnFilters[columnId]) === -1) {
                        return false;
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
        
        // Delete a row from the dataset
        function deleteRow(row) {
           
           var item = dataView.getItem(row);
           dataView.deleteItem(item.id);
           callback('delete', {'nid': item.id});
           
        }
        
        // All callbacks should be routed through this function
        function callback(op, data){
          
          $result = $('#result');
          $result.empty();
          $result.addClass('saving');
          
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
            url: Drupal.settings.Slickgrid.CallbackPath + '/' + op,
            data: data,
            success: callbackSuccess,
            dataType: "json"
          });

        }
        
        function updateSettings(setting, data){
          
          data.view = Drupal.settings.Slickgrid.View;
          data.setting = setting;
          
          callback('settings', data);
          
        }
        
        function callbackSuccess(response){
          
          $result = $('#result');
          $result.removeClass('saving');
          if(response && response.result){
            $result.append(response.result);
            timeout = setTimeout("$('#result').empty();",3000);
          }
          
        }

        init();

    }

})(jQuery);


