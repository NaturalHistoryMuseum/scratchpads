$(function() {
		NEXUS = new nexus();
});

// function nexusPopulateNote(columnDef, cell){
//   
//   var character_tid = nexusGetColumnTid(columnDef);
//   var taxa_tid = cell.id;
// 
//   $('#edit-character-tid').val(character_tid);
//   $('#edit-taxa-tid').val(taxa_tid);  
//   
//   if(notes[character_tid] && notes[character_tid][taxa_tid]){
//     
//     var note = notes[character_tid][taxa_tid];
//     
//   }else{
//     
//     var note = new Array();
//     note.body = '';
//     note.nid = '';
//     
//   }
//   
//   $('#edit-body').val(note.body);
//   $('#edit-nid').val(note.nid);
//   
//   // Note form is already on page, ready to be populated
//   $('#nexus-note-form').show();
//   
// }
// 
// 
// // AHAH callback for note being updated
// function nexusAhahCallbackNoteUpdated(response){
//   
//   if(response.op == 'delete'){
//     
//     notes[response.characterTid][response.taxaTid] = null;
//     
//   }else{
//     
//     // If there's no notes for any of these characters, create a new array 
//     if(typeof notes[response.characterTid] == 'undefined'){
//       notes[response.characterTid] = new Array();
//     }
// 
//     notes[response.characterTid][response.taxaTid] = new Array();
//     notes[response.characterTid][response.taxaTid]['body'] = $('#edit-body').val();
//     notes[response.characterTid][response.taxaTid]['nid'] = response.nid;
//     
//   }
//   
//   nexusAddLogData(response.data);
//   
// }




function nexus() {
  
  var tabs;
  var nid;
  var self;
  
  return {//
    
    init: function(n) {
      // Attach events
      grid.onSelectedRowsChanged = this.rowSelected;
      grid.onColumnHeaderClick = this.columnSelected;
      grid.onColumnsReordered = this.columnsReordered;
      
      // Attach tabs
      tabs = $("#tabs").tabs();
      
      nid = n;
      self = this;
      
      $('#add-character a').click(function(){
        
        args = {
          nid: self.getNid()
        };
        
        self.getCharacterForm(args);
        return false;
      });
      
    },
    
    attachCharacterFormEvents: function() {
      
      // Character form enhancements
      if($("input[name='type']").val() == 'controlled'){
       $('#nexus-state-wrapper').show();   
      }

      // Show / hide options on creating character form
      $("input[name='type']").change(
        function()
        {

          if($(this).val() == 'controlled'){
          $('#nexus-state-wrapper').show();  
          }else{
          $('#nexus-state-wrapper').hide();   
          }
        }
      );
      
      if($('select#edit-character-group').val() == 0){
        $('#edit-new-group-wrapper').show(); 
      };
      
      $("select[name='character_group']").change(
        function()
        {
            
          if($(this).val() > 0){
          $('#edit-new-group-wrapper').hide();  
          }else{
          $('#edit-new-group-wrapper').show();   
          }
        }
      );
      
      Drupal.attachBehaviors($('#nexus-character-form'));
      
    },
    
    getColumnTid: function(column) {
      return column.id;
    },

    getRowTid: function(row) {
      return $('div.r[row='+row+'] input.tid').val();
    },
    
    updateTitle: function(titleStr) {
      $('#tabs-wrapper h2').html(titleStr);
    },
    
    updateDataPanel: function(data) {
      $('#data-panel').html(data);
      tabs.tabs('select', 0);
    },
    
    columnSelected: function(column) {
    
      // Deselect active cell
      grid.commitCurrentEdit();
      $('div.c.selected').removeClass('selected');

      // Hide any selected rows
      $('div.r.selected').removeClass('selected');

      var columnIndex = grid.getColumnIndex(column.id);

      args = {
        tid: self.getColumnTid(column),
        nid: self.getNid()
      }
      
      self.getCharacterForm(args);

      $('div.c').removeClass('selectedColumn');
      $('div.c'+columnIndex).addClass('selectedColumn');
    
    },
    
    getCharacterForm: function(args){
      
      $.post(
        Drupal.settings.nexusCallback+'/get_character_form', 
        args, 
        function(response){
          self.updateDataPanel(response.data);
          self.attachCharacterFormEvents();
        },
        'json'
      );
      
      
      
    },
    
    rowSelected: function() {

      var row = grid.getSelectedRows();   

      var cell = $('div.selected:not(.r)').attr('cell');

      if(row.length == 1 && cell == 0){

        args = {
          tid: self.getRowTid(row)
        }

        $.post(
          Drupal.settings.nexusCallback+'/get_row_data', 
          args, 
          function(response){
           self.updateDataPanel(response.data);
          },
          'json'
        );

      }
      
    },
    
    columnsReordered: function(e, ui){

        data = {
          tid: $(ui.item).attr('id'),
          next: $(ui.item).next('div.h').attr('id'),
          prev: $(ui.item).prev('div.h').attr('id')
        }
        
      $.post(
        Drupal.settings.nexusCallback+'/reorder_columns', 
        data,
        function(response){
         self.columnsReorderedCallback(response);
        },
        'json'
      );
      
    },
    
    columnsReorderedCallback: function(response){
      
      // Need to change the class if it's been moved into a different group
      
      if(response.settings.adjacentTid){
        
        var group = $('#'+response.settings.tid+' span.group');
        
        var old_class = characterGroupClass[response.settings.tid];
        var new_class = characterGroupClass[response.settings.adjacentTid];
        
        group.removeClass(old_class);
        group.addClass(new_class);
        
        characterGroupClass[response.settings.tid] = new_class;
     
      }

      
    },
    
    populateNote: function(){
      
    },
    
    getNid: function(){
      return nid;
    },

    
  }
}







