$(document).ready(function(){
  $('.partnersmapinstitution').bt({positions: 'left',fill: 'rgba(0, 0, 0, .7)',cssStyles: {color: 'white', 'font-size': '14px', width: 'auto'},closeWhenOthersOpen: true,spikeLength: 10,strokeWidth: 0});
  $('.partnersmapinstitution').click(function(){
    $('#partnerswps li a').css('color','');
    $('#partnerswps li').css('background-color','transparent');
    $('.partnersmapinstitution').css('border', 'none');
    $(this).css('border', 'solid 2px yellow');
    var fadein_id = '#'+$(this).attr('id')+'_info';
    if($('#partnersinfo div:visible').length){
      $('#partnersinfo div:visible').fadeOut('500', function(){
        $(fadein_id).fadeIn('500');
      });
    } else {
      $(fadein_id).fadeIn('500');
    }
  });
  $('#partnerswps li').click(function(){
    $('#partnerswps li a').css('color','');
    $('#partnerswps li').css('background-color','transparent');
    $(this).css('background-color','black');
    $(this).children('a').css('color','white');
    $('.partnersmapinstitution').css('border', 'none');
    $('.partnersmapinstitution.'+$(this).attr('id')).css('border', 'solid 2px yellow');
    var fadein_id = '#'+$(this).attr('id')+'_info';
    if($('#partnersinfo div:visible').length){
      $('#partnersinfo div:visible').fadeOut('500', function(){
        $(fadein_id).fadeIn('500');
      });
    } else {
      $(fadein_id).fadeIn('500');
    }
    return false;
  });
});