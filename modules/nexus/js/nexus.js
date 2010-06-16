$(function() {
		NEXUS = new nexus();
});


function nexus() {
  
  var project_nid;
  var self;
  var $groups;
  var settings = [];
  var sorting;
  var gotTree;

  return {//
    
    init: function(nid) {
      
      // Attach events
      project_nid = nid; 
      self = this;
      self.initGroups();
      self.initBeautyTips();     
      
      grid.onViewportChanged  = self.onViewportChanged;

      if(!options.editable){
        return;
      }
      
      // Everything past this point are for editors only
            
      self.addGroupEvents();
      
      grid.onSelectedRowsChanged = self.onSelectedRowsChanged;
      grid.onColumnHeaderClick = self.onColumnHeaderClick;
      grid.onColumnsResized  = self.onColumnsResized;      
      grid.onViewportResized  = self.onViewportResized;
      
      self.initHeaders();
      
      $("body").click(function(){
        $("#dialog").hide();
      })
      
      // Everything past this point is for initial set up, not regeneration
      if(options.regenerated){
        return;
      }
      
      self.initTabs();
      self.initControls();
      
    },
    
    initControls: function(){
      
      $('a.add-character').click(function(){

      if (GlobalEditorLock.isEditing()){
        GlobalEditorLock.commitCurrentEdit()
      }  

        args = {
          project_nid: self.getProjectNid()
        };
        
        self.getCharacterForm(args);
        return false;
        
      });
      
    },
    
    initTabs: function(){
      
      // IE7 chokes if tabs are hidden before adding vertical tabs, so hide everthing here
      $('#matrix-editor-panels div.tab')
        .css('visibility', 'visible')
        .hide();
        
      $('#matrix-editor-panels #data-editor').show();  

      $('#matrix-editor-panels')
        .css('height', 'auto')
        .css('overflow', 'auto');
      
      
      $('#matrix-editor-controls .item-list a').click(function(){
        var id = this.href.split('#')[1];
        
        var $this = $(this);	
        
          if(id == 'edit-taxa' &! gotTree){

            if(typeof TREE != 'undefined'){
              
              TREE.displayTree($('#edit-taxa ul').attr('id'));
              
            }        
            
            gotTree = true;
            
          }else if(id == 'export-data'){	  
                    
        	  self.buildExportDataTab();
        	  
          }else if(id == 'organise-characters'){
            
            self.buildOrganiseCharactersTab();
                      
          }	  
        
        self.showTab(id, $this);        
        return false;
        
      });
      
      $('a.edit-taxa').click(function(){
        
        id = 'edit-taxa';
        $this = $('#matrix-editor-controls a[href=#'+id+']');
        self.showTab(id, $this);
        return false;
        
      })
      
    },
    
    initHeaders: function(){
      
      $('.grid-header .h span').click(function(){

        var columnIndex = grid.getColumnIndex($(this).parents('div.h').attr('id'));
      
        grid.onColumnHeaderClick(columns[columnIndex]);
        
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
      
      
      
    },
    
    // Triggered on scroll
    onViewportChanged: function(){

      $('#character-groups')[0].scrollLeft = $('.main-scroller').scrollLeft();

    },
    
    onViewportResized: function(height){

        args = {
          projectNid: self.getProjectNid(),
          setting: 'height',
          value: height
        };
        
        $.post(
          Drupal.settings.nexusCallback+'/update_project_setting', 
          args 
        );

    },

    // Add beauty tips handlers - bit convoluted to ensure they play nice with sortables...
    initBeautyTips: function(){

      $('.grid-header .h span').mouseover(function(){
        
        if(!sorting){
          $(this).btOn(); 
        }
        
        
      });
      
      $('.grid-header .h span').mousedown(function(){
        
        if($(this).hasClass('bt-active')){
          $(this).btOff(); 
        }
        
      });
      
      $('.grid-header .h span').mouseout(function(){
        
        if($(this).hasClass('bt-active')){
          $(this).btOff(); 
        }
      
        
      });
      
    
      $('.grid-header .h span').bt(
        {
          contentSelector: "NEXUS.getColumnBeautyTipText($(this).parents('.h').attr('id'))", 
          positions: 'top',
          offsetParent: 'body',
          shrinkToFit: true,
          fill: 'rgba(0, 0, 0, .7)',
          cssStyles: {color: 'white', 'font-size': '10px'},
          closeWhenOthersOpen: true,
          spikeLength: 10,
          trigger: 'none',
          strokeWidth: 0,
          preShow:          function(box){
                  
                  // Need to append to body so bt oesn't get obscured, but need to pos re-adjust for body margin
                  var top = parseInt($(box).css('top')) + parseInt($('body').css('margin-top')) - 5;
                  $(box).css('top', top + 'px');
                  
          },
        }
      );
              
      $('#myGrid .main-data div.c:not(:empty)').bt(
        {
          contentSelector: "NEXUS.getCellBeautyTipText($(this))", 
          positions: 'top',
          shrinkToFit: true,
          offsetParent: 'body',
          fill: 'rgba(0, 0, 0, .7)',
          cssStyles: {color: 'white', 'font-size': '10px'},
          closeWhenOthersOpen: true,
          spikeLength: 10,
          preShow:          function(box){
                  
                  // Need to append to body so bt oesn't get obscured, but need to pos re-adjust for body margin
                  var top = parseInt($(box).css('top')) + parseInt($('body').css('margin-top')) - 2;
                  $(box).css('top', top + 'px');
                  
          },
        }
      );

      
      
      
    },
    
    getColumnBeautyTipText: function(id){
      
      var columnIndex = grid.getColumnIndex(id);
      
      var tipText = '<span>' + columns[columnIndex]['group'] + ':</span> ' + columns[columnIndex]['term'];

      return tipText;
      
    },
    
    getCellBeautyTipText: function($cell){
      
      var columnIndex = $cell.attr('cell');
      
      var txt = $cell.text();
      
      if(txt == '-'){
        
        return '<span>'+ txt + '</span> Not applicable';
        
      }else if(txt == '?'){
        
        return '<span>'+ txt + '</span> Undefined';
        
      }
      
      switch (columns[columnIndex]['type']){
        
        case 'free':
        
          return txt;
        
        break;
        
        case 'quantitative':
        
          return 'Quantitative: <span>'+txt+'</span>';
        
        break;
        
        case 'controlled':
        
          if(typeof(columns[columnIndex]['states']) != 'undefined'){

            return '<span>'+ txt + ':</span> ' + columns[columnIndex]['states'][txt]['state'];

          }
        
        break;
        
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
       
       $("#dialog").fadeIn('fast');
       
       setTimeout(self.hideDialog, 2500);

     },
     
     hideDialog: function(){
       
       $("#dialog").fadeOut()
       
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
    
    addGroupEvents: function(){
      
      // Select group on click
      $('.character-group').click(function(){
        self.groupSelected($(this));
      });
      
    },
    
    initGroups: function(){

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
           .width(this.width + 3); // Knock of the 2 for the groups own border
                    
        }else{

          var oldWidth = parseInt(group.css('width'));
          
          group.width(oldWidth + this.width + 5);
          
        }
        
        prevGroupID = this.groupID;
 
      });
      
      if(group){
        group.appendTo($groups);
      }

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
           project_nid: self.getProjectNid(),
           no_tree: options['noTree']
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
          $('<p>For multiple states, please use <strong>/</strong> for <em>or</em> and <strong>+</strong> for <em>and</em>.<p>').appendTo($cellForm);
          
        }else if(typeof columnDef.validator == 'undefined'){ // This is a text field so add the textarea
          
          var defaultValue;
          
          $('<p>This is a <em>free state text character</em> - please enter text in the table cell or the field below.<p>').appendTo($cellForm);
          
          var $resizableTextarea = $(".note-form .resizable-textarea").clone()
          
          if(typeof value != 'undefined'){
            defaultValue = value;
          }
          
          var $textarea = $resizableTextarea.find('textarea');
          
          $textarea.attr('id', "text-state-dynamic-entry");
          $textarea.val(defaultValue);
          $textarea.change(function () {

            $('input.editor-text').val($(this).val());
            $('input.editor-text').effect("highlight", {color:"#9ADE0B"}, 300);
            grid.commitCurrentEdit();

          });

          $textarea.appendTo($cellForm);
          
          $container.keyup(function(){

            $textarea.val($(this).find('input').val());

          })
          
        }else{ // This is numeric
          
          $('<p>This is a <em>numeric character</em> - please enter only numeric values.<p><p>For numeric ranges, please enter as <em>x-y</em>.</p>').appendTo($cellForm);
          
        }
        
       $("#cell-data .note-form input[name='taxa_tid']").val(dataContext.id);
       $("#cell-data .note-form input[name='character_tid']").val(columnDef.id);

       self.toggleDataPanelItem('cell-data');

       // Are there notes / free state info?
       if(intersection_info[columnDef.id] && intersection_info[columnDef.id][dataContext.id]){

         var info = intersection_info[columnDef.id][dataContext.id];

         if(info.note){
           note_body = info.note.body;
           note_nid = info.note.nid;
         }

       }
       
       $('form.note-form textarea[name="body"]').val(note_body);
       $('input#note-nid').val(note_nid);


     },
    

    
    /* character form */
    renumberStates: function() {
      
      $('#nexus-states div.state-form:not(.ui-sortable-placeholder)').each(function(i){

        $(this).find('input.state-nid').attr({
          id: 'edit-states-'+i+'-nid',
          name: 'states['+i+'][nid]'
        });
        
        $(this).find('input.delta')
          .attr({
            id: 'edit-states-'+i+'-nid',
            name: 'states['+i+'][delta]'
          })
          .val(i);
        
        $(this).find('div.state input.state').attr({
          id: 'edit-states-'+i+'-state',
          name: 'states['+i+'][state]'
        });

        
        $(this).find('div.state label').attr('for', 'edit-states-'+i+'-state').html('State '+(i + 1)+':');
        
        

        
        $(this).find('div.state-description label').attr('for', 'edit-states-'+i+'-state-description');
        
        $(this).find('div.state-description textarea').attr({
          id: 'edit-states-'+i+'-state-description',
          name: 'states['+i+'][state_description]'
        });
        

      });
      
      
    },
    
    initCharacterFormEvents: function() {

       $('#nexus-states').sortable({ 
         containment: 'parent' , 
         axis: 'y', 
         handle: 'span.handle', 
         items: 'div.state-form' , 
         update: function(event, ui) {
           
           self.renumberStates();
       
         }
       
       });  

       if($("input[name='type']:checked").val() == 'controlled'){
         $('#nexus-state-wrapper').show();  
       }
       
       $('#character-type .description p.'+$("input[name='type']:checked").val()).show();
       $("input[name='type']:checked").parents('#character-type .form-radios .form-item').addClass('selected');  
       
       // Show / hide options on creating character form
       $("input[name='type']").click( //CLick works better than .change() in IE7
         function()
         {

           $('#character-type div.selected').removeClass('selected');
           $(this).parents('#character-type .form-radios .form-item').addClass('selected');   
           
           $('#character-type .description p').hide();
           $('#character-type .description p.'+$(this).val()).show();
           
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
     	 
       $('input.delete').click(function(){
   	   
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

           self.initCharacterFormEvents();

         },
         'json'
       );

     },
    
    // AHAH action callback
    addCharacterCallback: function(response){

        // Add the new character group to the drop down
        $("#edit-new-group").val("");

        if(response.group_tid && response.group_name){

          var option = '<option value="'+response.group_tid+'">'+response.group_name+'</option>';
          
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
      self.displayDialog();
      
    },
    
    deleteGroupCallback: function(response){
      
      $('#confirm-delete').hide();
      self.displayDialog();
      
    },
    
    groupSubmitCallback: function(response){

       self.updateDialog(response.data);

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
         form_build_id: $input.parents('form').find("input[name='form_build_id']").val()
       }

       $.post(Drupal.settings.nexusCallback+'/delete_state', args, function(){                  
         parentStateForm.remove();
         self.renumberStates();
       });

     },
    

    // AHAH action callback
    addStateCallback: function(response){
      
      self.attachStateFormEvents();
    
    },


    // AHAH action callback
    updateProjectCallback: function(response){
      
      $('#tabs-wrapper h2').html(response.title);
      
      self.displayDialog();
    
    },
    
    // AHAH action callback
    organiseCharactersCallback: function(response){
      // Remove the changed warnings
      $('#character-list .warning').remove();
      self.displayDialog();
    
    },

    updateTaxonomyCallback: function(response){
      
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
    
    markChanged: function($element){
      
      var marker = Drupal.theme('tableDragChangedMarker');
      if ($('span.tabledrag-changed', $element).length == 0) {
        $element.append(marker);
      }
      
      if(!$('div.warning').length){
        $('#character-list').append($(Drupal.theme('nexusChangedWarning')));
      }
      
    },
    
    buildOrganiseCharactersTab: function(){
      
      //  Only rebuild if the matrix has regenerated or first time tabs being viewed
      if($('#character-lists').length && (typeof (options.regenerated) == 'undefined' || options.regenerated == false)){
        return;
      }
      
      var $ul = $('<ul class="clearfix" id="character-lists">');

     for (i=1;i<columns.length;i++)
      {
          if(!($ul.find('#'+columns[i]['groupID']).length)){
                             
          $ul.append('<li id="'+columns[i]['groupID'] + '" class="sortable-character-group nexus-collapsed"><input type="hidden" name="groups[]" value="'+columns[i]['groupID']+'" /><h6><span class="h6-title" title="'+columns[i]['group']+'">'+columns[i]['short_group']+'</span><a href="#" class="expand-contract"></a></h6><ul></ul></li>');
                           
      }
                           
          $ul.find('#'+columns[i]['groupID']+' ul').append('<li id="'+columns[i]['id']+'" title="'+columns[i]['term']+'"><input type="hidden" name="characters['+columns[i]['groupID']+'][]" value="'+columns[i]['id']+'" />'+columns[i]['short_term']+'</li>');

      }
      
      $('a.expand-contract', $ul).click(function(){
        
        var $li = $(this).parents('li:first');

        $li.toggleClass('nexus-collapsed');
        
        return false;
        
      });
      
      $('#character-list').html($ul);
      
      $ul.sortable({
        stop: function(event, ui) { self.markChanged($('h6 span', ui.item)) }
      });
      
      $('ul', $ul).sortable({
           connectWith: '.sortable-character-group ul',
           stop: function(event, ui) { self.markChanged(ui.item) },
           containment: $('#character-list')
      });
      
      options.regenerated = false;
      
      
    },
    
    buildExportDataTab: function(){

      if($('#edit-characters-to-export options').length &! options.regenerated){
        return;
      }

      var $select = $('#edit-characters-to-export').empty();
      
      for (i=1;i<columns.length;i++)
      {

          if(!($select.find('#'+columns[i]['groupID']).length)){
                
          $select.append('<optgroup id="'+columns[i]['groupID']+'" label="'+columns[i]['group']+'"></optgroup>');
                           
          }
          
          $('#'+columns[i]['groupID'], $select).append('<option selected="selected" value="'+columns[i]['id']+'">'+columns[i]['term']+'</option');

      }

  	  

      
    }

    
  };
  
}


Drupal.theme.prototype.nexusChangedWarning = function () {
  
  return '<div class="warning">' + Drupal.theme('tableDragChangedMarker') + ' ' + Drupal.t("Changes made will not be saved until the form is submitted. Adding or changing characters & states will reset this form.") + '</div>';

}





