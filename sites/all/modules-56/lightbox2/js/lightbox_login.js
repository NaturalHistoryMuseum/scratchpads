// $Id: lightbox_login.js,v 1.1.4.2 2008/06/20 13:57:18 snpower Exp $

function lightbox2_login() {
  $("a[@href*='/user/login'], a[@href*='?q=user/login']").each(function() {
    $(this).attr({
      href: this.href.replace(/user\/login?/,"user/login/lightbox2"),
      rel: 'lightmodal[|width:250px; height:200px;]'
    });
  });
}

// Initialize the lightbox.
if (Drupal.jsEnabled) {
  $(document).ready(function(){
    lightbox2_login();
  });
}

