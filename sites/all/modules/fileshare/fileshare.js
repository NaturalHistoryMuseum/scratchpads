// $Id$

if (isJsEnabled()) {
  addLoadEvent(function() {
    target = document.getElementById('startFileShare');
    fileShareAutoAttach(target,'startFileShare');
    document.getElementById('_uploadform').style.display = 'block';
  });
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
    document.getElementById('_fsform').submit();
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
    } else {
      document.getElementById(tag).innerHTML = "jah error:\n" +
      req.statusText;
    }
  }
}