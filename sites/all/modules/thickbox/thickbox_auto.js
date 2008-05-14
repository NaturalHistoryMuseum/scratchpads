// $Id: thickbox_auto.js,v 1.2.2.4 2007/06/05 07:34:35 frjo Exp $

/**
 * ATTENTION: this script has to be loaded _before_ thickbox.js
 * Contributed by user kleingeist.
 */
$(document).ready(function() {

  // 1. "mark" the category overview pictures.
  $("ul.galleries a[img.image.image-thumbnail]").addClass("category");

  // 2. Group the images in specific contexts,

  // find all the nodes,
  $(".node .content").each(function(i) {
    var group = "node-g" + i;
    TB_drupal_rewrite(this, group);
  });

  // find the categories.
  $("ul.images").each(function(i) {
    var group = "gallery-g" + i;
    TB_drupal_rewrite(this, group);
  });

  // 3. Rewrite the remaining images without grouping.
  TB_drupal_rewrite(document, null);
});

function TB_drupal_rewrite(context, group) {
  // Process only images, that have not been rewritten (.thickbox) and that are not categories.
  $("a[img.image.image-thumbnail]", context).not(".thickbox").not(".category").each(function(i) {
    var img = $(this).children("img");
    var title = $(this).attr("title") || img.attr("title") || img.attr("alt") || null;

    /**
     * ATTENTION: Until the derivate Bug (http://drupal.org/node/125610) is fixed,
     * the script should allways use the original picture ("true || ").
     */
    if (thickbox_derivative == "original") {
      var href = img.attr("src").replace(".thumbnail", "");
    }
    else {
      var href = img.attr("src").replace(".thumbnail", "." + thickbox_derivative);
    }

    // Finally rewrite the link and wait for thickbox.js to apply the effects.
    $(this).attr({href: href, title: title, rel: group});
    $(this).addClass("thickbox");
  });
}
