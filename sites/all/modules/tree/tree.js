var itemOne = 0;

// Globals that are specific to the tree
var leafGap = 0.0; // Vertical space between leaves
var leafX = 0; // x-coodinates of leaves
var treeWidth = 0; // maximum dimensions of tree image

var debug = false;

// tree-specific (will be set when we include Javascript file for this tree)
var imageHeight = 0; 
var floatThumbHeight = 0.0;

// Determine browser and version.

function tvwDoTaxonDisplay(taxon){
  location.href='/taxtab/process'.taxon;
}

function Browser() {

  var ua, s, i;

  this.isIE    = false;
  this.isNS    = false;
  this.version = null;

  ua = navigator.userAgent;

  s = "MSIE";
  if ((i = ua.indexOf(s)) >= 0) {
    this.isIE = true;
    this.version = parseFloat(ua.substr(i + s.length));
    return;
  }

  s = "Netscape6/";
  if ((i = ua.indexOf(s)) >= 0) {
    this.isNS = true;
    this.version = parseFloat(ua.substr(i + s.length));
    return;
  }

  // Treat any other "Gecko" browser as NS 6.1.

  s = "Gecko";
  if ((i = ua.indexOf(s)) >= 0) {
    this.isNS = true;
    this.version = 6.1;
    return;
  }
}

var browser = new Browser();


// utility functions

// From http://acko.net/blog/mouse-handling-and-absolute-positions-in-javascript
function getAbsolutePosition(element) 
{
    var r = { x: element.offsetLeft, y: element.offsetTop };
    if (element.offsetParent) 
    {
      var tmp = getAbsolutePosition(element.offsetParent);
      r.x += tmp.x;
      r.y += tmp.y;
    }
    return r;
};	


//---------------------------------------------------------------------------------------
function getX(event)
{
	var x;
if (browser.isIE) {
   x = window.event.clientX + document.documentElement.scrollLeft
     + document.body.scrollLeft;
 }
 if (browser.isNS) {
   x = event.clientX + window.scrollX;
 }
return x;
}

//---------------------------------------------------------------------------------------
function getY(event)
{
	var y;
 if (browser.isIE) {
    y = window.event.clientY + document.documentElement.scrollTop
      + document.body.scrollTop;
  }
  if (browser.isNS) {    x = event.clientX + window.scrollX;
    y = event.clientY + window.scrollY;
  }	
	return y;
}

//---------------------------------------------------------------------------------------
function preventDefault(event)
{
	if (browser.isIE) {
    window.event.cancelBubble = true;
    window.event.returnValue = false;
  }
  if (browser.isNS)
    event.preventDefault();
}

//---------------------------------------------------------------------------------------
function capture(event)
{
	if (browser.isIE) {
    document.attachEvent("onmousemove", dragGo);
    document.attachEvent("onmouseup",   dragStop);
    window.event.cancelBubble = true;
    window.event.returnValue = false;
  }
  if (browser.isNS) {
    document.addEventListener("mousemove", dragGo,   true);
    document.addEventListener("mouseup",   dragStop, true);
    event.preventDefault();
  }
}



// Global object to hold drag information.
var dragObj = new Object();
dragObj.zIndex = 0;

//---------------------------------------------------------------------------------------
function dragStart(event, id) {

	var el;
	var x, y;

	dragObj.thumb = document.getElementById('tvwThumb');

	dragObj.thumbHeight = document.getElementById('tvwThumb').clientHeight;
	dragObj.viewerHeight = document.getElementById('tvwViewer').clientHeight;
	dragObj.objectHeight = imageHeight;

	dragObj.elNode = document.getElementById(id);
	dragObj.track1 = document.getElementById('tvwTrack1');
	dragObj.track2 = document.getElementById('tvwTrack2');
	
	//alert(id);
	
	if (id == 'tvwWell')
	{
		dragObj.thumb = document.getElementById('tvwThumb');
	}
	if (id == 'tvwThumb')
	{
		dragObj.well = document.getElementById('tvwWell');
	}

	// Get cursor position with respect to the page.
	var x = getX(event);
	var y = getY(event);

	// Save starting positions of cursor and element.
	dragObj.cursorStartX = x;
	dragObj.cursorStartY = y;
	dragObj.elStartLeft  = parseInt(dragObj.elNode.style.left, 10);
	dragObj.elStartTop   = parseInt(dragObj.elNode.style.top,  10);

	if (isNaN(dragObj.elStartLeft)) dragObj.elStartLeft = 0;
	if (isNaN(dragObj.elStartTop))  dragObj.elStartTop  = 0;
}


