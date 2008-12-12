// $Id: siblingsForm.js,v 1.1.2.4 2007/08/18 16:49:05 mh86 Exp $

/**
 * @file loads next siblings, if only parts of long lists are initially loaded
 */
 
//global killswitch
if (Drupal.jsEnabled) {
  $(document).ready(function() {
    Drupal.attachSiblingsForm();  
  })
}

/**
 * adds link for loading next siblings terms, when click terms get loaded through ahah
 * adds all needed js like treeview, weightning, etc.. to new added terms
 */
Drupal.attachSiblingsForm = function(ul) {
  var url = Drupal.settings.siblingsForm['url'];
  var list = "li.has-more-siblings div.term-has-more-siblings";
  if (ul) {
    list = $(ul).find(list);
  }
  
  $(list).bind('click', function() {
    $(this).unbind("click");
    var li = this.parentNode.parentNode;
    var all = $('li', li.parentNode);
    var currentIndex = all.index(li);

    var page = Drupal.getPage(li);
    var prev_id = Drupal.getTermId(li);
    var parentId = Drupal.getParentId(li);
    
    url += '/'+ Drupal.getTreeId() +'/'+ page +'/'+ prev_id +'/'+ parentId;
    
    $.get(url, null, function(data) {
      $(li).find(".term-has-more-siblings").remove();
      $(li).after(data);
      Drupal.attachTreeviewToSiblings($('li', li.parentNode), currentIndex);
      Drupal.attachChildFormToSiblings($('li', li.parentNode), currentIndex);
      Drupal.attachUpdateWeightTerms($('li', li.parentNode), currentIndex);
      Drupal.attachTermDataToSiblings($('li', li.parentNode), currentIndex);
      
      $(li).removeClass("last").removeClass("has-more-siblings");
      $(li).find('.term-operations').hide();
      Drupal.swapClasses(li, "lastExpandable", "expandable");
      Drupal.attachSiblingsForm($(li).parent());
    });
  });
}

/**
 * helper function for getting out the current page
 */
Drupal.getPage = function(li) { 
  return $(li).find("input:hidden[@class=page]").attr("value");
}
