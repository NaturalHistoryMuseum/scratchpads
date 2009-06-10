<?php
class darwincore_handler_field_tid extends views_handler_field {

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_taxonomy'] = array('default' => FALSE);
    return $options;
  }

  /**
   * Provide link to taxonomy option
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to_taxonomy'] = array(
      '#title' => t('Link this field to its taxonomy term page'),
      '#description' => t('This will override any other link you have set.'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_taxonomy']),
    );
  }
  
  /**
   * Render it dude!
   */
  function render($values) {
    $value = $values->{$this->field_alias};
    $term = taxonomy_get_term($value);
    if (!empty($this->options['link_to_taxonomy']) && $this->options['link_to_taxonomy']){
      return l($term->name, 'taxonomy/term/'.$value);      
    } else {
      return $term->name;
    }
  }
}