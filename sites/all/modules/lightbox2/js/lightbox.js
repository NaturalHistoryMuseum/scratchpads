/* $Id$ */

/**
 * jQuery Lightbox
 * @author
 *   Stella Power, <http://drupal.org/user/66894>
 *
 * Based on Lightbox v2.03.3 by Lokesh Dhakar
 * <http://www.huddletogether.com/projects/lightbox2/>
 * Also partially based on the jQuery Lightbox by Warren Krewenki
 *   <http://warren.mesozen.com>
 *
 * Originally written to make use of the Prototype framework, and
 * Script.acalo.us, now altered to use jQuery.
 *
 * Permission has been granted to Mark Ashmead & other Drupal Lightbox2 module
 * maintainers to distribute this file via Drupal.org
 * Under GPL license.
 *
 * Slideshow, iframe and video functionality added by Stella Power.
 *
 */

/**
 * Table of Contents
 * -----------------
 * Configuration
 * Global Variables
 * Lightbox Class Declaration
 * - initialize()
 * - updateImageList()
 * - start()
 * - changeImage()
 * - imgNodeLoadingError()
 * - imgLoadingError()
 * - resizeImageContainer()
 * - showImage()
 * - updateDetails()
 * - updateNav()
 * - enableKeyboardNav()
 * - disableKeyboardNav()
 * - keyboardAction()
 * - preloadNeighborImages()
 * - end()
 *
 * Miscellaneous Functions
 * - getPageScroll()
 * - getPageSize()
 * - pause()
 * - toggleSelectsFlash()
 * - parseRel()
 * - setStyles()
 *
 * Slideshow Functions
 * - togglePlayPause()
 *
 * Video Functions (requires lightbox_video.js)
 *
 * On load event
 * - initialize()
 *
 */


