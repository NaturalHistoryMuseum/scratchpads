/***
 * 
 * (c) 2009 Michael Leibman (michael.leibman@gmail.com)
 * All rights reserved.
 * 
 * 
 * TODO:
 * 	- frozen columns
 * 	- built-in row reorder
 * 	- add custom editor options
 * 	- consistent events (EventHelper?  jQuery events?)
 * 	- break resizeCanvas() into two functions to handle container resize and data size changes
 * 	- improve rendering speed by merging column extra cssClass into dynamically-generated .c{x} rules
 * 	- expose row height in the API
 * 	- improve rendering speed by reusing removed row nodes and doing one .replaceChild() instead of two .removeChild() and .appendChild()
 * 
 * KNOWN ISSUES:
 * 	- keyboard navigation doesn't "jump" over unselectable cells for now
 *  - main page must have at least one STYLE element for jQuery Rule to work
 * 
 * 
 * OPTIONS:
 * 	enableAddRow			-	If true, a blank row will be displayed at the bottom - typing values in that row will add a new one.
 * 	manualScrolling			-	Disable automatic rerender on scroll.  Client will take care of calling Grid.onScroll().
 * 	editable				-	If false, no cells will be switched into edit mode.
 * 	editOnDoubleClick		-	Cell will not automatically go into edit mode without being double-clicked.
 * 	enableCellNavigation	-	If false, no cells will be selectable.
 * 	defaultColumnWidth		-	Default column width in pixels (if columns[cell].width is not specified).
 * 	enableColumnReorder		-	Allows the user to reorder columns.
 * 	asyncEditorLoading		-	Makes cell editors load asynchronously after a small delay.
 * 								This greatly increases keyboard navigation speed.
 * 	
 * 
 * COLUMN DEFINITION (columns) OPTIONS:
 * 	id						-	Column ID.
 * 	name					-	Column name to put in the header.
 * 	field					-	Property of the data context to bind to.
 * 	formatter				-	Function responsible for rendering the contents of a cell.
 * 	editor					-	An Editor class.
 * 	validator				-	An extra validation function to be passed to the editor.
 * 	unselectable			-	If true, the cell cannot be selected (and therefore edited).
 * 	cannotTriggerInsert		-	If true, a new row cannot be created from just the value of this cell.
 * 	setValueHandler			-	If true, this handler will be called to set field value instead of context[field].
 * 	width					-	Width of the column in pixels.
 * 	resizable				-	If false, the column cannot be resized.
 * 	minWidth				-	Minimum allowed column width for resizing.
 * 	maxWidth				-	Maximum allowed column width for resizing.
 * 	cssClass				-	A CSS class to add to the cell.
 * 	rerenderOnResize		-	Rerender the column when it is resized (useful for columns relying on cell width or adaptive formatters).
 * 	
 * 
 * EVENTS:
 * 
 * ...
 * 
 * 
 * NOTES:
 * 
 * 	Cell/row DOM manipulations are done directly bypassing jQuery's DOM manipulation methods.
 * 	This increases the speed dramatically, but can only be done safely because there are no event handlers
 * 	or data associated with any cell/row DOM nodes.  Cell editors must make sure they implement .destroy() 
 * 	and do proper cleanup.
 * 
 * 
 * @param {jQuery} $container	Container object to create the grid in.
 * @param {Array} data			An array of objects for databinding.
 * @param {Array} columns		An array of column definitions.
 * @param {Object} options		Grid options.
 * 
 */
