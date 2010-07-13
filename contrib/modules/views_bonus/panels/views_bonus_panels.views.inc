<?php
// $Id: views_bonus_panels.views.inc,v 1.2 2009/01/03 04:50:45 neclimdul Exp $
/**
 * @file
 * Views include file with views hooks.
 */

/**
 * Implementation of hook_views_plugins().
 */
function views_bonus_panels_views_plugins() {
  return array(
    'style' => array(
      'panels_threecol' => array(
        'title' => t('Panels: Teasers, 3 columns'),
        'help' => t('...'),
        'handler' => 'views_bonus_plugin_style_panels_threecol',
        'theme' => 'views_bonus_panels_threecol',
        //'uses fields' => TRUE,
        //'uses options' => TRUE,
        'uses row plugin' => TRUE,
        'type' => 'normal',
      ),
      'panels_threecol_stack' => array(
        'title' => t('Panels: Teasers, 1 top + 3 columns'),
        'help' => t('...'),
        'handler' => 'views_bonus_plugin_style_panels_threecol_stack',
        'theme' => 'views_bonus_panels_threecol_stack',
        'uses row plugin' => TRUE,
        'type' => 'normal',
      ),
      'panels_twocol' => array(
        'title' => t('Panels: Teasers, 2 columns'),
        'help' => t('...'),
        'handler' => 'views_bonus_plugin_style_panels_twocol',
        'theme' => 'views_bonus_panels_twocol',
        'uses row plugin' => TRUE,
        'type' => 'normal',
      ),
      'panels_twocol_stack' => array(
        'title' => t('Panels: Teasers, 1 top + 2 columns'),
        'help' => t('...'),
        'handler' => 'views_bonus_plugin_style_panels_twocol_stack',
        'theme' => 'views_bonus_panels_twocol_stack',
        'uses row plugin' => TRUE,
        'type' => 'normal',
      ),
      'panels_threecol_term' => array(
        'title' => t('Panels: Teasers, By Term, 3 columns'),
        'help' => t('...'),
        'handler' => 'views_bonus_plugin_style_panels_threecol_term',
        'theme' => 'views_bonus_panels_threecol_term',
        'uses row plugin' => TRUE,
        'type' => 'normal',
      ),
    ),
  );
}
