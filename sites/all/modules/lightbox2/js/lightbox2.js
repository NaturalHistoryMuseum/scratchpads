/* $Id$ */

function alt_layout_handler(event) {
  if ($("input[@name=lightbox2_lite]:checked").val() != 1) {
    if ($("input[@name=lightbox2_use_alt_layout]:checked").val() == 1) {
      $("input[@name=lightbox2_force_show_nav]").attr("disabled", "disabled");
    }
    else {
      $("input[@name=lightbox2_force_show_nav]").removeAttr("disabled");
    }
  }
}

function lightbox2_lite_general_handler(event) {
  // Enable / disable the non-lightbox2-lite options.
  if ($("input[@name=lightbox2_lite]:checked").val() == 1) {
    $("input[@name=lightbox2_use_alt_layout]").attr("disabled", "disabled");
    $("input[@name=lightbox2_force_show_nav]").attr("disabled", "disabled");
    $("input[@name=lightbox2_disable_zoom]").attr("disabled", "disabled");
    $("input[@name=lightbox2_image_count_str]").attr("disabled", "disabled");
    $("input[@name=lightbox2_page_count_str]").attr("disabled", "disabled");
    $("select[@name=lightbox2_display_image_size]").attr("disabled", "disabled");
    $("select[@name='lightbox2_trigger_image_size[]']").attr("disabled", "disabled");
    $("input[@name=lightbox2_overlay_opacity]").attr("disabled", "disabled");
  }
  else {
    $("input[@name=lightbox2_use_alt_layout]").removeAttr("disabled");
    $("input[@name=lightbox2_force_show_nav]").removeAttr("disabled");
    $("input[@name=lightbox2_disable_zoom]").removeAttr("disabled");
    $("input[@name=lightbox2_image_count_str]").removeAttr("disabled");
    $("input[@name=lightbox2_page_count_str]").removeAttr("disabled");
    $("select[@name=lightbox2_display_image_size]").removeAttr("disabled");
    $("select[@name='lightbox2_trigger_image_size[]']").removeAttr("disabled");
    $("input[@name=lightbox2_overlay_opacity]").removeAttr("disabled");
    alt_layout_handler();
  }
}

function image_node_handler(event) {
  // Image node, flickr, gallery2, inline and custom images stuff.
  if ($("input[@name=lightbox2_lite]").val() != 1) {
    if ($("select[@name=lightbox2_image_node]").val() !== 0 ||
      $("select[@name=lightbox2_flickr]").val() !== 0 ||
      $("select[@name=lightbox2_gallery2_blocks]").val() !== 0 ||
      $("select[@name=lightbox2_image_assist_custom]").val() !== 0 ||
      $("select[@name=lightbox2_inline]").val() !== 0 ||
      $("textarea[@name=lightbox2_custom_trigger_classes]").val() !== '') {

      $("input[@name=lightbox2_node_link_text]").removeAttr("disabled");
      $("input[@name=lightbox2_node_link_target]").removeAttr("disabled");
      $("input[@name=lightbox2_imagefield_group_node_id]").removeAttr("disabled");
    }
    else {
      $("input[@name=lightbox2_node_link_text]").attr("disabled", "disabled");
      $("input[@name=lightbox2_node_link_target]").attr("disabled", "disabled");
      $("input[@name=lightbox2_imagefield_group_node_id]").attr("disabled", "disabled");
    }

    // Image node only stuff.
    if ($("select[@name=lightbox2_image_node]").val() !== 0) {
      $("input[@name=lightbox2_disable_nested_galleries]").removeAttr("disabled");
      $("select[@name=lightbox2_display_image_size]").removeAttr("disabled");
      $("select[@name='lightbox2_trigger_image_size[]']").removeAttr("disabled");
    }
    else {
      $("input[@name=lightbox2_disable_nested_galleries]").attr("disabled", "disabled");
      $("select[@name=lightbox2_display_image_size]").attr("disabled", "disabled");
      $("select[@name='lightbox2_trigger_image_size[]']").attr("disabled", "disabled");
    }
  }
}

