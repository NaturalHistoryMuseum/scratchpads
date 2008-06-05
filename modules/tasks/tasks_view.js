// $Id$
function toggleVisibility(element) {
  st = document.getElementById(element).style;
  if (st) {
    // Firefox vs IE: 'block' display tables wrong in Firefox, use 'table' instead', but IE doesn't support 'table'. thus display='' works in both
    st.display = st.display=='none'? '':'none';
  }else{
    alert('didnt find object');
  }
}