Drupal.behaviors.remote_issues_block = function(context){
  var remote_issues_block_hidden = true;
  $('#block-remote_issue_block-0', context).click(function(){
    if(remote_issues_block_hidden){
      $('#block-remote_issue_block-0').animate({right:0}, 1000);
      remote_issues_block_hidden = false;
    } else {
      $('#block-remote_issue_block-0').animate({right:-375}, 1000);
      remote_issues_block_hidden = true;      
    }
  });
};