//---------------------------------------------------------------------------------------
function dragGo(event) 
{
  	var x = getX(event);
	var y = getY(event);
	
  	// Move drag element by the same amount the cursor has moved.
  	dragObj.elNode.style.left = (dragObj.elStartLeft + x - dragObj.cursorStartX) + "px";
  	dragObj.elNode.style.top  = (dragObj.elStartTop  + y - dragObj.cursorStartY) + "px";

	preventDefault(event);
}

//---------------------------------------------------------------------------------------
function dragStop(event) 
{
  // Stop capturing mousemove and mouseup events.

  if (browser.isIE) {
    document.detachEvent("onmousemove", dragGo);
    document.detachEvent("onmouseup",   dragStop);
  }
  if (browser.isNS) {
    document.removeEventListener("mousemove", dragGo,   true);
    document.removeEventListener("mouseup",   dragStop, true);
  }
}


//---------------------------------------------------------------------------------------
function captureThumb(event)
{
	if (browser.isIE) {
    document.attachEvent("onmousemove", dragThumb);
    document.attachEvent("onmouseup",   thumbStop);
    window.event.cancelBubble = true;
    window.event.returnValue = false;
  }
  if (browser.isNS) {
    document.addEventListener("mousemove", dragThumb,   true);
    document.addEventListener("mouseup",   thumbStop, true);
    event.preventDefault();
  }
} 

//---------------------------------------------------------------------------------------
function dragThumb(event) 
{
  	var x = getX(event);
	var y = getY(event);

	pos = (dragObj.elStartTop  + y - dragObj.cursorStartY);

	if (pos <= 0) pos = 0;
	if (pos + floatThumbHeight >= dragObj.viewerHeight) 
	{
		if (debug)
		{
			showStatus('pos=' + pos + ' ' + dragObj.thumbHeight + ' ' + dragObj.viewerHeight);
		}
			
		pos = (dragObj.viewerHeight - floatThumbHeight);
	}
	adjustScrollbar(pos);

	moveWellToThumbPos(pos)

  	// Move drag element by the same amount the cursor has moved.
  	dragObj.elNode.style.top  = pos + "px";

	preventDefault(event);
}

//---------------------------------------------------------------------------------------
function thumbStop(event) 
{
  // Stop capturing mousemove and mouseup events.

  if (browser.isIE) {
    document.detachEvent("onmousemove", dragThumb);
    document.detachEvent("onmouseup",   thumbStop);
  }
  if (browser.isNS) {
    document.removeEventListener("mousemove", dragThumb,   true);
    document.removeEventListener("mouseup",   thumbStop, true);
  }
}

//---------------------------------------------------------------------------------------
function captureSurface(event)
{
	if (browser.isIE) {
    document.attachEvent("onmousemove", dragSurface);
    document.attachEvent("onmouseup",   surfaceStop);
    window.event.cancelBubble = true;
    window.event.returnValue = false;
  }
  if (browser.isNS) {
    document.addEventListener("mousemove", dragSurface,   true);
    document.addEventListener("mouseup",   surfaceStop, true);
    event.preventDefault();
  }
} 
//---------------------------------------------------------------------------------------
function dragSurface(event) 
{
  	var x = getX(event);
	var y = getY(event);

	pos = (dragObj.elStartTop  + y - dragObj.cursorStartY);

	// Make sure we don't go beyond bounds of object being viewed
	if (pos >= 0) pos = 0;
	if (pos < (dragObj.viewerHeight - dragObj.objectHeight))
	{
		pos = (dragObj.viewerHeight - dragObj.objectHeight);
	}
	s = moveThumbToWellPos(pos);
	if (debug)
	{
		showStatus('s=' + s);
	}
	adjustScrollbar(-s);

  	// Move drag element by the same amount the cursor has moved.
  	dragObj.elNode.style.top  = pos + "px";

	preventDefault(event);
}

//---------------------------------------------------------------------------------------
function surfaceStop(event) 
{

  // Stop capturing mousemove and mouseup events.
  if (browser.isIE) {
    document.detachEvent("onmousemove", dragSurface);
    document.detachEvent("onmouseup",   surfaceStop);
  }
  if (browser.isNS) {
    document.removeEventListener("mousemove", dragSurface,   true);
    document.removeEventListener("mouseup",   surfaceStop, true);
  }
}

//---------------------------------------------------------------------------------------
function adjustScrollbar(top)
{
	dragObj.track1.style.height = top + "px";
	dragObj.track2.style.height = dragObj.viewerHeight - dragObj.thumbHeight - top + "px";
	dragObj.track2.style.top = top + dragObj.thumbHeight + 1 + "px";
}

//---------------------------------------------------------------------------------------
function moveThumbToWellPos(pos)
{
	d = dragObj.objectHeight - dragObj.viewerHeight;
	r = dragObj.viewerHeight/dragObj.objectHeight;
	t = pos * r;

	dragObj.thumb.style.top  = -t + "px";	

	return t;
}

