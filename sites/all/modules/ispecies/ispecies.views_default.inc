<?php
function ispecies_views_default_views() {
  $ispecies_views = module_invoke_all('ispecies');
  
  /*
   * What do we need from hook_ispecies array
   * 
   * name (will have module prepended, so only needs to be unique per module)
   * title
   * description
   * callback_url 
   */
  foreach($ispecies_views as $module => $ispecies_view){
    // Begin copy and paste of output from the Export tab of a view.
    $view = new view;
    $view->name = $ispecies_view['name'];
    $view->description = $ispecies_view['description'];
    $view->tag = '';
    $view->view_php = '';
    $view->base_table = 'node';
    $view->is_cacheable = FALSE;
    $view->api_version = 2;
    $view->disabled = TRUE;
    $handler = $view->new_display('default', $ispecies_view['title'], 'default');
    $handler->override_option('fields', array(
      'title' => array(
        'label' => '',
        'alter' => array(
          'alter_text' => 0,
          'text' => '',
          'make_link' => 0,
          'path' => '',
          'alt' => '',
          'prefix' => '',
          'suffix' => '',
          'help' => '',
          'trim' => 0,
          'max_length' => '',
          'word_boundary' => 1,
          'ellipsis' => 1,
          'html' => 0,
        ),
        'link_to_node' => 1,
        'exclude' => 0,
        'id' => 'title',
        'table' => 'node',
        'field' => 'title',
        'relationship' => 'none',
      ),
      'body' => array(
        'label' => '',
        'alter' => array(
          'alter_text' => 0,
          'text' => '',
          'make_link' => 0,
          'path' => '',
          'alt' => '',
          'prefix' => '',
          'suffix' => '',
          'help' => '',
          'trim' => 0,
          'max_length' => '',
          'word_boundary' => 1,
          'ellipsis' => 1,
          'html' => 0,
        ),
        'exclude' => 0,
        'id' => 'body',
        'table' => 'node_revisions',
        'field' => 'body',
        'relationship' => 'none',
      ),
    ));
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
        'default_action' => 'empty',
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
        'relationship' => 'none',
        'default_options_div_prefix' => '',
        'default_argument_user' => 0,
        'default_argument_fixed' => '',
        'default_argument_php' => '',
        'validate_argument_node_type' => array(
          'blog' => 0,
          'poll' => 0,
          'biblio' => 0,
          'countriesmap' => 0,
          'node_example' => 0,
          'image' => 0,
          'ispecies' => 0,
          'tree' => 0,
          'forum' => 0,
          'darwincore' => 0,
          'darwincorelocation' => 0,
          'feed' => 0,
          'group' => 0,
          'page' => 0,
          'simplenews' => 0,
          'story' => 0,
        ),
        'validate_argument_node_access' => 0,
        'validate_argument_nid_type' => 'nid',
        'validate_argument_vocabulary' => array(
          '10' => 0,
          '1' => 0,
          '9' => 0,
          '6' => 0,
          '4' => 0,
          '5' => 0,
          '11' => 0,
          '7' => 0,
          '2' => 0,
          '3' => 0,
        ),
        'validate_argument_type' => 'tid',
        'user_argument_type' => '',
        'restrict_user_roles' => 0,
        'user_roles' => array(),
        'validate_argument_php' => '',
      ),
    ));
    $handler->override_option('filters', array(
      'tid' => array(
        'operator' => 'or',
        'value' => '',
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
          'operator' => FALSE,
          'label' => '',
        ),
        'type' => 'textfield',
        'vid' => '6',
        'id' => 'tid',
        'table' => 'term_node',
        'field' => 'tid',
        'hierarchy' => 0,
        'relationship' => 'none',
        'reduce_duplicates' => 0,
      ),
      'type' => array(
        'operator' => 'in',
        'value' => array(
          'ispecies' => 'ispecies',
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
      'ispecies' => array(
        'operator' => '=',
        'value' => $ispecies_view['name'],
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
          'operator' => FALSE,
          'label' => '',
        ),
        'case' => 1,
        'id' => 'ispecies',
        'table' => 'ispecies',
        'field' => 'ispecies',
        'relationship' => 'none',
      ),
    ));
    $handler->override_option('access', array(
      'type' => 'none',
    ));
    $handler->override_option('title', $ispecies_view['title']);
    $handler->override_option('empty', '<script type="text/javascript">ispecies_callback(\''.$ispecies_view['callback_url'].'\');</script>');
    $handler->override_option('empty_format', '2'); // FIXME - this could change
    
    $views[$view->name] = $view;
  }
  // At the end, return array of default views.
  return $views;
}