Drupal.behaviors.remote_issues_block = function(context){
  var remote_issues_block_hidden = true;
  $('#remote-issue-tab #open-close', context).click(function(){
    if(remote_issues_block_hidden){
      $('#remote-issue-tab').animate({right:0}, 1000);
      remote_issues_block_hidden = false;
    } else {
      $('#remote-issue-tab').animate({right:-375}, 1000);
      remote_issues_block_hidden = true;      
    }
  });
  var timeoutid = false;
  $('#remote-issue-tab .items li').mousemove(function(){
    var parentthis = this;    
    if(timeoutid){
      window.clearTimeout(timeoutid);    
    }
    timeoutid = window.setTimeout(function(){
      $(parentthis).children().children('p').slideDown(200);
      $(parentthis).siblings().each(function(){
        $(this).children().children('p').slideUp(1000);
      });          
    }, 500);
  });
};