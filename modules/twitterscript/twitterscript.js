Drupal.twitterscript = new Object;
Drupal.behaviors.twitterscript = function(context){
  Drupal.twitterscript.init(context);
};
Drupal.twitterscript.init = function(context){
  $('.twitterscript', context).each(function(){
    var twitterscript = this;
    $.getJSON('http://search.twitter.com/search.json?rpp=5&q=' + $(this).html() + '&callback=?', function(data){
      var html_to_embed = '<ul class="twitterscript-list">';
      $.each(data.results, function(){
        html_to_embed += '<li><a href="http://twitter.com/'+this.from_user+'"><img src="'+this.profile_image_url+'"/></a>'+ autolink(this.text) +'</li>';
      });
      html_to_embed += '</ul>';
      $(twitterscript).replaceWith(html_to_embed);
    });
  });
}

function autolink(s){
  var hlink = /\s(ht|f)tp:\/\/([^ \,\;\:\!\)\(\"\'\<\>\f\n\r\t\v])+/g;
  return (s.replace (hlink, function ($0,$1,$2){
    s = $0.substring(1,$0.length); 
    // remove trailing dots, if any
    while (s.length>0 && s.charAt(s.length-1)=='.') 
      s=s.substring(0,s.length-1);
    // add hlink
    return " " + s.link(s); 
  }));
}