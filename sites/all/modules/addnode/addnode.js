// $Id $
// Author: Obslogic (Mike Smith aka Lionfish)
$(document).ready(function()
{  
  var fieldid;
  $('span.addnode_form').each(function(){
    $(this).css('display','none');
  });
  
  $('select.addnode_select').each(function(){
    $(this).attr('disabled', true);
  });
  
  //the 'select from select box' has been selected
  $('a.addnode_select_link').click(function()
  {
    fieldid=this.id;
    $('select.addnode_select').filter('[@id='+fieldid+']').attr("disabled", false);
    $('input.addnode_source').filter('[@name=addnode_'+fieldid+']').val('');//clear to show not creating a node
    $('span.addnode_form').filter('.'+fieldid).hide();

  });

  //click on the general 'create new' links
  $('span.addnode_links').click(function()
  {
    fieldid=this.id;
    $('select.addnode_select').filter('[@id='+fieldid+']').attr("disabled", true);
    $('select.addnode_select').filter('[@id='+fieldid+']').selectNone();
  });
  
  //click on particular form type
  $('a.addnode_item').click(function()
  {
    var typeid=this.id;
    $('span.addnode_form').filter('[@id='+typeid+']').filter('.'+fieldid).siblings().hide();
    $('span.addnode_form').filter('[@id='+typeid+']').filter('.'+fieldid).show();
    $('input.addnode_source').filter('[@name=addnode_'+fieldid+']').val(typeid);
  });
});

//unselects every item in 'this'
jQuery.fn.selectNone = function()
{
  this.each(function()
  {
    for (var i=0;i<this.options.length;i++)
    {
      option = this.options[i];
      option.selected = false;
    }
  });
}
