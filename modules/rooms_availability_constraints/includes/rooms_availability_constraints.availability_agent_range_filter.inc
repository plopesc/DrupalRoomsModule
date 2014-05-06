<?php

/**
 * Filter by group_size and group_size_children.
 */
class AvailabilityAgentDurationFilter extends AvailabilityAgentCommonFilter {

  const MINIMUM_DURATION_FILTER = 'minimum';
  const MAXIMUM_DURATION_FILTER = 'maximum';

  /**
   * The start date datetime.
   *
   * @var DateTime
   */
  protected $start_date;

  /**
   * The end date datetime.
   *
   * @var DateTime
   */
  protected $end_date;

  /**
   * The interval between start and end dates.
   *
   * @var DateInterval
   */
  protected $interval;

  /**
   * Track of discarded units to exclude it form nest queries.
   *
   * @var array
   */
  protected $excluded_units;

  public function __construct($units, $parameters) {
    parent::__construct($units, $parameters);
    $this->start_date = $this->parameters['start_date'];

    $this->end_date = $this->parameters['end_date'];
    $this->end_date = $this->end_date->modify('+1 days');

    $this->interval = date_diff($this->start_date, $this->end_date);

    $this->excluded_units = array();
  }

  /**
   * @return array
   */
  public function applyFilter() {
    $this->applyMustDayFilter();
    if (empty($this->units)) {
      return array();
    }

    $this->applyIfDayDurationFilter();
    if (empty($this->units)) {
      return array();
    }

    $this->applyDurationFilter();

    return $this->units;
  }

  /**
   * Filters the units that does not match the must day filter.
   */
  protected function applyMustDayFilter() {
    $query = $this->basicFilter();

    $query->fieldCondition('rooms_constraints_range', 'constraint_type', ROOMS_AVAILABILITY_CONSTRAINT_MUST_START, '=', 'rooms_availability_constraints');
    $query->fieldCondition('rooms_constraints_range', 'start_day', $this->start_date->format('N'), '!=', 'rooms_availability_constraints');

    $this->filterUnits($query);
  }

  /**
   * Filters the units that does not match the if day filter.
   */
  protected function applyIfDayDurationFilter() {
    $this->applyIfDayRangeFilter();
    $this->applyIfDayRangeFilter(self::MAXIMUM_DURATION_FILTER);

  }

  /**
   * Filters the units that does not match the simple duration filter.
   */
  protected function applyDurationFilter() {
    $this->applyRangeFilter();
    $this->applyRangeFilter(self::MAXIMUM_DURATION_FILTER);
  }

  /**
   * Returns the basic filter query.
   *
   * @return EntityFieldQuery
   *   The most basic filter.
   */
  protected function basicFilter() {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'rooms_unit')
      ->propertyCondition('bookable', 1)
      ->fieldCondition('rooms_constraints_range', 'start_date', $this->start_date->format('Y-m-d'), '<=', 'rooms_availability_constraints')
      ->fieldCondition('rooms_constraints_range', 'end_date', $this->end_date->format('Y-m-d'), '>=', 'rooms_availability_constraints');
    if (!empty($this->excluded_units)) {
      $query->propertyCondition('unit_id', $this->excluded_units, 'NOT IN');
    }
    return $query;
  }

  /**
   * Filters the unit list given a filter query.
   * @param EntityFieldQuery $query
   */
  protected function filterUnits($query) {
    $excluded_values = $query->execute();

    if (isset($excluded_values['rooms_unit'])) {
      foreach ($excluded_values['rooms_unit'] as $unit_id => $unit) {
        unset($this->units[$unit_id]);
        $this->excluded_units[$unit_id] = $unit_id;
      }
    }
  }

  /**
   * Returns the duration filter query.
   * @param $duration_filter
   * @return EntityFieldQuery
   */
  protected function durationFilter($duration_filter) {
    $query = $this->basicFilter();

    if ($duration_filter == static::MINIMUM_DURATION_FILTER) {
      $query->fieldCondition('rooms_constraints_range', 'minimum_stay', $this->interval->days, '>', 'rooms_availability_constraints');
    }
    else {
      $query->fieldCondition('rooms_constraints_range', 'maximum_stay', $this->interval->days, '<', 'rooms_availability_constraints');
    }

    return $query;
  }

  /**
   * Applies the range filter for a given starting day.
   *
   * @param $duration_filter
   */
  protected function applyIfDayRangeFilter($duration_filter = self::MINIMUM_DURATION_FILTER) {
    $query = $this->durationFilter($duration_filter);

    $query->fieldCondition('rooms_constraints_range', 'constraint_type', ROOMS_AVAILABILITY_CONSTRAINT_IF_STARTS, '=', 'rooms_availability_constraints');
    $query->fieldCondition('rooms_constraints_range', 'start_day', $this->start_date->format('N'), '=', 'rooms_availability_constraints');

    $this->filterUnits($query);
  }

  /**
   * Applies the generic range filter.
   * @param $duration_filter
   */
  protected function applyRangeFilter($duration_filter = self::MINIMUM_DURATION_FILTER) {
    $query = $this->durationFilter($duration_filter);

    $this->filterUnits($query);
  }

}