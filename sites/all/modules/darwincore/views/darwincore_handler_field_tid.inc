<?php
class darwincore_handler_field_tid extends views_handler_field {
  function render($values) {
    $value = $values->{$this->field_alias};
    $term = taxonomy_get_term($value);
    return l($term->name, 'taxonomy/term/'.$value);
  }
}