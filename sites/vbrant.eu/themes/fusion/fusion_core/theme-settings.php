<?php
// $Id: theme-settings.php,v 1.1.2.1 2009/09/29 04:18:15 sociotech Exp $

/**
 * Theme setting defaults
 */
function fusion_core_default_theme_settings() {
  $defaults = array(
    'user_notverified_display'              => 1,
    'search_snippet'                        => 1,
    'search_info_type'                      => 1,
    'search_info_user'                      => 1,
    'search_info_date'                      => 1,
    'search_info_comment'                   => 1,
    'search_info_upload'                    => 1,
    'rebuild_registry'                      => 0,
    'fix_css_limit'                         => 0,
    'block_config_link'                     => 1,
    'grid_mask'                             => 0,
    'theme_grid'                            => 'grid16-960',
    'sidebar_first_width'                   => 0,
    'sidebar_last_width'                    => 0,
    'theme_font'                            => 'none',
    'theme_font_size'                       => '',
    'theme_color'                           => '',
  );

  // Add site-wide theme settings
  $defaults = array_merge($defaults, theme_get_settings());

  return $defaults;
}


/**
 * Initialize theme settings if needed
 */
function fusion_core_initialize_theme_settings($theme_name) {
  $theme_settings = theme_get_settings($theme_name);
  if (is_null($theme_settings['theme_font_size']) || $theme_settings['rebuild_registry'] == 1) {
    // Rebuild theme registry & notify user
    if($theme_settings['rebuild_registry'] == 1) {
      drupal_rebuild_theme_registry();
      drupal_set_message(t('Theme registry rebuild completed. <a href="!link">Turn off</a> this feature for production websites.', array('!link' => url('admin/build/themes/settings/' . $GLOBALS['theme']))), 'warning');
    }

    // Retrieve saved or site-wide theme settings
    $theme_setting_name = str_replace('/', '_', 'theme_'. $theme_name .'_settings');
    $settings = (variable_get($theme_setting_name, FALSE)) ? theme_get_settings($theme_name) : theme_get_settings();

    // Skip toggle_node_info_ settings
    if (module_exists('node')) {
      foreach (node_get_types() as $type => $name) {
        unset($settings['toggle_node_info_'. $type]);
      }
    }

    // Retrieve default theme settings
    $defaults = fusion_core_default_theme_settings();

    // Set combined default & saved theme settings
    variable_set($theme_setting_name, array_merge($defaults, $settings));

    // Force theme settings refresh
    theme_get_setting('', TRUE);
  }
}


