(function($) {

  $.extend(true, window, {
    Batch: Batch
  });
  
  function Batch($dom, node_type)
  {
    var bid;
    var self = this;
    var uploader = $dom.pluploadQueue();
    var objHttpDataRequest;
    
    this.init = function(){
      
      uploader.bind('FileUploaded', self.fileUploaded);
      
    }
    
    this.fileUploaded = function(){
      
      // If BID isn't defined, get it
      if(!bid){
        if(!objHttpDataRequest){
          self.getBID();
        }
      }else if(self.complete()){ // Or if this is the last image to be uploaded
        self.addLink();
      }
      
    }
    
    this.getBID = function(){
      
      objHttpDataRequest = $.ajax({
        url: "scratchpad-slickgrid/callback/get-bid",
        dataType: 'json',
        success: function(response){
          
          bid = response.bid;

          if(self.complete()){
            self.addLink();
          }

        },

      });
      
    }
    
    this.complete = function(){
      
      return uploader.total.uploaded == uploader.files.length;
      
    }
    
    this.addLink = function(){
      
      
      $dom.append('<a class="slickgrid-edit-link" href="/grid/'+node_type+'/'+bid+'">Edit these items</a>');
      
      
    }
    
    this.init();
    
  }
    


})(jQuery);