function SlickGrid($container,data,columns,options)
{
	// settings
	var defaults = {
		enableAddRow: true,
		manualScrolling: false,
		editable: false,
		editOnDoubleClick: false,
		enableCellNavigation: true,
		defaultColumnWidth: 80,
		enableColumnReorder: false,
		asyncEditorLoading: true,
		fixedFirstColumn: false,
		rowHeight: 24
	};
	
	// consts
	var CAPACITY = 50;
	var BUFFER = 5;  // will be set to equal one page
	
	// private
	var uid = "slickgrid_" + Math.round(1000000 * Math.random());
	var self = this;
	var $divHeadersScroller;
	var $divHeaders;
	var $divSideHeader;
	var $divMainScroller;
	var $divMainData;
	var $divMain;
	var viewportH, viewportW;
		
	var currentRow, currentCell;
	var currentCellNode = null;
	var currentEditor = null;	
	
	var rowsCache = {};
	var renderedRows = 0;
	var numVisibleRows;
	var lastRenderedScrollTop = 0;
	var currentScrollTop = 0;
	var currentScrollLeft = 0;
	var scrollDir = 1;
	var avgRowRenderTime = 10;
	
	var selectedRows = [];
	var selectedRowsLookup = {};
	var columnsById = {};
	
	// async call handles
	var h_editorLoader = null;
	var h_render = null;	
	
	// perf counters
	var counter_rows_rendered = 0;
	var counter_rows_removed = 0;
	
	var fragment = document.createDocumentFragment();
	
	// internal
	var _forceSyncScrolling = false;
	
	// Used for fixed first column
	var $appendTo = $divHeaders;
	var fixedColumnCache = {};
	
	function init() {
		options = $.extend({},defaults,options);
		
		$container
			.empty()
			.attr("tabIndex",0)
			.attr("hideFocus",true)
			.css("overflow","hidden")
			.css("outline",0)
			.css("position","relative")
			.addClass(uid);
		
		$divHeadersScroller = $("<div class='grid-header' style='overflow:hidden;position:relative;' />").appendTo($container);
		$divHeaders = $("<div style='width:10000px' />").appendTo($divHeadersScroller);
		$divMainData = $("<div class='main-data'>").appendTo($container);
		$divMainScroller = $("<div class='main-scroller' tabIndex='0' hideFocus style='width:100%;overflow:scroll;outline:0;position:relative;outline:0px;'>").appendTo($divMainData);
		$divMain = $("<div class='grid-canvas' tabIndex='0' hideFocus />").appendTo($divMainScroller);
		
    if(options.fixedFirstColumn){
		  $divSideHeader = $("<div class='side-header grid-canvas' />").appendTo($container);
		  $divSideHeader.css('margin-top', $divHeadersScroller.outerHeight()+'px');
		  $container.addClass('fixedFirstCol');
    }
		
		
		
		$divMainScroller.height( $container.innerHeight() - $divHeadersScroller.outerHeight() );
		
		for (var i = 0; i < columns.length; i++) 
		{
			var m = columns[i];
			
			columnsById[m.id] = i;
			
			if (!m.width)
				m.width = options.defaultColumnWidth;
				
			if (!m.formatter)
				m.formatter = defaultFormatter;
			
			if(i == 0 && options.fixedFirstColumn){
			  
        updateFixedColumnWidth(m.width);
        
        m.cssClass += ' fixed-column-header';
			  $appendTo = $container;
			  			  
			}else{
			  
			  $appendTo = $divHeaders;
			  
			}
			
			var header = $("<div class='h c" + i + ' ' + m.cssClass + "' cell=" + i + " id='" + m.id + "' />")
       .html(m.name)
       .width(m.width)
       .appendTo($appendTo);
			

				
			// todo:  this is for demo purposes only
			if (m.rerenderOnResize)
				header.append(" <img src='images/help.png' align='absmiddle' title='This column has an adaptive formatter.  Resize to a smaller size to see alternative data representation.'>");
		}
		
		$container.find(".h").each(function() {
			var cell = parseInt($(this).attr("cell"));
			var m = columns[cell];
			
			if (!m.resizable) return;
			
			$(this).resizable({
				handles: "e",
				minWidth: (m.minWidth) ? m.minWidth : null,
				maxWidth: (m.maxWidth) ? m.maxWidth : null,
				stop: function(e, ui) {
					var cellId = $(this).attr("id");
					var cell = columnsById[cellId];
					columns[cell].width = $(this).width();
					$.rule("." + uid + " .grid-canvas .c" + cell, "style").css("width", columns[cell].width + "px");
					
					if(options.fixedFirstColumn && cell == 0){
					  
					  updateFixedColumnWidth($(this).width());
					  
					}
					
					if (self.onColumnsResized)
						self.onColumnsResized(e, ui, $(this).width());
					
					resizeCanvas();
					
					// todo:  rerender single column instead of everything
					if (columns[cell].rerenderOnResize)
						removeAllRows();
					
					render();
				}
			});
		});
		
		
		
		// ignore .ui-resizable-handle to prevent sortable interfering with resizable
    if (options.enableColumnReorder){

     var a;

     $divHeaders.sortable({
         axis:"x", 
         containment: $divHeadersScroller,
         scroll: true,
         cancel:".ui-resizable-handle",
         
         start: function (event, ui) { 
         
           if (self.onColumnsReorderStart)
             self.onColumnsReorderStart(event, ui);
           
         },
         
         sort: function(event, ui) { 
           
           var rightScrollPos = $divMainScroller.width() + $divMainScroller.scrollLeft() - 35;
           
           var scrollMax = $divMain.width() - $divMainScroller.width(); 
         
           var positionRight = ui.item.width() + ui.position.left;

           if(positionRight > rightScrollPos){
             
             $('.main-scroller').animate({scrollLeft:scrollMax}, 700);
             
           }else if(ui.position.left < ($divMainScroller.scrollLeft() + 20)){
             
             $('.main-scroller').animate({scrollLeft:0}, 700);
             
           }else{
             
             $('.main-scroller').stop();
             
           }
             
        },
        
        deactivate: function(event, ui){
      
          $('.main-scroller').stop();
          
        },
      
         update: function(e,ui) {
            
            var newOrder = $divHeaders.sortable("toArray");
            
            // If there's a fixed first column, it won't be part of newOrder
            if(options.fixedFirstColumn){
               newOrder.unshift(columns[0]['id']);
            }
            
            var lookup = {};
            for (var i=0; i<columns.length; i++)
            {
              lookup[columns[i].id] = columns[i];
            }
            
            for (var i=0; i<newOrder.length; i++)
            {
              columnsById[newOrder[i]] = i;
              columns[i] = lookup[newOrder[i]];
            }
            
                 removeAllRows();
                 removeCssRules();
                 createCssRules();
                 render();
            
            if (self.onColumnsReordered)
              self.onColumnsReordered(e, ui);
              
            e.stopPropagation();
              
            console.timeEnd("column reorder");        
          }
        });
        
      }
  	
		$divHeaders.bind("click", function(e) {
			if (!$(e.target).hasClass(".h")) return;
			
			var id = $(e.target).attr("id");
			
			if (self.onColumnHeaderClick)
				self.onColumnHeaderClick(columns[columnsById[id]]);
		});	
		
		
		createCssRules();
		resizeCanvas();
		render();
		
		

		if (!options.manualScrolling)
			$divMainScroller.bind("scroll", handleScroll);
			
    $divMain.bind("keydown", handleKeyDown);
    $divMain.bind("click", handleClick);
    $divMain.bind("dblclick", handleDblClick);

    if(options.fixedFirstColumn){
      
      $divSideHeader.bind("keydown", handleKeyDown);
      $divSideHeader.bind("click", handleClick);
      $divSideHeader.bind("dblclick", handleDblClick);

    }
    
		if ($.browser.msie) 
			$divMainScroller[0].onselectstart = function() {
				if (event.srcElement.tagName != "INPUT" && event.srcElement.tagName != "TEXTAREA") 
					return false; 
				};
	}
	
	function createCssRules() {
		for (var i = 0; i < columns.length; i++) {
			$.rule("." + uid + " .grid-canvas .c" + i + " { width:" + columns[i].width + "px }").appendTo("style");
		}
	}
	
	function removeCssRules() {
		for (var i = 0; i < columns.length; i++) {
			$.rule("." + uid + " .grid-canvas .c" + i, "style").remove();
		}
	}
		
	function destroy() {
		if (currentEditor)
			cancelCurrentEdit();
		
		$divHeaders.sortable("destroy");
		$divHeaders.find(".h").resizable("destroy");
		
		removeCssRules();
		
		$container.empty().removeClass(uid);
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// General
	
	function setColumnHeaderCssClass(id,classesToAdd,classesToRemove) {
		$divHeaders.find(".h[id=" + id + "]").removeClass(classesToRemove).addClass(classesToAdd);
	}
	
	function getColumnIndex(id) {
		return columnsById[id];	
	}

	function getSelectedRows() {
		return selectedRows.concat();
	}	

	function setSelectedRows(rows) {
		if (GlobalEditorLock.isEditing() && !GlobalEditorLock.hasLock(self))
			throw "Grid : setSelectedRows : cannot set selected rows when somebody else has an edit lock";
		
		var lookup = {};
		for (var i=0; i<rows.length; i++)
			lookup[rows[i]] = true;
		
		// unselect old rows
		for (var i=0; i<selectedRows.length; i++)
		{
			var row = selectedRows[i];
			if (rowsCache[row] && !lookup[row])
				$(rowsCache[row]).removeClass("selected");
		}

		// select new ones
		for (var i=0; i<rows.length; i++)
		{
			var row = rows[i];
			if (rowsCache[row] && !selectedRowsLookup[row])
				$(rowsCache[row]).addClass("selected");
		}

		selectedRows = rows.concat();
		selectedRowsLookup = lookup;				
	}

	function setOptions(args) {
		if (currentEditor && !commitCurrentEdit())
			return;
		
		setSelectedCell(null);
		
		if (options.enableAddRow != args.enableAddRow)
			removeRow(data.length);
			
		options = $.extend(options,args);		
		
		render();
	}
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Rendering / Scrolling

	function defaultFormatter(row, cell, value, columnDef, dataContext) { 
		return (value == null || value == undefined) ? "" : value;
	}

	function appendRowHtml(stringArray,row) {
		var d = data[row];
		var dataLoading = row < data.length && !d;
		var css = "r" + (dataLoading ? " loading" : "") + (selectedRowsLookup[row] ? " selected" : "");
		var placeholder;
		
		stringArray.main.push("<div class='" + css + "' row='" + row + "' style='top:" + (options.rowHeight*row) + "px'>");
		
		if(options.fixedFirstColumn){		  
		stringArray.fixedCol.push("<div class='" + css + "' row='" + row + "' style='top:" + (options.rowHeight*row) + "px'>");  		  
		}
		
		for (var i= 0, cols=columns.length; i<cols; i++) 
		{
		  
		  if(options.fixedFirstColumn && i == 0){	
		    placeholder = stringArray.fixedCol;
		  }else{
		    placeholder = stringArray.main;
		  }
		  
			var m = columns[i];

			placeholder.push("<div " + (m.unselectable ? "" : "hideFocus tabIndex=0 ") + "class='c c" + i + (m.cssClass ? " " + m.cssClass : "") + "' cell=" + i + " style=\"height: "+(options.rowHeight - 5)+"px\">");

			// if there is a corresponding row (if not, this is the Add New row or this data hasn't been loaded yet)				
			if (d && row < data.length)
				placeholder.push(m.formatter(row, i, d[m.field], m, d));
			
			placeholder.push("</div>");
		}
		
		stringArray.main.push("</div>");
		
		if(options.fixedFirstColumn){		  
		stringArray.fixedCol.push("</div>"); 
		}
				
	}
	
	
	function getRowHtml(row) {
		var html = [];
		
		appendRowHtml(html,row);
		
		return html.join("");
	}
	
	function cleanupRows(visibleFrom,visibleTo) {
    console.time("cleanupRows");
    
    var rowsBefore = renderedRows;
    var parentNode = $divMain[0];
      
        for (var i in rowsCache)
        {
         if ((i < visibleFrom || i > visibleTo) && i != currentRow)
         {
           parentNode.removeChild(rowsCache[i]);
           
           if(options.fixedFirstColumn){
              $divSideHeader[0].removeChild(fixedColumnCache[i]);
              delete fixedColumnCache[i];
            }
           
           delete rowsCache[i];
           renderedRows--;   
           
           counter_rows_removed++; 
         }
        }
    
    console.log("removed " + (rowsBefore - renderedRows) + " rows");
    console.timeEnd("cleanupRows");
	}
	
	function removeAllRows() {
		console.log("removeAllRows")
		$divMain[0].innerHTML = "";
		rowsCache= {};
		
		if(options.fixedFirstColumn){
		  $divSideHeader[0].innerHTML = "";
		  fixedColumnCache= {};
		}
		
		counter_rows_removed += renderedRows;
		renderedRows = 0;
	}	

	function removeRow(row) {
	  
		var node = rowsCache[row];
		if (!node) return;
		
		if (currentEditor && currentRow == row)
			throw "Grid : removeRow : Cannot remove a row that is currently in edit mode";	
		
		// if we're removing rows, we're probably not scrolling
		scrollDir = 0;
		
    node.parentNode.removeChild(node);
    node = null;
       
    delete rowsCache[row];
    
    
    if(options.fixedFirstColumn){
      
      var fixedColNode = fixedColumnCache[row];
      fixedColNode.parentNode.removeChild(fixedColNode);
      fixedColNode = null;
      delete fixedColNode[row];

		}
    	
		renderedRows--;
		
		counter_rows_removed++;
		
	}
	
	function removeRows(rows) {
    console.time("removeRows");
    
    if (!rows || !rows.length) return;
    
    scrollDir = 0;
    var nodes = [];
    var fixedColNodes = [];
    
    for (var i=0, rl=rows.length; i<rl; i++) {
     if (currentEditor && currentRow == i)
       throw "Grid : removeRow : Cannot remove a row that is currently in edit mode";  
     
     var node = rowsCache[rows[i]];
     
     if (!node) continue;
     
     nodes.push(rows[i]);
     
     fixedColNodes.push(rows[i]);
     
    }
    
    if (renderedRows > 10 && nodes.length == renderedRows) {
     $divMain[0].innerHTML = "";
     $divSideHeader[0].innerHTML = "";
     rowsCache= {};
     fixedColumnCache = {};
     counter_rows_removed += renderedRows;
     renderedRows = 0;
    } else {
     for (var i=0, nl=nodes.length; i<nl; i++) {
       var node = rowsCache[nodes[i]];
       node.parentNode.removeChild(node);
       delete rowsCache[nodes[i]];
       
       var fixedColNode = fixedColumnCache[fixedColNodes[i]];
       fixedColNode.parentNode.removeChild(fixedColNode);
       delete fixedColumnCache[fixedColNodes[i]];
       
       renderedRows--;
       counter_rows_removed++;
     }
    }
    
    console.timeEnd("removeRows");
	}
	
	function updateCell(row,cell) {
		if (!rowsCache[row]) return;
		var $cell = $(rowsCache[row]).find(".c[cell=" + cell + "]");
		if ($cell.length == 0) return;
		
		var m = columns[cell];	
		var d = data[row];	
		
		if (currentEditor && currentRow == row && currentCell == cell)
			currentEditor.setValue(d[m.field]);
		else 
			$cell[0].innerHTML = d ? m.formatter(row, cell, d[m.field], m, d) : ""
	}

	function updateRow(row) {
		if (!rowsCache[row]) return;
		
		// todo:  perf:  iterate over direct children?
		$(rowsCache[row]).find(".c").each(function(i) {
			var m = columns[i];
			
			if (row == currentRow && i == currentCell && currentEditor)
				currentEditor.setValue(data[currentRow][m.field]);
			else if (data[row])
				this.innerHTML = m.formatter(row, i, data[row][m.field], m, data[row]);
			else
				this.innerHTML = "";
		});
	}

	function resizeCanvas() {
		viewportW = $divMainScroller.innerWidth();
		viewportH = $divMainScroller.innerHeight();
	    
		BUFFER = numVisibleRows = Math.ceil(viewportH / options.rowHeight);
		CAPACITY = Math.max(CAPACITY, numVisibleRows + 2*BUFFER);

		var totalWidth = 0;
		for (var i=0; i<columns.length; i++)
		{
			if(options.fixedFirstColumn && i == 0 && columns.length > 1){
			 continue; 
			}  
			totalWidth += (columns[i].width + 5);
		}
		$divMain.width(totalWidth);
	  
	  	// remove the rows that are now outside of the data range
		// this helps avoid redundant calls to .removeRow() when the size of the data decreased by thousands of rows
		var parentNode = $divMain[0];
		var l = options.enableAddRow ? data.length : data.length - 1;
		for (var i in rowsCache)
		{
			if (i >= l)
			{
        parentNode.removeChild(rowsCache[i]);
        delete rowsCache[i];
        
        if(options.fixedFirstColumn){
                 $divSideHeader[0].removeChild(fixedColumnCache[i]);
                 delete fixedColumnCache[i];
               }
				
				renderedRows--;
				
				counter_rows_removed++;
			}
		}
		
	    var newHeight = Math.max(options.rowHeight * (data.length + numVisibleRows - 2) - (viewportH  - $.getScrollbarWidth() - 20), viewportH - $.getScrollbarWidth());
		
        // browsers sometimes do not adjust scrollTop/scrollHeight when the height of contained objects changes
		if ($divMainScroller.scrollTop() > newHeight - $divMainScroller.height() + $.getScrollbarWidth()) {
			$divMainScroller.scrollTop(newHeight - $divMainScroller.height() + $.getScrollbarWidth());
		}
		$divMain.height(newHeight);
		
		if(options.fixedFirstColumn){
		  $divSideHeader.height(newHeight);
		}		
		
	}
	
	function getViewport()
	{
		return {
			top:	Math.floor(currentScrollTop / options.rowHeight),
			bottom:	Math.floor((currentScrollTop + viewportH) / options.rowHeight)
		};	
	}
	
	function renderRows(from,to) {
		console.time("renderRows");
		
		var parentNode = $divMain[0];
		
		var rowsBefore = renderedRows;
		var stringArray = {
  		main: [],
  		fixedCol: []
  	};
		var rows =[];
		var _start = new Date();
		
		for (var i = from; i <= to; i++) {
      if (rowsCache[i]) continue;
			renderedRows++;
			
			rows.push(i);
			appendRowHtml(stringArray,i);
			counter_rows_rendered++;
			
		}

		
		var x = document.createElement("div");
		x.innerHTML = stringArray.main.join("");
		
    if(options.fixedFirstColumn){
      
      var y = document.createElement("div");
  		y.innerHTML = stringArray.fixedCol.join("");

		}
		
    for (var i = 0, l = x.childNodes.length; i < l; i++){
      
      if(options.fixedFirstColumn){
      fixedColumnCache[rows[i]] = $divSideHeader[0].appendChild(y.firstChild);
      }
    
      rowsCache[rows[i]] = parentNode.appendChild(x.firstChild);
      
    } 

		
		if (renderedRows - rowsBefore > 5)
			avgRowRenderTime = (new Date() - _start) / (renderedRows - rowsBefore);
		
		console.log("rendered " + (renderedRows - rowsBefore) + " rows");
		console.timeEnd("renderRows");		
	}	
	
	function render() {
		var vp = getViewport();
		var from = Math.max(0, vp.top - (scrollDir >= 0 ? 5 : BUFFER));
		var to = Math.min(options.enableAddRow ? data.length : data.length - 1, vp.bottom + (scrollDir > 0 ? BUFFER : 5));
		
		if (renderedRows > 10 && Math.abs(lastRenderedScrollTop - currentScrollTop) > options.rowHeight*CAPACITY)
			removeAllRows();
		else //if (renderedRows >= CAPACITY)
			cleanupRows(from,to);

		renderRows(from,to);		
		
		lastRenderedScrollTop = currentScrollTop;
		h_render = null;
	}

	function handleScroll() {
	  
		currentScrollTop = $divMainScroller[0].scrollTop;
		var scrollDistance = Math.abs(lastRenderedScrollTop - currentScrollTop);
		var scrollLeft = $divMainScroller[0].scrollLeft;
		
		if (scrollLeft != currentScrollLeft)
			$divHeadersScroller[0].scrollLeft = currentScrollLeft = scrollLeft;

		// Moved event to here so will always be called
	  if (self.onViewportChanged)
      self.onViewportChanged();
		
		if (scrollDistance < 5*options.rowHeight) return;
		
		if (lastRenderedScrollTop == currentScrollTop)
			scrollDir = 0;
		else if (lastRenderedScrollTop < currentScrollTop)
			scrollDir = 1;
		else	
			scrollDir = -1;
		
		if (h_render)
			window.clearTimeout(h_render);

		if (scrollDistance < numVisibleRows*options.rowHeight) // || avgRowRenderTime*CAPACITY < 30 ||  _forceSyncScrolling) 
			render();
		else
			h_render = window.setTimeout(render, 50);
			

	}


	//////////////////////////////////////////////////////////////////////////////////////////////
	// Interactivity

	function handleKeyDown(e) {

		switch (e.which) {
			case 27:  // esc
				if (GlobalEditorLock.isEditing() && GlobalEditorLock.hasLock(self))
					cancelCurrentEdit(self);
				
				if (currentCellNode)
					currentCellNode.focus();
				
				break;
			
			case 9:  // tab
				if (e.shiftKey)
					gotoDir(0,-1,true);	//gotoPrev();
				else
					gotoDir(0,1,true);	//gotoNext();
					
				break;
				
			case 37:  // left
				gotoDir(0,-1);
				break;
				
			case 39:  // right			
				gotoDir(0,1);
				break;
				
			case 38:  // up
				gotoDir(-1,0);
				break;
				
			case 40:  // down
			case 13:  // enter
				gotoDir(1,0);
				break;
								
			default:

				// do we have any registered handlers?
				if (self.onKeyDown && data[currentRow])
				{
					// grid must not be in edit mode
					if (!currentEditor) 
					{
						// handler will return true if the event was handled
						if (self.onKeyDown(e, currentRow, currentCell)) 
						{
							e.stopPropagation();
							e.preventDefault();
							return false;
						}
					}
				}			
			
				// exit without cancelling the event
				return;
		}
		
		e.stopPropagation();
		e.preventDefault();
		return false;		
	}	
	
	function handleClick(e)
	{
		var $cell = $(e.target).closest(".c");
		
		if ($cell.length == 0) return;
		
		// are we editing this cell?
		if (currentCellNode == $cell[0] && currentEditor != null) return;
		
		var row = parseInt($cell.parent().attr("row"));
		var cell = parseInt($cell.attr("cell"));		
	
		var validated = null;
	
		// do we have any registered handlers?
		if (data[row] && self.onClick)
		{
			// grid must not be in edit mode
			if (!currentEditor || (validated = commitCurrentEdit())) 
			{
				// handler will return true if the event was handled
        if (self.onClick(e, row, cell)) 
        {
         e.stopPropagation();
         e.preventDefault();
         return false;
        }
			}
		}


		if (options.enableCellNavigation && !columns[cell].unselectable) 
		{
			// commit current edit before proceeding
			if (validated == true || (validated == null && commitCurrentEdit())) 
				setSelectedCellAndRow($cell[0]);
		}
	}
	
	function handleDblClick(e)
	{
		var $cell = $(e.target).closest(".c");
		
		if ($cell.length == 0) return;
		
		// are we editing this cell?
		if (currentCellNode == $cell[0] && currentEditor != null) return;
				
		if (options.editOnDoubleClick)
			makeSelectedCellEditable();
	}

	function getCellFromPoint(x,y) {
		var row = Math.floor(y/options.rowHeight);
		var cell = 0;
		
		var w = 0;		
		for (var i=0; i<columns.length && w<y; i++)
		{
			w += columns[i].width;
			cell++;
		}
		
		return {row:row,cell:cell-1};
	}


	//////////////////////////////////////////////////////////////////////////////////////////////
	// Cell switching
	
	function setSelectedCell(newCell,async)
	{
		if (currentCellNode != null) 
		{
			makeSelectedCellNormal();			
			
			$(currentCellNode).removeClass("selected");
		}
		
		currentCellNode = newCell;
		
		if (currentCellNode != null) 
		{
			currentRow = parseInt($(currentCellNode).parent().attr("row"));
			currentCell = parseInt($(currentCellNode).attr("cell"));
			
			$(currentCellNode).addClass("selected");
			
			scrollSelectedCellIntoView();
			
			if (options.editable && !options.editOnDoubleClick && (data[currentRow] || currentRow == data.length)) 
			{
				window.clearTimeout(h_editorLoader);
				
				if (async) 
					h_editorLoader = window.setTimeout(makeSelectedCellEditable, 100);
				else 
					makeSelectedCellEditable();
			}
		}
		else
		{
			currentRow = null;
			currentCell = null;	
		}
	}
	
	function setSelectedCellAndRow(newCell,async) {
		setSelectedCell(newCell,async);
		
		if (newCell) 
			setSelectedRows([currentRow]);
		else
			setSelectedRows([]);
			
		if (self.onSelectedRowsChanged)
			self.onSelectedRowsChanged();			
	}
	
	function clearTextSelection()
	{
		if (document.selection && document.selection.empty) 
			document.selection.empty();
		else if (window.getSelection) 
		{
			var sel = window.getSelection();
			if (sel && sel.removeAllRanges) 
				sel.removeAllRanges();
		}
	}	

	function isCellPotentiallyEditable(row,cell) {
		// is the data for this row loaded?
		if (row < data.length && !data[row])
			return false;
		
		// are we in the Add New row?  can we create new from this cell?
		if (columns[cell].cannotTriggerInsert && row >= data.length)
			return false;
			
		// does this cell have an editor?
		if (!columns[cell].editor)
			return false;
			
		return true;		
	}

	function makeSelectedCellNormal() {
		if (!currentEditor) return;
					
		currentEditor.destroy();
		$(currentCellNode).removeClass("editable invalid");
		
		if (data[currentRow]) 
			currentCellNode.innerHTML = columns[currentCell].formatter(currentRow, currentCell, data[currentRow][columns[currentCell].field], columns[currentCell], data[currentRow]);
		
		currentEditor = null;
		
		// if there previously was text selected on a page (such as selected text in the edit cell just removed),
		// IE can't set focus to anything else correctly
		if ($.browser.msie) clearTextSelection();

		GlobalEditorLock.leaveEditMode(self);		
	}

	function makeSelectedCellEditable()
	{
		if (!currentCellNode) return;
		
		if (!options.editable)
			throw "Grid : makeSelectedCellEditable : should never get called when options.editable is false";
		
		// cancel pending async call if there is one
		window.clearTimeout(h_editorLoader);
		
		if (!isCellPotentiallyEditable(currentRow,currentCell))
			return;

		GlobalEditorLock.enterEditMode(self);

		$(currentCellNode).addClass("editable");
		
		var value = null;
	
		// if there is a corresponding row
		if (data[currentRow])
			value = data[currentRow][columns[currentCell].field];

		currentCellNode.innerHTML = "";
		
		currentEditor = new columns[currentCell].editor($(currentCellNode), columns[currentCell], value, data[currentRow]);
	}

	function scrollSelectedCellIntoView() {
		if (!currentCellNode) return;
		
		var scrollTop = $divMainScroller[0].scrollTop;
		
		// need to page down?
		if ((currentRow + 2) * options.rowHeight > scrollTop + viewportH) 
		{
			$divMainScroller[0].scrollTop = (currentRow ) * options.rowHeight;
			
			handleScroll();
		}
		// or page up?
		else if (currentRow * options.rowHeight < scrollTop)
		{
			$divMainScroller[0].scrollTop = (currentRow + 2) * options.rowHeight - viewportH;
			
			handleScroll();			
		}	
	}

	function gotoDir(dy, dx, rollover)
	{
		if (!currentCellNode) return;
		if (!options.enableCellNavigation) return;		
		if (!GlobalEditorLock.commitCurrentEdit()) return;
		
		var nextRow = rowsCache[currentRow + dy];
		var nextCell = nextRow ? $(nextRow).find(".c[cell=" + (currentCell + dx) + "][tabIndex=0]") : null;
		
		if (rollover && dy == 0 && !(nextRow && nextCell && nextCell.length))
		{
			if (!nextCell || !nextCell.length)
			{
				if (dx > 0) 
				{
					nextRow = rowsCache[currentRow + dy + 1];
					nextCell = nextRow ? $(nextRow).find(".c[cell][tabIndex=0]:first") : null;						
				}
				else
				{
					nextRow = rowsCache[currentRow + dy - 1];
					nextCell = nextRow ? $(nextRow).find(".c[cell][tabIndex=0]:last") : null;		
				}
			}
		}
		

	  var numCols = columns.length - 1;
	  
	  var numRows = data.length - 1;
	  
		
    if(!nextRow){ // User is at top or bottom of col moving up or down

      if(currentRow == 0){ //At the top

        // We are at the top left
        if((options.fixedFirstColumn && currentCell == 1) || currentCell == 0){
          
          gotoCell(numRows, numCols);
          
        }else{
          
          gotoCell(numRows, (currentCell -1));
          
        }

      }else{

        if(numRows == currentRow && currentCell == numCols){
          
          // We are the far bottom right          
          if(options.fixedFirstColumn){
            
            gotoCell(0, 1);
            
          }else{
            
            gotoCell(0, 0);
            
          }
          
          
        }else{
          
          gotoDir((0 - numRows), 1); 
          
        }
        
      }
      
    }else if (!nextCell.length){
			
			if(options.fixedFirstColumn){
			  var gotoCol = numCols - 1;
			}else{
			  var gotoCol = numCols;
			}
			
		  if(currentCell == 1){ //At the top

        gotoDir(-1, gotoCol);
        
      }else{
        
        gotoDir(1, (0 - gotoCol)); 
        
      }
		
		}
		
		if (nextRow && nextCell && nextCell.length) 
		{
			setSelectedCellAndRow(nextCell[0],options.asyncEditorLoading);
			
			// if no editor was created, set the focus back on the cell
			if (!currentEditor) 
				currentCellNode.focus();
		}
		else 
			currentCellNode.focus();
	}

	function gotoCell(row,cell)
	{
		if (row > data.length || row < 0 || cell >= columns.length || cell < 0) return;
		if (!options.enableCellNavigation || columns[cell].unselectable) return;
		
		if (!GlobalEditorLock.commitCurrentEdit()) return;
		
		if (!rowsCache[row])
			renderRows(row,row);
		
		var cell = $(rowsCache[row]).find(".c[cell=" + cell + "][tabIndex=0]")[0];
		
		setSelectedCellAndRow(cell);
		
		// if no editor was created, set the focus back on the cell
		if (!currentEditor) 
			currentCellNode.focus();
	}


	//////////////////////////////////////////////////////////////////////////////////////////////
	// IEditor implementation for GlobalEditorLock	
	
	function commitCurrentEdit() {
		if (currentEditor)
		{
			if (currentEditor.isValueChanged())
			{
				var validationResults = currentEditor.validate();
				
				if (validationResults.valid) 
				{
					var value = currentEditor.getValue();
					
					if (currentRow < data.length) {
						if (columns[currentCell].setValueHandler) {
							makeSelectedCellNormal();
							columns[currentCell].setValueHandler(value, columns[currentCell], data[currentRow]);
						}
						else {
              data[currentRow][columns[currentCell].field] = value;
							makeSelectedCellNormal();
						}
					}
					else if (self.onAddNewRow) {
							makeSelectedCellNormal();
							self.onAddNewRow(columns[currentCell], value);
						}
					
					return true;
				}
				else 
				{
					$(currentCellNode).addClass("invalid");
					$(currentCellNode).stop(true,true).effect("highlight", {color:"red"}, 300);
					
					if (self.onValidationError)
						self.onValidationError(currentCellNode, validationResults, currentRow, currentCell, columns[currentCell]);
					
					currentEditor.focus();
					return false;
				}
			}
			
			makeSelectedCellNormal();
		}
		
		
		return true;
	};
	
	function cancelCurrentEdit() {
		makeSelectedCellNormal();
	};
	
	function updateFixedColumnWidth(width){
    
    width = width + 2;
    
	  $divHeadersScroller.css('margin-left', width+'px');
	  $divMainData.css('margin-left', width+'px');
	  $.rule($divSideHeader, "style").css("width", width + "px");
	  
	};
	
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Debug
	
	this.debug = function() {
		var s = "";
		
		s += ("\n" + "counter_rows_rendered:  " + counter_rows_rendered);	
		s += ("\n" + "counter_rows_removed:  " + counter_rows_removed);	
		s += ("\n" + "renderedRows:  " + renderedRows);	
		s += ("\n" + "numVisibleRows:  " + numVisibleRows);			
		s += ("\n" + "CAPACITY:  " + CAPACITY);			
		s += ("\n" + "BUFFER:  " + BUFFER);		
		s += ("\n" + "avgRowRenderTime:  " + avgRowRenderTime);
		
		alert(s);
	};
	
	this.benchmark_render_200 = function() {
		removeAllRows();
		
		// render 200 rows in the viewport
		renderRows(0, 200);
		
		cleanupRows();
	};
	
	this.stressTest = function() {
		console.time("benchmark-stress");

		renderRows(0,500);
		
		cleanupRows();
		
		console.timeEnd("benchmark-stress");
		
		window.setTimeout(self.stressTest, 50);
	};
	
	this.benchmarkFn = function(fn) {
		var s = new Date();
		
		var args = new Array(arguments);
		args.splice(0,1);
		
		self[fn].call(this,args);
		
		alert("Grid : benchmarkFn : " + fn + " : " + (new Date() - s) + "ms");		
	};	
	



	init();	
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Public API
	
	$.extend(this, {
		// Events
		"onColumnHeaderClick":	null,
		"onClick":			null,
		"onKeyDown":		null,
		"onAddNewRow":		null,
		"onValidationError":	null,
		"onViewportChanged":	null,
		"onSelectedRowsChanged":	null,
		"onColumnsReordered":	null,
		"onColumnsReorderStart":	null,
		"onColumnsResized":	null,
		
		// Methods
		"destroy":			destroy,
		"getColumnIndex":	getColumnIndex,
		"updateCell":		updateCell,
		"updateRow":		updateRow,
		"removeRow":		removeRow,
		"removeRows":		removeRows,
		"removeAllRows":	removeAllRows,
		"render":			render,
		"getViewport":		getViewport,
		"resizeCanvas":		resizeCanvas,
		"scroll":			scroll,  // TODO
		"getCellFromPoint":	getCellFromPoint,
		"gotoCell":			gotoCell,
		"editCurrentCell":	makeSelectedCellEditable,
		"getSelectedRows":	getSelectedRows,
		"setSelectedRows":	setSelectedRows,
		"setColumnHeaderCssClass":	setColumnHeaderCssClass,
		
		// IEditor implementation
		"commitCurrentEdit":	commitCurrentEdit,
		"cancelCurrentEdit":	cancelCurrentEdit
	});	
}
