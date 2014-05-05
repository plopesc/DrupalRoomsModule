<?php

/**
 * Implements hook_field_info().
 */
function rooms_availability_constraints_field_info() {
  return array(
    'rooms_availability_constraints_range' => array(
      'label' => t('Availability ranges'),
      'description' => t('Bookable unit availability options.'),
      'settings' => array(),
      'default_widget' => 'rooms_availability_constraints_range',
      'default_formatter' => 'rooms_availability_constraints_range',
    ),
  );
}

/**
 * Implements hook_field_is_empty().
 */
function rooms_availability_constraints_field_is_empty($item, $field) {
  return empty($item['start_date']) || empty($item['start_date']) || (empty($item['maximum_stay']) && empty($item['minimum_stay']));
}

/**
 * Implements hook_field_widget_info().
 */
function rooms_availability_constraints_field_widget_info() {
  return array(
    'rooms_availability_constraints_range' => array(
      'label' => t('Availability range'),
      'description' => t(''),
      'field types' => array('rooms_availability_constraints_range'),
      'settings' => array(),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function rooms_availability_constraints_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  if ($instance['widget']['type'] == 'rooms_availability_constraints_range') {
    $range = rooms_date_range_fields();
    $element['start_date'] = $range['rooms_start_date'];
    $element['end_date'] = $range['rooms_end_date'];

    $element['start_date']['#required'] = FALSE;
    $element['start_date']['#default_value'] = isset($items[$delta]['start_date']) ? $items[$delta]['start_date'] : '';
    $element['end_date']['#required'] = FALSE;
    $element['end_date']['#default_value'] = isset($items[$delta]['end_date']) ? $items[$delta]['end_date'] : '';

    $element['minimum_stay'] = array(
      '#type' => 'textfield',
      '#title' => t('Minimum stay'),
      '#size' => 2,
      '#default_value' => isset($items[$delta]['minimum_stay']) ? $items[$delta]['minimum_stay'] : '',
    );

    $element['maximum_stay'] = array(
      '#type' => 'textfield',
      '#title' => t('Maximum stay'),
      '#size' => 2,
      '#default_value' => isset($items[$delta]['maximum_stay']) ? $items[$delta]['maximum_stay'] : '',
    );

    $element['start_day'] = array(
      '#type' => 'select',
      '#options' => array(1 => t('Sunday'), 2 => t('Monday'), 3 => t('Tuesday'), 4 => t('Wednesday'), 5 => t('Thursday'), 6 => t('Friday'), 7 => t('Saturday')),
      '#empty_option' => t('- Select -'),
      '#default_value' => isset($items[$delta]['start_day']) ? $items[$delta]['start_day'] : '',
    );

    if ($field['cardinality'] == FIELD_CARDINALITY_UNLIMITED) {
      $element['remove'] = array(
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#attributes' => array(
          'class' => array('remove-booking-option'),
        ),
        '#attached' => array(
          'js' => array(
            drupal_get_path('module', 'rooms_availability_constraints') . '/js/rooms_availability_constraints_range.js'
          )
        ),
      );
    }

    return $element;
  }
}

/**
 * Implements hook_field_validate().
 */
function rooms_availability_constraints_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  foreach ($items as $delta => $item) {
    if (is_array($item) && isset($item['start_date']) && isset($item['end_date'])) {
      if ($item['start_date'] >= $item['end_date']) {
        $errors[$field['field_name']][$langcode][$delta][] = array(
          'error' => 'rooms_availability_constraints_end_date',
          'message' => t('End date must be greater than start date'),
        );
      }
      $minimum_set = $maximum_set = FALSE;
      if (!empty($item['minimum_stay'])) {
        if (!((int) $item['minimum_stay'] == $item['minimum_stay'] && (int) $item['minimum_stay'] > 0)) {
          $errors[$field['field_name']][$langcode][$delta][] = array(
            'error' => 'rooms_availability_constraints_minimum_stay',
            'message' => t('Minimum stay value must be a positive integer value'),
          );
        }
        else {
          $minimum_set = TRUE;
        }
      }

      if (!empty($item['maximum_stay'])){
        if (!((int) $item['maximum_stay'] == $item['maximum_stay'] && (int) $item['maximum_stay'] > 0)) {
          $errors[$field['field_name']][$langcode][$delta][] = array(
            'error' => 'rooms_availability_constraints_maximum_stay',
            'message' => t('Maximum stay value must be a positive integer value'),
          );
        }
        else {
          $maximum_set = TRUE;
        }
      }

      if ($maximum_set && $minimum_set && ($item['minimum_stay'] > $item['maximum_stay'])) {
        $errors[$field['field_name']][$langcode][$delta][] = array(
          'error' => 'rooms_availability_constraints_range',
          'message' => t('Minimum stay can\'t be longer than maximum stay'),
        );
      }
    }
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 * FORM_ID = rooms_unit_type_form
 */
function rooms_availability_constraints_form_rooms_unit_type_form_alter(&$form, &$form_state, $form_id) {
  $form['#submit'][] = 'rooms_availability_constraints_rooms_unit_type_form_submit';
}

function rooms_availability_constraints_rooms_unit_type_form_submit($form, &$form_state) {
  rooms_availability_constraints_add_availability_range_field($form_state['unit_type']);
}

/**
 * Adds the default Availability range field to certain bundle.
 */
function rooms_availability_constraints_add_availability_range_field($entity) {
  // Create field ('rooms_booking_unit_options') if not exist
  if (field_read_field('rooms_constraints_range') === FALSE) {
    $field = array(
      'field_name' => 'rooms_constraints_range',
      'type' => 'rooms_availability_constraints_range',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    );
    field_create_field($field);
  }

  if (field_read_instance('rooms_unit', 'rooms_constraints_range', $entity->type) === FALSE) {
    // Create the instance on the bundle.
    $instance = array(
      'field_name' => 'rooms_constraints_range',
      'entity_type' => 'rooms_unit',
      'label' => 'Availability ranges',
      'bundle' => $entity->type,
      // If you don't set the "required" property then the field wont be required by default.
      'required' => FALSE,
      'widget' => array(
        'type' => 'rooms_availability_constraints_range',
      ),
    );
    field_create_instance($instance);
  }
}