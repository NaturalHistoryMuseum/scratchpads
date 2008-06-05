// $Id$

function toggletasks() {
  var tasksdrop = document.getElementById('orderby');
  var completeddate = document.getElementById('completedate');

  if(tasksdrop.style.display=="none") {
    tasksdrop.style.display='block';
    completeddate.style.display='none';
  } else {
    tasksdrop.style.display='none';
    completeddate.style.display='block';
  }
}

function checktasksdropdown() {
  var tasksdrop = document.getElementById('orderby');
  var completeddate = document.getElementById('completedate');

  if(document.getElementById('edit-completed').checked == true) {
    tasksdrop.style.display='none';
    completeddate.style.display='block';
  } else {
    tasksdrop.style.display='block';
    completeddate.style.display='none';
  }
}

window.onload = checktasksdropdown;