// vertical positioning in the viewport

(function($){
  $.fn.vCenter = function(options) {
    var pos = {
      sTop : function() {
        return window.pageYOffset || document.documentElement && document.documentElement.scrollTop ||	document.body.scrollTop;
      },
      wHeight : function() { 
        return window.innerHeight || document.documentElement && document.documentElement.clientHeight || document.body.clientHeight;
      }
    };
    return this.each(function(index) {
      if (index == 0) {
        var $this = $(this);
        var elHeight = $this.height();
		    var elTop = pos.sTop() + (pos.wHeight() / 2) - (elHeight / 2) - 100;
        $this.css({
          position: 'absolute',
          marginTop: '0',
          top: elTop
        });


      }
    });
  };

})(jQuery);


