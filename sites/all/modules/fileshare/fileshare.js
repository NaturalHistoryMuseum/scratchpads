// $Id$

/*
 * Since there is no more hasClass function in 
 * drupal's misc/drupal.js file since 5.0,
 * It is copied here
 */
$(document).ready(function() {
  target = document.getElementById('startFileShare');
  fileShareAutoAttach(target,'startFileShare');
  document.getElementById('-uploadform').style.display = 'block';
});

function hasClass(node, className) {
  if (node.className == className) {
    return true;
  }
  var reg = new RegExp('(^| )'+ className +'($| )')
  if (reg.test(node.className)) {
    return true;
  }
  return false;
}

function fileshare_folder(target,tag) {
  var targetElement = document.getElementById(tag);
  if(targetElement.style.display != 'block'){
    if(targetElement.innerHTML == '') {
      fileShareAutoAttach(target,tag);
    }
    targetElement.style.display = 'block';
    target.className = 'fs_open';
  } else {
    targetElement.style.display = 'none';
    target.className = 'fs_closed';
    targetElement.innerHTML = '';
    // Rebuild Load dropdown menu so that closed directories don't show up in list
    loadTargetDir();
  }
  return false;
}

function submitDelete(deleteme) {
 if (confirm('Are you sure you wish to delete '+deleteme+'?\r\nThis cannot be undone.')) { 
    document.getElementById('edit-deletefile').value = deleteme;
    document.getElementById('-fsform').submit();
  }
}

function loadTargetDir() {
  target=document.getElementById('edit-targetdir');
  if (target) {
    // Remove all options by setting the Options array's length to 0
    target.options.length=0;
    var anchors = document.getElementsByTagName('a');
    var root = document.getElementById('edit-root').value;
    // Add root option
    target.options[target.options.length]=new Option('/',root);
    for (var i = 0; anch = anchors[i]; i++) {
      if (anch && (hasClass(anch, 'fs_open') || hasClass(anch,'fs_closed'))) { 
        // Read in the message from the 'alt' attribute
        path = anch.getAttribute('alt');
        // establish base path from fileshare
        path = path.replace(root,'');
        target.options[target.options.length]=new Option(path,root+path);
      }
    }
  }
}

function fileShareAutoAttach(target,tag) {
  url = target.getAttribute('name');
  //target.removeAttribute('title');
  jah(url,tag);
  return false;
}

function jah(url,tag) {
  // native XMLHttpRequest object
  document.getElementById(tag).innerHTML = '<span class="loading">&nbsp;loading...&nbsp;</span>';
  if (window.XMLHttpRequest) {
    req = new XMLHttpRequest();
    req.onreadystatechange = function() {jahDone(tag);};
    req.open("GET", url, true);
    req.send(null);
  // IE/Windows ActiveX version
  } else if (window.ActiveXObject) {
    req = new ActiveXObject("Microsoft.XMLHTTP");
    if (req) {
      req.onreadystatechange = function() {jahDone(tag);};
      req.open("GET", url, true);
      req.send();
    }
  }
}

function jahDone(tag) {
  // only if req is "loaded"
  if (req.readyState == 4) {
    // only if "OK"
    if (req.status == 200) {
      results = req.responseText;
      document.getElementById(tag).innerHTML = results;
      // Load dropdown menu after new elements have appeared
      loadTargetDir();
    } else {
      document.getElementById(tag).innerHTML = "Fileshare (Ensure you're logged in to view files):\n" +
      req.statusText;
    }
  }
}
