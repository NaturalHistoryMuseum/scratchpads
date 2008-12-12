// $Id: tree.js,v 1.4.2.4 2007/08/19 11:51:13 mh86 Exp $

/**
 * @files js for collapsible tree view with some helper functions for updating tree structure
 */

//global killswitch
if (Drupal.jsEnabled) {
  $(document).ready(function() {
    var settings = Drupal.settings.taxonomytree || [];
    if (settings['id']) {
      if (!(settings['id'] instanceof Array)) {
        var ul = $('#'+ settings['id']).find("ul");
        Drupal.attachTreeview(ul);
        Drupal.attachThrobber();
        
      }
    }
  });
}

/**
 * adds collapsibel treeview to a given list
 */
Drupal.attachTreeview = function(ul) {
  $(ul)
    .addClass("treeview")
    .find("li[ul]").prepend("<div class='hitArea'/>").find("ul").hide().end()
    .find("div.hitArea").click(function() {
      Drupal.toggleTree(this);
    });
}

/**
 * adds treeview to next siblings
 */
Drupal.attachTreeviewToSiblings = function(all, currentIndex) {
  var nextSiblings = all.gt(currentIndex);
  nextSiblings.children("ul").each(function() {
    var ul_nested = $(this);
    var li = $(ul_nested).parent();
    $(li).prepend("<div class='hitArea'/>");
    $(ul_nested).hide();
    $(li).find("div.hitArea").click(function() {
      Drupal.toggleTree(this);
    });
  });
}

/**
 * toggles a collapsible/expandable tree element by swaping classes
 */
Drupal.toggleTree = function(node) {
  $(node).parent().find("ul:first").toggle();
  Drupal.swapClasses(node.parentNode, "expandable", "collapsable");
  Drupal.swapClasses(node.parentNode, "lastExpandable", "lastCollapsable");
}

/**
 * helper function for swapping two classes
 */
Drupal.swapClasses = function(node, c1, c2) {
  if ($.className.has(node, c1)) {
    $(node).removeClass(c1).addClass(c2);
  } 
  else if ($.className.has(node, c2)) {
    $(node).removeClass(c2).addClass(c1);
  } 
}

/**
 * returns terms id of a given list element
 */
Drupal.getTermId = function(li) {
  var id = $(li).find("input:hidden[@class=term-id]").attr("value");
  return id;
}

/**
 * return term id of a prent of a given list element
 * if no parent exists (root level), returns 0
 */
Drupal.getParentId = function(li) {
  var parentId;
  try {
    var parentLi = $(li).parent("ul").parent("li");
    parentId = Drupal.getTermId(parentLi);
  } catch(e) {
    return 0;
  }
  return parentId;
}

/**
 * update classes for tree view, if list elements get swaped
 */
Drupal.updateTree = function(upTerm, downTerm) {  
  if ($(upTerm).is(".last")) {
    $(upTerm).removeClass("last");
    Drupal.updateTreeDownTerm(downTerm); 
  }
  else if ($(upTerm).is(".lastExpandable")) {
    $(upTerm).removeClass("lastExpandable").addClass("expandable");
    Drupal.updateTreeDownTerm(downTerm); 
  }
  else if ($(upTerm).is(".lastCollapsable")) {
    $(upTerm).removeClass("lastCollapsable").addClass("collapsable");
    Drupal.updateTreeDownTerm(downTerm);  
  }
}

/**
 * update classes for tree view for a list element moved downwards
 */
Drupal.updateTreeDownTerm = function(downTerm) {
  if ($(downTerm).is(".expandable")) {
    $(downTerm).removeClass("expandable").addClass("lastExpandable");
  }
  else if ($(downTerm).is(".collapsable")) {
    $(downTerm).removeClass("collapsable").addClass("lastCollapsable");
  }
  else {
    $(downTerm).addClass("last");
  }
}

/**
 * returns Tree Id
 */
Drupal.getTreeId = function() {
  return Drupal.settings.taxonomytree['id']; 
}

/**
 * return Voc Id
 */
Drupal.getVocId = function() {
  return Drupal.settings.taxonomytree['vid']; 
}

/**
 * attaches a throbber element to the taxonomy manager
 */
Drupal.attachThrobber = function() {
  var div = '#taxonomy-manager';
  $('<div><img src="'+ Drupal.settings.taxonomy_manager['modulePath'] +'images/ajax-loader.gif" alt="" height="25"></div>').hide()
    .ajaxStart(function(){
      $(this).show();
      $(div).css('opacity', '0.5');
    })
    .ajaxStop(function(){
      $(this).hide();
      $(div).css('opacity', '1');
    })
    .appendTo("#taxonomy-manager-toolbar-throbber"); 
}
