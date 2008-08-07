// $Id $
/**
 * $file: addnode support functions to help with the following:
 *  - display popup window and transfer newly created node ids to the select box
 *  - controls to move nodes between the unselected and selected boxes.
 *  - controls to reorder ndoes in the selected boxes
 *  - controls to calculate the order of nodes and send that to the drupal BE
 **/

/**
 * document ready hook - create a submit handler for the node edit form 
 * so that the addnode order fields are intiialized properly before the 
 * submit.
 **/
$(document).ready( function() {
	$('input.form-submit').click(function() {

	  //make sure we initialize all the addnode order fields
		$('.addnode-order').each( function() {
			//load the values with the orders.
			var nm = this.id;

			//field names contain a '-' but variables contain underscores
			//so need to swap the two.
			nm = nm.substring(5); //strip the "edit-" portion
			nm = nm.substring(0, nm.length-5); //strip the "-nids" part
			nm = nm.replace(/-/g, '_');
			addnode_update_order(nm);
		});

    //cannot send an unselected select box back - just select all the nodes
		//in the "to_" select box. - else validation will fail.
		$('.addnode-select').each( function() {
			var o = this.options;
			for(i=0;i<o.length;i++) {
				this.options[i].selected = true;
			}
		});
	});
});

/**
 * call back to help nodeassist insert the newly created node intou our select
 * boxes.
 * just identifies the select box and adds the newly created node to it.
 */
function updateNewNodes(doc, args, id, title) {
		if(id != '') {
    //Add new nids to select - will it let us?
	  	var u = doc.getElementById(args);
			var option = new Option(title,id);
			var len = u.options.length;
			u.options[len] = option;
			u.options[len].selected = true;
		}
}

/**
 * for multi-select addnode fields - move the node from the selected list to
 * the unselected list.
 **/
function addnode_remove_item(field_name) {
  var to_obj = $('#from_' + field_name).get(0);
  var from_obj = $('#to_' + field_name).get(0);
	addnode_move_item(from_obj, to_obj);
  addnode_fix_empty(field_name);
}

/**
 * for multi-select addnode fields - move the node to the selected list from
 * the unselected list.
 **/
function addnode_add_item(field_name) {
  var from_obj = $('#from_' + field_name).get(0);
  var to_obj = $('#to_' + field_name).get(0);
	addnode_move_item(from_obj, to_obj);
  addnode_fix_empty(field_name);
}

/**
 * helper function to move the items from one select list to another.
 **/
function addnode_move_item(from_obj, to_obj) {
	var i = 0;
  while(i < from_obj.options.length) {
		if(from_obj.options[i].selected && from_obj.options[i].value !=0) {
			var o = from_obj.options[i];
			len = to_obj.options.length;
			to_obj.options[len] = new Option(o.text, o.value, o.defaultSelected, o.selected);
			from_obj.options[i] = null;
		}
		else {
			i++;
		}
  }
}

/**
 * don't let our lists become empty - this adds a "None" entry to the list
 * if there are no entries in the list.
 **/
function addnode_fix_empty_list(obj) {
	if(obj.options.length == 0) {
		obj.options[0] = new Option('None', 0);
		obj.options[0].disabled = true;
	}

	if(obj.options.length > 1 && obj.options[0].value == 0) {
		obj.options[0] = null;
	}
}

/**
 * fix empty list for a specific field
 **/
function addnode_fix_empty(field_name) {
  var from_obj = $('#from_' + field_name).get(0);
  var to_obj = $('#to_' + field_name).get(0);

	addnode_fix_empty_list(from_obj);
	addnode_fix_empty_list(to_obj);
}

/**
 * function to move the selected items up by one in the select box.
 **/
function addnode_move_item_up(field_name) {
	var to_obj = $('#to_' + field_name).get(0);
	for(i=1; i< to_obj.options.length; i++) {
		if(to_obj.options[i].selected) {
			 addnode_swap_item(to_obj, i, i-1);
		}
	}
}

/**
 * helper function to swap two option elements in a select box
 **/
function addnode_swap_item(obj, i, j) {
  var o = obj.options;
  var ti = new Option(o[i].text, o[i].value, o[i].defaultSelected, o[i].selected);
  var tj = new Option(o[j].text, o[j].value, o[j].defaultSelected, o[j].selected);

  o[i] = tj;
	o[j] = ti;
}

/**
 * function to move the selected items down by one in the select box.
 **/
function addnode_move_item_down(field_name) {
	var to_obj = $('#to_' + field_name).get(0);
	for(i = to_obj.options.length-2; i>=0; i--) {
		if(to_obj.options[i].selected) {
			 addnode_swap_item(to_obj, i, i+1);
		}
	}
}

/**
 * function recalculates the new order or sequence of nodes in the select
 * box and sets the value of a hidden field to the new order.
 **/
function addnode_update_order(field_name) {
	$('#to_' + field_name).each(function() {
		var obj = this.options;
		var ids = new Array();
		for (var i = 0; i < obj.length; i++) {
			if (obj[i].value != 0) {
				ids[i] = obj[i].value;
			}
		}
		var new_order = new String(ids.join(","));
		field_name = field_name.replace(/_/g, '-');
		$('#edit-' + field_name + '-nids').val(new_order);
	});
}

function addnode_callback(fn,arg,id,title) {
	if( id == '' ) {
		return;
	}

	if(window[fn]) {
		window[fn](this.parent.document,arg,id,title);
	}
}
