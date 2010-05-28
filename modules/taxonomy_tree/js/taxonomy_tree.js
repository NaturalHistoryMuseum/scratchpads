Drupal.behaviors.taxonomyTree = function(context) {

  $('div.taxonomy-tree:not(.tt-processed)', context)
    .addClass('tt-processed')
		.each(function() {			
			// treeview behavior added
			$(this).find('ul').treeview({
                          url: Drupal.settings.taxonomyTree.path,
                          toggle: function() {

                            $(this).one('ajaxComplete', function(){    
                            Drupal.attachBehaviors($(this));
                            });  

                          }
                        });
		})	

  $('ul input.taxonomy-tree-checkbox:not(.tt-processed)', context)
    .addClass('tt-processed')
    .click(function(event){

      var $li = $(this).parents('li:eq(0)');
      var tid = getTid($li.attr('id'));

      $li.attr('id', tid +'-'+ ($(this).is(':checked') ? 1 : 0));  
      event.stopPropagation();

    })  
      
}

getTid = function(id){

  var split_id = [];
  split_id = id.split('-');
  return split_id[0];
  
}









