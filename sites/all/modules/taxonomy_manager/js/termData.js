// $Id: termData.js,v 1.1.2.3 2007/11/24 18:31:18 mh86 Exp $

/**
 * @file js support for term editing form for ajax saving and tree updating
 */

//global killswitch
if (Drupal.jsEnabled) {
  $(document).ready(function() {
    //use tree settings....
    var settings = Drupal.settings.taxonomytree || [];
    if (settings['id']) {
      if (!(settings['id'] instanceof Array)) {
        if (Drupal.settings.termData['tid']) {
          Drupal.termDataForm(Drupal.settings.termData['tid'], Drupal.settings.termData['term_url']);
        }
        var ul = $('#'+ settings['id']).find("ul");
        Drupal.attachTermData(ul);
      }
    }
  });
}

/**
 * adds click events to the term links in the tree structure
 */
Drupal.attachTermData = function(ul) {
  $(ul).find('a.term-data-link').click(function() {
    var li = $(this).parents("li");
    var tid = Drupal.getTermId(li);
    Drupal.termDataLoad(this.href, tid, li);
    return false;
  });
}

/**
 * attaches click events to next siblings
 */
Drupal.attachTermDataToSiblings = function(all, currentIndex) {
  var nextSiblings = all.gt(currentIndex);
  $(nextSiblings).find('a.term-data-link').click(function() {
    var li = $(this).parents("li");
    var tid = Drupal.getTermId(li);
    Drupal.termDataLoad(this.href, tid, li);
    return false;
  });
}

/**
 * loads ahah form from given link and displays it on the right side
 */
Drupal.termDataLoad = function(href, tid, li) {
  var url = href +'/true';
  $.get(url, null, function(data) {
    var div = $('#taxonomy-term-data');
    $(div).html(data);
    Drupal.termDataForm(tid, href, li);
  });
}

/**
 * adds events to possible operations
 */
Drupal.termDataForm = function(tid, href, li) {
  try {
    Drupal.autocompleteAutoAttach();
    Drupal.textareaAttach();
  } catch(e) {} //autocomplete or textarea js not added to page
  
  var param = new Object();
  param['tid'] = tid;
  
  $('.term-data-autocomplete-add').click(function() {
    param['attr_type'] = $(this).find("img").attr("class");
    param['value'] = $(this).parent().find('input:text').attr('value');
    param['op'] = 'add';
    Drupal.termDataSend(li, tid, href, param);
  });
  
  $('.taxonomy-term-data-operations').click(function() {
    param['attr_type'] = $(this).find("img").attr("class");
    var value = $(this).siblings(".taxonomy-term-data-name").attr("id");
    param['value'] = value.substring(5);
    param['op'] = 'delete';
    Drupal.termDataSend(li, tid, href, param);
    $(this).parent().remove();
  });
  
  $('#term-data-name-save').click(function() {
    param['attr_type'] = 'name';
    param['value'] = $('#term-data-name-field').find('input:text').attr('value');
    param['op'] = 'update';
    Drupal.termDataSend(li, tid, href, param);
    Drupal.updateTermName(tid, param['value']);
  });
  
  $('#term-data-description-save').click(function() {
    param['value'] = $('#term-data-description-field').find('textarea').attr('value');
    param['attr_type'] = 'description';
    param['op'] = 'update';
    Drupal.termDataSend(li, tid, href, param);
  });
  
  $('#edit-term-data-weight').change(function() {
    param['value'] = this.value;
    param['attr_type'] = 'weight';
    param['op'] = 'update';
    Drupal.termDataSend(li, tid, href, param);
  });
}

/**
 * sends updated data through param
 */
Drupal.termDataSend = function(li, tid, href, param) {
  var url= Drupal.settings.termData['url'];
  if (param['value'] != '' && param['attr_type'] != '') {
    //synchronous to ensure consistent data
    $.ajax({
      data: param, 
      type: "POST", 
      url: url,
      complete: function() {
        Drupal.termDataUpdate(li, tid, href, param);
      }
    });
  }
}

/**
 * updates term data form and if necessary tree structure
 */
Drupal.termDataUpdate = function(li, tid, href, param) {
  if (param['op'] == 'add') {
    Drupal.termDataLoad(href, tid);
  }
  if (param['attr_type'] == 'parent' || (param['attr_type'] == 'related' && param['op'] == 'add')) {
    Drupal.loadRootForm();
  }
  else if (param['attr_type'] == 'weight') {
    var parentLi = $(li).parents("li");
    try {
      parentLi.attr("id");
      Drupal.loadChildForm(parentLi, true);
    } catch (e) {
      //no parent li --> root level terms
      //load whole Tree
      Drupal.loadRootForm();
    }
  }
}

/**
 * updates term name in tree structure
 */
Drupal.updateTermName = function(tid, name) {
  $('fieldset#taxonomy-term-data-fieldset legend').html(name);
  $('ul.treeview li input:hidden[@class=term-id][@value='+ tid +']')
    .siblings('div.term-item')
    .find('div.form-item label.option a').html(name);
}