//---------------------------------------------------------------------------------------
function moveWellToThumbPos(pos)
{
	d = dragObj.objectHeight - dragObj.viewerHeight;
	r = dragObj.objectHeight / dragObj.viewerHeight;
	t = pos * r;
	dragObj.well.style.top = -t + "px";

	return t;
}

//---------------------------------------------------------------------------------------
function handle(delta) 
{

	dragObj.thumb = document.getElementById('tvwThumb');
	dragObj.thumbHeight = dragObj.thumb.clientHeight;
	dragObj.well = document.getElementById('tvwWell');
	dragObj.viewerHeight = document.getElementById('tvwViewer').clientHeight;
	dragObj.objectHeight = imageHeight;
	dragObj.track1 = document.getElementById('tvwTrack1');
	dragObj.track2 = document.getElementById('tvwTrack2');

	pos = dragObj.thumb.offsetTop;
	if (debug)
	{
		showStatus(dragObj.thumb.offsetTop);
	}
	if (delta < 0)
	{
		pos += dragObj.thumbHeight;

		if (pos + dragObj.thumbHeight >= dragObj.viewerHeight) 
		{
			pos = (dragObj.viewerHeight - floatThumbHeight);
		}

		if (debug) { showStatus(pos); }

		dragObj.thumb.style.top  = pos + "px";
		adjustScrollbar(pos);
		moveWellToThumbPos(pos);
	}
	else
	{
		pos -= dragObj.thumbHeight;

		if (pos < 0)
		{
			pos = 0;
		}

		if (debug) { showStatus(pos); }

		dragObj.thumb.style.top  = pos + "px";
		adjustScrollbar(pos);
		moveWellToThumbPos(pos);

	}

}

//---------------------------------------------------------------------------------------
function wheel(event)
{
	var delta = 0;
	if (!event) event = window.event;
	if (event.wheelDelta) {
		delta = event.wheelDelta/120; 
		if (window.opera) delta = -delta;
	} else if (event.detail) {
		delta = -event.detail/3;
	}
	if (delta)
		handle(delta);
	if (event.preventDefault)
		event.preventDefault();
	event.returnValue = false;
}

//---------------------------------------------------------------------------------------
// Go to an absolute location on the image (in image coordinates). We would use this
// in a search and find operation, for example
function gotoWellPos(pos)
{	
	dragObj.thumb = document.getElementById('tvwThumb');
	dragObj.thumbHeight = dragObj.thumb.clientHeight;
	dragObj.well = document.getElementById('tvwWell');
	dragObj.viewerHeight = document.getElementById('tvwViewer').clientHeight;
	dragObj.objectHeight = imageHeight;

	dragObj.track1 = document.getElementById('tvwTrack1');
	dragObj.track2 = document.getElementById('tvwTrack2');

	if (pos < 0) pos = 0;
	if (pos > (dragObj.objectHeight - dragObj.viewerHeight)) pos = dragObj.objectHeight - dragObj.viewerHeight;

	dragObj.well.style.top = -pos + "px";

	d = dragObj.objectHeight - dragObj.viewerHeight;
	r = dragObj.viewerHeight/dragObj.objectHeight;
	t = pos * r;

	dragObj.thumb.style.top = t + "px";

	adjustScrollbar(t)

}

//---------------------------------------------------------------------------------------
// for debugging
function showStatus(msg)
{
		document.getElementById('tvwTreeStatus').innerHTML = "<span>" + msg + "</span>";
}

//---------------------------------------------------------------------------------------
function showLabel()
{
	var offsetY = 8;
	var w = dragObj.objectHeight = imageHeight;
	var v = document.getElementById('tvwViewer').clientHeight;

	var leafPos = leafGap * itemOne;

	// Ensure selected node is visible
	var pos = leafPos + offsetY; // position on image;
	pos -= (v/2); // try and center the selection
	gotoWellPos(pos);

	// Highlight selected node by displaying a DIV
	l = document.getElementById('tvwLabelOne');
	l.style.display="inline";
	l.style.top = (leafPos + offsetY - l.clientHeight/2 ) + "px";
	l.style.left = leafX + "px";
	l.style.width = (treeWidth - leafX) + "px";


	// Display corresponding marker on tree scroller

	// Need offset of scrollbar from top of widget
	s = 0;
	// Allow for height of marker
	//s -= 2;

	// ratio of viewer to tree image
	r = v / w;

	if (debug) { showStatus('s=' + s + " " + r);}

	t = s + leafPos * r;

	l = document.getElementById('tvwMarker1');
	l.style.top = t + "px";
	l.style.display = 'inline';
}

