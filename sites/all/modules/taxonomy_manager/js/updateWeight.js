// $Id$

/**
 * @file js for changing weights of terms with Up and Down arrows
 */

//object to store weights (tid => weight)
var weights = new Object();

//global killswitch
if (Drupal.jsEnabled) {
  $(document).ready(function() {
    var settings = Drupal.settings.updateWeight || [];
    Drupal.attachUpdateWeightToolbar(settings['up'], settings['down']);
    Drupal.attachUpdateWeightTerms();     
  });
}

/**
 * adds click events for Up and Down buttons in the toolbar, which
 * allow the moving of selected (can be more) terms
 */
Drupal.attachUpdateWeightToolbar = function(upButton, downButton) {
  var selected;
  var url = Drupal.settings.updateWeight['url'];  
  
  $('#'+ upButton).click(function() {
    selected = Drupal.getSelectedTerms();
    for (var i=0; i < selected.length; i++) {
      var upTerm = selected[i];
      var downTerm = $(upTerm).prev(); 
    
      Drupal.orderTerms(upTerm, downTerm);
    }
    if (selected.length > 0) {
      $.post(url, weights);
    }
  });
  
  
  $('#'+ downButton).click(function() {
    selected = Drupal.getSelectedTerms();
    for (var i=selected.length-1; i >= 0; i--) {
      var downTerm = selected[i];
      var upTerm = $(downTerm).next();
      
      Drupal.orderTerms(upTerm, downTerm);
    }
    if (selected.length > 0) {
      $.post(url, weights);
    }
  });
}

/**
 * adds small up and down arrows to each term
 * arrows get displayed on mouseover
 */
Drupal.attachUpdateWeightTerms = function(parent, currentIndex) {
  var settings = Drupal.settings.updateWeight || [];
  var disable = settings['disable_mouseover'];
  
  if (!disable) {
    var url = Drupal.settings.updateWeight['url'];
    
    var termArrowsClass = '.term-operations';
    var termLineClass = '.term-line';
    var termUpClass = '.term-up';
    var termDownClass = '.term-down';
    
    if (parent && currentIndex) {
      parent = parent.gt(currentIndex);
    }
    if (parent) {
      termArrowsClass = $(parent).find(termArrowsClass);
      termLineClass = $(parent).find(termLineClass);
      termUpClass = $(parent).find(termUpClass);
      termDownClass = $(parent).find(termDownClass);
    }
    
    $(termArrowsClass).hide();
    
    $(termLineClass).mouseover(function() {
      $(this).find('.term-operations').show();
    });
    
    $(termLineClass).mouseout(function() {
      $(this).find('.term-operations').hide();
    });
    
    
    $(termUpClass).click(function() {
      var upTerm = $(this).parents("li").eq(0);
      var downTerm = $(upTerm).prev(); 
    
      Drupal.orderTerms(upTerm, downTerm);
      $.post(url, weights);
    
      $(downTerm).find(termLineClass).unbind('mouseover');
      setTimeout(function() {
        $(upTerm).find('.term-operations').hide();
        $(downTerm).find(termLineClass).mouseover(function() {
          $(this).find('.term-operations').show();
        });
      }, 1500);
    
    });
  
  
    $(termDownClass).click(function() {
      var downTerm = $(this).parents("li").eq(0);
      var upTerm = $(downTerm).next();
    
      Drupal.orderTerms(upTerm, downTerm);
      $.post(url, weights);
    
      $(upTerm).find(termLineClass).unbind('mouseover');
      setTimeout(function() {
        $(downTerm).find('.term-operations').hide();
        $(upTerm).find(termLineClass).mouseover(function() {
          $(this).find('.term-operations').show();
        });
      }, 1500); 
    });
  }

}

/**
 * return array of selected terms
 */
Drupal.getSelectedTerms = function() {
  var terms = new Array();
  $('.treeview').find("input[@type=checkbox][@checked]").each(function() {
    var term = $(this).parents("li").eq(0);
    terms.push(term);
  });
  
  return terms;
}

/**
 * reorders terms
 *   - swap list elements in DOM
 *   - post updated weights to callback in php
 *   - update classes of tree view
 */
Drupal.orderTerms = function(upTerm, downTerm) {
  try {
    Drupal.getTermId(upTerm);
    Drupal.swapTerms(upTerm, downTerm);
    Drupal.swapWeights(upTerm, downTerm);
    Drupal.updateTree(upTerm, downTerm);
  } catch(e) {
    //no next item, because term to update is last child, continue
  }
}

/**
 * simple swap of two elements
 */
Drupal.swapTerms = function(upTerm, downTerm) { 
  $(upTerm).after(downTerm);
  $(downTerm).before(upTerm);
}

/**
 * updating weights of swaped terms
 * if two terms have different weights, then weights are being swaped
 * else, if both have same weights, upTerm gets decreased
 *
 * if prev/next siblings of up/down terms have same weights as current
 * swaped, they have to be updated by de/increasing weight (by 1) to ensure
 * unique position of swaped terms
 */
Drupal.swapWeights = function(upTerm, downTerm) {
  var upWeight = Drupal.getWeight(upTerm);
  var downWeight = Drupal.getWeight(downTerm);
  var downTid = Drupal.getTermId(downTerm);
  var upTid = Drupal.getTermId(upTerm);
  
  //same weight, decrease upTerm
  if (upWeight == downWeight) {
    weights[upTid] = --upWeight;
  }
  //different weights, swap
  else {
    weights[upTid] = downWeight;
    weights[downTid] = upWeight;
  }
  
  
  var updateUpTerms = false;
  var updateDownTerms = false;
  
  try {
    if (Drupal.getWeight($(upTerm).prev()) == upWeight) {
      updateUpTerms = true;
    }
  } catch(e) {
    //no prev
  }
  
  try {
    if (Drupal.getWeight($(upTerm).next()) == downWeight) {
      updateDownTerms = true;
    }
  } catch(e) {
    //no next
  }
  
  //in/decrease all siblings if necessary
  if (updateUpTerms || updateDownTerms) {
    $(upTerm).siblings().each(function() {
      var id = Drupal.getTermId(this);
      if (id != downTid) {
        var weight = Drupal.getWeight(this);
        if (weight <= upWeight && updateUpTerms) {
          weights[id] = --weight;
        }
        else if (weight >= downWeight && updateDownTerms) {
          weights[id] = ++weight;
        }
      }
    });
  }
}

/**
 * helper to return weight of a term
 */
Drupal.getWeight = function(li) {
  var id = Drupal.getTermId(li);
  var weight;
  
  if (weights[id] != null) {
    weight = weights[id];
  }
  else {
    weight = $(li).find("input:hidden[@class=weight-form]").attr("value");
  }
  
  return weight;
}


