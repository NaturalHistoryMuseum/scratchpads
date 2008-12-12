// $Id: childForm.js,v 1.2.2.3 2007/08/15 17:10:22 mh86 Exp $

/**
 *@file generates nested children terms, which are loaded through ahah
 */

//global killswitch
if (Drupal.jsEnabled) {
  $(document).ready(function() {
    Drupal.attachChildForm();  
  });
}

/**
 * add click events to expandable parents, where child terms have to be loaded
 */
Drupal.attachChildForm = function(subTree) {
  var list = "li.has-children div.hitArea";
  if (subTree) {
    list = $(subTree).find(list);
  }
  
  $(list).click(function() {
    Drupal.loadChildForm($(this).parent());
  });
}

/**
 * add click events to expandable parents to next siblings
 */
Drupal.attachChildFormToSiblings = function(all, currentIndex) {
  var nextSiblings = all.gt(currentIndex);
  $(nextSiblings).filter('.has-children').find('div.hitArea').click(function() {
    Drupal.loadChildForm($(this).parent());
  });
}

/**
 * loads child terms and appends html to list
 * adds treeview, weighting etc. js to inserted child list
 */
Drupal.loadChildForm = function(li, update) {
  if ($(li).is(".has-children") || update == true) {
    var parentId = Drupal.getTermId(li);
    var url = Drupal.settings.childForm['url'];
    url += '/'+ Drupal.getTreeId() +'/'+ Drupal.getVocId() +'/'+ parentId;
    $.get(url, null, function(data) {
      $(li).find("ul").remove();
      $(li).find("div.term-line").after(data);
      var ul = $(li).find("ul");
      Drupal.attachTreeview(ul);
      Drupal.attachSiblingsForm(ul);
      Drupal.attachUpdateWeightTerms(li);
      Drupal.attachChildForm(li);
      Drupal.attachTermData($(li).find("ul"));
      $(li).removeClass("has-children");
    });     
  }
}

/**
 * function for reloading root tree elements
 */
Drupal.loadRootForm = function() {
  var treeId = Drupal.getTreeId();
  var url = Drupal.settings.childForm['url'];
  url += '/'+ treeId +'/'+ Drupal.getVocId() +'/0/true';
  $.get(url, null, function(data) {
    $('#'+ treeId).html(data);
    var ul = $('#'+ treeId).find("ul");
    Drupal.attachTreeview(ul);
    Drupal.attachSiblingsForm();
    Drupal.attachUpdateWeightTerms();
    Drupal.attachChildForm();
    Drupal.attachTermData();
  });
}


