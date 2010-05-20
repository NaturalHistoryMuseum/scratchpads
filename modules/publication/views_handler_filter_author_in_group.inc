<?php
// $Id: views_handler_filter_node_status.inc,v 1.2 2009/06/03 00:26:39 merlinofchaos Exp $
/**
 * Filter by published status
 */
class views_handler_filter_author_in_group extends views_handler_filter_numeric {
  function admin_summary() { }
  function operator_form() { }

  function value_form(&$form, &$form_state) {
    
    $form['value'] = array(
      '#type' => 'textfield',
      '#title' => empty($form_state['exposed']) ? t('Value') : '',
      '#size' => 30,
    );
    
  }

  function query() {
    
    if (empty($this->value['value'])) {
      return;
    }
    
    $this->query->add_where($this->options['group'],'node.uid IN (SELECT uid FROM og_uid WHERE nid = %d)', $this->value['value']);    
    
  }
  
}
