function addLink(addr){
	insertLink(addr);
}

function insertLink(url) {
  var inst = tinyMCE.getInstanceById(tinyMCE.getWindowArg('editor_id'));
  var elm = inst.getFocusElement();

  elm = tinyMCE.getParentElement(elm, "a");

  tinyMCEPopup.execCommand("mceBeginUndoLevel");

  // Create new anchor elements
  if (elm == null) {
    if (tinyMCE.isSafari)
      tinyMCEPopup.execCommand("mceInsertContent", false, '<a href="'+url+'">' + inst.selection.getSelectedHTML() + '</a>');
    else
      tinyMCEPopup.execCommand("createlink", false, url);

    var elementArray = tinyMCE.getElementsByAttributeValue(inst.getBody(), "a", "href", url);
    for (var i=0; i<elementArray.length; i++) {
      var elm = elementArray[i];

      // Move cursor behind the new anchor
      if (tinyMCE.isGecko) {
        var sp = inst.getDoc().createTextNode(" ");

        if (elm.nextSibling)
          elm.parentNode.insertBefore(sp, elm.nextSibling);
        else
          elm.parentNode.appendChild(sp);

        // Set range after link
        var rng = inst.getDoc().createRange();
        rng.setStartAfter(elm);
        rng.setEndAfter(elm);

        // Update selection
        var sel = inst.getSel();
        sel.removeAllRanges();
        sel.addRange(rng);
      }

      //setAllAttribs(elm);
    }
  } //else
    //setAllAttribs(elm);

  tinyMCE._setEventsEnabled(inst.getBody(), false);
  tinyMCEPopup.execCommand("mceEndUndoLevel");
  tinyMCEPopup.close();
}

