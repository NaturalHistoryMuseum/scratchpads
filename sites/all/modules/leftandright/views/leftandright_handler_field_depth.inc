<?php
class leftandright_handler_field_depth extends views_handler_field {
  function render($values) {
    $term = taxonomy_get_term($values->tid);
    $value = $values->{$this->field_alias};
    $output = '<p style="padding:0;margin:0;padding-left:'.($value*20).'px">'.l($term->name, 'taxonomy/term/'.$term->tid).'</p>';
    if($this->options['description']){
      $output .= '<p style="padding:0;margin:0;padding-left:'.(($value*20)+5).'px;font-size:90%">'.check_plain($term->description).'</p>';
    }
    return $output;
  }
  function allow_advanced_render() {
    return FALSE;
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['description'] = array('default' => 0);
    return $options;
  }
  
  /**
   * Default options form that provides the label widget that all fields
   * should have.
   */
  function options_form(&$form, &$form_state) {
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#default_value' => isset($this->options['label']) ? $this->options['label'] : '',
      '#description' => t('The label for this field that will be displayed to end users if the style requires it.'),
    );
    $form['exclude'] = array(
      '#type' => 'checkbox',
      '#title' => t('Exclude from display'),
      '#default_value' => $this->options['exclude'],
      '#description' => t('Check this box to not display this field, but still load it in the view.  Use this option to not show a grouping field in each record, or when doing advanced theming.'),
    );
    $form['description'] = array(
      '#type' => 'checkbox',
      '#title' => t('Include description'),
      '#default_value' => isset($this->options['description']) ? $this->options['description'] : 0,
      '#description' => t("Check to include the term's description")
    );
  }
}