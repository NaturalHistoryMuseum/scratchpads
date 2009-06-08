
/* Validate required field */

function requiredFieldValidator(value) {
	if (value == null || value == undefined || !value.length)
		return {valid:false, msg:"This is a required field"};
	else	
		return {valid:true, msg:null};
}

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


function initMatrixEditor(){
    
  // initialize the model
	dataView = new DataView();
	dataView.beginUpdate();
	dataView.setItems(data);
	
	if(typeof filter != 'undefined'){
	  dataView.setFilter(filter);
	}
	
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
	
	grid.onColumnsResized = function(e, ui){
	  
	  if(ui.originalSize.width != $(ui.element).width()){

      args = {
        width: $(ui.element).width(),
        field: ui.element.attr('id'),
        view: Drupal.settings.matrixEditorViewName
      };
            
      $.post(
        Drupal.settings.matrixEditorCallbackPath+'/column_resized', 
        args 
      );
      
    };  

	};
	
	grid.onViewportResized = function(height){
	  
	  args = {
      height: height,
      view: Drupal.settings.matrixEditorViewName
    };
          
    $.post(
      Drupal.settings.matrixEditorCallbackPath+'/viewport_resized', 
      args 
    );
	  
	};
	
	grid.onColumnsReordered = function(e, ui){

    var args = 'view='+Drupal.settings.matrixEditorViewName;

    if($('#myGrid').hasClass('.fixedFirstCol')){

      args += '&cols[]='+$('div.h.fixed-column-header').attr('id');
      
    }

    $('.ui-sortable .h').each(function(i){
      
      args += '&cols[]='+$(this).attr('id');
      
    });

    $.post(
      Drupal.settings.matrixEditorCallbackPath+'/reorder_columns', 
      args
    );

  };
	
	// Scroll headers with view port
  $('#myGrid div.main-scroller').scroll(function(e){
    
    var top = parseInt("-"+e.target.scrollTop);
    $('.side-header').css("top",top+"px");
    
  });
  
  // Size the viewport

  if(typeof options['viewportHeight'] != 'undefined'){
    
  var h = parseInt(options['viewportHeight']);
  var s = h - $('#myGrid .grid-header').height();

   $('#myGrid').height(h);
   $('#resizableWrapper').height(h);  
   $('.main-scroller').height(s);
   
  }
  
  $('#resizableWrapper').resizable({ 
    alsoResize: '.main-scroller, #myGrid',
    maxWidth: $('#myGrid').width(),
    minWidth: $('#myGrid').width(),
    stop: function(e, ui) {

      if(ui.originalSize.height != ui.size.height){
        
      grid.onViewportResized(ui.size.height);
      
      }
      
    }
     
  });
	
	
	// wire up model events to drive the grid
	dataView.onRowCountChanged.subscribe(function(args) {
		grid.resizeCanvas();
	});
	
	
	dataView.onRowsChanged.subscribe(function(rows) {
		grid.removeRows(rows);
		grid.render();
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

function matrixFilter(item) {
		
	if (searchString != "" && item["title"].indexOf(searchString) == -1)
		return false;
		
	return true;
	
}




/************************** Cell formatter functions ************************/

// Format the row selector field
var selectorCellFormatter = function(row, cell, value, columnDef, dataContext) {
  
  return (!dataContext ? "" : value);

};


var MatrixTextCellEditor = function($container, columnDef, value, dataContext) {
    var $input;
    var defaultValue = value;
    var scope = this;
    
    this.init = function() {

        
          args = {
            field: columnDef.id,
            nid: dataContext.nid
          }

        $.post(
          Drupal.settings.matrixEditorCallbackPath+'/get_form_field', 
          args,
          function(response){
            
            if(response.status){
              
              $input = $("<INPUT type=text class='editor-text' />");

              if (value != null) 
              {
                  $input[0].defaultValue = value;
                  $input.val(defaultValue);
              }

              $input.appendTo($container);
              $input.focus().select();
              
              $('#matrix-editor-panel').html(response.data);
              
            }

          },
          'json'
        );
        
    }
    
    this.destroy = function() {
        $input.remove();
    }
    
    this.focus = function() {
        $input.focus();
    }
    
    this.setValue = function(value) {
        $input.val(value);
        defaultValue = value;
    }
    
    this.getValue = function() {
        return $input.val();
    }
    
    this.isValueChanged = function() {
        return (!($input.val() == "" && defaultValue == null)) && ($input.val() != defaultValue);
    }
    
    this.validate = function() {
        if (columnDef.validator) 
        {
            var validationResults = columnDef.validator(scope.getValue());
            if (!validationResults.valid) 
                return validationResults;
        }
        
        return {
            valid: true,
            msg: null
        };
    }
    
    this.init();
}









