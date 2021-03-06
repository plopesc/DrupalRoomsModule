<?php

/**
 * @file
 * This file contains no working PHP code; it exists to provide additional
 * documentation for doxygen as well as to document hooks in the standard
 * Drupal manner.
 */

/**
 * Allows to modify the booking amount before apply price modifiers.
 *
 * @param $price
 *   The calculated booking amount.
 * @param array $booking_info
 * Array containing the booking information. Contains the following key/value
 *   pairs:
 *   - start_date: DateTime object containing the booking start date.
 *   - end_date: DateTime object containing the booking end date. In this case,
 *   the end date represents the last night the unit is blocked, so it is one
 *   day before the checkout date entered in the booking form.
 *   - unit: The RoomsUnit entity the booking is related to.
 *   - booking_parameters: Array containing some other booking parameters:
 *     - group_size: The total number of persons included in the booking.
 *     - group_size_children: The number of children.
 *     - childrens_age: Array containing children's age.
 */
function hook_rooms_booking_amount_before_modifiers_alter(&$price, $booking_info) {
  // Hardcode a 100$ price whe booking longer than 5 days.

  $period = $booking_info ['start_date']->diff($booking_data['end_date'])->days + 1;

  if ($period > 5) {
    $price = 100;
  }
}
