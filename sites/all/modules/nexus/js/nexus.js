$(function() {
		NEXUS = new nexus();
});


function nexus() {
  
  var project_nid;
  var self;
  var $groups;
  var settings = [];
  var sorting;

  return {//
    
    init: function(nid, node_settings) {
      // Attach events
      project_nid = nid;
      self = this;
      
      if(node_settings){
        settings = node_settings;
      }
      
      grid.onSelectedRowsChanged = self.onSelectedRowsChanged;
      grid.onColumnHeaderClick = self.onColumnHeaderClick;
      grid.onColumnsReordered = self.onColumnsReordered;
      grid.onColumnsReorderStart = self.onColumnsReorderStart;
      grid.onColumnsResized  = self.onColumnsResized;
      grid.onViewportChanged  = self.onViewportChanged;
      
      self.sizeViewport();
      
      self.beautyTips();
      
      $('.grid-header .h span').click(function(){

        var columnIndex = grid.getColumnIndex($(this).parents('div.h').attr('id'));
      
        grid.onColumnHeaderClick(columns[columnIndex]);
        
      });
      
      $('#matrix-editor-controls .item-list a').click(function(){
        var id = this.href.split('#')[1];
        
        var $this = $(this);
        self.showTab(id, $this);        
        return false;
        
      });
      
      $('a.edit-taxa').click(function(){
        
        id = 'edit-taxa';
        $this = $('#matrix-editor-controls a[href=#'+id+']');
        self.showTab(id, $this);
        return false;
        
      })
      
      $('a.add-character').click(function(){

        args = {
          project_nid: self.getProjectNid()
        };
        
        self.getCharacterForm(args);
        return false;
        
      });
      

      $('#myGrid div.main-scroller').scroll(function(e){
        
        var top = parseInt("-"+e.target.scrollTop);
        $('.side-header').css("top",top+"px");
        
      });
      
      $('div.side-header').click(function(e){
        
        if ($(e.target).hasClass("toggle")) {
          
          var row = $(e.target).parents('.r').attr('row');
          
          var item = dataView.rows[row];
          
          args = {
            tid: item.id,
            value: (item._collapsed ? 1 : 0),
            setting: 'collapsed'
          };

          $.post(
            Drupal.settings.nexusCallback+'/update_term_setting', 
            args 
          );
          
        }
        
      });
      
      self.addGroups();
      
      self.addResizableViewport();
      
    },
    
    // Triggered on scroll
    onViewportChanged: function(){

      $('#character-groups')[0].scrollLeft = $('.main-scroller').scrollLeft();

    },
    
    addResizableViewport: function(){

      $('#myGrid').resizable({ 
        alsoResize: '.main-scroller',
        maxWidth: $('#myGrid').width(),
        minWidth: $('#myGrid').width(),
        stop: function(event, ui) {
          
          if(ui.originalSize.height != ui.size.height){
            
            args = {
              projectNid: self.getProjectNid(),
              setting: 'height',
              value: ui.size.height
            };
            
            $.post(
              Drupal.settings.nexusCallback+'/update_project_setting', 
              args 
            );
            
          }
          
        }
         
      });

    },
    
    beautyTips: function(){

      $('.grid-header .h span').hover(function(){
        
        if(!sorting){
          $(this).btOn(); 
        }
        
        
      });
      
      $('.grid-header .h span').mousedown(function(){
      
        $(this).btOff(); 
        
      });
      
      $('.grid-header .h span').mouseout(function(){
        
        if(!sorting){
        $(this).btOff(); 
        }
        
      });
      
      
      $('.grid-header .h span').bt(
        {
          contentSelector: "NEXUS.getBeautyTipText($(this).parents('.h').attr('id'))",
          positions: 'top',
          offsetParent: 'body',
          shrinkToFit: true,
          fill: 'black',
          spikeLength: 10,
          cssStyles: {color: 'white', 'font-size': '10px'},
          closeWhenOthersOpen: true,
          trigger: 'none',
          preShow: function() {
            return false;
          },
        }
      );
      
    },
    
    getBeautyTipText: function(id){
      
      var tiptext;
      
      $(columns).each(function(){
        
        if(this.id == id){
          
         tiptext = this.term;
         return false;
          
        }
        
      })
      // console.log(columns[id]);
      return tiptext;
      
    },
    
    sizeViewport: function(){
     
      
      // Add the height saved in project node
      if(typeof settings.height != 'undefined'){
        
        var h = parseInt(settings.height);

       $('#myGrid').height(h);
       
       
       
       $('.main-scroller').height(h - $('#myGrid .grid-header').height());
       
      }
      
    },
    
    onColumnsResized: function(e, ui){
           
      var cell = $(ui.element).attr('cell');
      var group_id = columns[cell]['groupID'];

      // Sometimes UI doesn't change the original width, but UI has new size so get the actual element width
      
      if(ui.originalSize.width != $(ui.element).width()){
        
        var width = $('#'+group_id).width() - ui.originalSize.width + $(ui.element).width();
      
        $('#'+group_id).width(width);
        
        if($(ui.element).attr('id') == 'taxa'){
          
          args = {
            value: $(ui.element).width(),
            projectNid: self.getProjectNid(),
            setting: 'taxa_column_width'
          };
      
          $.post(
            Drupal.settings.nexusCallback+'/update_project_setting', 
            args 
          );
          
          $('#character-groups').css('margin-left', $(ui.element).width()+'px');
          
        }else{
          
          args = {
            tid: $(ui.element).attr('id'),
            value: $(ui.element).width(),
            setting: 'width'
          };
      
          $.post(
            Drupal.settings.nexusCallback+'/update_term_setting', 
            args 
          );
          
        }
        
      }
      

      
      
      
    },
    
    showTab: function(id, $a){
      
      $('#matrix-editor-panels .tab:visible').hide();
      $('#matrix-editor-controls li.active').removeClass('active');
      $('#'+id).show();
      $a.parent('li').addClass('active');
      
    },
    
    updateDataPanel: function(data) {
       $('#non-cell-data').html(data);
       self.toggleDataPanelItem('non-cell-data');
     },
    
     toggleDataPanelItem: function(id){

       $('#matrix-editor-panels div.panel:not(#'+id+')').hide();    
       $('#'+id).show();
       
       var $a = $('#matrix-editor-controls li.last a');
       self.showTab('data-editor', $a);       

     },

     updateDialog: function(message){

       $('#dialog').html(message);
       
       self.displayDialog();

     },
     
     displayDialog: function(){
       
       $("#dialog div.messages").dialog({ 
         buttons: { "OK": function() { $(this).dialog("close"); } } });

     },
     
     createDialog: function(type, message){
     
       var div = document.createElement('div');
       div.setAttribute("class","error");
       var text = document.createTextNode(message);
       div.appendChild(text);
       
       self.updateDialog(div);
     
     },

     getProjectNid: function(){
       return project_nid;
     },
    
    onColumnHeaderClick: function(column) {
    
      // Deselect active cell
      grid.commitCurrentEdit();
      $('div.c.selected').removeClass('selected');

      // Hide any selected rows
      $('div.r.selected').removeClass('selected');
         
      var columnIndex = grid.getColumnIndex(column.id);
      
      $('div.c, div.h').removeClass('selectedColumn');
      $('div.c'+columnIndex).addClass('selectedColumn');
      
      args = {
        character_tid: column.id,
        project_nid: self.getProjectNid()
      }
      
      self.getCharacterForm(args);
    
    },
    
    addGroups: function(){

      // $('#character-groups').width()
      $('#character-groups').html('');
      
      $('#character-groups').css('margin-left', $('.main-data').css('margin-left'));
      
      $groups = $("<div class='scroll'>").appendTo($('#character-groups'));
    
      var group = null;
      
      var prevGroupID;
      
      $(columns).each(function(i){
        
        if(i == 0){
         return; // continue
        }
        
        if(this.groupID != prevGroupID){
          
          if(group){
            group.appendTo($groups);
          }
         
          group = $("<div class='character-group' id='"+this.groupID+"'/>")
           .html(this.group)
           .width(this.width + 4)
           .click(function(){
             self.groupSelected($(this));
           });
                    
        }else{

          var oldWidth = parseInt(group.css('width'));
          
          group.width(oldWidth + this.width + 4);
          
        }
        
        prevGroupID = this.groupID;
 
      });
      
      if(group){
        group.appendTo($groups);
      }
      
      $($groups).sortable({ 
        axis: 'x', 
        scroll: true,
        start: function(e, ui){
        
          sorting = true;
                    
        },
        sort: function(e, ui){
          
          var $charGroup = $('#character-groups');
          var charGroupMarginLeft = parseInt($charGroup.css('margin-left'));
          var charGroupWidth = $charGroup.width();
  
          if((ui.position.left - 50) < charGroupMarginLeft){
           
          $('.main-scroller').animate({scrollLeft:0}, 1000);
            
          }else if((ui.position.left + ui.item.width()) > charGroupWidth + charGroupMarginLeft - 50){
           
            $('.main-scroller').animate({scrollLeft:50000}, 1000);
            
          }
          
        },
        containment: $('#character-groups'),
        update: function(e, ui){
        
          $('#character-groups')[0].scrollLeft = 0;
          self.onGroupsReordered(e, ui);
          
          ('..main-scroller').addClass('loading');
          $('.grid-canvas').hide();
          $('.main-scroller').stop();
                    
        },
        deactivate: function(event, ui){

          $('.main-scroller').stop();

        },
        
        
      });
      





    },
    
    onGroupsReordered: function(e, ui){
      
      var data = 'project_nid='+self.getProjectNid();
      data += '&active_group='+ui.item.attr('id');
      
      $('.character-group').each(function(){

        data+='&groups[]='+$(this).attr('id');

      });

      $.post(
        Drupal.settings.nexusCallback+'/reorder_groups', 
        data,
        function(response){

          self.updateDialog(response.data);
          
          $('#'+response.active_group).addClass('selected');

        },
        'json'
      );
      
      sorting = false;
      
    },

    
    groupsDeselect: function(){
      
      $('div#character-groups div.selected').removeClass('selected');
      
    },
    
    groupSelected: function($this){
      
      self.deselectColumn();
      self.groupsDeselect();
      
      $this.addClass('selected');
      
      args = {
        group_tid: $this.attr('id'),
        project_nid: self.getProjectNid()
      }

      $.post(
        Drupal.settings.nexusCallback+'/get_group_form', 
        args, 
        function(response){
          
          self.updateDataPanel(response.data);
          self.attachDeleteFormEvents();
          Drupal.settings.ahah = response.ahah;
          Drupal.attachBehaviors($('#nexus-group-form'));
          
        },
        'json'
      );
      
    },
    
    // Unselect all column cels
    deselectColumn: function(){
      
      $('div.selectedColumn').removeClass('selectedColumn');
      
    },
    
    onSelectedRowsChanged: function() {

       var row = grid.getSelectedRows();
       
       
       self.deselectColumn();
          

       var cell = $('div.selected:not(.r)').attr('cell');

       if(row.length == 1 && cell == 0){

         args = {
           taxa_tid: data[row]['id'],
           project_nid: self.getProjectNid()
         }

         $.post(
           Drupal.settings.nexusCallback+'/get_row_data', 
           args, 
           function(response){

            self.updateDataPanel(response.data);           
            $('#inherit-states').click(function(){

               $.post(
               Drupal.settings.nexusCallback+'/inherit_states',
               args,
               function(response){

                 self.updateDialog(response.data);

               },
               'json'
               );

              return false;

            })

           },
           'json'
         );

       }

     },

     // After reordering
     onColumnsReordered: function(e, ui){

         data = {
           character_tid: $(ui.item).attr('id'),
           next_tid: $(ui.item).next('div.h').attr('id'),
           prev_tid: $(ui.item).prev('div.h').attr('id')
         }

       $.post(
         Drupal.settings.nexusCallback+'/reorder_columns', 
         data,
         function(response){
          self.columnsReorderedCallback(response);
         },
         'json'
       );
       
       sorting = false;

     },
     
     onColumnsReorderStart: function(event, ui){
       
       sorting = true;
       
     },



     columnsReorderedCallback: function(response){

       $(columns).each(function(i){
         
         if(i > 0){

          $('#'+this.id+' span').html(i);
           
         }
         
         if(this.id == response.settings.tid){
           
           if(this.group != response.settings.groupTid){
             
             this.groupID = parseInt(response.settings.groupTid);
             this.group = response.settings.group;
             self.addGroups();
             
           }
           
           
         }
         
       });

     }, 

     cellSelected: function($container, columnDef, value, dataContext){

       var note_body = '';
       var note_nid = '';
       var $cellForm;
       
       $('#character-info').html('');
       
       $cellForm = $('<div>').appendTo($('#character-info'));
       $('<h1>Edit character state</h1><p>'+columnDef.term+'</p>').appendTo($cellForm);

        if(columnDef.states){
          
          $('<p>This is a <em>controlled state character</em>. Available options are:<p>').appendTo($cellForm);
          
          // Create the option list 
          var ul = document.createElement('ul');
          
          for(key in columnDef.states){        
          
            // Add an option
            var li = document.createElement('li');
            var strong = document.createElement('strong');
            var keyText = document.createTextNode(key);
            strong.appendChild(keyText);
            li.appendChild(strong);
            var state = document.createTextNode(columnDef.states[key]['state']);
            li.appendChild(state);
          
            if(columnDef.states[key]['state_description']){
              var br = document.createElement('br');
              li.appendChild(br); 
              var state_description = document.createTextNode(columnDef.states[key]['state_description']);
              li.appendChild(state_description);           
            }
          
            ul.appendChild(li);
          
          }
          
          $(ul).appendTo($cellForm);

        }else if(typeof columnDef.validator == 'undefined'){ // This is a text field so add the textarea
          
          $('<p>This is a <em>free state text character</em> - please enter text in the table cell or the field below.<p>').appendTo($cellForm);
          
          var $resizableTextarea = $("#edit-body-1-wrapper .resizable-textarea").clone()
          var defaultValue;
          
          if(typeof value != 'undefined'){
            defaultValue = value;
          }
          
          var $textarea = $resizableTextarea.find('textarea');
          
          $textarea.attr('id', "text-state-dynamic-entry");
          $textarea.val(defaultValue);
          $textarea.change(function () {

            $('input.editor-text').val($(this).val());
            $('input.editor-text').effect("highlight", {color:"#9ADE0B"}, 300);
            $('input.editor-text').trigger('change');

          });

          $textarea.appendTo($cellForm);
          
        }else{ // This is numeric
          
          $('<p>This is a <em>numeric character</em> - please enter only numeric values.<p>').appendTo($cellForm);
          
        }

       $('#cell-data #node-form #edit-taxa-tid').val(dataContext.id);
       $('#cell-data #node-form #edit-character-tid').val(columnDef.id);

       self.toggleDataPanelItem('cell-data');

       // Are there notes / free state info?
       if(intersection_info[columnDef.id] && intersection_info[columnDef.id][dataContext.id]){

         var info = intersection_info[columnDef.id][dataContext.id];

         if(info.note){
           note_body = info.note.body;
           note_nid = info.note.nid;

         }

       }


       $('#edit-body-1').val(note_body);
       $('#edit-note-nid').val(note_nid);


     },
    

    
    /* character form */
    
    
    attachCharacterFormEvents: function() {

       $('#nexus-states').sortable({ 
         containment: 'parent' , 
         axis: 'y', 
         handle: 'span.handle', 
         items: 'div.state-form' , 
         update: function(event, ui) {

           $('#nexus-states div.state-form:not(.ui-sortable-placeholder)').each(function(i){

             $(this).find('input.state-nid').attr({
               id: 'edit-states-'+i+'-nid',
               name: 'states['+i+'][nid]'
             });

             $(this).find('div.state label').attr('for', 'edit-states-'+i+'-state');

             $(this).find('div.state input').attr({
               id: 'edit-states-'+i+'-state',
               name: 'states['+i+'][state]'
             });

             $(this).find('div.state-description label').attr('for', 'edit-states-'+i+'-state-description');

             $(this).find('div.state-description textarea').attr({
               id: 'edit-states-'+i+'-state-description',
               name: 'states['+i+'][state_description]'
             });


           });


         }

       });  

       if($("input[name='type']:checked").val() == 'controlled'){
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


       self.attachDeleteFormEvents();
       
       Drupal.attachBehaviors($('#nexus-character-form'));

       self.attachStateFormEvents();

     },
     
     attachDeleteFormEvents: function(){
       
       $('#edit-delete').click(function(){
         
         $('div.columns').hide();
         $('#confirm-delete').show();
         
         return false;
         
       });
       
       $('#edit-cancel').click(function(){
         
         $('#confirm-delete').hide();
         $('div.columns').show();
         return false;
         
       });
       
     },
      
     getCharacterForm: function(args){

       $.post(
         Drupal.settings.nexusCallback+'/get_character_form', 
         args, 
         function(response){
           
           Drupal.settings.ahah = response.ahah;

           self.updateDataPanel(response.data);

           self.attachCharacterFormEvents();

         },
         'json'
       );

     },
    
    // AHAH action callback
    addCharacterCallback: function(response){

        // Add the new character group to the drop down
        $("#edit-new-group").val("");

        if(response.group_tid && response.group_name){

          var option = document.createElement('option');
          option.text = response.group_name;
          option.value = response.group_tid;
          $("#edit-character-group").append(option);
          $("#edit-character-group").val(response.group_tid);
          $('#edit-new-group-wrapper').hide();
          

        }

       // Reset the character form
        $('#nexus-character-form #edit-name-wrapper input').val('');
        $('#edit-description').val('');

        // Only keep the first controlled state input
        var default_form = $('#nexus-states .state-form:first');

        // And reset it's values
        $(default_form).find('.state input').val('');
        $(default_form).find('.state-description textarea').val('');
        $(default_form).find('.state-nid').val('');

        // Remove all the rest
        $('#nexus-states .state-form:gt(0)').remove();
        
        self.displayDialog();

    },
    
    saveCharacterCallback: function(response){

        self.displayDialog();
        
    },
    
    // AHAH action callback
    deleteCharacterCallback: function(response){
      
      $('#confirm-delete').hide();
      
    },

    
    
    /* state form */
    
    attachStateFormEvents: function(){
      
       $('input.delete-state').click(function(){

         self.deleteState($(this));
         return false;

       });
       
       $('input.expand-state').click(function(){

         var description = $(this).parents('div.state-form').find('div.state-description');
         
         description.toggle('fast');
         
         return false;

       });

     },

     deleteState: function($input){

       var parentStateForm = $input.parents('div.state-form');

       args = {
         state_nid: $(parentStateForm).find('input.state-nid').val(),
         delta: $(parentStateForm).find('input.delta').val(),
         form_build_id: $input.parents('form').find("input[name='form_build_id']").val(),
       }

       $.post(Drupal.settings.nexusCallback+'/delete_state', args, function(){
         parentStateForm.remove();
       });

     },
    

    // AHAH action callback
    addStateCallback: function(response){
      
      self.updateDialog(response.messages);
      self.attachStateFormEvents();
    
    },


    // AHAH action callback
    updateProjectCallback: function(response){
      
      $('#tabs-wrapper h2').html(response.title);
      
      self.displayDialog();
    
    },    


    /* notes */
    
    saveNoteCallback: function(response){
      
      if(response.op == 'delete'){
        
        intersection_info[response.character_tid][response.taxa_tid]['note'] = null;
        
      }else{ // Store the note in the intersection data array
        
        // If there's no notes for any of these characters, create a new array 
        if(typeof intersection_info[response.character_tid] == 'undefined'){
          intersection_info[response.character_tid] = new Array();
        }
        
        if(typeof intersection_info[response.character_tid][response.taxa_tid] == 'undefined'){
          intersection_info[response.character_tid][response.taxa_tid] = new Array();
        }
        
        intersection_info[response.character_tid][response.taxa_tid]['note'] = response.new_note;

      }
      
      self.updateDialog(response.data);
    
    },

    
  }
}







