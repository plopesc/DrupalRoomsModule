<?php

/**
 * Filter by group_size and group_size_children.
 */
class AvailabilityAgentDurationFilter extends AvailabilityAgentCommonFilter {

  const MINIMUM_DURATION_FILTER = 'minimum';
  const MAXIMUM_DURATION_FILTER = 'maximum';

  protected $start_date;
  protected $end_date;
  protected $interval;

  public function __construct($units, $parameters) {
    parent::__construct($units, $parameters);
    $this->start_date = $this->parameters['start_date'];

    $this->end_date = $this->parameters['end_date'];
    $this->end_date = $this->end_date->modify('+1 days');

    $this->interval = date_diff($this->start_date, $this->end_date);
  }

  public function applyFilter() {

    $this->applyDurationFilter();
    $this->applyDurationFilter(self::MAXIMUM_DURATION_FILTER);

    return $this->units;
  }

  protected function applyDurationFilter($duration_filter = self::MINIMUM_DURATION_FILTER) {

    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'rooms_unit')
      ->propertyCondition('bookable', 1)
      ->fieldCondition('rooms_constraints_range', 'start_date', $this->start_date->format('Y-m-d'), '<=', 'rooms_availability_constraints')
      ->fieldCondition('rooms_constraints_range', 'end_date', $this->end_date->format('Y-m-d'), '>=', 'rooms_availability_constraints');

    if ($duration_filter == static::MINIMUM_DURATION_FILTER) {
      $query->fieldCondition('rooms_constraints_range', 'minimum_stay', $this->interval->days, '<', 'rooms_availability_constraints');
    }
    else {
      $query->fieldCondition('rooms_constraints_range', 'maximum_stay', $this->interval->days, '>', 'rooms_availability_constraints');
    }
    $excluded_values = $query->execute();

    if (isset($excluded_values['rooms_unit'])) {
      foreach ($excluded_values['rooms_unit'] as $unit_id => $unit) {
        unset($this->units['rooms_unit'][$unit_id]);
      }
    }

  }

}