function lightbox2_lite_auto_handler(event) {
  // Enable / disable the image node options.
  if ($("input[@name=lightbox2_lite]").val() == 1) {
    // Disable iframe options.
    $("input[@name=lightbox2_default_frame_width]").attr("disabled", "disabled");
    $("input[@name=lightbox2_default_frame_height]").attr("disabled", "disabled");
    $("input[@name=lightbox2_frame_border]").attr("disabled", "disabled");

    // Disable slideshow options.
    $("input[@name=lightbox2_slideshow_interval]").attr("disabled", "disabled");
    $("input[@name=lightbox2_slideshow_automatic_exit]").attr("disabled", "disabled");
    $("input[@name=lightbox2_slideshow_show_play_pause]").attr("disabled", "disabled");
    $("input[@name=lightbox2_slideshow_pause_on_next_click]").attr("disabled", "disabled");
    $("input[@name=lightbox2_slideshow_pause_on_prev_click]").attr("disabled", "disabled");

    // Disable automatic image handling options.
    $("select[@name=lightbox2_image_node]").attr("disabled", "disabled");
    $("input[@name=lightbox2_node_link_text]").attr("disabled", "disabled");
    $("input[@name=lightbox2_node_link_target]").attr("disabled", "disabled");
    $("input[@name=lightbox2_imagefield_group_node_id]").attr("disabled", "disabled");
    $("select[@name=lightbox2_display_image_size]").attr("disabled", "disabled");
    $("select[@name='lightbox2_trigger_image_size[]']").attr("disabled", "disabled");
    $("select[@name=lightbox2_flickr]").attr("disabled", "disabled");
    $("select[@name=lightbox2_gallery2_blocks]").attr("disabled", "disabled");
    $("select[@name=lightbox2_image_assist_custom]").attr("disabled", "disabled");
    $("select[@name=lightbox2_inline]").attr("disabled", "disabled");
    $("textarea[@name=lightbox2_custom_trigger_classes]").attr("disabled", "disabled");
    $("input[@name=lightbox2_disable_nested_galleries]").attr("disabled", "disabled");
    $("input[@name=lightbox2_disable_nested_acidfree_galleries]").attr("disabled", "disabled");
  }
  else {
    // Enable iframe options.
    $("input[@name=lightbox2_default_frame_width]").removeAttr("disabled");
    $("input[@name=lightbox2_default_frame_height]").removeAttr("disabled");
    $("input[@name=lightbox2_frame_border]").removeAttr("disabled");

    // Enable slideshow options.
    $("input[@name=lightbox2_slideshow_interval]").removeAttr("disabled");
    $("input[@name=lightbox2_slideshow_automatic_exit]").removeAttr("disabled");
    $("input[@name=lightbox2_slideshow_show_play_pause]").removeAttr("disabled");
    $("input[@name=lightbox2_slideshow_pause_on_next_click]").removeAttr("disabled");
    $("input[@name=lightbox2_slideshow_pause_on_prev_click]").removeAttr("disabled");

    // Enable automatic image handling options.
    $("select[@name=lightbox2_image_node]").removeAttr("disabled");
    $("select[@name=lightbox2_flickr]").removeAttr("disabled");
    $("select[@name=lightbox2_gallery2_blocks]").removeAttr("disabled");
    $("select[@name=lightbox2_image_assist_custom]").removeAttr("disabled");
    $("select[@name=lightbox2_inline]").removeAttr("disabled");
    $("textarea[@name=lightbox2_custom_trigger_classes]").removeAttr("disabled");
    $("input[@name=lightbox2_node_link_text]").removeAttr("disabled");
    $("input[@name=lightbox2_node_link_target]").removeAttr("disabled");
    $("input[@name=lightbox2_imagefield_group_node_id]").removeAttr("disabled");
    $("select[@name=lightbox2_display_image_size]").removeAttr("disabled");
    $("select[@name='lightbox2_trigger_image_size[]']").removeAttr("disabled");
    $("input[@name=lightbox2_disable_nested_galleries]").removeAttr("disabled");
    $("input[@name=lightbox2_disable_nested_acidfree_galleries]").removeAttr("disabled");
    image_node_handler();
  }
}

if (Drupal.jsEnabled) {
  $(document).ready(function () {

    // Handle lightbox2_settings_form.
    lightbox2_lite_general_handler();
    lightbox2_lite_auto_handler();
    image_node_handler();
    $("input[@name=lightbox2_lite]").bind("click", lightbox2_lite_general_handler);
    $("input[@name=lightbox2_use_alt_layout]").bind("click", alt_layout_handler);
    $("select[@name=lightbox2_image_node]").bind("click", image_node_handler);
    $("select[@name=lightbox2_flickr]").bind("click", image_node_handler);
    $("select[@name=lightbox2_gallery2_blocks]").bind("click", image_node_handler);
    $("select[@name=lightbox2_image_assist_custom]").bind("click", image_node_handler);
    $("select[@name=lightbox2_inline]").bind("click", image_node_handler);
    $("textarea[@name=lightbox2_custom_trigger_classes]").bind("change", image_node_handler);
  });
}


