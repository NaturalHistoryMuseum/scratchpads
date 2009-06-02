/** Clones & tweaks of slickgrid functions **/

// Extended to allow hiding items

function nexusFilter(item) {
	
	$displayItem = true;
		
	if (searchString){
	  
	  var taxa = item['taxa'].toLowerCase();
    searchString = searchString.toLowerCase();
	 
	  if(taxa.indexOf(searchString) == -1){
  	  $displayItem = false;
  	}
	  
	}

    if (item.parent != null) {
      
     var parent = data[item.parent];
     
     while (parent) {
       
       var parent_taxa = parent.taxa.toLowerCase();
       
       if (parent._collapsed){
         return false;
       }else if(parent_taxa.indexOf(searchString) != -1){
         $displayItem = true;
       }
       
       parent = data[parent.parent];
       
     }
    
    }
  

	return $displayItem;

}







/**************************  Cell editor definition ************************/

var nexusCellEditor = function($container, columnDef, value, dataContext) {
    var $input;
    var defaultValue = value;
    var scope = this;
    
    this.init = function() {
      
        $input =  $("<input tabIndex='0' type=text class='editor-text' autocomplete='off' />");
        
        if (value != null) 
        {
            $input[0].defaultValue = value;
            $input.val(defaultValue);
        }
        
        $input.appendTo($container);
        $input.focus().select();
        
        NEXUS.cellSelected($container, columnDef, value, dataContext);
        
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
    
    this.validate = function(){
      
      if (columnDef.validator) 
      {
          var validationResults = columnDef.validator(scope.getValue(), columnDef, $container);
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

/************************** Cell formatter functions ************************/

// Format the row selector field
var nexusSelectorCellFormatter = function(row, cell, value, columnDef, dataContext) {
  
  return (!dataContext ? "" : value);

};

/************************** Cell validator functions ************************/


function nexusControlledStateValidator(value, columnDef, $container) {
  
  var character_tid = this.id;
  var valid = true;
  var error;
  
  if(value){
    
    if(value.indexOf('+') != -1){

      var values = (value.split('+'));

    }else if(value.indexOf('/') != -1){
      
      var values = (value.split('/'));
      
    }else {

      var values = [value];

    }
    
    // Check the values match one of the allowed values  
    $(values).each(function(i) {
    
     if(!columnDef['states'][values[i]]){
         
      valid = false;
      
      error = "'"+values[i]+"' is not an option for this controlled state. Please select from: <br />";
      
        for(var j in columnDef['states']){

          error+= '<strong>'+j+"</strong> "+columnDef['states'][j]['state']+"<br />";
          
        }
        
      return;
      
     }else if(values.length > 1 && (values[i].indexOf('?') != -1 || values[i].indexOf('-') != -1)){
       
      valid = false; 
      
      error = 'Sorry, you cannot select both a state and '+values[i]+'.';
      
       
     }
    
    }); 
    
    
  }
  
  if(error){
    nexusErrorBeautyTip($container, error)
  }

  
  return {valid: valid, msg:null};


}


function nexusNumericStateValidator(value, columnDef, $container) {

  if (value.indexOf('?') == -1 && !value.toString().match(/^[-]?\d*\.?\d*$/) ){ 
    
    nexusErrorBeautyTip($container, 'Please enter a numeric value')
    
    valid = false;
    
  }else{
    
    valid = true;
    
  }
  
  return {valid:valid, msg:null};
  
  
}

function nexusErrorBeautyTip($container, error){
 
  $container.bt(error, 
    {
      offsetParent: 'body',
      positions: 'right',
      fill: 'rgba(0, 0, 0, .7)',
      strokeWidth: 0,
      spikeLength: 10,
      cssStyles: {color: 'white', 'font-size': '10px'},
      closeWhenOthersOpen: true,
      trigger: 'none',
      width: 150
    });
    
  
  $container.btOn();
  
  
  // On lost focus
  // ANd key down?
  $container.click(function(){
    
    if($container.hasClass('bt-active')){
      $container.btOff();
    }
    
  });
  
  $container.keydown(function(){
    
    if($container.hasClass('bt-active')){
      $container.btOff();
    }
    
  })
  
}




/************************** Update cell handlers ************************/







var nexusFreeEntryUpdateItem = function(value,columnDef,item) {

  var character_tid = columnDef.id;
  var taxa_tid = item.id;
  
  var data = {
    character_tid: columnDef.id,
    taxa_tid: item.id,
    body: value
  }
  
  if(intersection_info[character_tid] && intersection_info[character_tid][taxa_tid] && intersection_info[character_tid][taxa_tid]['state_nid']){
    data.nid = intersection_info[character_tid][taxa_tid]['state_nid'];
  }

	$.post(Drupal.settings.nexusCallback+'/save_free_state', data);
	
	item[columnDef.field] = value;
	dataView.updateItem(item.id,item);
	
}




var nexusControlledStateUpdateItem = function(value,columnDef,item) {

  var data = 'character_tid='+columnDef.id+'&taxa_tid='+item.id;
  
  if(value.indexOf('+') != -1){
    
    var values = (value.split('+'));
    
    $(values).each(function() {
      
      data += '&nids[]='+columnDef['states'][this]['nid']; 
      
    });

  }else if(value.indexOf('/') != -1){
    
    data += '&or=1'
    
    var values = (value.split('/'));
    
    $(values).each(function() {
      
      data += '&nids[]='+columnDef['states'][this]['nid']; 
      
    });
    
  }else if(value){
    
    data += '&nids[]='+columnDef['states'][value]['nid']; 
    
  }

	$.post(Drupal.settings.nexusCallback+'/save_state', data);
	
	item[columnDef.field] = value;
	dataView.updateItem(item.id, item);
	
	
}
