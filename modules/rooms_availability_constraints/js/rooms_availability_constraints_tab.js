(function ($) {

  Drupal.behaviors.nodeFieldsetSummaries = {
    attach: function (context) {
      $('fieldset.unit-type-form-constraints', context).drupalSetSummary(function (context) {
        var vals = [];

        if ($('.form-item-rooms-availability-range-unit input', context).is(':checked')) {
          vals.push(Drupal.t('Range constraints <b>enabled</b> at unit level'));
        }
        else {
          vals.push(Drupal.t('Range constraints disabled at unit level'));
        }

        if ($('.form-item-rooms-availability-range-type input', context).is(':checked')) {
          vals.push(Drupal.t('Range constraints <b>enabled</b> at unit type level'));
        }
        else {
          vals.push(Drupal.t('Range constraints disabled at unit type level'));
        }

        return vals.join('<br />');
      });

    }
  };

})(jQuery);
