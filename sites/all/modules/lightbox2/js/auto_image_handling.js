/* $Id: auto_image_handling.js,v 1.1.2.10 2008/05/17 20:39:06 snpower Exp $ */

// Image Node Auto-Format with Auto Image Grouping.
// Original version by Steve McKenzie.
// Altered by Stella Power for jQuery version.

function parse_url(url, param) {
  param = param.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");
  url = url.replace(/&amp;/, "&");
  var regexS = "[\\?&]"+param+"=([^&#]*)";
  var regex = new RegExp(regexS);
  var results = regex.exec(url);
  if (results === null) {
    return "";
  }
  else {
    return results[1];
  }
}


function lightbox2_init_triggers(classes, rel_type, custom_class) {
  var settings = Drupal.settings.lightbox2;

  var link_target  = "";
  if (settings.node_link_target !== 0) {
    link_target = 'target="'+ settings.node_link_target +'"';
  }

  $("a["+classes+"]").each(function(i) {

    if ((!settings.disable_for_gallery_lists && !settings.disable_for_acidfree_gallery_lists) || (!$(this).parents("td.giAlbumCell").attr("class") && !$(this).parents(".galleries").length && !$(this).parents(".acidfree-folder").length && !$(this).parents(".acidfree-list").length) || ($(this).parents(".galleries").length && !settings.disable_for_gallery_lists) || (($(this).parents(".acidfree-folder").length || $(this).parents(".acidfree-list").length) && !settings.disable_for_acidfree_gallery_lists)) {

      var child = $(this).find(classes);

      // Ensure the child has a class attribute we can work with.
      if ($(child).attr("class")) {

        // Set the alt text.
        var alt = $(child).attr("alt");
        if (!alt) {
          alt = "";
        }

        // Set the image node link text.
        var link_text = settings.node_link_text;

        // Set the rel attribute.
        var rel = "lightbox";
        var lightframe = false;
        if (rel_type == "lightframe_ungrouped") {
          rel = "lightframe";
          lightframe = true;
        }
        else if (rel_type == "lightframe") {
          lightframe = true;
        }
        if (rel_type != "lightbox_ungrouped" && rel_type != "lightframe_ungrouped") {
          rel = rel_type + "[" + $(child).attr("class") + "]";
        }

        // Set the basic href attribute - need to ensure there's no language
        // string (e.g. /en) prepended to the URL.
        var id = null;
        var href = $(child).attr("src");
        var orig_href = $(this).attr("href");
        var pattern = new RegExp(settings.file_path);
        if (orig_href.match(pattern)) {
          var lang_pattern = new RegExp(settings.base_path + "\\w\\w\\/");
          orig_href = orig_href.replace(lang_pattern, settings.base_path);
        }

        // Handle flickr images.
        if ($(child).attr("class").match("flickr-photo-img") ||
          $(child).attr("class").match("flickr-photoset-img")) {
          href = $(child).attr("src").replace("_s", "").replace("_t", "").replace("_m", "").replace("_b", "");
          if (rel_type != "lightbox_ungrouped" && rel_type != "lightframe_ungrouped") {
            rel = rel_type + "[flickr]";
            if ($(child).parents("div.block-flickr").attr("class")) {
              id = $(child).parents("div.block-flickr").attr("id");
              rel = rel_type + "["+ id +"]";
            }
          }
        }

        // Handle "image-img_assist_custom" images.
        else if ($(child).attr("class").match("image-img_assist_custom")) {
          // Image assist uses "+" signs for spaces which doesn't work for
          // normal links.
          orig_href = orig_href.replace(/\+/, " ");
          href = orig_href;
        }

        // Handle "inline" images.
        else if ($(child).attr("class").match("inline")) {
          href = orig_href;
        }

        // Handle gallery2 block images.
        else if ($(child).attr("class").match("ImageFrame_image") || $(child).attr("class").match("ImageFrame_none")) {
          var thumb_id = parse_url(href, "g2_itemId");
          var new_id = parse_url(orig_href, "g2_itemId");
          if (new_id && thumb_id) {
            var g2pattern = new RegExp("g2_itemId="+thumb_id);
            var replacement = "g2_itemId="+ new_id;
            href = href.replace(g2pattern, replacement);
          }
          rel = rel_type + "[gallery2]";
          if ($(child).parents("div.block-gallery").attr("class")) {
            id = $(child).parents("div.block-gallery").attr("id");
            rel = rel_type + "["+ id +"]";
          }
        }


        // Set the href attribute.
        else if (settings.image_node_sizes != '()') {
          href = $(child).attr("src").replace(new RegExp(settings.image_node_sizes), ((settings.display_image_size === "")?settings.display_image_size:"."+ settings.display_image_size)).replace(/(image\/view\/\d+)(\/\w*)/, ((settings.display_image_size === "")?"$1/_original":"$1/"+ settings.display_image_size));
          if (rel_type != "lightbox_ungrouped" && rel_type != "lightframe_ungrouped") {
            rel = rel_type + "[node_images]";
            if ($(child).parents("div.block-image").attr("class")) {
              id = $(child).parents("div.block-image").attr("id");
              rel = rel_type + "["+ id +"]";
            }
          }
        }

        // Modify the image url.
        var img_title = $(child).attr("title");
        if (!img_title) {
          img_title = $(this).attr("title");
          if (!img_title) {
            img_title = $(child).attr("alt");
          }
          $(child).attr({title: img_title});
        }
        if (lightframe) {
          href = orig_href;
        }
        if (!custom_class) {
          var title_link = "";
          if (link_text.length) {
            title_link = "<br /><a href=\"" + orig_href + "\" id=\"node_link_text\" "+ link_target +" >"+ link_text + "</a>";
          }
          $(this).attr({
            rel: rel,
            title: alt + title_link,
            href: href
          });
        }
        else {
          $(this).attr({
            rel: rel,
            title: alt,
            href: orig_href
          });
        }
      }
    }

  });

}

if (Drupal.jsEnabled) {
  $(document).ready(function lightbox2_image_nodes() {

    var settings = Drupal.settings.lightbox2;

    // Don't do it on the image assist popup selection screen.
    var img_assist = document.getElementById("img_assist_thumbs");
    if (!img_assist) {

      // Select the enabled image types.
      lightbox2_init_triggers(settings.trigger_lightbox_classes, "lightbox_ungrouped");
      lightbox2_init_triggers(settings.custom_trigger_classes, "lightbox_ungrouped", true);
      lightbox2_init_triggers(settings.trigger_lightbox_group_classes, "lightbox");
      lightbox2_init_triggers(settings.trigger_slideshow_classes, "lightshow");
      lightbox2_init_triggers(settings.trigger_lightframe_classes, "lightframe_ungrouped");
      lightbox2_init_triggers(settings.trigger_lightframe_group_classes, "lightframe");

    }
  });
}
