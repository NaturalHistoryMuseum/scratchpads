
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
  
  if($.browser.msie && $.browser.version < 7){
    $("#myGrid").html('Sorry, this feature isn\'t currently available for IE6');
    return;
  }
    
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
	
	// Only for the matrix editor view
	$(".view-content #myGrid .c").click(function(e){
	  
	  if(e.metaKey || e.ctrlKey){
	   
	    if($("#myGrid .c.selected").attr('cell') == $(this).attr('cell')){
	      
	      $(this).toggleClass('multiSelected');
	      
	    }
	   
	    e.stopPropagation();
      e.preventDefault();
      return false;
	    
	  }else{
	    
	    $("#myGrid .c").removeClass('multiSelected');
	    
	  }
	  
	});
	
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
	
  $('#matrix-editor-panel input.form-submit').click(function(){    
  
    $loader = $("<img class='imageLoader' src='/"+Drupal.settings.matrixEditorPath+"/images/ajax-loader.gif' />").insertAfter($(this));
  
    var args = $("#matrix-editor-panel form").serialize();
    
    args+='&view='+Drupal.settings.matrixEditorViewName;
  
    if($('#matrix-editor-field iframe').length){
  
      args += '&body='+$('#matrix-editor-field iframe').contents().find('body').html();
      
    }
    
    // Are there multiple nodes selected?
    $('#myGrid div.c.multiSelected').each(function(){
      
      var row = $(this).parents('.r').attr('row');
      var item = dataView.rows[row];
      
      args += '&nids[]='+item.id;
      
    });
    
       $.post(
         Drupal.settings.matrixEditorCallbackPath+'/update_node', 
         args,
         function(response){
           
             var cell = $('#myGrid .editor-text').parents('div.c').attr('cell');
             var columnID = columns[cell]['id'];
             
             $('#myGrid .editor-text, #myGrid .multiSelected').each(function(){

               var row = $(this).parents('div.r').attr('row');
               data[row][columnID] = response.data;               
               $(this).html(response.data);

             });
            
            $result = $("#matrix-editor-result");
            $result.html(response.result);
            
            $result.fadeIn('fast');
     
            setTimeout(
              function(){$result.fadeOut()}, 
              2500
            );
            
            $loader.remove();

         },
         'json'
      );  
    
    return false;
    
  });
  
  
}





/************************** Cell formatter functions ************************/

// Format the row selector field
var selectorCellFormatter = function(row, cell, value, columnDef, dataContext) {
  
  return (!dataContext ? "" : value);

};



var MatrixCellEditor = function($container, columnDef, value, dataContext) {
    var $input;
    var defaultValue = value;
    var scope = this;
    
    this.init = function() {
      
        $input =  $("<div tabIndex='0' type=text class='editor-text' autocomplete='off' />");
        
        if (value != null) 
        {
            $input[0].defaultValue = value;
            $input.html(defaultValue);
        }
        
        field_args = columnDef.args+'&field='+columnDef.id+'&nid='+dataContext.id+'&view_field='+columnDef.field;
        
        $('#matrix-editor-field').html('<div class="progress"><div class="bar"><div class="filled"></div></div>'+
        '<div class="message">Loading form...</div></div>');
        
        $.post(
           Drupal.settings.matrixEditorCallbackPath+'/get_form_field', 
           field_args,
           function(response){
        	   
             $('#matrix-editor-field').html($(response.data));
             
             if($('#matrix-editor-panel').is(':hidden')){
               $('#matrix-editor-panel').slideDown(1000);
             }

             Drupal.attachBehaviors($('#matrix-editor-field'));
             
           },
           'json'
        );
        
        $input.appendTo($container);
        $input.focus().select();
  
    }
    
    
    this.destroy = function() {
      $input.remove();
    }
    
    
    this.focus = function() {
        $input.focus();
    }
    
    this.setValue = function(value) {
        $input.html(value);
        defaultValue = value;
    }
    
    this.getValue = function() {
        return $input.html();
    }
    
    this.isValueChanged = function() {
        return (!($input.html() == "" && defaultValue == null)) && ($input.html() != defaultValue);
    }
    
    this.validate = function() {
        return {
            valid: true,
            msg: null
        };
    }
    
    this.init();
    
}


$(document).ready(function() {
	  $('#matrix-editor-toggle-advanced-options').click(function() {
		  
		  var el = $('.view-filters, .attachment-before', '.view-matrix-editor-page');
		  
		  if(el.is(':visible')){
			el.hide('fast');
			$(this).removeClass('me-collapsed');
		  }else{
			  el.show('fast');
			  $(this).addClass('me-collapsed');
			  
		  }
		  
		  return false;

		}); 
	 });