var Lightbox = {
  overlayOpacity : 0.6, // Controls transparency of shadow overlay.
  fadeInSpeed: 'normal', // Controls the speed of the image appearance.
  slideDownSpeed: 'slow', // Controls the speed of the image resizing animation.
  borderSize : 10, // If you adjust the padding in the CSS, you will need to update this variable.
  infoHeight: 20,
  alternative_layout : false,
  imageArray : [],
  imageNum : null,
  activeImage : null,
  inprogress : false,
  disableZoom : false,
  isZoomedIn : false,
  rtl : false,

  // Slideshow options.
  slideInterval : 5000, // In milliseconds.
  showPlayPause : true, // True to display pause/play buttons next to close.
  autoExit : true, // True to automatically close Lightbox after the last image.
  pauseOnNextClick : false, // True to pause the slideshow when the "Next" button is clicked.
  pauseOnPrevClick : true, // True to pause the slideshow when the "Prev" button is clicked.
  slideIdArray : [],
  slideIdCount : 0,
  isSlideshow : false,
  isPaused : false,

  // Video options.
  isVideo : false,
  videoId : false,
  videoWidth : 400,
  videoHeight : 400,
  videoHTML : null,

  // Iframe options.
  isLightframe : false,
  iframe_width : 600,
  iframe_height : 400,
  iframe_border : 1,


  // initialize()
  // Constructor runs on completion of the DOM loading. Calls updateImageList
  // and then the function inserts html at the bottom of the page which is used
  // to display the shadow overlay and the image container.
  initialize: function() {

    var settings = Drupal.settings.lightbox2;
    Lightbox.overlayOpacity = settings.overlay_opacity;
    Lightbox.rtl = settings.rtl;
    Lightbox.disableZoom = settings.disable_zoom;
    Lightbox.slideInterval = settings.slideshow_interval;
    Lightbox.showPlayPause = settings.show_play_pause;
    Lightbox.autoExit = settings.slideshow_automatic_exit;
    Lightbox.pauseOnNextClick = settings.pause_on_next_click;
    Lightbox.pauseOnPrevClick = settings.pause_on_previous_click;
    Lightbox.alternative_layout = settings.use_alt_layout;
    Lightbox.iframe_width = settings.iframe_width;
    Lightbox.iframe_height = settings.iframe_height;
    Lightbox.iframe_border = settings.iframe_border;

    // Attach lightbox to any links with rel 'lightbox', 'lightshow' or
    // 'lightframe'.
    Lightbox.updateImageList();

    // MAKE THE LIGHTBOX DIVS
    // Code inserts html at the bottom of the page that looks similar to this:
    // (default layout)
    //
    // <div id="overlay"></div>
    // <div id="lightbox">
    //  <div id="outerImageContainer">
    //    <div id="loading">
    //     <a href="#" id="loadingLink">
    //      <img src="images/loading.gif">
    //     </a>
    //    </div>
    //   <div id="imageContainer">  <!-- Or <div id="frameContainer">  Or <div id="videoContainer"> -->
    //    <img id="lightboxImage">  <!-- Or <iframe id="lightboxFrame">  -->
    //    <div style="" id="hoverNav">
    //     <a href="#" id="prevLink"></a>
    //     <a href="#" id="nextLink"></a>
    //    </div>
    //   </div>
    //  </div>
    //  <div id="imageDataContainer">
    //   <div id="imageData">
    //    <div id="imageDetails">
    //     <span id="caption"></span>
    //     <span id="numberDisplay"></span>
    //    </div>
    //    <div id="bottomNav">
    //     <a href="#" id="bottomNavClose">
    //      <img src="images/close.gif">
    //     </a>
    //    </div>
    //   </div>
    //  </div>
    // </div>

    var Body = document.getElementsByTagName("body").item(0);

    var Overlay = document.createElement("div");
    Overlay.setAttribute('id', 'overlay');
    Overlay.style.display = 'none';
    Body.appendChild(Overlay);

    var LightboxDiv = document.createElement("div");
    LightboxDiv.setAttribute('id', 'lightbox');
    LightboxDiv.style.display = 'none';
    Body.appendChild(LightboxDiv);

    var OuterImageContainer = document.createElement("div");
    OuterImageContainer.setAttribute('id', 'outerImageContainer');
    LightboxDiv.appendChild(OuterImageContainer);

    var VideoContainer = document.createElement("div");
    VideoContainer.setAttribute('id', 'videoContainer');
    VideoContainer.style.display = 'none';
    OuterImageContainer.appendChild(VideoContainer);

    var FrameContainer = document.createElement("div");
    FrameContainer.setAttribute('id', 'frameContainer');
    FrameContainer.style.display = 'none';
    OuterImageContainer.appendChild(FrameContainer);

    var LightboxFrame = document.createElement("iframe");
    LightboxFrame.setAttribute('id', 'lightboxFrame');
    LightboxFrame.style.display = 'none';
    if (!Lightbox.iframe_border) {
      LightboxFrame.style.border = 'none';
    }
    FrameContainer.appendChild(LightboxFrame);

    var ImageContainer = document.createElement("div");
    ImageContainer.setAttribute('id', 'imageContainer');
    OuterImageContainer.appendChild(ImageContainer);

    var Loading = document.createElement("div");
    Loading.setAttribute('id', 'loading');
    OuterImageContainer.appendChild(Loading);

    var LoadingLink = document.createElement("a");
    LoadingLink.setAttribute('id', 'loadingLink');
    LoadingLink.setAttribute('href', '#');
    Loading.appendChild(LoadingLink);

    var LightboxImage = document.createElement("img");
    LightboxImage.setAttribute('id', 'lightboxImage');
    ImageContainer.appendChild(LightboxImage);

    var HoverNav = document.createElement("div");
    HoverNav.setAttribute('id', 'hoverNav');

    var PrevLink = document.createElement("a");
    PrevLink.setAttribute('id', 'prevLink');
    PrevLink.setAttribute('href', '#');

    var NextLink = document.createElement("a");
    NextLink.setAttribute('id', 'nextLink');
    NextLink.setAttribute('href', '#');

    var ImageDataContainer = document.createElement("div");
    ImageDataContainer.setAttribute('id', 'imageDataContainer');
    ImageDataContainer.className = 'clearfix';

    var ImageData = document.createElement("div");
    ImageData.setAttribute('id', 'imageData');

    var FrameHoverNav = document.createElement("div");
    FrameHoverNav.setAttribute('id', 'frameHoverNav');

    var FramePrevLink = document.createElement("a");
    FramePrevLink.setAttribute('id', 'framePrevLink');
    FramePrevLink.setAttribute('href', '#');

    var FrameNextLink = document.createElement("a");
    FrameNextLink.setAttribute('id', 'frameNextLink');
    FrameNextLink.setAttribute('href', '#');

    var ImageDetails = document.createElement("div");
    ImageDetails.setAttribute('id', 'imageDetails');

    var Caption = document.createElement("span");
    Caption.setAttribute('id', 'caption');

    var NumberDisplay = document.createElement("span");
    NumberDisplay.setAttribute('id', 'numberDisplay');

    var BottomNav = document.createElement("div");
    BottomNav.setAttribute('id', 'bottomNav');

    var BottomNavCloseLink = document.createElement("a");
    BottomNavCloseLink.setAttribute('id', 'bottomNavClose');
    BottomNavCloseLink.setAttribute('href', '#');

    var BottomNavZoomLink = document.createElement("a");
    BottomNavZoomLink.setAttribute('id', 'bottomNavZoom');
    BottomNavZoomLink.setAttribute('href', '#');

    var BottomNavZoomOutLink = document.createElement("a");
    BottomNavZoomOutLink.setAttribute('id', 'bottomNavZoomOut');
    BottomNavZoomOutLink.setAttribute('href', '#');

    // Slideshow play / pause buttons
    var LightshowPause = document.createElement("a");
    LightshowPause.setAttribute('id', 'lightshowPause');
    LightshowPause.setAttribute('href', '#');
    LightshowPause.style.display = 'none';

    var LightshowPlay = document.createElement("a");
    LightshowPlay.setAttribute('id', 'lightshowPlay');
    LightshowPlay.setAttribute('href', '#');
    LightshowPlay.style.display = 'none';

    if (!settings.use_alt_layout) {
      ImageContainer.appendChild(HoverNav);
      HoverNav.appendChild(PrevLink);
      HoverNav.appendChild(NextLink);

      LightboxDiv.appendChild(ImageDataContainer);
      ImageDataContainer.appendChild(ImageData);

      ImageData.appendChild(FrameHoverNav);
      FrameHoverNav.appendChild(FramePrevLink);
      FrameHoverNav.appendChild(FrameNextLink);

      ImageData.appendChild(ImageDetails);
      ImageDetails.appendChild(Caption);
      ImageDetails.appendChild(NumberDisplay);

      ImageData.appendChild(BottomNav);
      BottomNav.appendChild(BottomNavCloseLink);
      BottomNav.appendChild(BottomNavZoomLink);
      BottomNav.appendChild(BottomNavZoomOutLink);
      BottomNav.appendChild(LightshowPause);
      BottomNav.appendChild(LightshowPlay);

    }

    // New layout.
    else {
      LightboxDiv.appendChild(ImageDataContainer);

      ImageDataContainer.appendChild(ImageData);

      ImageData.appendChild(HoverNav);
      HoverNav.appendChild(PrevLink);
      HoverNav.appendChild(NextLink);

      ImageData.appendChild(ImageDetails);
      ImageDetails.appendChild(Caption);
      ImageDetails.appendChild(NumberDisplay);
      ImageDetails.appendChild(LightshowPause);
      ImageDetails.appendChild(LightshowPlay);

      ImageContainer.appendChild(BottomNav);
      BottomNav.appendChild(BottomNavCloseLink);
      BottomNav.appendChild(BottomNavZoomLink);
      BottomNav.appendChild(BottomNavZoomOutLink);

    }



    $("#overlay").click(function() { Lightbox.end(); } ).hide();
    $("#lightbox").click(function() { Lightbox.end(); } );
    $("#loadingLink").click(function() { Lightbox.end(); return false;} );
    $('#prevLink').click(function() { Lightbox.changeImage(Lightbox.activeImage - 1); return false; } );
    $('#nextLink').click(function() { Lightbox.changeImage(Lightbox.activeImage + 1); return false; } );
    $('#framePrevLink').click(function() { Lightbox.changeImage(Lightbox.activeImage - 1); return false; } );
    $('#frameNextLink').click(function() { Lightbox.changeImage(Lightbox.activeImage + 1); return false; } );
    $("#bottomNavClose").click(function() { Lightbox.end(); return false; } );
    $("#bottomNavZoom").click(function() { Lightbox.changeImage(Lightbox.activeImage, true); return false; } );
    $("#bottomNavZoomOut").click(function() { Lightbox.changeImage(Lightbox.activeImage, false); return false; } );
    $("#lightshowPause").click(function() { Lightbox.togglePlayPause("lightshowPause", "lightshowPlay"); return false; } );
    $("#lightshowPlay").click(function() { Lightbox.togglePlayPause("lightshowPlay", "lightshowPause"); return false; } );

    // Fix positioning of Prev and Next links.
    $('#prevLink').css({ paddingTop: Lightbox.borderSize});
    $('#nextLink').css({ paddingTop: Lightbox.borderSize});
    $('#framePrevLink').css({ paddingTop: Lightbox.borderSize});
    $('#frameNextLink').css({ paddingTop: Lightbox.borderSize});

    // Force navigation links to always be displayed
    if (settings.force_show_nav) {
      $('#prevLink').addClass("force_show_nav");
      $('#nextLink').addClass("force_show_nav");
    }

  },

  // updateImageList()
  // Loops through anchor tags looking for 'lightbox', 'lightshow' and
  // 'lightframe' references and applies onclick events to appropriate links.
  // You can rerun after dynamically adding images w/ajax.
  updateImageList : function() {

    // Attach lightbox to any links with rel 'lightbox', 'lightshow' or
    // 'lightframe'.
    var anchors = $('a');
    var areas = $('area');
    var relAttribute = null;
    var i = 0;

    // Loop through all anchor tags.
    for (i = 0; i < anchors.length; i++) {
      var anchor = anchors[i];
      relAttribute = String(anchor.rel);

      // Use the string.match() method to catch 'lightbox', 'lightshow' and
      // 'lightframe' references in the rel attribute.
      if (anchor.href) {
        if (relAttribute.toLowerCase().match('lightbox')) {
          anchor.onclick = function() { Lightbox.start(this, false, false, false); return false; };
        }
        else if (relAttribute.toLowerCase().match('lightshow')) {
          anchor.onclick = function() { Lightbox.start(this, true, false, false); return false; };
        }
        else if (relAttribute.toLowerCase().match('lightframe')) {
          anchor.onclick = function() { Lightbox.start(this, false, true, false); return false; };
        }
        else if (relAttribute.toLowerCase().match('lightvideo')) {
          anchor.onclick = function() { Lightbox.start(this, false, false, true); return false; };
        }
      }
    }

    // Loop through all area tags.
    // todo: combine anchor & area tag loops.
    for (i = 0; i < areas.length; i++) {
      var area = areas[i];
      relAttribute = String(area.rel);

      // Use the string.match() method to catch 'lightbox', 'lightshow' and
      // 'lightframe' references in the rel attribute.
      if (area.href) {
        if (relAttribute.toLowerCase().match('lightbox')) {
          area.onclick = function() { Lightbox.start(this, false, false, false); return false; };
        }
        else if (relAttribute.toLowerCase().match('lightshow')) {
          area.onclick = function() { Lightbox.start(this, true, false, false); return false; };
        }
        else if (relAttribute.toLowerCase().match('lightframe')) {
          area.onclick = function() { Lightbox.start(this, false, true, false); return false; };
        }
        else if (relAttribute.toLowerCase().match('lightvideo')) {
          area.onclick = function() { Lightbox.start(this, false, false, true); return false; };
        }
      }
    }
  },

  // start()
  // Display overlay and lightbox. If image is part of a set, add siblings to
  // imageArray.
  start: function(imageLink, slideshow, lightframe, lightvideo) {

    // Replaces hideSelectBoxes() and hideFlash() calls in original lightbox2.
    Lightbox.toggleSelectsFlash('hide');

    // Stretch overlay to fill page and fade in.
    var arrayPageSize = Lightbox.getPageSize();
    $("#overlay").hide().css({
      width: '100%',
      zIndex: '10090',
      height: arrayPageSize[1] + 'px',
      opacity : Lightbox.overlayOpacity
    }).fadeIn();

    Lightbox.isSlideshow = slideshow;
    Lightbox.isLightframe = lightframe;
    Lightbox.isVideo = lightvideo;
    Lightbox.imageArray = [];
    Lightbox.imageNum = 0;

    var anchors = $(imageLink.tagName);
    var anchor = null;
    var rel = imageLink.rel.match(/\w+/)[0];
    var rel_info = Lightbox.parseRel(imageLink);
    var rel_group = rel_info[0];
    var rel_style = null;
    var i = 0;


    // Handle lightbox images with no grouping.
    if ((rel == 'lightbox' || rel == 'lightshow') && !rel_group) {
      Lightbox.imageArray.push([imageLink.href, imageLink.title]);
    }

    // Handle iframes with no grouping.
    else if (rel == 'lightframe' && !rel_group) {
      rel_style = (!rel_info[1] ? 'width: '+ Lightbox.iframe_width +'px; height: '+ Lightbox.iframe_height +'px; scrolling: auto;' : rel_info[1]);
      Lightbox.imageArray.push([imageLink.href, imageLink.title, rel_style]);
    }

    // Handle video.
    else if (rel == "lightvideo") {
      // rel_group contains style information for videos.
      rel_style = (!rel_group ? 'width: 400px; height: 400px;' : rel_group);
      Lightbox.imageArray.push([imageLink.href, imageLink.title, rel_style]);
    }

    // Handle iframes and lightbox & slideshow images.
    else if (rel == 'lightbox' || rel == 'lightshow' || rel == 'lightframe') {

      // Loop through anchors, find other images in set, and add them to
      // imageArray.
      if (!Lightbox.isLightframe) {
        for (i = 0; i < anchors.length; i++) {
          anchor = anchors[i];
          if (anchor.href && (anchor.rel == imageLink.rel)) {
            Lightbox.imageArray.push([anchor.href, anchor.title]);
          }
        }
      }
      // Loop through frame links separately - need to fetch style information.
      else {
        for (i = 0; i < anchors.length; i++) {
          anchor = anchors[i];
          if (anchor.href) {
            var rel_data = Lightbox.parseRel(anchor);
            if (rel_data[0] == rel_group) {
              rel_style = (!rel_data[1] ? 'width: '+ Lightbox.iframe_width +'px; height: '+ Lightbox.iframe_height +'px; scrolling: auto;' : rel_data[1]);
              Lightbox.imageArray.push([anchor.href, anchor.title, rel_style]);
            }
          }
        }
      }

      // Remove duplicates.
      for (i = 0; i < Lightbox.imageArray.length; i++) {
        for (j = Lightbox.imageArray.length-1; j > i; j--) {
          if (Lightbox.imageArray[i][0] == Lightbox.imageArray[j][0]) {
            Lightbox.imageArray.splice(j,1);
          }
        }
      }
      while (Lightbox.imageArray[Lightbox.imageNum][0] != imageLink.href) {
        Lightbox.imageNum++;
      }
    }

    if (Lightbox.isSlideshow && Lightbox.showPlayPause && Lightbox.isPaused) {
      $('#lightshowPlay').show();
      $('#lightshowPause').hide();
    }

    // Calculate top and left offset for the lightbox.
    var arrayPageScroll = Lightbox.getPageScroll();
    var lightboxTop = arrayPageScroll[1] + (arrayPageSize[3] / 10);
    var lightboxLeft = arrayPageScroll[0];
    $('#lightbox').css({
      zIndex: '10500',
      top: lightboxTop + 'px',
      left: lightboxLeft + 'px'
    }).show();

    Lightbox.changeImage(Lightbox.imageNum);
  },

  // changeImage()
  // Hide most elements and preload image in preparation for resizing image
  // container.
  changeImage: function(imageNum, zoomIn) {

    if (Lightbox.inprogress === false) {
      if (Lightbox.isSlideshow) {
        for (var i = 0; i < Lightbox.slideIdCount; i++) {
          window.clearTimeout(Lightbox.slideIdArray[i]);
        }
      }
      Lightbox.inprogress = true;

      var settings = Drupal.settings.lightbox2;
      if (Lightbox.disableZoom && !Lightbox.isSlideshow) {
        zoomIn = true;
      }
      Lightbox.isZoomedIn = zoomIn;

      Lightbox.activeImage = imageNum;

      // Hide elements during transition.
      $('#loading').css({zIndex: '10500'}).show();
      if (!Lightbox.alternative_layout) {
        $('#imageContainer').hide();
      }
      $('#frameContainer').hide();
      $('#videoContainer').hide();
      $('#lightboxImage').hide();
      $('#lightboxFrame').hide();
      $('#hoverNav').hide();
      $('#prevLink').hide();
      $('#nextLink').hide();
      $('#frameHoverNav').hide();
      $('#framePrevLink').hide();
      $('#frameNextLink').hide();
      $('#imageDataContainer').hide();
      $('#numberDisplay').hide();
      $('#bottomNavZoom').hide();
      $('#bottomNavZoomOut').hide();

      // Preload image content, but not iframe pages.
      if (!Lightbox.isLightframe && !Lightbox.isVideo) {
        imgPreloader = new Image();
        imgPreloader.onerror = function() { Lightbox.imgNodeLoadingError(this); };

        imgPreloader.onload = function() {
          var photo = document.getElementById('lightboxImage');
          photo.src = Lightbox.imageArray[Lightbox.activeImage][0];

          var imageWidth = imgPreloader.width;
          var imageHeight = imgPreloader.height;

          // Resize code.
          var arrayPageSize = Lightbox.getPageSize();
          var targ = { w:arrayPageSize[2] - (Lightbox.borderSize * 2), h:arrayPageSize[3] - (Lightbox.borderSize * 6) - (Lightbox.infoHeight * 4) - (arrayPageSize[3] / 10) };
          var orig = { w:imgPreloader.width, h:imgPreloader.height };

          // Image is very large, so show a smaller version of the larger image
          // with zoom button.
          if (zoomIn !== true) {
            var ratio = 1.0; // Shrink image with the same aspect.
            $('#bottomNavZoomOut').hide();
            $('#bottomNavZoom').hide();
            if ((orig.w >= targ.w || orig.h >= targ.h) && orig.h && orig.w) {
              ratio = ((targ.w / orig.w) < (targ.h / orig.h)) ? targ.w / orig.w : targ.h / orig.h;
              if (!Lightbox.isSlideshow) {
                $('#bottomNavZoom').css({zIndex: '10500'}).show();
              }
            }

            imageWidth  = Math.floor(orig.w * ratio);
            imageHeight = Math.floor(orig.h * ratio);
          }

          else {
            $('#bottomNavZoom').hide();
            // Only display zoom out button if the image is zoomed in already.
            if ((orig.w >= targ.w || orig.h >= targ.h) && orig.h && orig.w) {
              // Only display zoom out button if not a slideshow and if the
              // buttons aren't disabled.
              if (!Lightbox.disableZoom && Lightbox.isSlideshow === false) {
                $('#bottomNavZoomOut').css({zIndex: '10500'}).show();
              }
            }
          }

          photo.style.width = (imageWidth) + 'px';
          photo.style.height = (imageHeight) + 'px';
          Lightbox.resizeImageContainer(imageWidth, imageHeight);

          // Clear onLoad, IE behaves irratically with animated gifs otherwise.
          imgPreloader.onload = function() {};
        };

        imgPreloader.src = Lightbox.imageArray[Lightbox.activeImage][0];
      }

      // Set up frame size, etc.
      else if (Lightbox.isLightframe) {
        var iframe = document.getElementById('lightboxFrame');
        var iframeStyles = Lightbox.imageArray[Lightbox.activeImage][2];
        iframe = Lightbox.setStyles(iframe, iframeStyles);
        Lightbox.resizeImageContainer(parseInt(iframe.width, 10), parseInt(iframe.height, 10));
      }
      else if (Lightbox.isVideo) {
        var container = document.getElementById('videoContainer');
        var videoStyles = Lightbox.imageArray[Lightbox.activeImage][2];
        container = Lightbox.setStyles(container, videoStyles);
        Lightbox.videoHeight =  parseInt(container.height, 10);
        Lightbox.videoWidth =  parseInt(container.width, 10);
        Lightvideo.startVideo(Lightbox.imageArray[Lightbox.activeImage][0]);
        Lightbox.resizeImageContainer(parseInt(container.width, 10), parseInt(container.height, 10));
      }

    }
  },

  // imgNodeLoadingError()
  imgNodeLoadingError: function(image) {
    var settings = Drupal.settings.lightbox2;
    var original_image = Lightbox.imageArray[Lightbox.activeImage][0];
    if (settings.display_image_size !== "") {
      original_image = original_image.replace(new RegExp("."+settings.display_image_size), "");
    }
    Lightbox.imageArray[Lightbox.activeImage][0] = original_image;
    image.onerror = function() { Lightbox.imgLoadingError(image); };
    image.src = original_image;
  },

  // imgLoadingError()
  imgLoadingError: function(image) {
    var settings = Drupal.settings.lightbox2;
    Lightbox.imageArray[Lightbox.activeImage][0] = settings.default_image;
    image.src = settings.default_image;
  },

  // resizeImageContainer()
  resizeImageContainer: function(imgWidth, imgHeight) {

    // Get current width and height.
    this.widthCurrent = $('#outerImageContainer').width();
    this.heightCurrent = $('#outerImageContainer').height();

    // Get new width and height.
    var widthNew = (imgWidth  + (Lightbox.borderSize * 2));
    var heightNew = (imgHeight  + (Lightbox.borderSize * 2));

    // Scalars based on change from old to new.
    this.xScale = ( widthNew / this.widthCurrent) * 100;
    this.yScale = ( heightNew / this.heightCurrent) * 100;

    // Calculate size difference between new and old image, and resize if
    // necessary.
    wDiff = this.widthCurrent - widthNew;
    hDiff = this.heightCurrent - heightNew;

    $('#outerImageContainer').animate({width: widthNew, height: heightNew}, 'linear', function() {
      Lightbox.showImage();
    });


    // If new and old image are same size and no scaling transition is
    // necessary.  Do a quick pause to prevent image flicker.
    if ((hDiff === 0) && (wDiff === 0)) {
      if (navigator.appVersion.indexOf("MSIE") != -1) {
        Lightbox.pause(250);
      }
      else {
        Lightbox.pause(100);
      }
    }

    var settings = Drupal.settings.lightbox2;
    if (!settings.use_alt_layout) {
      $('#prevLink').css({height: imgHeight + 'px'});
      $('#nextLink').css({height: imgHeight + 'px'});
    }
    $('#imageDataContainer').css({width: widthNew + 'px'});

  },

  // showImage()
  // Display image and begin preloading neighbors.
  showImage: function() {
    $('#loading').hide();

    // Handle display of iframes.
    if (Lightbox.isLightframe || Lightbox.isVideo) {
      Lightbox.updateDetails();
      if (Lightbox.isLightframe) {
        $('#frameContainer').show();
        if ($.browser.safari) {
          $('#lightboxFrame').css({zIndex: '10500'}).show();
        }
        else {
          $('#lightboxFrame').css({zIndex: '10500'}).fadeIn(Lightbox.fadeInSpeed);
        }
        try {
          document.getElementById("lightboxFrame").src = Lightbox.imageArray[Lightbox.activeImage][0];
        } catch(e) {}
      }
      else {
        $("#videoContainer").html(Lightbox.videoHTML);
        $('#videoContainer').css({zIndex: '10500'}).show();
        $("#videoContainer").click(function() { return false; } );
      }
    }

    // Handle display of image content.
    else {
      $('#imageContainer').show();
      if ($.browser.safari) {
        $('#lightboxImage').css({zIndex: '10500'}).show();
      }
      else {
        $('#lightboxImage').css({zIndex: '10500'}).fadeIn(Lightbox.fadeInSpeed);
      }
      Lightbox.updateDetails();
      this.preloadNeighborImages();
    }
    Lightbox.inprogress = false;

    // Slideshow specific stuff.
    if (Lightbox.isSlideshow) {
      if (Lightbox.activeImage == (Lightbox.imageArray.length - 1)) {
        if (Lightbox.autoExit) {
          Lightbox.slideIdArray[Lightbox.slideIdCount++] = setTimeout(function () {Lightbox.end('slideshow');}, Lightbox.slideInterval);
        }
      }
      else {
        if (!Lightbox.isPaused) {
          Lightbox.slideIdArray[Lightbox.slideIdCount++] = setTimeout(function () {Lightbox.changeImage(Lightbox.activeImage + 1);}, Lightbox.slideInterval);
        }
      }
      if (Lightbox.showPlayPause && Lightbox.imageArray.length > 1 && !Lightbox.isPaused) {
        $('#lightshowPause').show();
        $('#lightshowPlay').hide();
      }
      else if (Lightbox.showPlayPause && Lightbox.imageArray.length > 1) {
        $('#lightshowPause').hide();
        $('#lightshowPlay').show();
      }
    }

    // Adjust the page overlay size.
    var arrayPageSize = Lightbox.getPageSize();
    var arrayPageScroll = Lightbox.getPageScroll();
    var pageHeight = arrayPageSize[1];
    if (Lightbox.isZoomedIn && arrayPageSize[1] > arrayPageSize[3]) {
      pageHeight = pageHeight + arrayPageScroll[1] + (arrayPageSize[3] / 10);
    }
    $('#overlay').css({height: pageHeight + 'px'});
  },

  // updateDetails()
  // Display caption, image number, and bottom nav.
  updateDetails: function() {

    $("#imageDataContainer").hide();

    var caption = Lightbox.imageArray[Lightbox.activeImage][1];
    // If caption is not null.
    if (caption) {
      $('#caption').html(caption).css({zIndex: '10500'}).show();
    }
    else {
      $('#caption').hide();
    }

    // If image is part of set display 'Image x of x'.
    var settings = Drupal.settings.lightbox2;
    var numberDisplay = null;
    if (Lightbox.imageArray.length > 1) {
      var currentImage = Lightbox.activeImage + 1;
      if (!Lightbox.isLightframe) {
        numberDisplay = settings.image_count.replace(/\!current/, currentImage).replace(/\!total/, Lightbox.imageArray.length);
      }
      else {
        numberDisplay = settings.page_count.replace(/\!current/, currentImage).replace(/\!total/, Lightbox.imageArray.length);
      }
      $('#numberDisplay').html(numberDisplay).css({zIndex: '10500'}).show();
    }

    $("#imageDataContainer").hide().slideDown(Lightbox.slideDownSpeed);
    if (Lightbox.rtl) {
      $("#bottomNav").css({float: 'left'});
    }

    Lightbox.updateNav();
  },

  // updateNav()
  // Display appropriate previous and next hover navigation.
  updateNav: function() {

    $('#hoverNav').css({zIndex: '10500'}).show();
    var prevLink = '#prevLink';
    var nextLink = '#nextLink';

    // Slideshow is separated as we need to show play / pause button.
    if (Lightbox.isSlideshow) {
      if (Lightbox.activeImage !== 0) {
        $(prevLink).css({zIndex: '10500'}).show().click(function() {
          if (Lightbox.pauseOnPrevClick) {
            Lightbox.togglePlayPause("lightshowPause", "lightshowPlay");
          }
          Lightbox.changeImage(Lightbox.activeImage - 1); return false;
        });
      }
      else {
        $(prevLink).hide();
      }

      // If not last image in set, display next image button.
      if (Lightbox.activeImage != (Lightbox.imageArray.length - 1)) {
        $(nextLink).css({zIndex: '10500'}).show().click(function() {
          if (Lightbox.pauseOnNextClick) {
            Lightbox.togglePlayPause("lightshowPause", "lightshowPlay");
          }
          Lightbox.changeImage(Lightbox.activeImage + 1); return false;
        });
      }
      // Safari browsers need to have hide() called again.
      else {
        $(nextLink).hide();
      }
    }

    // All other types of content.
    else {

      if (Lightbox.isLightframe && !Lightbox.alternative_layout) {
        $('#hoverNav').css({zIndex: '10500'}).hide();
        $('#frameHoverNav').css({zIndex: '10500'}).show();
        prevLink = '#framePrevLink';
        nextLink = '#frameNextLink';
      }
      // If not first image in set, display prev image button.
      if (Lightbox.activeImage !== 0) {
        $(prevLink).css({zIndex: '10500'}).show().click(function() {
          Lightbox.changeImage(Lightbox.activeImage - 1); return false;
        });
      }
      // Safari browsers need to have hide() called again.
      else {
        $(prevLink).hide();
      }

      // If not last image in set, display next image button.
      if (Lightbox.activeImage != (Lightbox.imageArray.length - 1)) {
        $(nextLink).css({zIndex: '10500'}).show().click(function() {
          Lightbox.changeImage(Lightbox.activeImage + 1); return false;
        });
      }
      // Safari browsers need to have hide() called again.
      else {
        $(nextLink).hide();
      }
    }

    this.enableKeyboardNav();
  },


  // enableKeyboardNav()
  enableKeyboardNav: function() {
    $(document).bind("keydown", this.keyboardAction);
  },

  // disableKeyboardNav()
  disableKeyboardNav: function() {
    $(document).unbind("keydown", this.keyboardAction);
  },

  // keyboardAction()
  keyboardAction: function(e) {
    if (e === null) { // IE.
      keycode = event.keyCode;
      escapeKey = 27;
    }
    else { // Mozilla.
      keycode = e.keyCode;
      escapeKey = e.DOM_VK_ESCAPE;
    }

    key = String.fromCharCode(keycode).toLowerCase();

    // Close lightbox.
    if (key == 'x' || key == 'o' || key == 'c' || keycode == escapeKey) {
      Lightbox.end();

    // Display previous image (p, <-).
    }
    else if (key == 'p' || keycode == 37) {
      if (Lightbox.activeImage !== 0) {
        Lightbox.changeImage(Lightbox.activeImage - 1);
      }

    // Display next image (n, ->).
    }
    else if (key == 'n' || keycode == 39) {
      if (Lightbox.activeImage != (Lightbox.imageArray.length - 1)) {
        Lightbox.changeImage(Lightbox.activeImage + 1);
      }
    }
    // Zoom in.
    else if (key == 'z' && !Lightbox.disableZoom && !Lightbox.isSlideshow && !Lightbox.isLightframe) {
      if (Lightbox.isZoomedIn) {
        Lightbox.changeImage(Lightbox.activeImage, false);
      }
      else if (!Lightbox.isZoomedIn) {
        Lightbox.changeImage(Lightbox.activeImage, true);
      }
    }
    // Toggle play / pause (space).
    else if (keycode == 32 && Lightbox.isSlideshow) {
      if (Lightbox.isPaused) {
        Lightbox.togglePlayPause("lightshowPlay", "lightshowPause");
      }
      else {
        Lightbox.togglePlayPause("lightshowPause", "lightshowPlay");
      }
      return false;
    }
  },

  preloadNeighborImages: function() {

    if ((Lightbox.imageArray.length - 1) > Lightbox.activeImage) {
      preloadNextImage = new Image();
      preloadNextImage.src = Lightbox.imageArray[Lightbox.activeImage + 1][0];
    }
    if (Lightbox.activeImage > 0) {
      preloadPrevImage = new Image();
      preloadPrevImage.src = Lightbox.imageArray[Lightbox.activeImage - 1][0];
    }

  },

  end: function(caller) {
    var closeClick = (caller == 'slideshow' ? false : true);
    if (Lightbox.isSlideshow && Lightbox.isPaused && !closeClick) {
      return;
    }
    Lightbox.disableKeyboardNav();
    $('#lightbox').hide();
    $("#overlay").fadeOut();
    Lightbox.activeImage = null;
    // Replaces calls to showSelectBoxes() and showFlash() in original
    // lightbox2.
    Lightbox.toggleSelectsFlash('visible');
    if (Lightbox.isSlideshow) {
      for (var i = 0; i < Lightbox.slideIdCount; i++) {
        window.clearTimeout(Lightbox.slideIdArray[i]);
      }
      $('#lightshowPause').hide();
      $('#lightshowPlay').hide();
    }
    else if (Lightbox.isLightframe) {
      document.getElementById("lightboxFrame").src = '';
      $('#lightboxFrame').hide();
      $('#frameContainer').hide();
    }
    else if (Lightbox.isVideo) {
      $('#videoContainer').hide();
      $('#videoContainer').html("");
    }
  },





  // getPageScroll()
  // Returns array with x,y page scroll values.
  // Core code from - quirksmode.com.
  getPageScroll : function() {

    var xScroll, yScroll;

    if (self.pageYOffset) {
      yScroll = self.pageYOffset;
      xScroll = self.pageXOffset;
    }
    else if (document.documentElement && document.documentElement.scrollTop) {  // Explorer 6 Strict
      yScroll = document.documentElement.scrollTop;
      xScroll = document.documentElement.scrollLeft;
    }
    else if (document.body) {// All other Explorers.
      yScroll = document.body.scrollTop;
      xScroll = document.body.scrollLeft;
    }

    arrayPageScroll = [xScroll,yScroll];
    return arrayPageScroll;
  },

  // getPageSize()
  // Returns array with page width, height and window width, height.
  // Core code from - quirksmode.com.
  // Edit for Firefox by pHaez.
  getPageSize : function() {

    var xScroll, yScroll;

    if (window.innerHeight && window.scrollMaxY) {
      xScroll = window.innerWidth + window.scrollMaxX;
      yScroll = window.innerHeight + window.scrollMaxY;
    }
    // All but Explorer Mac.
    else if (document.body.scrollHeight > document.body.offsetHeight) {
      xScroll = document.body.scrollWidth;
      yScroll = document.body.scrollHeight;
    }
    // Explorer Mac...would also work in Explorer 6 Strict, Mozilla and Safari.
    else {
      xScroll = document.body.offsetWidth;
      yScroll = document.body.offsetHeight;
    }

    var windowWidth, windowHeight;

    if (self.innerHeight) { // All except Explorer.
      if (document.documentElement.clientWidth) {
        windowWidth = document.documentElement.clientWidth;
      }
      else {
        windowWidth = self.innerWidth;
      }
      windowHeight = self.innerHeight;
    }
    // Explorer 6 Strict Mode.
    else if (document.documentElement && document.documentElement.clientHeight) {
      windowWidth = document.documentElement.clientWidth;
      windowHeight = document.documentElement.clientHeight;
    }
    else if (document.body) { // Other Explorers.
      windowWidth = document.body.clientWidth;
      windowHeight = document.body.clientHeight;
    }

    // For small pages with total height less then height of the viewport.
    if (yScroll < windowHeight) {
      pageHeight = windowHeight;
    }
    else {
      pageHeight = yScroll;
    }


    // For small pages with total width less then width of the viewport.
    if (xScroll < windowWidth) {
      pageWidth = xScroll;
    }
    else {
      pageWidth = windowWidth;
    }

    arrayPageSize = [pageWidth, pageHeight, windowWidth, windowHeight];
    return arrayPageSize;
  },


  // pause(numberMillis)
  pause : function(ms) {
    var date = new Date();
    var curDate = null;
    do { curDate = new Date(); }
    while (curDate - date < ms);
  },


  // toggleSelectsFlash()
  // Hide / unhide select lists and flash objects as they appear above the
  // lightbox in some browsers.
  toggleSelectsFlash: function (state) {
    if (state == 'visible') {
      $("select.lightbox_hidden, embed.lightbox_hidden, object.lightbox_hidden").show();
    }
    else if (state == 'hide') {
      $("select:visible, embed:visible, object:visible").addClass("lightbox_hidden");
      $("select.lightbox_hidden, embed.lightbox_hidden, object.lightbox_hidden").hide();
    }
  },

  // parseRel()
  parseRel: function (link) {
    var rel_info = [];
    if (link.rel.match(/\[(.*)\]/)) {
      rel_info = link.rel.match(/\[(.*)\]/)[1].split('|');
    }
    return rel_info;
  },

  // setStyles()
  setStyles: function(item, styles) {
    var stylesArray = styles.split(';');
    for (var i = 0; i< stylesArray.length; i++) {
      if (stylesArray[i].indexOf('width:') >= 0) {
        var w = stylesArray[i].replace('width:', '');
        item.width = jQuery.trim(w);
      }
      else if (stylesArray[i].indexOf('height:') >= 0) {
        var h = stylesArray[i].replace('height:', '');
        item.height = jQuery.trim(h);
      }
      else if (stylesArray[i].indexOf('scrolling:') >= 0) {
        var scrolling = stylesArray[i].replace('scrolling:', '');
        item.scrolling = jQuery.trim(scrolling);
      }
      else if (stylesArray[i].indexOf('overflow:') >= 0) {
        var overflow = stylesArray[i].replace('overflow:', '');
        item.overflow = jQuery.trim(overflow);
      }
    }
    return item;
  },



  // togglePlayPause()
  // Hide the pause / play button as appropriate.  If pausing the slideshow also
  // clear the timers, otherwise move onto the next image.
  togglePlayPause: function(hideId, showId) {
    if (Lightbox.isSlideshow && hideId == "lightshowPause") {
      for (var i = 0; i < Lightbox.slideIdCount; i++) {
        window.clearTimeout(Lightbox.slideIdArray[i]);
      }
    }
    $('#' + hideId).hide();
    $('#' + showId).show();

    if (hideId == "lightshowPlay") {
      Lightbox.isPaused = false;
      if (Lightbox.activeImage == (Lightbox.imageArray.length - 1)) {
        Lightbox.end();
      }
      else {
        Lightbox.changeImage(Lightbox.activeImage + 1);
      }
    }
    else {
      Lightbox.isPaused = true;
    }
  }
};

// Initialize the lightbox.
if (Drupal.jsEnabled) {
  $(document).ready(function(){
    Lightbox.initialize();
  });
}
