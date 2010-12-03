/**
 * JS specifically for views - needs to display even when there is no content in the matrix editor
 */
$(document).ready(function() {
	$('#matrix-editor-toggle-advanced-options').click(function() {
		var el = $('.view-filters, .attachment-before', '.matrix-editor-view');
		if (el.is(':visible')) {
			el.hide();
			$(this).removeClass('me-collapsed');
		} else {
			el.show();
			$(this).addClass('me-collapsed');
		}
		return false;
	});
});