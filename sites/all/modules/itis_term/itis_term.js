Drupal.itis_term = new Object;

Drupal.itis_term.init = function(context) {
  $('.itis_term_unit input', context).keydown(function(){
    Drupal.itis_term.showorhide_unit_fields(context);
  });
  $('.itis_term_unit input', context).keyup(function(){
    Drupal.itis_term.showorhide_unit_fields(context);
  });
  $('.itis_term_unit input', context).bind('focusout', function(){
    Drupal.itis_term.showorhide_unit_fields(context);
  });
  $('#itis_term_usage select', context).change(function(){
    Drupal.itis_term.showorhide_usage_fields(context);
  });
  Drupal.itis_term.showorhide_usage_fields(context);
  Drupal.itis_term.showorhide_unit_fields(context);
  $('#itis_term_name').hide();
}

Drupal.itis_term.showorhide_unit_fields = function(context){
  if($('#itis_term_unit_name1 input', context).val() == ''){
    $('#itis_term_unit_name2', context).hide(500);
    $('#itis_term_unit_ind1', context).hide(500);
  } else {
    $('#itis_term_unit_name2', context).fadeIn(500);
    $('#itis_term_unit_ind1', context).fadeIn(500);
    $('#itis_term_name input').val($('#itis_term_unit_name1 input', context).val());
  }
  if($('#itis_term_unit_name2 input', context).val() == ''){
    $('#itis_term_unit_name3', context).hide(500);
    $('#itis_term_unit_ind2', context).hide(500);
  } else {
    $('#itis_term_unit_name3', context).fadeIn(500);
    $('#itis_term_unit_ind2', context).fadeIn(500);
  }
  if($('#itis_term_unit_name3 input', context).val() == ''){
    $('#itis_term_unit_name4', context).hide(500);
    $('#itis_term_unit_ind3', context).hide(500);
  } else {
    $('#itis_term_unit_name4', context).fadeIn(500);
    $('#itis_term_unit_ind3', context).fadeIn(500);
  }
  if($('#itis_term_unit_name4 input', context).val() == ''){
    $('#itis_term_unit_ind4', context).hide(500);
  } else {
    $('#itis_term_unit_ind4', context).fadeIn(500);
  }
}

Drupal.itis_term.showorhide_usage_fields = function(context){
  if($('#itis_term_usage select').val()==='invalid' || $('#itis_term_usage select').val()==='not accepted'){
    $('#itis_term_accepted_name').fadeIn(500);
    $('#itis_term_unacceptability_reason').fadeIn(500);
  } else {
    $('#itis_term_accepted_name').fadeOut(500);
    $('#itis_term_unacceptability_reason').fadeOut(500);
  }
}

Drupal.behaviors.itis_term = function(context){
  Drupal.itis_term.init(context);
};