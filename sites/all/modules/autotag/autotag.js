// On initial load, add dragable to all li dragable items
$(document).ready(function () {
  // Prevent duplicate attachment of the collapsible behavior.
  $ui('.autotag-dragable').draggable({helper: 'clone'});
  $ui('#autotag-dnd-right').droppable({
  	accept: ".autotag-dragable",
  	activeClass: 'autotag-dnd-drop-active',
  	hoverClass: 'autotag-dnd-drop-hover',
  	drop: function(ev, ui) {
  		ui.draggable.appendTo("#autotag-dnd-right>ul");
  	}
  });
  $ui('#autotag-dnd-left').droppable({
  	accept: ".autotag-dragable",
  	activeClass: 'autotag-dnd-drop-active',
  	hoverClass: 'autotag-dnd-drop-hover',
  	drop: function(ev, ui) {
  		ui.draggable.appendTo("#autotag-dnd-left>ul");
  	}
  });
  $ui('#autotag-dnd-right>ul').droppable({
  	accept: ".autotag-dragable",
  	activeClass: 'autotag-dnd-drop-active',
  	hoverClass: 'autotag-dnd-drop-hover',
  	drop: function(ev, ui) {
  		ui.draggable.appendTo("#autotag-dnd-right");
  	}
  });
  $ui('#autotag-dnd-left>ul').droppable({
  	accept: ".autotag-dragable",
  	activeClass: 'autotag-dnd-drop-active',
  	hoverClass: 'autotag-dnd-drop-hover',
  	drop: function(ev, ui) {
  		ui.draggable.appendTo("#autotag-dnd-left");
  	}
  });
  $ui('.autotag-dragable').droppable({
  	accept: ".autotag-dragable",
  	drop: function(ev, ui) {
  		ui.draggable.appendTo($(this).parent());
  	}
  });
});