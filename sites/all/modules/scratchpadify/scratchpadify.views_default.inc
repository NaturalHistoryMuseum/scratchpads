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
  // Don't create an iSpecies view
  unset($content_types['ispecies']);
  foreach($content_types as $type => $type_details){
    // Begin copy and paste of output from the Export tab of a view.
    $view = new view;
    $view->name = $type;
    $view->description = "[Full nodes view] ".strip_tags($type_details['description']);
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
        'validate_user_roles' => array(),
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
  // ---------------------- BIBLIO TABLE ---------------------------------------
  $view = new view;
  $view->name = 'biblio_table';
  $view->description = 'Biblio nodes formatted in a table';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'biblio_year' => array(
      'label' => 'Year of Publication',
      'alter' => array(),
      'biblio_label' => 1,
      'exclude' => 0,
      'id' => 'biblio_year',
      'table' => 'biblio',
      'field' => 'biblio_year',
      'relationship' => 'none',
    ),
    'biblio_authors' => array(
      'label' => 'Authors',
      'alter' => array(),
      'biblio_label' => 1,
      'style_name' => 'cse',
      'exclude' => 0,
      'id' => 'biblio_authors',
      'table' => 'biblio',
      'field' => 'biblio_authors',
      'relationship' => 'none',
    ),
    'title' => array(
      'label' => 'Title',
      'alter' => array(),
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'biblio' => 'biblio',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    )
  ));
  $handler->override_option('arguments', array(
    'term_node_tid_depth' => array(
      'default_action' => 'ignore',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'depth' => '0',
      'break_phrase' => 0,
      'set_breadcrumb' => 0,
      'id' => 'term_node_tid_depth',
      'table' => 'node',
      'field' => 'term_node_tid_depth',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
      'validate_argument_php' => '',
    ),
  ));
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'biblio_authors' => 'biblio_authors',
      'title' => 'title',
      'biblio_year' => 'biblio_year',
    ),
    'info' => array(
      'biblio_authors' => array(
        'separator' => '',
      ),
      'title' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'biblio_year' => array(
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler->override_option('title', 'Bibliography');
  $handler->override_option('items_per_page', 0);  
  $views[$view->name] = $view;
  
  
  // ---------------------- VOCABULARIES ---------------------------------------
  $view = new view;
  $view->name = 'vocabularies';
  $view->description = '';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'term_data';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'depth' => array(
      'label' => '',
      'alter' => array(),
      'exclude' => 0,
      'id' => 'depth',
      'table' => 'leftandright',
      'field' => 'depth',
      'relationship' => 'none',
      'description' => 1
    ),
  ));
  $handler->override_option('sorts', array(
    'lft' => array(
      'order' => 'ASC',
      'id' => 'lft',
      'table' => 'leftandright',
      'field' => 'lft',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('arguments', array(
    'vid' => array(
      'default_action' => 'empty',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '%1',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'id' => 'vid',
      'table' => 'vocabulary',
      'field' => 'vid',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('items_per_page', 0);
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'vocabularies');
  $views[$view->name] = $view;
  
  // At the end, return array of default views.
  return $views;
}