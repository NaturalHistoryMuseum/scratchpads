<?php
function scratchpadify_views_default_views() {
  
  // Lets get all the content types
  $content_types = content_types();
  
  /*
   * What do we need from hook_ispecies array
   * 
   * name (will have module prepended, so only needs to be unique per module)
   * title
   * description
   * callback_url 
   */
  foreach($content_types as $type => $type_details){
    // Begin copy and paste of output from the Export tab of a view.
    $view = new view;
    $view->name = $type;
    $view->description = $type_details->name;
    $view->tag = '';
    $view->view_php = '';
    $view->base_table = 'node';
    $view->is_cacheable = FALSE;
    $view->api_version = 2;
    $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
    $handler = $view->new_display('default', 'Defaults', 'default');
    $handler->override_option('sorts', array(
      'weight' => array(
        'order' => 'ASC',
        'id' => 'weight',
        'table' => 'node_weight',
        'field' => 'weight',
        'relationship' => 'none',
      ),
    ));
    $handler->override_option('arguments', array(
      'tid' => array(
        'default_action' => 'not found',
        'style_plugin' => 'default_summary',
        'style_options' => array(),
        'wildcard' => 'all',
        'wildcard_substitution' => 'All',
        'title' => '',
        'default_argument_type' => 'fixed',
        'default_argument' => '',
        'validate_type' => 'none',
        'validate_fail' => 'not found',
        'break_phrase' => 0,
        'add_table' => 0,
        'require_value' => 0,
        'reduce_duplicates' => 0,
        'set_breadcrumb' => 0,
        'id' => 'tid',
        'table' => 'term_node',
        'field' => 'tid',
        'validate_user_argument_type' => 'uid',
        'validate_user_roles' => array(
          '2' => 0,
          '3' => 0,
          '4' => 0,
          '5' => 0,
        ),
        'relationship' => 'none',
        'default_options_div_prefix' => '',
        'default_argument_user' => 0,
        'default_argument_fixed' => '',
        'default_argument_php' => '',
        'validate_argument_node_type' => array(
        ),
        'validate_argument_node_access' => 0,
        'validate_argument_nid_type' => 'nid',
        'validate_argument_vocabulary' => array(
        ),
        'validate_argument_type' => 'tid',
        'validate_argument_transform' => 0,
        'validate_user_restrict_roles' => 0,
        'validate_argument_php' => '',
      ),
    ));
    $handler->override_option('filters', array(
      'type' => array(
        'operator' => 'in',
        'value' => array(
          'darwincore' => $type,
        ),
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
          'operator' => FALSE,
          'label' => '',
        ),
        'id' => 'type',
        'table' => 'node',
        'field' => 'type',
        'relationship' => 'none',
      ),
    ));
    $handler->override_option('access', array(
      'type' => 'none',
    ));
    $handler->override_option('row_plugin', 'node');
    $handler->override_option('title', $type_details['name']);
    $views[$view->name] = $view;
  }
  // At the end, return array of default views.
  return $views;
}