//---------------------------------------------------------------------------------------
function doTaxa(taxa, event)
{
	var x = getX(event);
	var y = getY(event);
	
	var r = getAbsolutePosition(taxa);
	
	// position relative to taxa DIV
	var ty = y - r.y ; 
	
	// Offset by where we are in the viewer
	var top = parseInt(document.getElementById('tvwWell').style.top);
	if (isNaN(top)) top = 0;

	ty -= top;
	ty -= 9; // font indent
	ty = ty/leafGap; // scale to taxon coordinates
	
	ty = Math.round(Math.abs(ty)); // make into an integer

	// This function can be edited by user to add whatever functionality they want.
	tvwDoTaxonDisplay(ty);	
}



// from http://redescape.wordpress.com/2007/08/10/javascript-auto-complete-input-field/


var autocomplete = {
  initialize: function(){
  
	var theBody = document.getElementById('tvwTreeTools');

  var theResults = document.createElement('div');
    theResults.setAttribute('id', 'theResults');
    theResults.style.left = '0px';
    theResults.style.top = '0px';
    theResults.style.position = 'absolute';
    theResults.style.zIndex = '99';
    theResults.style.border = '1px solid black'; //#aaaaaa';
    theResults.style.display = 'none';
   theResults.style.background = 'white';
    theBody.appendChild(theResults);
  },

  fill: function(val, index){
    this.theInput.value = val;
    this.theInput.style.background = '#ffffff';
    var theResults = document.getElementById('theResults');
    theResults.innerHTML = '';
    theResults.style.display = 'none';

	/* rdmp set external global so we know what leaf user wants. */
	itemOne = index;
  },

  change: function(obj, event, arr){
    var theInput = obj;
    this.theInput = theInput;

	/* rdmp - eat the shift key */
	if (event.keyCode == 16) return;

    var theResults = document.getElementById('theResults');
    if(theInput.value == ''){
      theInput.style.background = '#ffffff';
      theResults.innerHTML = '';
      theResults.style.display = 'none';
    }
    else{

      var obj = theInput;
      if(obj.offsetParent){
        x = obj.offsetLeft;
        y = obj.offsetTop;
        h = obj.offsetHeight;
        w = obj.offsetWidth;
    /*    while(obj = obj.offsetParent){
          x += obj.offsetLeft;
          y += obj.offsetTop;
        }*/
      }

      var totalChars = theInput.value.length;
      var resultsTotal = 0;
      theResults.innerHTML = '';
      var exactItem = false;
      for(i=0;i<arr.length;i++){
        if(arr[i].substr(0, totalChars).toLowerCase() == theInput.value.substr(0, totalChars).toLowerCase()){
          if(resultsTotal == 0 && event.keyCode !== 8){theInput.value = arr[i];}
          resultsStyle = 'font-family: Verdana,arial; font-size: 10px; color: #000000; text-decoration: none; padding: 2px; display: block';

			/* rdmp I've added i as a parameter to fill */
          theResults.innerHTML += '<a href="javascript:autocomplete.fill(\'' + arr[i] + '\', ' + i + ')" style="' + resultsStyle + '">' + arr[i] + '</a>';
          resultsTotal++;
        }
        if(arr[i].toLowerCase() == theInput.value.toLowerCase()){exactItem = true;  itemOne = i; }

		/* To avoid exploding if most of the possible input values start with the same letter, set a limit for bailing out */
		if (resultsTotal > 20) 
		{
			theResults.innerHTML += '<span style="' + resultsStyle + '">&hellip;</span>';			
			break;
		}


      }
      if(resultsTotal == 0)
	{
		/* We don't have this string so highlight input control background in red */
		theInput.style.background = '#ffaaaa';
	}
      else
{
	/* We have some hits */
	theInput.style.background = '#ffffff';
}

      if(event.keyCode !== 8){
        if(document.all){
          var theRange = theInput.createTextRange();
          theRange.moveStart('character', totalChars);
          theRange.moveEnd('character', theInput.value.length);
          theRange.select();
        }
        else{
          theInput.setSelectionRange(totalChars, theInput.value.length);
        }
        theInput.focus();
      }

      if(exactItem && resultsTotal < 2)
	{
        theInput.style.background = '#ffffff';
        theResults.innerHTML = '';
        theResults.style.display = 'none';


      }
      else if(resultsTotal == 0){
        theInput.style.background = '#ffaaaa';
        theResults.innerHTML = '';
        theResults.style.display = 'none';
      }
      else{
        theResults.style.left = x + 'px';
        theResults.style.top = (y + h) + 'px';
        theResults.style.display = 'block';
      }
    }
  }
}