// $Id: acquia-prosper-script.js,v 1.1.2.1 2009/09/03 09:30:00 sociotech Exp $

Drupal.behaviors.prosperBlocks = function (context) {
  $(".prosper-rounded-title h2.block-title").corner("top 5px"); 
  $(".prosper-shoppingcart h2.block-title").corner("top 5px"); 
  $(".prosper-menu-list h2.block-title").corner("top 5px"); 
  $(".prosper-grayborder-darkbackground .inner").corner("7px"); 
};