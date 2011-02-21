/**
 * JS specifically for views - needs to display even when there is no content in the matrix editor
 */

var acdb = [];

/**
  * Attaches the autocomplete behavior to all required fields
  */
Drupal.behaviors.matrix_editor_views = function(context) {
    // Reattach the autocomplete behaviours, but add them to a globally accessible variable
    $('input.autocomplete-processed:not(.matrix-editor-autocomplete-processed)', context).each(function() {
        var uri = this.value;
        if (!acdb[uri]) {
            acdb[uri] = new Drupal.ACDB(uri);
        }
        var input = $('#' + this.id.substr(0, this.id.length - 13))
        .unbind('keydown')
        // Remove the event triggers for the old autocomplete
        .unbind('keyup')
        .unbind('blur')
        .attr('autocomplete', 'OFF')[0];
        $(input.form).submit(Drupal.autocompleteSubmit);
        new Drupal.jsAC(input, acdb[uri]);
        $(this).addClass('matrix-editor-autocomplete-processed');
    });

    $('.matrix-editor-toggle-advanced-options', context).click(function() {
        var id = $(this).attr('href');
        $('.matrix-editor-option:not(' + id + ')').hide();
        $('a.matrix-editor-toggle-advanced-options').removeClass('expanded');
        var $id = $(id);
        $id.toggle();
        if ($id.is(':visible')) {
            $(this).addClass('expanded');
        } else {
            $(this).removeClass('expanded');
        }
        return false;
    });


    $('#edit-vocabulary', context).change(function() {
        var uri = $('#edit-tid-autocomplete').val();
        // Update the URI so it uses the selected vocabulary
        acdb[uri]['uri'] = 'http://scratchpads/taxonomy/autocomplete/' + $(this).val();
        // Clear the cache
        acdb[uri]['cache'] = [];
        // CLear any existing selected terms
        $('#edit-tid').val(null);
    });

    // Date selection
    $('#edit-date-options', context).change(function() {
        $('.date-views-filter-wrapper').hide();
        matrixEditorClearDates();

        if ($(this).val().length) {

            if ($(this).val() == 'enter_dates') {

                // User has selected to enter their own dates - show the fields
                $('.date-views-filter-wrapper').show();

            } else {
                // There are timestamps that need to be added to the form
                if ($(this).val().indexOf('|') !== -1) {
                    // Does it have a start and end time?
                    var timestamps = $(this).val().split('|');
                } else {
                    var timestamps = [$(this).val()];
                }
                $.each(timestamps,
                function(index, value) {
                    var dateTime = value.split('T');
                    if (index == 0) {
                        var range = 'min';
                    } else {
                        var range = 'max';
                    }

                    $('input[name="date_filter[' + range + '][date]"]').val(dateTime[0]);
                    $('input[name="date_filter[' + range + '][time]"]').val(dateTime[1]);

                });

            }

        }


    });

    // On load show the date options if user has selected to enter own dates
    if ($('#edit-date-options').val() == 'enter_dates') {
        $('.date-views-filter-wrapper').show();
    }

};

function matrixEditorClearDates() {
    $.each(['min', 'max'],
    function(index, range) {
        $('input[name="date_filter[' + range + '][date]"]').val(null);
        $('input[name="date_filter[' + range + '][time]"]').val(null);
    })
}
