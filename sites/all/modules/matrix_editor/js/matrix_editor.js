
/* Validate required field */

function requiredFieldValidator(value) {
	if (value == null || value == undefined || !value.length)
		return {valid:false, msg:"This is a required field"};
	else	
		return {valid:true, msg:null};
}

/* BTODO: Turn this into a function for checking nde - data. ANd move to nde module */
var TaskNameFormatter = function(row, cell, value, columnDef, dataContext) {

    var spacer = "<span style=\'display:inline-block;height:1px;width:" + (15 * dataContext["indent"]) + "px\'></span>";
    var idx = dataView.getIdxById(dataContext.id);
	if (data[idx+1] && data[idx+1].indent > data[idx].indent) {
		if (dataContext._collapsed)
			return spacer + " <span class=\'toggle expand\'></span>&nbsp;" + value;
		else
			return spacer + " <span class=\'toggle collapse\'></span>&nbsp;" + value;
	}				
	else
		return spacer + " <span class=\'toggle\'></span>&nbsp;" + value;
};

// Extended to allow hiding items
function matrixFilter(item) {
	if (item["percentComplete"] < percentCompleteThreshold) 
		return false;
		
	if (searchString != "" && item["taxa"].indexOf(searchString) == -1)
		return false;
		
	var idx = dataView.getIdxById(item.id);

	if (item.parent != null) {
		var parent = data[item.parent];
		
		while (parent) {
		  
			if (parent._collapsed || (parent["percentComplete"] < percentCompleteThreshold) || (searchString != "" && parent["taxa"].indexOf(searchString) == -1) ) 
        return false;
			
			parent = data[parent.parent];
		}
	}

	if(item._hidden &! showHiddenRows){	  
	    return false;	  
	}
		
	return true;
}

function percentCompleteSort(a,b) {
	return a["percentComplete"] - b["percentComplete"];
}

// Need post callbacks for this
function updateItem(value,columnDef,item) {
  item[columnDef.field] = value;
  dataView.updateItem(item.id,item);
}

function addItem(columnDef,value) {
	var item = {"id": "new_" + (Math.round(Math.random()*10000)), "indent":0, "title":"New task", "duration":"1 day", "percentComplete":0, "start":"01/01/2009", "finish":"01/01/2009", "effortDriven":false};
	item[columnDef.field] = value;
	dataView.addItem(item);
}


var dataView;
var grid;
var percentCompleteThreshold = 0;
var searchString = "";
var indent = 0;
var parents = [];






// Added by matrix editor module 
function init_matrix_editor(columns, data, options, filter){
    
  // initialize the model
	dataView = new DataView();
	dataView.beginUpdate();
	dataView.setItems(data);
	dataView.setFilter(filter);
	dataView.endUpdate();
	
	// initialize the grid
	grid = new SlickGrid($("#myGrid"), dataView.rows, columns, options);
	
	grid.onAddNewRow = addItem;
			
	grid.onClick = function(e,row,cell) {

		if ($(e.target).hasClass("toggle")) {
			
			var item = dataView.rows[row];
			if (item) {
				if (!item._collapsed){
				  item._collapsed = true;
				}else{
				  item._collapsed = false;
				}
	
        dataView.updateItem(item.id,item);           
			}
			
			return true;
		}
		
		return false;
	}
	
	
	// wire up model events to drive the grid
	dataView.onRowCountChanged.subscribe(function(args) {
		grid.resizeCanvas();
	});
	
	dataView.onRowsChanged.subscribe(function(rows) {
		grid.removeRows(rows);
		grid.render();
	});
	
	// wire up the slider to apply the filter to the model
	$("#pcSlider").slider({
		"range":	"min",
		"slide":	function(event,ui) {
			if (GlobalEditorLock.isEditing()) 
				GlobalEditorLock.cancelCurrentEdit();
			
			percentCompleteThreshold = ui.value;
			dataView.refresh();
		}
	});
	
	
	// wire up the search textbox to apply the filter to the model
	$("#txtSearch").keyup(function(e) {
		if (GlobalEditorLock.isEditing()) 
			GlobalEditorLock.cancelCurrentEdit();
					
		// clear on Esc			
		if (e.which == 27) 
			this.value = "";
		
		searchString = this.value;
		dataView.refresh();
	});
  
}



function initMatrixEditor(){
    
  // initialize the model
	dataView = new DataView();
	dataView.beginUpdate();
	dataView.setItems(data);
	dataView.setFilter(filter);
	dataView.endUpdate();
	
	// initialize the grid
	grid = new SlickGrid($("#myGrid"), dataView.rows, columns, options);
	
	grid.onAddNewRow = addItem;
			
	grid.onClick = function(e,row,cell) {

		if ($(e.target).hasClass("toggle")) {
			
			var item = dataView.rows[row];
			if (item) {
				if (!item._collapsed){
				  item._collapsed = true;
				}else{
				  item._collapsed = false;
				}
	
        dataView.updateItem(item.id,item);           
			}
			
			return true;
		}
		
		return false;
	}
	
	
	// wire up model events to drive the grid
	dataView.onRowCountChanged.subscribe(function(args) {
		grid.resizeCanvas();
	});
	
	dataView.onRowsChanged.subscribe(function(rows) {
		grid.removeRows(rows);
		grid.render();
	});
	
	// wire up the slider to apply the filter to the model
	$("#pcSlider").slider({
		"range":	"min",
		"slide":	function(event,ui) {
			if (GlobalEditorLock.isEditing()) 
				GlobalEditorLock.cancelCurrentEdit();
			
			percentCompleteThreshold = ui.value;
			dataView.refresh();
		}
	});
	
	
	// wire up the search textbox to apply the filter to the model
	$("#txtSearch").keyup(function(e) {
		if (GlobalEditorLock.isEditing()) 
			GlobalEditorLock.cancelCurrentEdit();
					
		// clear on Esc			
		if (e.which == 27) 
			this.value = "";
		
		searchString = this.value;
		dataView.refresh();
	});
  
}









