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
      
        $input = this.getInput();
        
        if (value != null) 
        {
            $input[0].defaultValue = value;
            $input.val(defaultValue);
        }
        
        $input.appendTo($container);
        $input.focus().select();
        
        this.populateData();
        
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
        
    this.populateData = function(){
      
      var character = characters[columnDef.id];
      
      row = ($($container).parents('div.r').attr('row'));
      cell = $($container).attr('cell');

      var div = document.createElement('div');
      var h1 = document.createElement('h1');
      var name = document.createTextNode(character.name);
      h1.appendChild(name);
      div.appendChild(h1);

      NEXUS.populateNote(columnDef, dataContext);

      if(character.states){

        // Create the option list 
        var ul = document.createElement('ul');

        for(key in character.states){        

          // Add an option
          var li = document.createElement('li');
          var strong = document.createElement('strong');
          var keyText = document.createTextNode(key);
          strong.appendChild(keyText);
          li.appendChild(strong);
          var state = document.createTextNode(character.states[key]['state']);
          li.appendChild(state);
          
          if(character.states[key]['state_description']){
            var br = document.createElement('br');
            li.appendChild(br); 
            var state_description = document.createTextNode(character.states[key]['state_description']);
            li.appendChild(state_description);           
          }

          ul.appendChild(li);

        }

        div.appendChild(ul);

      }

      $('#matrix-editor-data').html(div);
      
    }
    
    this.getInput = function(){
      
      character_tid = NEXUS.getColumnTid(columnDef);

      if(characters[character_tid]['states']){

          var select = "<select tabIndex='0'><option value=''></option>";

          for(key in characters[character_tid]['states']){
            
            select += "<option value='"+key+"'>"+key+" "+characters[character_tid]['states'][key]['state']+"</option>";

          }

          select += '</select>';

          $input = $(select);

      }else{

        $input = $("<input tabIndex='0' type=text class='editor-text' />");

      }

      return $input;
      
    }
    
    this.validate = function(){
      
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

/************************** Cell formatter functions ************************/

// Format the row selector field
var nexusSelectorCellFormatter = function(row, cell, value, columnDef, dataContext) {
  
  return '<input type="hidden" class="tid" value="'+dataContext.id+'" />'+row;

};


/************************** Cell validator functions ************************/


function nexusQuantitativeFieldValidator(value) {
    
  var character_tid = columns[cell]['id'];
  var valid = false;
    
 if (value.length){
   
   // Is it one of the options
   if(characters[character_tid]['options'][value]){
     
     valid = true; 
     
   }else{
     
     valid = false;  
     
   }
   
 }else{
   
   valid = true;
   
 }
 
 return {valid:valid, msg:null};

}



/************************** Update cell handlers ************************/






var nexusQualitativeUpdateItem = function(value,columnDef,item) {

  var data = {
    character_tid: NEXUS.getColumnTid(columnDef),
    taxa_tid: item.id,
    title: value
  }
  
  if(free_states_data && free_states_data[character_tid] && free_states_data[character_tid][item.id]){
    data.nid = free_states_data[character_tid][item.id];
  }

	$.post(Drupal.settings.nexusCallback+'/set_free_state', data);
	
	item[columnDef.field] = value;
	dataView.updateItem(item.id,item);
	
}




var nexusQuantitativeUpdateItem = function(value,columnDef,item) {

  var data = {
    character_tid: NEXUS.getColumnTid(columnDef),
    taxa_tid: item.id
  }

  if(value){
    data.nid = characters[character_tid]['states'][value]['nid'];
  }

	$.post(Drupal.settings.nexusCallback+'/set_state', data);
	
	item[columnDef.field] = value;
	dataView.updateItem(item.id,item);
	
	
}