/**
* Implementation of THEMEHOOK_settings() function.
*
* @param $saved_settings
*   array An array of saved settings for this theme.
* @return
*   array A form array.
*/
function phptemplate_settings($saved_settings) {
  global $base_url;

  // Get default theme settings from this theme's .info file
  $theme_name = arg(4);   // get theme name from url: admin/build/themes/settings/theme_name
  $theme_data = system_theme_data();   // get data for all themes
  $default_theme_settings = ($theme_name) ? $theme_data[$theme_name]->info['settings'] : '';

  // Retrieve & combine default and saved theme settings
  $defaults = fusion_core_default_theme_settings();
  $settings = array_merge($defaults, $saved_settings);

  // Create theme settings form widgets using Forms API

  // TNT Fieldset
  $form['tnt_container'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fusion theme settings'),
    '#description' => t('Use these settings to enhance the appearance and functionality of your Fusion theme.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  // General Settings
  $form['tnt_container']['general_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  // Theme fonts
  $form['tnt_container']['general_settings']['theme_font_config'] = array(
    '#type' => 'fieldset',
    '#title' => t('Typography'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  // Font family settings
  $form['tnt_container']['general_settings']['theme_font_config']['theme_font_config_font'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Font family'),
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE,
   );
  $form['tnt_container']['general_settings']['theme_font_config']['theme_font_config_font']['theme_font'] = array(
    '#type'          => 'radios',
    '#title'         => t('Select a new font family'),
    '#default_value' => $settings['theme_font'] ? $settings['theme_font'] : 'none',
    '#options'       => array(
      'none' => t('None (use theme default)'),
      'font-family-sans-serif-sm' => '<span class="font-family-sans-serif-sm">' . t('Sans serif - smaller (Helvetica Neue, Arial, Helvetica, sans-serif)') . '</span>',
      'font-family-sans-serif-lg' => '<span class="font-family-sans-serif-lg">' . t('Sans serif - larger (Verdana, Geneva, Arial, Helvetica, sans-serif)') . '</span>',
      'font-family-serif-sm' => '<span class="font-family-serif-sm">' . t('Serif - smaller (Garamond, Perpetua, Nimbus Roman No9 L, Times New Roman, serif)') . '</span>',
      'font-family-serif-lg' => '<span class="font-family-serif-lg">' . t('Serif - larger (Baskerville, Georgia, Palatino, Palatino Linotype, Book Antiqua, URW Palladio L, serif)') . '</span>',
      'font-family-myriad' => '<span class="font-family-myriad">' . t('Myriad (Myriad Pro, Myriad, Trebuchet MS, Arial, Helvetica, sans-serif)') . '</span>',
      'font-family-lucida' => '<span class="font-family-lucida">' . t('Lucida (Lucida Sans, Lucida Grande, Lucida Sans Unicode, Verdana, Geneva, sans-serif)') . '</span>',
    ),
  );
  // Font size settings
  // Get default font size from .info
  $default_font_size = (isset($default_theme_settings['base-font-size'])) ? $default_theme_settings['base-font-size'] : 'none';
  $current_font_size = ($settings['theme_font_size']) ? $settings['theme_font_size'] : $default_font_size;
  $form['tnt_container']['general_settings']['theme_font_config']['theme_font_config_size'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Font size'),
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE,
  );
  $form['tnt_container']['general_settings']['theme_font_config']['theme_font_config_size']['theme_font_size'] = array(
    '#type'          => 'radios',
    '#title'         => t('Change the base font size'),
    '#description'   => t('Adjusts all text in proportion to your base font size.'),
    '#default_value' => $current_font_size,
    '#options'       => array(
      'none'         => t('No setting'),
      'font-size-10' => t('10px'),
      'font-size-11' => t('11px'),
      'font-size-12' => t('12px'),
      'font-size-13' => t('13px'),
      'font-size-14' => t('14px'),
      'font-size-15' => t('15px'),
      'font-size-16' => t('16px'),
      'font-size-17' => t('17px'),
      'font-size-18' => t('18px'),
    ),
  );
  $form['tnt_container']['general_settings']['theme_font_config']['theme_font_config_size']['theme_font_size']['#options'][$default_font_size] .= t(' - Theme Default');

  // Theme grid
  $form['tnt_container']['general_settings']['theme_grid_config'] = array(
    '#type' => 'fieldset',
    '#title' => t('Layout'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['tnt_container']['general_settings']['theme_grid_config']['theme_grid'] = array(
    '#type'          => 'radios',
    '#title'         => t('Select a grid layout for your theme'),
    '#default_value' => $settings['theme_grid'] ? $settings['theme_grid'] : 'grid16-960',
    '#options'       => array(
      'grid16-960' => t('960px 16 column grid'),
      'grid16-fluid' => t('Fluid 16 column grid'),
    ),
  );
  $form['tnt_container']['general_settings']['theme_grid_config']['sidebar_first_width'] = array(
    '#type'          => 'select',
    '#title'         => t('Select a different width for your first sidebar'),
    '#default_value' => $settings['sidebar_first_width'] ? $settings['sidebar_first_width'] : 0,
    '#options'       => array(
      0 => t('No change (use theme default)'),
      2 => t('2 grid units (2 x 60 = 120px)'),
      3 => t('3 grid units (3 x 60 = 180px)'),
      4 => t('4 grid units (4 x 60 = 240px)'),
      5 => t('5 grid units (5 x 60 = 300px)'),
      6 => t('6 grid units (6 x 60 = 360px)'),
      7 => t('7 grid units (7 x 60 = 420px)'),
      8 => t('8 grid units (8 x 60 = 480px)'),
    ),
  );
  $form['tnt_container']['general_settings']['theme_grid_config']['sidebar_last_width'] = array(
    '#type'          => 'select',
    '#title'         => t('Select a different width for your last sidebar'),
    '#default_value' => $settings['sidebar_last_width'] ? $settings['sidebar_last_width'] : 0,
    '#options'       => array(
      0 => t('No change (use theme default)'),
      2 => t('2 grid units (2 x 60 = 120px)'),
      3 => t('3 grid units (3 x 60 = 180px)'),
      4 => t('4 grid units (4 x 60 = 240px)'),
      5 => t('5 grid units (5 x 60 = 300px)'),
      6 => t('6 grid units (6 x 60 = 360px)'),
      7 => t('7 grid units (7 x 60 = 420px)'),
      8 => t('8 grid units (8 x 60 = 480px)'),
    ),
  );

  // Search Settings
  if (module_exists('search')) {
    $form['tnt_container']['general_settings']['search_container'] = array(
      '#type' => 'fieldset',
      '#title' => t('Search results'),
      '#description' => t('What additional information should be displayed on your search results page?'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['tnt_container']['general_settings']['search_container']['search_results']['search_snippet'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display text snippet'),
      '#default_value' => $settings['search_snippet'],
    );
    $form['tnt_container']['general_settings']['search_container']['search_results']['search_info_type'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display content type'),
      '#default_value' => $settings['search_info_type'],
    );
    $form['tnt_container']['general_settings']['search_container']['search_results']['search_info_user'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display author name'),
      '#default_value' => $settings['search_info_user'],
    );
    $form['tnt_container']['general_settings']['search_container']['search_results']['search_info_date'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display posted date'),
      '#default_value' => $settings['search_info_date'],
    );
    $form['tnt_container']['general_settings']['search_container']['search_results']['search_info_comment'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display comment count'),
      '#default_value' => $settings['search_info_comment'],
    );
    $form['tnt_container']['general_settings']['search_container']['search_results']['search_info_upload'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display attachment count'),
      '#default_value' => $settings['search_info_upload'],
    );
  }

  // Username
  $form['tnt_container']['general_settings']['username'] = array(
    '#type' => 'fieldset',
    '#title' => t('Username'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['tnt_container']['general_settings']['username']['user_notverified_display'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display "not verified" for unregistered usernames'),
    '#default_value' => $settings['user_notverified_display'],
  );

  // Theme color DISABLED
  $color_enabled = false;
  if ($color_enabled) {
    $form['tnt_container']['general_settings']['theme_color_config'] = array(
      '#type' => 'fieldset',
      '#title' => t('Theme color'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['tnt_container']['general_settings']['theme_color_config']['theme_color'] = array(
      '#type'          => 'radios',
      '#title'         => t('Select a theme color'),
      '#default_value' => $settings['theme_color'] ? $settings['theme_color'] : 'blue',
      '#options'       => array(
        'blue' => t('Blue'),
        'red' => t('Red'),
        'green' => t('Green'),
      ),
    );
  }

  // Admin settings
  $form['tnt_container']['admin_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Administrator settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
 $form['tnt_container']['admin_settings']['block_config_link'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display block configure links for administrators.'),
    '#default_value' => $settings['block_config_link'],
    '#description' => t('This setting provides convenient hover links to block configuration pages directly from the block.'),
  );
 $form['tnt_container']['admin_settings']['grid_mask'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable grid overlay mask for administrators.'),
    '#default_value' => $settings['grid_mask'],
    '#description' => t('This setting enables a "GRID" button in the upper left corner of each page to toggle a grid overlay and block outlines, which can help with visualizing page layout and block positioning.'),
  );

  // Developer settings
  $form['tnt_container']['themedev'] = array(
    '#type' => 'fieldset',
    '#title' => t('Developer settings'),
    '#collapsible' => TRUE,
    '#collapsed' => $settings['rebuild_registry'] ? FALSE : TRUE,
  );
 $form['tnt_container']['themedev']['rebuild_registry'] = array(
    '#type' => 'checkbox',
    '#title' => t('Rebuild theme registry for every page.'),
    '#default_value' => $settings['rebuild_registry'],
    '#description' => t('This setting is useful while developing themes (see <a href="!link">rebuilding the theme registry</a>). However, it <strong>significantly degrades performance</strong> and should be turned off for any production website.', array('!link' => 'http://drupal.org/node/173880#theme-registry')),
  );
 $form['tnt_container']['themedev']['fix_css_limit'] = array(
    '#type' => 'checkbox',
    '#title' => t('Avoid IE stylesheet limit.'),
    '#default_value' => $settings['fix_css_limit'],
    '#description' => t('This setting groups css files so Internet Explorer can see more than 30 of them. This is useful when you cannot use aggregation (e.g., when developing or using private file downloads). But because it degrades performance and can load files out of order, CSS aggregation (<a href="!link">Optimize CSS files</a>) is <strong>strongly</strong> recommended instead for any production website.', array('!link' => $base_url .'/admin/settings/performance')),
  );

  // Return theme settings form
  return $form;
}
