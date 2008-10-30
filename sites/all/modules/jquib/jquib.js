(function(){
/*
 * jQuery 1.2.3 - New Wave Javascript
 *
 * Copyright (c) 2008 John Resig (jquery.com)
 * Dual licensed under the MIT (MIT-LICENSE.txt)
 * and GPL (GPL-LICENSE.txt) licenses.
 *
 * $Date$
 * $Rev$
 */

// Map over jQuery in case of overwrite
if ( typeof window.jQuery != "undefined" ) {
  var _jQuery = window.jQuery;
}


var jQuery = window.jQuery = function( selector, context ) {
	// The jQuery object is actually just the init constructor 'enhanced'
	return new jQuery.prototype.init( selector, context );
};

// Map over the $ in case of overwrite
if ( window.$ )
	var _$ = window.$;
	
// Map the jQuery namespace to the '$' one
window.$ = jQuery;

// A simple way to check for HTML strings or ID strings
// (both of which we optimize for)
var quickExpr = /^[^<]*(<(.|\s)+>)[^>]*$|^#(\w+)$/;

// Is it a simple selector
var isSimple = /^.[^:#\[\.]*$/;

jQuery.fn = jQuery.prototype = {
	init: function( selector, context ) {
		// Make sure that a selection was provided
		selector = selector || document;

		// Handle $(DOMElement)
		if ( selector.nodeType ) {
			this[0] = selector;
			this.length = 1;
			return this;

		// Handle HTML strings
		} else if ( typeof selector == "string" ) {
			// Are we dealing with HTML string or an ID?
			var match = quickExpr.exec( selector );

			// Verify a match, and that no context was specified for #id
			if ( match && (match[1] || !context) ) {

				// HANDLE: $(html) -> $(array)
				if ( match[1] )
					selector = jQuery.clean( [ match[1] ], context );

				// HANDLE: $("#id")
				else {
					var elem = document.getElementById( match[3] );

					// Make sure an element was located
					if ( elem )
						// Handle the case where IE and Opera return items
						// by name instead of ID
						if ( elem.id != match[3] )
							return jQuery().find( selector );

						// Otherwise, we inject the element directly into the jQuery object
						else {
							this[0] = elem;
							this.length = 1;
							return this;
						}

					else
						selector = [];
				}

			// HANDLE: $(expr, [context])
			// (which is just equivalent to: $(content).find(expr)
			} else
				return new jQuery( context ).find( selector );

		// HANDLE: $(function)
		// Shortcut for document ready
		} else if ( jQuery.isFunction( selector ) )
			return new jQuery( document )[ jQuery.fn.ready ? "ready" : "load" ]( selector );

		return this.setArray(
			// HANDLE: $(array)
			selector.constructor == Array && selector ||

			// HANDLE: $(arraylike)
			// Watch for when an array-like object, contains DOM nodes, is passed in as the selector
			(selector.jquery || selector.length && selector != window && !selector.nodeType && selector[0] != undefined && selector[0].nodeType) && jQuery.makeArray( selector ) ||

			// HANDLE: $(*)
			[ selector ] );
	},
	
	// The current version of jQuery being used
	jquery: "1.2.3",

	// The number of elements contained in the matched element set
	size: function() {
		return this.length;
	},
	
	// The number of elements contained in the matched element set
	length: 0,

	// Get the Nth element in the matched element set OR
	// Get the whole matched element set as a clean array
	get: function( num ) {
		return num == undefined ?

			// Return a 'clean' array
			jQuery.makeArray( this ) :

			// Return just the object
			this[ num ];
	},
	
	// Take an array of elements and push it onto the stack
	// (returning the new matched element set)
	pushStack: function( elems ) {
		// Build a new jQuery matched element set
		var ret = jQuery( elems );

		// Add the old object onto the stack (as a reference)
		ret.prevObject = this;

		// Return the newly-formed element set
		return ret;
	},
	
	// Force the current matched set of elements to become
	// the specified array of elements (destroying the stack in the process)
	// You should use pushStack() in order to do this, but maintain the stack
	setArray: function( elems ) {
		// Resetting the length to 0, then using the native Array push
		// is a super-fast way to populate an object with array-like properties
		this.length = 0;
		Array.prototype.push.apply( this, elems );
		
		return this;
	},

	// Execute a callback for every element in the matched set.
	// (You can seed the arguments with an array of args, but this is
	// only used internally.)
	each: function( callback, args ) {
		return jQuery.each( this, callback, args );
	},

	// Determine the position of an element within 
	// the matched set of elements
	index: function( elem ) {
		var ret = -1;

		// Locate the position of the desired element
		this.each(function(i){
			if ( this == elem )
				ret = i;
		});

		return ret;
	},

	attr: function( name, value, type ) {
		var options = name;
		
		// Look for the case where we're accessing a style value
		if ( name.constructor == String )
			if ( value == undefined )
				return this.length && jQuery[ type || "attr" ]( this[0], name ) || undefined;

			else {
				options = {};
				options[ name ] = value;
			}
		
		// Check to see if we're setting style values
		return this.each(function(i){
			// Set all the styles
			for ( name in options )
				jQuery.attr(
					type ?
						this.style :
						this,
					name, jQuery.prop( this, options[ name ], type, i, name )
				);
		});
	},

	css: function( key, value ) {
		// ignore negative width and height values
		if ( (key == 'width' || key == 'height') && parseFloat(value) < 0 )
			value = undefined;
		return this.attr( key, value, "curCSS" );
	},

	text: function( text ) {
		if ( typeof text != "object" && text != null )
			return this.empty().append( (this[0] && this[0].ownerDocument || document).createTextNode( text ) );

		var ret = "";

		jQuery.each( text || this, function(){
			jQuery.each( this.childNodes, function(){
				if ( this.nodeType != 8 )
					ret += this.nodeType != 1 ?
						this.nodeValue :
						jQuery.fn.text( [ this ] );
			});
		});

		return ret;
	},

	wrapAll: function( html ) {
		if ( this[0] )
			// The elements to wrap the target around
			jQuery( html, this[0].ownerDocument )
				.clone()
				.insertBefore( this[0] )
				.map(function(){
					var elem = this;

					while ( elem.firstChild )
						elem = elem.firstChild;

					return elem;
				})
				.append(this);

		return this;
	},

	wrapInner: function( html ) {
		return this.each(function(){
			jQuery( this ).contents().wrapAll( html );
		});
	},

	wrap: function( html ) {
		return this.each(function(){
			jQuery( this ).wrapAll( html );
		});
	},

	append: function() {
		return this.domManip(arguments, true, false, function(elem){
			if (this.nodeType == 1)
				this.appendChild( elem );
		});
	},

	prepend: function() {
		return this.domManip(arguments, true, true, function(elem){
			if (this.nodeType == 1)
				this.insertBefore( elem, this.firstChild );
		});
	},
	
	before: function() {
		return this.domManip(arguments, false, false, function(elem){
			this.parentNode.insertBefore( elem, this );
		});
	},

	after: function() {
		return this.domManip(arguments, false, true, function(elem){
			this.parentNode.insertBefore( elem, this.nextSibling );
		});
	},

	end: function() {
		return this.prevObject || jQuery( [] );
	},

	find: function( selector ) {
		var elems = jQuery.map(this, function(elem){
			return jQuery.find( selector, elem );
		});

		return this.pushStack( /[^+>] [^+>]/.test( selector ) || selector.indexOf("..") > -1 ?
			jQuery.unique( elems ) :
			elems );
	},

	clone: function( events ) {
		// Do the clone
		var ret = this.map(function(){
			if ( jQuery.browser.msie && !jQuery.isXMLDoc(this) ) {
				// IE copies events bound via attachEvent when
				// using cloneNode. Calling detachEvent on the
				// clone will also remove the events from the orignal
				// In order to get around this, we use innerHTML.
				// Unfortunately, this means some modifications to 
				// attributes in IE that are actually only stored 
				// as properties will not be copied (such as the
				// the name attribute on an input).
				var clone = this.cloneNode(true),
					container = document.createElement("div");
				container.appendChild(clone);
				return jQuery.clean([container.innerHTML])[0];
			} else
				return this.cloneNode(true);
		});

		// Need to set the expando to null on the cloned set if it exists
		// removeData doesn't work here, IE removes it from the original as well
		// this is primarily for IE but the data expando shouldn't be copied over in any browser
		var clone = ret.find("*").andSelf().each(function(){
			if ( this[ expando ] != undefined )
				this[ expando ] = null;
		});
		
		// Copy the events from the original to the clone
		if ( events === true )
			this.find("*").andSelf().each(function(i){
				if (this.nodeType == 3)
					return;
				var events = jQuery.data( this, "events" );

				for ( var type in events )
					for ( var handler in events[ type ] )
						jQuery.event.add( clone[ i ], type, events[ type ][ handler ], events[ type ][ handler ].data );
			});

		// Return the cloned set
		return ret;
	},

	filter: function( selector ) {
		return this.pushStack(
			jQuery.isFunction( selector ) &&
			jQuery.grep(this, function(elem, i){
				return selector.call( elem, i );
			}) ||

			jQuery.multiFilter( selector, this ) );
	},

	not: function( selector ) {
		if ( selector.constructor == String )
			// test special case where just one selector is passed in
			if ( isSimple.test( selector ) )
				return this.pushStack( jQuery.multiFilter( selector, this, true ) );
			else
				selector = jQuery.multiFilter( selector, this );

		var isArrayLike = selector.length && selector[selector.length - 1] !== undefined && !selector.nodeType;
		return this.filter(function() {
			return isArrayLike ? jQuery.inArray( this, selector ) < 0 : this != selector;
		});
	},

	add: function( selector ) {
		return !selector ? this : this.pushStack( jQuery.merge( 
			this.get(),
			selector.constructor == String ? 
				jQuery( selector ).get() :
				selector.length != undefined && (!selector.nodeName || jQuery.nodeName(selector, "form")) ?
					selector : [selector] ) );
	},

	is: function( selector ) {
		return selector ?
			jQuery.multiFilter( selector, this ).length > 0 :
			false;
	},

	hasClass: function( selector ) {
		return this.is( "." + selector );
	},
	
	val: function( value ) {
		if ( value == undefined ) {

			if ( this.length ) {
				var elem = this[0];

				// We need to handle select boxes special
				if ( jQuery.nodeName( elem, "select" ) ) {
					var index = elem.selectedIndex,
						values = [],
						options = elem.options,
						one = elem.type == "select-one";
					
					// Nothing was selected
					if ( index < 0 )
						return null;

					// Loop through all the selected options
					for ( var i = one ? index : 0, max = one ? index + 1 : options.length; i < max; i++ ) {
						var option = options[ i ];

						if ( option.selected ) {
							// Get the specifc value for the option
							value = jQuery.browser.msie && !option.attributes.value.specified ? option.text : option.value;
							
							// We don't need an array for one selects
							if ( one )
								return value;
							
							// Multi-Selects return an array
							values.push( value );
						}
					}
					
					return values;
					
				// Everything else, we just grab the value
				} else
					return (this[0].value || "").replace(/\r/g, "");

			}

			return undefined;
		}

		return this.each(function(){
			if ( this.nodeType != 1 )
				return;

			if ( value.constructor == Array && /radio|checkbox/.test( this.type ) )
				this.checked = (jQuery.inArray(this.value, value) >= 0 ||
					jQuery.inArray(this.name, value) >= 0);

			else if ( jQuery.nodeName( this, "select" ) ) {
				var values = value.constructor == Array ?
					value :
					[ value ];

				jQuery( "option", this ).each(function(){
					this.selected = (jQuery.inArray( this.value, values ) >= 0 ||
						jQuery.inArray( this.text, values ) >= 0);
				});

				if ( !values.length )
					this.selectedIndex = -1;

			} else
				this.value = value;
		});
	},
	
	html: function( value ) {
		return value == undefined ?
			(this.length ?
				this[0].innerHTML :
				null) :
			this.empty().append( value );
	},

	replaceWith: function( value ) {
		return this.after( value ).remove();
	},

	eq: function( i ) {
		return this.slice( i, i + 1 );
	},

	slice: function() {
		return this.pushStack( Array.prototype.slice.apply( this, arguments ) );
	},

	map: function( callback ) {
		return this.pushStack( jQuery.map(this, function(elem, i){
			return callback.call( elem, i, elem );
		}));
	},

	andSelf: function() {
		return this.add( this.prevObject );
	},

	data: function( key, value ){
		var parts = key.split(".");
		parts[1] = parts[1] ? "." + parts[1] : "";

		if ( value == null ) {
			var data = this.triggerHandler("getData" + parts[1] + "!", [parts[0]]);
			
			if ( data == undefined && this.length )
				data = jQuery.data( this[0], key );

			return data == null && parts[1] ?
				this.data( parts[0] ) :
				data;
		} else
			return this.trigger("setData" + parts[1] + "!", [parts[0], value]).each(function(){
				jQuery.data( this, key, value );
			});
	},

	removeData: function( key ){
		return this.each(function(){
			jQuery.removeData( this, key );
		});
	},
	
	domManip: function( args, table, reverse, callback ) {
		var clone = this.length > 1, elems; 

		return this.each(function(){
			if ( !elems ) {
				elems = jQuery.clean( args, this.ownerDocument );

				if ( reverse )
					elems.reverse();
			}

			var obj = this;

			if ( table && jQuery.nodeName( this, "table" ) && jQuery.nodeName( elems[0], "tr" ) )
				obj = this.getElementsByTagName("tbody")[0] || this.appendChild( this.ownerDocument.createElement("tbody") );

			var scripts = jQuery( [] );

			jQuery.each(elems, function(){
				var elem = clone ?
					jQuery( this ).clone( true )[0] :
					this;

				// execute all scripts after the elements have been injected
				if ( jQuery.nodeName( elem, "script" ) ) {
					scripts = scripts.add( elem );
				} else {
					// Remove any inner scripts for later evaluation
					if ( elem.nodeType == 1 )
						scripts = scripts.add( jQuery( "script", elem ).remove() );

					// Inject the elements into the document
					callback.call( obj, elem );
				}
			});

			scripts.each( evalScript );
		});
	}
};

// Give the init function the jQuery prototype for later instantiation
jQuery.prototype.init.prototype = jQuery.prototype;

function evalScript( i, elem ) {
	if ( elem.src )
		jQuery.ajax({
			url: elem.src,
			async: false,
			dataType: "script"
		});

	else
		jQuery.globalEval( elem.text || elem.textContent || elem.innerHTML || "" );

	if ( elem.parentNode )
		elem.parentNode.removeChild( elem );
}

jQuery.extend = jQuery.fn.extend = function() {
	// copy reference to target object
	var target = arguments[0] || {}, i = 1, length = arguments.length, deep = false, options;

	// Handle a deep copy situation
	if ( target.constructor == Boolean ) {
		deep = target;
		target = arguments[1] || {};
		// skip the boolean and the target
		i = 2;
	}

	// Handle case when target is a string or something (possible in deep copy)
	if ( typeof target != "object" && typeof target != "function" )
		target = {};

	// extend jQuery itself if only one argument is passed
	if ( length == 1 ) {
		target = this;
		i = 0;
	}

	for ( ; i < length; i++ )
		// Only deal with non-null/undefined values
		if ( (options = arguments[ i ]) != null )
			// Extend the base object
			for ( var name in options ) {
				// Prevent never-ending loop
				if ( target === options[ name ] )
					continue;

				// Recurse if we're merging object values
				if ( deep && options[ name ] && typeof options[ name ] == "object" && target[ name ] && !options[ name ].nodeType )
					target[ name ] = jQuery.extend( target[ name ], options[ name ] );

				// Don't bring in undefined values
				else if ( options[ name ] != undefined )
					target[ name ] = options[ name ];

			}

	// Return the modified object
	return target;
};

var expando = "jQuery" + (new Date()).getTime(), uuid = 0, windowData = {};

// exclude the following css properties to add px
var exclude = /z-?index|font-?weight|opacity|zoom|line-?height/i;

jQuery.extend({
	noConflict: function( deep ) {
		window.$ = _$;

		if ( deep )
			window.jQuery = _jQuery;

		return jQuery;
	},

	// See test/unit/core.js for details concerning this function.
	isFunction: function( fn ) {
		return !!fn && typeof fn != "string" && !fn.nodeName && 
			fn.constructor != Array && /function/i.test( fn + "" );
	},
	
	// check if an element is in a (or is an) XML document
	isXMLDoc: function( elem ) {
		return elem.documentElement && !elem.body ||
			elem.tagName && elem.ownerDocument && !elem.ownerDocument.body;
	},

	// Evalulates a script in a global context
	globalEval: function( data ) {
		data = jQuery.trim( data );

		if ( data ) {
			// Inspired by code by Andrea Giammarchi
			// http://webreflection.blogspot.com/2007/08/global-scope-evaluation-and-dom.html
			var head = document.getElementsByTagName("head")[0] || document.documentElement,
				script = document.createElement("script");

			script.type = "text/javascript";
			if ( jQuery.browser.msie )
				script.text = data;
			else
				script.appendChild( document.createTextNode( data ) );

			head.appendChild( script );
			head.removeChild( script );
		}
	},

	nodeName: function( elem, name ) {
		return elem.nodeName && elem.nodeName.toUpperCase() == name.toUpperCase();
	},
	
	cache: {},
	
	data: function( elem, name, data ) {
		elem = elem == window ?
			windowData :
			elem;

		var id = elem[ expando ];

		// Compute a unique ID for the element
		if ( !id ) 
			id = elem[ expando ] = ++uuid;

		// Only generate the data cache if we're
		// trying to access or manipulate it
		if ( name && !jQuery.cache[ id ] )
			jQuery.cache[ id ] = {};
		
		// Prevent overriding the named cache with undefined values
		if ( data != undefined )
			jQuery.cache[ id ][ name ] = data;
		
		// Return the named cache data, or the ID for the element	
		return name ?
			jQuery.cache[ id ][ name ] :
			id;
	},
	
	removeData: function( elem, name ) {
		elem = elem == window ?
			windowData :
			elem;

		var id = elem[ expando ];

		// If we want to remove a specific section of the element's data
		if ( name ) {
			if ( jQuery.cache[ id ] ) {
				// Remove the section of cache data
				delete jQuery.cache[ id ][ name ];

				// If we've removed all the data, remove the element's cache
				name = "";

				for ( name in jQuery.cache[ id ] )
					break;

				if ( !name )
					jQuery.removeData( elem );
			}

		// Otherwise, we want to remove all of the element's data
		} else {
			// Clean up the element expando
			try {
				delete elem[ expando ];
			} catch(e){
				// IE has trouble directly removing the expando
				// but it's ok with using removeAttribute
				if ( elem.removeAttribute )
					elem.removeAttribute( expando );
			}

			// Completely remove the data cache
			delete jQuery.cache[ id ];
		}
	},

	// args is for internal usage only
	each: function( object, callback, args ) {
		if ( args ) {
			if ( object.length == undefined ) {
				for ( var name in object )
					if ( callback.apply( object[ name ], args ) === false )
						break;
			} else
				for ( var i = 0, length = object.length; i < length; i++ )
					if ( callback.apply( object[ i ], args ) === false )
						break;

		// A special, fast, case for the most common use of each
		} else {
			if ( object.length == undefined ) {
				for ( var name in object )
					if ( callback.call( object[ name ], name, object[ name ] ) === false )
						break;
			} else
				for ( var i = 0, length = object.length, value = object[0]; 
					i < length && callback.call( value, i, value ) !== false; value = object[++i] ){}
		}

		return object;
	},
	
	prop: function( elem, value, type, i, name ) {
			// Handle executable functions
			if ( jQuery.isFunction( value ) )
				value = value.call( elem, i );
				
			// Handle passing in a number to a CSS property
			return value && value.constructor == Number && type == "curCSS" && !exclude.test( name ) ?
				value + "px" :
				value;
	},

	className: {
		// internal only, use addClass("class")
		add: function( elem, classNames ) {
			jQuery.each((classNames || "").split(/\s+/), function(i, className){
				if ( elem.nodeType == 1 && !jQuery.className.has( elem.className, className ) )
					elem.className += (elem.className ? " " : "") + className;
			});
		},

		// internal only, use removeClass("class")
		remove: function( elem, classNames ) {
			if (elem.nodeType == 1)
				elem.className = classNames != undefined ?
					jQuery.grep(elem.className.split(/\s+/), function(className){
						return !jQuery.className.has( classNames, className );	
					}).join(" ") :
					"";
		},

		// internal only, use is(".class")
		has: function( elem, className ) {
			return jQuery.inArray( className, (elem.className || elem).toString().split(/\s+/) ) > -1;
		}
	},

	// A method for quickly swapping in/out CSS properties to get correct calculations
	swap: function( elem, options, callback ) {
		var old = {};
		// Remember the old values, and insert the new ones
		for ( var name in options ) {
			old[ name ] = elem.style[ name ];
			elem.style[ name ] = options[ name ];
		}

		callback.call( elem );

		// Revert the old values
		for ( var name in options )
			elem.style[ name ] = old[ name ];
	},

	css: function( elem, name, force ) {
		if ( name == "width" || name == "height" ) {
			var val, props = { position: "absolute", visibility: "hidden", display:"block" }, which = name == "width" ? [ "Left", "Right" ] : [ "Top", "Bottom" ];
		
			function getWH() {
				val = name == "width" ? elem.offsetWidth : elem.offsetHeight;
				var padding = 0, border = 0;
				jQuery.each( which, function() {
					padding += parseFloat(jQuery.curCSS( elem, "padding" + this, true)) || 0;
					border += parseFloat(jQuery.curCSS( elem, "border" + this + "Width", true)) || 0;
				});
				val -= Math.round(padding + border);
			}
		
			if ( jQuery(elem).is(":visible") )
				getWH();
			else
				jQuery.swap( elem, props, getWH );
			
			return Math.max(0, val);
		}
		
		return jQuery.curCSS( elem, name, force );
	},

	curCSS: function( elem, name, force ) {
		var ret;

		// A helper method for determining if an element's values are broken
		function color( elem ) {
			if ( !jQuery.browser.safari )
				return false;

			var ret = document.defaultView.getComputedStyle( elem, null );
			return !ret || ret.getPropertyValue("color") == "";
		}

		// We need to handle opacity special in IE
		if ( name == "opacity" && jQuery.browser.msie ) {
			ret = jQuery.attr( elem.style, "opacity" );

			return ret == "" ?
				"1" :
				ret;
		}
		// Opera sometimes will give the wrong display answer, this fixes it, see #2037
		if ( jQuery.browser.opera && name == "display" ) {
			var save = elem.style.outline;
			elem.style.outline = "0 solid black";
			elem.style.outline = save;
		}
		
		// Make sure we're using the right name for getting the float value
		if ( name.match( /float/i ) )
			name = styleFloat;

		if ( !force && elem.style && elem.style[ name ] )
			ret = elem.style[ name ];

		else if ( document.defaultView && document.defaultView.getComputedStyle ) {

			// Only "float" is needed here
			if ( name.match( /float/i ) )
				name = "float";

			name = name.replace( /([A-Z])/g, "-$1" ).toLowerCase();

			var getComputedStyle = document.defaultView.getComputedStyle( elem, null );

			if ( getComputedStyle && !color( elem ) )
				ret = getComputedStyle.getPropertyValue( name );

			// If the element isn't reporting its values properly in Safari
			// then some display: none elements are involved
			else {
				var swap = [], stack = [];

				// Locate all of the parent display: none elements
				for ( var a = elem; a && color(a); a = a.parentNode )
					stack.unshift(a);

				// Go through and make them visible, but in reverse
				// (It would be better if we knew the exact display type that they had)
				for ( var i = 0; i < stack.length; i++ )
					if ( color( stack[ i ] ) ) {
						swap[ i ] = stack[ i ].style.display;
						stack[ i ].style.display = "block";
					}

				// Since we flip the display style, we have to handle that
				// one special, otherwise get the value
				ret = name == "display" && swap[ stack.length - 1 ] != null ?
					"none" :
					( getComputedStyle && getComputedStyle.getPropertyValue( name ) ) || "";

				// Finally, revert the display styles back
				for ( var i = 0; i < swap.length; i++ )
					if ( swap[ i ] != null )
						stack[ i ].style.display = swap[ i ];
			}

			// We should always get a number back from opacity
			if ( name == "opacity" && ret == "" )
				ret = "1";

		} else if ( elem.currentStyle ) {
			var camelCase = name.replace(/\-(\w)/g, function(all, letter){
				return letter.toUpperCase();
			});

			ret = elem.currentStyle[ name ] || elem.currentStyle[ camelCase ];

			// From the awesome hack by Dean Edwards
			// http://erik.eae.net/archives/2007/07/27/18.54.15/#comment-102291

			// If we're not dealing with a regular pixel number
			// but a number that has a weird ending, we need to convert it to pixels
			if ( !/^\d+(px)?$/i.test( ret ) && /^\d/.test( ret ) ) {
				// Remember the original values
				var style = elem.style.left, runtimeStyle = elem.runtimeStyle.left;

				// Put in the new values to get a computed value out
				elem.runtimeStyle.left = elem.currentStyle.left;
				elem.style.left = ret || 0;
				ret = elem.style.pixelLeft + "px";

				// Revert the changed values
				elem.style.left = style;
				elem.runtimeStyle.left = runtimeStyle;
			}
		}

		return ret;
	},
	
	clean: function( elems, context ) {
		var ret = [];
		context = context || document;
		// !context.createElement fails in IE with an error but returns typeof 'object'
		if (typeof context.createElement == 'undefined') 
			context = context.ownerDocument || context[0] && context[0].ownerDocument || document;

		jQuery.each(elems, function(i, elem){
			if ( !elem )
				return;

			if ( elem.constructor == Number )
				elem = elem.toString();
			
			// Convert html string into DOM nodes
			if ( typeof elem == "string" ) {
				// Fix "XHTML"-style tags in all browsers
				elem = elem.replace(/(<(\w+)[^>]*?)\/>/g, function(all, front, tag){
					return tag.match(/^(abbr|br|col|img|input|link|meta|param|hr|area|embed)$/i) ?
						all :
						front + "></" + tag + ">";
				});

				// Trim whitespace, otherwise indexOf won't work as expected
				var tags = jQuery.trim( elem ).toLowerCase(), div = context.createElement("div");

				var wrap =
					// option or optgroup
					!tags.indexOf("<opt") &&
					[ 1, "<select multiple='multiple'>", "</select>" ] ||
					
					!tags.indexOf("<leg") &&
					[ 1, "<fieldset>", "</fieldset>" ] ||
					
					tags.match(/^<(thead|tbody|tfoot|colg|cap)/) &&
					[ 1, "<table>", "</table>" ] ||
					
					!tags.indexOf("<tr") &&
					[ 2, "<table><tbody>", "</tbody></table>" ] ||
					
				 	// <thead> matched above
					(!tags.indexOf("<td") || !tags.indexOf("<th")) &&
					[ 3, "<table><tbody><tr>", "</tr></tbody></table>" ] ||
					
					!tags.indexOf("<col") &&
					[ 2, "<table><tbody></tbody><colgroup>", "</colgroup></table>" ] ||

					// IE can't serialize <link> and <script> tags normally
					jQuery.browser.msie &&
					[ 1, "div<div>", "</div>" ] ||
					
					[ 0, "", "" ];

				// Go to html and back, then peel off extra wrappers
				div.innerHTML = wrap[1] + elem + wrap[2];
				
				// Move to the right depth
				while ( wrap[0]-- )
					div = div.lastChild;
				
				// Remove IE's autoinserted <tbody> from table fragments
				if ( jQuery.browser.msie ) {
					
					// String was a <table>, *may* have spurious <tbody>
					var tbody = !tags.indexOf("<table") && tags.indexOf("<tbody") < 0 ?
						div.firstChild && div.firstChild.childNodes :
						
						// String was a bare <thead> or <tfoot>
						wrap[1] == "<table>" && tags.indexOf("<tbody") < 0 ?
							div.childNodes :
							[];
				
					for ( var j = tbody.length - 1; j >= 0 ; --j )
						if ( jQuery.nodeName( tbody[ j ], "tbody" ) && !tbody[ j ].childNodes.length )
							tbody[ j ].parentNode.removeChild( tbody[ j ] );
					
					// IE completely kills leading whitespace when innerHTML is used	
					if ( /^\s/.test( elem ) )	
						div.insertBefore( context.createTextNode( elem.match(/^\s*/)[0] ), div.firstChild );
				
				}
				
				elem = jQuery.makeArray( div.childNodes );
			}

			if ( elem.length === 0 && (!jQuery.nodeName( elem, "form" ) && !jQuery.nodeName( elem, "select" )) )
				return;

			if ( elem[0] == undefined || jQuery.nodeName( elem, "form" ) || elem.options )
				ret.push( elem );

			else
				ret = jQuery.merge( ret, elem );

		});

		return ret;
	},
	
	attr: function( elem, name, value ) {
		// don't set attributes on text and comment nodes
		if (!elem || elem.nodeType == 3 || elem.nodeType == 8)
			return undefined;

		var fix = jQuery.isXMLDoc( elem ) ?
			{} :
			jQuery.props;

		// Safari mis-reports the default selected property of a hidden option
		// Accessing the parent's selectedIndex property fixes it
		if ( name == "selected" && jQuery.browser.safari )
			elem.parentNode.selectedIndex;
		
		// Certain attributes only work when accessed via the old DOM 0 way
		if ( fix[ name ] ) {
			if ( value != undefined )
				elem[ fix[ name ] ] = value;

			return elem[ fix[ name ] ];

		} else if ( jQuery.browser.msie && name == "style" )
			return jQuery.attr( elem.style, "cssText", value );

		else if ( value == undefined && jQuery.browser.msie && jQuery.nodeName( elem, "form" ) && (name == "action" || name == "method") )
			return elem.getAttributeNode( name ).nodeValue;

		// IE elem.getAttribute passes even for style
		else if ( elem.tagName ) {

			if ( value != undefined ) {
				// We can't allow the type property to be changed (since it causes problems in IE)
				if ( name == "type" && jQuery.nodeName( elem, "input" ) && elem.parentNode )
					throw "type property can't be changed";

				// convert the value to a string (all browsers do this but IE) see #1070
				elem.setAttribute( name, "" + value );
			}

			if ( jQuery.browser.msie && /href|src/.test( name ) && !jQuery.isXMLDoc( elem ) ) 
				return elem.getAttribute( name, 2 );

			return elem.getAttribute( name );

		// elem is actually elem.style ... set the style
		} else {
			// IE actually uses filters for opacity
			if ( name == "opacity" && jQuery.browser.msie ) {
				if ( value != undefined ) {
					// IE has trouble with opacity if it does not have layout
					// Force it by setting the zoom level
					elem.zoom = 1; 
	
					// Set the alpha filter to set the opacity
					elem.filter = (elem.filter || "").replace( /alpha\([^)]*\)/, "" ) +
						(parseFloat( value ).toString() == "NaN" ? "" : "alpha(opacity=" + value * 100 + ")");
				}
	
				return elem.filter && elem.filter.indexOf("opacity=") >= 0 ?
					(parseFloat( elem.filter.match(/opacity=([^)]*)/)[1] ) / 100).toString() :
					"";
			}

			name = name.replace(/-([a-z])/ig, function(all, letter){
				return letter.toUpperCase();
			});

			if ( value != undefined )
				elem[ name ] = value;

			return elem[ name ];
		}
	},
	
	trim: function( text ) {
		return (text || "").replace( /^\s+|\s+$/g, "" );
	},

	makeArray: function( array ) {
		var ret = [];

		// Need to use typeof to fight Safari childNodes crashes
		if ( typeof array != "array" )
			for ( var i = 0, length = array.length; i < length; i++ )
				ret.push( array[ i ] );
		else
			ret = array.slice( 0 );

		return ret;
	},

	inArray: function( elem, array ) {
		for ( var i = 0, length = array.length; i < length; i++ )
			if ( array[ i ] == elem )
				return i;

		return -1;
	},

	merge: function( first, second ) {
		// We have to loop this way because IE & Opera overwrite the length
		// expando of getElementsByTagName

		// Also, we need to make sure that the correct elements are being returned
		// (IE returns comment nodes in a '*' query)
		if ( jQuery.browser.msie ) {
			for ( var i = 0; second[ i ]; i++ )
				if ( second[ i ].nodeType != 8 )
					first.push( second[ i ] );

		} else
			for ( var i = 0; second[ i ]; i++ )
				first.push( second[ i ] );

		return first;
	},

	unique: function( array ) {
		var ret = [], done = {};

		try {

			for ( var i = 0, length = array.length; i < length; i++ ) {
				var id = jQuery.data( array[ i ] );

				if ( !done[ id ] ) {
					done[ id ] = true;
					ret.push( array[ i ] );
				}
			}

		} catch( e ) {
			ret = array;
		}

		return ret;
	},

	grep: function( elems, callback, inv ) {
		var ret = [];

		// Go through the array, only saving the items
		// that pass the validator function
		for ( var i = 0, length = elems.length; i < length; i++ )
			if ( !inv && callback( elems[ i ], i ) || inv && !callback( elems[ i ], i ) )
				ret.push( elems[ i ] );

		return ret;
	},

	map: function( elems, callback ) {
		var ret = [];

		// Go through the array, translating each of the items to their
		// new value (or values).
		for ( var i = 0, length = elems.length; i < length; i++ ) {
			var value = callback( elems[ i ], i );

			if ( value !== null && value != undefined ) {
				if ( value.constructor != Array )
					value = [ value ];

				ret = ret.concat( value );
			}
		}

		return ret;
	}
});

var userAgent = navigator.userAgent.toLowerCase();

// Figure out what browser is being used
jQuery.browser = {
	version: (userAgent.match( /.+(?:rv|it|ra|ie)[\/: ]([\d.]+)/ ) || [])[1],
	safari: /webkit/.test( userAgent ),
	opera: /opera/.test( userAgent ),
	msie: /msie/.test( userAgent ) && !/opera/.test( userAgent ),
	mozilla: /mozilla/.test( userAgent ) && !/(compatible|webkit)/.test( userAgent )
};

var styleFloat = jQuery.browser.msie ?
	"styleFloat" :
	"cssFloat";
	
jQuery.extend({
	// Check to see if the W3C box model is being used
	boxModel: !jQuery.browser.msie || document.compatMode == "CSS1Compat",
	
	props: {
		"for": "htmlFor",
		"class": "className",
		"float": styleFloat,
		cssFloat: styleFloat,
		styleFloat: styleFloat,
		innerHTML: "innerHTML",
		className: "className",
		value: "value",
		disabled: "disabled",
		checked: "checked",
		readonly: "readOnly",
		selected: "selected",
		maxlength: "maxLength",
		selectedIndex: "selectedIndex",
		defaultValue: "defaultValue",
		tagName: "tagName",
		nodeName: "nodeName"
	}
});

jQuery.each({
	parent: function(elem){return elem.parentNode;},
	parents: function(elem){return jQuery.dir(elem,"parentNode");},
	next: function(elem){return jQuery.nth(elem,2,"nextSibling");},
	prev: function(elem){return jQuery.nth(elem,2,"previousSibling");},
	nextAll: function(elem){return jQuery.dir(elem,"nextSibling");},
	prevAll: function(elem){return jQuery.dir(elem,"previousSibling");},
	siblings: function(elem){return jQuery.sibling(elem.parentNode.firstChild,elem);},
	children: function(elem){return jQuery.sibling(elem.firstChild);},
	contents: function(elem){return jQuery.nodeName(elem,"iframe")?elem.contentDocument||elem.contentWindow.document:jQuery.makeArray(elem.childNodes);}
}, function(name, fn){
	jQuery.fn[ name ] = function( selector ) {
		var ret = jQuery.map( this, fn );

		if ( selector && typeof selector == "string" )
			ret = jQuery.multiFilter( selector, ret );

		return this.pushStack( jQuery.unique( ret ) );
	};
});

jQuery.each({
	appendTo: "append",
	prependTo: "prepend",
	insertBefore: "before",
	insertAfter: "after",
	replaceAll: "replaceWith"
}, function(name, original){
	jQuery.fn[ name ] = function() {
		var args = arguments;

		return this.each(function(){
			for ( var i = 0, length = args.length; i < length; i++ )
				jQuery( args[ i ] )[ original ]( this );
		});
	};
});

jQuery.each({
	removeAttr: function( name ) {
		jQuery.attr( this, name, "" );
		if (this.nodeType == 1) 
			this.removeAttribute( name );
	},

	addClass: function( classNames ) {
		jQuery.className.add( this, classNames );
	},

	removeClass: function( classNames ) {
		jQuery.className.remove( this, classNames );
	},

	toggleClass: function( classNames ) {
		jQuery.className[ jQuery.className.has( this, classNames ) ? "remove" : "add" ]( this, classNames );
	},

	remove: function( selector ) {
		if ( !selector || jQuery.filter( selector, [ this ] ).r.length ) {
			// Prevent memory leaks
			jQuery( "*", this ).add(this).each(function(){
				jQuery.event.remove(this);
				jQuery.removeData(this);
			});
			if (this.parentNode)
				this.parentNode.removeChild( this );
		}
	},

	empty: function() {
		// Remove element nodes and prevent memory leaks
		jQuery( ">*", this ).remove();
		
		// Remove any remaining nodes
		while ( this.firstChild )
			this.removeChild( this.firstChild );
	}
}, function(name, fn){
	jQuery.fn[ name ] = function(){
		return this.each( fn, arguments );
	};
});

jQuery.each([ "Height", "Width" ], function(i, name){
	var type = name.toLowerCase();
	
	jQuery.fn[ type ] = function( size ) {
		// Get window width or height
		return this[0] == window ?
			// Opera reports document.body.client[Width/Height] properly in both quirks and standards
			jQuery.browser.opera && document.body[ "client" + name ] || 
			
			// Safari reports inner[Width/Height] just fine (Mozilla and Opera include scroll bar widths)
			jQuery.browser.safari && window[ "inner" + name ] ||
			
			// Everyone else use document.documentElement or document.body depending on Quirks vs Standards mode
			document.compatMode == "CSS1Compat" && document.documentElement[ "client" + name ] || document.body[ "client" + name ] :
		
			// Get document width or height
			this[0] == document ?
				// Either scroll[Width/Height] or offset[Width/Height], whichever is greater
				Math.max( 
					Math.max(document.body["scroll" + name], document.documentElement["scroll" + name]), 
					Math.max(document.body["offset" + name], document.documentElement["offset" + name]) 
				) :

				// Get or set width or height on the element
				size == undefined ?
					// Get width or height on the element
					(this.length ? jQuery.css( this[0], type ) : null) :

					// Set the width or height on the element (default to pixels if value is unitless)
					this.css( type, size.constructor == String ? size : size + "px" );
	};
});

var chars = jQuery.browser.safari && parseInt(jQuery.browser.version) < 417 ?
		"(?:[\\w*_-]|\\\\.)" :
		"(?:[\\w\u0128-\uFFFF*_-]|\\\\.)",
	quickChild = new RegExp("^>\\s*(" + chars + "+)"),
	quickID = new RegExp("^(" + chars + "+)(#)(" + chars + "+)"),
	quickClass = new RegExp("^([#.]?)(" + chars + "*)");

jQuery.extend({
	expr: {
		"": function(a,i,m){return m[2]=="*"||jQuery.nodeName(a,m[2]);},
		"#": function(a,i,m){return a.getAttribute("id")==m[2];},
		":": {
			// Position Checks
			lt: function(a,i,m){return i<m[3]-0;},
			gt: function(a,i,m){return i>m[3]-0;},
			nth: function(a,i,m){return m[3]-0==i;},
			eq: function(a,i,m){return m[3]-0==i;},
			first: function(a,i){return i==0;},
			last: function(a,i,m,r){return i==r.length-1;},
			even: function(a,i){return i%2==0;},
			odd: function(a,i){return i%2;},

			// Child Checks
			"first-child": function(a){return a.parentNode.getElementsByTagName("*")[0]==a;},
			"last-child": function(a){return jQuery.nth(a.parentNode.lastChild,1,"previousSibling")==a;},
			"only-child": function(a){return !jQuery.nth(a.parentNode.lastChild,2,"previousSibling");},

			// Parent Checks
			parent: function(a){return a.firstChild;},
			empty: function(a){return !a.firstChild;},

			// Text Check
			contains: function(a,i,m){return (a.textContent||a.innerText||jQuery(a).text()||"").indexOf(m[3])>=0;},

			// Visibility
			visible: function(a){return "hidden"!=a.type&&jQuery.css(a,"display")!="none"&&jQuery.css(a,"visibility")!="hidden";},
			hidden: function(a){return "hidden"==a.type||jQuery.css(a,"display")=="none"||jQuery.css(a,"visibility")=="hidden";},

			// Form attributes
			enabled: function(a){return !a.disabled;},
			disabled: function(a){return a.disabled;},
			checked: function(a){return a.checked;},
			selected: function(a){return a.selected||jQuery.attr(a,"selected");},

			// Form elements
			text: function(a){return "text"==a.type;},
			radio: function(a){return "radio"==a.type;},
			checkbox: function(a){return "checkbox"==a.type;},
			file: function(a){return "file"==a.type;},
			password: function(a){return "password"==a.type;},
			submit: function(a){return "submit"==a.type;},
			image: function(a){return "image"==a.type;},
			reset: function(a){return "reset"==a.type;},
			button: function(a){return "button"==a.type||jQuery.nodeName(a,"button");},
			input: function(a){return /input|select|textarea|button/i.test(a.nodeName);},

			// :has()
			has: function(a,i,m){return jQuery.find(m[3],a).length;},

			// :header
			header: function(a){return /h\d/i.test(a.nodeName);},

			// :animated
			animated: function(a){return jQuery.grep(jQuery.timers,function(fn){return a==fn.elem;}).length;}
		}
	},
	
	// The regular expressions that power the parsing engine
	parse: [
		// Match: [@value='test'], [@foo]
		/^(\[) *@?([\w-]+) *([!*$^~=]*) *('?"?)(.*?)\4 *\]/,

		// Match: :contains('foo')
		/^(:)([\w-]+)\("?'?(.*?(\(.*?\))?[^(]*?)"?'?\)/,

		// Match: :even, :last-chlid, #id, .class
		new RegExp("^([:.#]*)(" + chars + "+)")
	],

	multiFilter: function( expr, elems, not ) {
		var old, cur = [];

		while ( expr && expr != old ) {
			old = expr;
			var f = jQuery.filter( expr, elems, not );
			expr = f.t.replace(/^\s*,\s*/, "" );
			cur = not ? elems = f.r : jQuery.merge( cur, f.r );
		}

		return cur;
	},

	find: function( t, context ) {
		// Quickly handle non-string expressions
		if ( typeof t != "string" )
			return [ t ];

		// check to make sure context is a DOM element or a document
		if ( context && context.nodeType != 1 && context.nodeType != 9)
			return [ ];

		// Set the correct context (if none is provided)
		context = context || document;

		// Initialize the search
		var ret = [context], done = [], last, nodeName;

		// Continue while a selector expression exists, and while
		// we're no longer looping upon ourselves
		while ( t && last != t ) {
			var r = [];
			last = t;

			t = jQuery.trim(t);

			var foundToken = false;

			// An attempt at speeding up child selectors that
			// point to a specific element tag
			var re = quickChild;
			var m = re.exec(t);

			if ( m ) {
				nodeName = m[1].toUpperCase();

				// Perform our own iteration and filter
				for ( var i = 0; ret[i]; i++ )
					for ( var c = ret[i].firstChild; c; c = c.nextSibling )
						if ( c.nodeType == 1 && (nodeName == "*" || c.nodeName.toUpperCase() == nodeName) )
							r.push( c );

				ret = r;
				t = t.replace( re, "" );
				if ( t.indexOf(" ") == 0 ) continue;
				foundToken = true;
			} else {
				re = /^([>+~])\s*(\w*)/i;

				if ( (m = re.exec(t)) != null ) {
					r = [];

					var merge = {};
					nodeName = m[2].toUpperCase();
					m = m[1];

					for ( var j = 0, rl = ret.length; j < rl; j++ ) {
						var n = m == "~" || m == "+" ? ret[j].nextSibling : ret[j].firstChild;
						for ( ; n; n = n.nextSibling )
							if ( n.nodeType == 1 ) {
								var id = jQuery.data(n);

								if ( m == "~" && merge[id] ) break;
								
								if (!nodeName || n.nodeName.toUpperCase() == nodeName ) {
									if ( m == "~" ) merge[id] = true;
									r.push( n );
								}
								
								if ( m == "+" ) break;
							}
					}

					ret = r;

					// And remove the token
					t = jQuery.trim( t.replace( re, "" ) );
					foundToken = true;
				}
			}

			// See if there's still an expression, and that we haven't already
			// matched a token
			if ( t && !foundToken ) {
				// Handle multiple expressions
				if ( !t.indexOf(",") ) {
					// Clean the result set
					if ( context == ret[0] ) ret.shift();

					// Merge the result sets
					done = jQuery.merge( done, ret );

					// Reset the context
					r = ret = [context];

					// Touch up the selector string
					t = " " + t.substr(1,t.length);

				} else {
					// Optimize for the case nodeName#idName
					var re2 = quickID;
					var m = re2.exec(t);
					
					// Re-organize the results, so that they're consistent
					if ( m ) {
						m = [ 0, m[2], m[3], m[1] ];

					} else {
						// Otherwise, do a traditional filter check for
						// ID, class, and element selectors
						re2 = quickClass;
						m = re2.exec(t);
					}

					m[2] = m[2].replace(/\\/g, "");

					var elem = ret[ret.length-1];

					// Try to do a global search by ID, where we can
					if ( m[1] == "#" && elem && elem.getElementById && !jQuery.isXMLDoc(elem) ) {
						// Optimization for HTML document case
						var oid = elem.getElementById(m[2]);
						
						// Do a quick check for the existence of the actual ID attribute
						// to avoid selecting by the name attribute in IE
						// also check to insure id is a string to avoid selecting an element with the name of 'id' inside a form
						if ( (jQuery.browser.msie||jQuery.browser.opera) && oid && typeof oid.id == "string" && oid.id != m[2] )
							oid = jQuery('[@id="'+m[2]+'"]', elem)[0];

						// Do a quick check for node name (where applicable) so
						// that div#foo searches will be really fast
						ret = r = oid && (!m[3] || jQuery.nodeName(oid, m[3])) ? [oid] : [];
					} else {
						// We need to find all descendant elements
						for ( var i = 0; ret[i]; i++ ) {
							// Grab the tag name being searched for
							var tag = m[1] == "#" && m[3] ? m[3] : m[1] != "" || m[0] == "" ? "*" : m[2];

							// Handle IE7 being really dumb about <object>s
							if ( tag == "*" && ret[i].nodeName.toLowerCase() == "object" )
								tag = "param";

							r = jQuery.merge( r, ret[i].getElementsByTagName( tag ));
						}

						// It's faster to filter by class and be done with it
						if ( m[1] == "." )
							r = jQuery.classFilter( r, m[2] );

						// Same with ID filtering
						if ( m[1] == "#" ) {
							var tmp = [];

							// Try to find the element with the ID
							for ( var i = 0; r[i]; i++ )
								if ( r[i].getAttribute("id") == m[2] ) {
									tmp = [ r[i] ];
									break;
								}

							r = tmp;
						}

						ret = r;
					}

					t = t.replace( re2, "" );
				}

			}

			// If a selector string still exists
			if ( t ) {
				// Attempt to filter it
				var val = jQuery.filter(t,r);
				ret = r = val.r;
				t = jQuery.trim(val.t);
			}
		}

		// An error occurred with the selector;
		// just return an empty set instead
		if ( t )
			ret = [];

		// Remove the root context
		if ( ret && context == ret[0] )
			ret.shift();

		// And combine the results
		done = jQuery.merge( done, ret );

		return done;
	},

	classFilter: function(r,m,not){
		m = " " + m + " ";
		var tmp = [];
		for ( var i = 0; r[i]; i++ ) {
			var pass = (" " + r[i].className + " ").indexOf( m ) >= 0;
			if ( !not && pass || not && !pass )
				tmp.push( r[i] );
		}
		return tmp;
	},

	filter: function(t,r,not) {
		var last;

		// Look for common filter expressions
		while ( t && t != last ) {
			last = t;

			var p = jQuery.parse, m;

			for ( var i = 0; p[i]; i++ ) {
				m = p[i].exec( t );

				if ( m ) {
					// Remove what we just matched
					t = t.substring( m[0].length );

					m[2] = m[2].replace(/\\/g, "");
					break;
				}
			}

			if ( !m )
				break;

			// :not() is a special case that can be optimized by
			// keeping it out of the expression list
			if ( m[1] == ":" && m[2] == "not" )
				// optimize if only one selector found (most common case)
				r = isSimple.test( m[3] ) ?
					jQuery.filter(m[3], r, true).r :
					jQuery( r ).not( m[3] );

			// We can get a big speed boost by filtering by class here
			else if ( m[1] == "." )
				r = jQuery.classFilter(r, m[2], not);

			else if ( m[1] == "[" ) {
				var tmp = [], type = m[3];
				
				for ( var i = 0, rl = r.length; i < rl; i++ ) {
					var a = r[i], z = a[ jQuery.props[m[2]] || m[2] ];
					
					if ( z == null || /href|src|selected/.test(m[2]) )
						z = jQuery.attr(a,m[2]) || '';

					if ( (type == "" && !!z ||
						 type == "=" && z == m[5] ||
						 type == "!=" && z != m[5] ||
						 type == "^=" && z && !z.indexOf(m[5]) ||
						 type == "$=" && z.substr(z.length - m[5].length) == m[5] ||
						 (type == "*=" || type == "~=") && z.indexOf(m[5]) >= 0) ^ not )
							tmp.push( a );
				}
				
				r = tmp;

			// We can get a speed boost by handling nth-child here
			} else if ( m[1] == ":" && m[2] == "nth-child" ) {
				var merge = {}, tmp = [],
					// parse equations like 'even', 'odd', '5', '2n', '3n+2', '4n-1', '-n+6'
					test = /(-?)(\d*)n((?:\+|-)?\d*)/.exec(
						m[3] == "even" && "2n" || m[3] == "odd" && "2n+1" ||
						!/\D/.test(m[3]) && "0n+" + m[3] || m[3]),
					// calculate the numbers (first)n+(last) including if they are negative
					first = (test[1] + (test[2] || 1)) - 0, last = test[3] - 0;
 
				// loop through all the elements left in the jQuery object
				for ( var i = 0, rl = r.length; i < rl; i++ ) {
					var node = r[i], parentNode = node.parentNode, id = jQuery.data(parentNode);

					if ( !merge[id] ) {
						var c = 1;

						for ( var n = parentNode.firstChild; n; n = n.nextSibling )
							if ( n.nodeType == 1 )
								n.nodeIndex = c++;

						merge[id] = true;
					}

					var add = false;

					if ( first == 0 ) {
						if ( node.nodeIndex == last )
							add = true;
					} else if ( (node.nodeIndex - last) % first == 0 && (node.nodeIndex - last) / first >= 0 )
						add = true;

					if ( add ^ not )
						tmp.push( node );
				}

				r = tmp;

			// Otherwise, find the expression to execute
			} else {
				var fn = jQuery.expr[ m[1] ];
				if ( typeof fn == "object" )
					fn = fn[ m[2] ];

				if ( typeof fn == "string" )
					fn = eval("false||function(a,i){return " + fn + ";}");

				// Execute it against the current filter
				r = jQuery.grep( r, function(elem, i){
					return fn(elem, i, m, r);
				}, not );
			}
		}

		// Return an array of filtered elements (r)
		// and the modified expression string (t)
		return { r: r, t: t };
	},

	dir: function( elem, dir ){
		var matched = [];
		var cur = elem[dir];
		while ( cur && cur != document ) {
			if ( cur.nodeType == 1 )
				matched.push( cur );
			cur = cur[dir];
		}
		return matched;
	},
	
	nth: function(cur,result,dir,elem){
		result = result || 1;
		var num = 0;

		for ( ; cur; cur = cur[dir] )
			if ( cur.nodeType == 1 && ++num == result )
				break;

		return cur;
	},
	
	sibling: function( n, elem ) {
		var r = [];

		for ( ; n; n = n.nextSibling ) {
			if ( n.nodeType == 1 && (!elem || n != elem) )
				r.push( n );
		}

		return r;
	}
});

/*
 * A number of helper functions used for managing events.
 * Many of the ideas behind this code orignated from 
 * Dean Edwards' addEvent library.
 */
jQuery.event = {

	// Bind an event to an element
	// Original by Dean Edwards
	add: function(elem, types, handler, data) {
		if ( elem.nodeType == 3 || elem.nodeType == 8 )
			return;

		// For whatever reason, IE has trouble passing the window object
		// around, causing it to be cloned in the process
		if ( jQuery.browser.msie && elem.setInterval != undefined )
			elem = window;

		// Make sure that the function being executed has a unique ID
		if ( !handler.guid )
			handler.guid = this.guid++;
			
		// if data is passed, bind to handler 
		if( data != undefined ) { 
			// Create temporary function pointer to original handler 
			var fn = handler; 

			// Create unique handler function, wrapped around original handler 
			handler = function() { 
				// Pass arguments and context to original handler 
				return fn.apply(this, arguments); 
			};

			// Store data in unique handler 
			handler.data = data;

			// Set the guid of unique handler to the same of original handler, so it can be removed 
			handler.guid = fn.guid;
		}

		// Init the element's event structure
		var events = jQuery.data(elem, "events") || jQuery.data(elem, "events", {}),
			handle = jQuery.data(elem, "handle") || jQuery.data(elem, "handle", function(){
				// returned undefined or false
				var val;

				// Handle the second event of a trigger and when
				// an event is called after a page has unloaded
				if ( typeof jQuery == "undefined" || jQuery.event.triggered )
					return val;
		
				val = jQuery.event.handle.apply(arguments.callee.elem, arguments);
		
				return val;
			});
		// Add elem as a property of the handle function
		// This is to prevent a memory leak with non-native
		// event in IE.
		handle.elem = elem;
			
			// Handle multiple events seperated by a space
			// jQuery(...).bind("mouseover mouseout", fn);
			jQuery.each(types.split(/\s+/), function(index, type) {
				// Namespaced event handlers
				var parts = type.split(".");
				type = parts[0];
				handler.type = parts[1];

				// Get the current list of functions bound to this event
				var handlers = events[type];

				// Init the event handler queue
				if (!handlers) {
					handlers = events[type] = {};
		
					// Check for a special event handler
					// Only use addEventListener/attachEvent if the special
					// events handler returns false
					if ( !jQuery.event.special[type] || jQuery.event.special[type].setup.call(elem) === false ) {
						// Bind the global event handler to the element
						if (elem.addEventListener)
							elem.addEventListener(type, handle, false);
						else if (elem.attachEvent)
							elem.attachEvent("on" + type, handle);
					}
				}

				// Add the function to the element's handler list
				handlers[handler.guid] = handler;

				// Keep track of which events have been used, for global triggering
				jQuery.event.global[type] = true;
			});
		
		// Nullify elem to prevent memory leaks in IE
		elem = null;
	},

	guid: 1,
	global: {},

	// Detach an event or set of events from an element
	remove: function(elem, types, handler) {
		// don't do events on text and comment nodes
		if ( elem.nodeType == 3 || elem.nodeType == 8 )
			return;

		var events = jQuery.data(elem, "events"), ret, index;

		if ( events ) {
			// Unbind all events for the element
			if ( types == undefined || (typeof types == "string" && types.charAt(0) == ".") )
				for ( var type in events )
					this.remove( elem, type + (types || "") );
			else {
				// types is actually an event object here
				if ( types.type ) {
					handler = types.handler;
					types = types.type;
				}
				
				// Handle multiple events seperated by a space
				// jQuery(...).unbind("mouseover mouseout", fn);
				jQuery.each(types.split(/\s+/), function(index, type){
					// Namespaced event handlers
					var parts = type.split(".");
					type = parts[0];
					
					if ( events[type] ) {
						// remove the given handler for the given type
						if ( handler )
							delete events[type][handler.guid];
			
						// remove all handlers for the given type
						else
							for ( handler in events[type] )
								// Handle the removal of namespaced events
								if ( !parts[1] || events[type][handler].type == parts[1] )
									delete events[type][handler];

						// remove generic event handler if no more handlers exist
						for ( ret in events[type] ) break;
						if ( !ret ) {
							if ( !jQuery.event.special[type] || jQuery.event.special[type].teardown.call(elem) === false ) {
								if (elem.removeEventListener)
									elem.removeEventListener(type, jQuery.data(elem, "handle"), false);
								else if (elem.detachEvent)
									elem.detachEvent("on" + type, jQuery.data(elem, "handle"));
							}
							ret = null;
							delete events[type];
						}
					}
				});
			}

			// Remove the expando if it's no longer used
			for ( ret in events ) break;
			if ( !ret ) {
				var handle = jQuery.data( elem, "handle" );
				if ( handle ) handle.elem = null;
				jQuery.removeData( elem, "events" );
				jQuery.removeData( elem, "handle" );
			}
		}
	},

	trigger: function(type, data, elem, donative, extra) {
		// Clone the incoming data, if any
		data = jQuery.makeArray(data || []);

		if ( type.indexOf("!") >= 0 ) {
			type = type.slice(0, -1);
			var exclusive = true;
		}

		// Handle a global trigger
		if ( !elem ) {
			// Only trigger if we've ever bound an event for it
			if ( this.global[type] )
				jQuery("*").add([window, document]).trigger(type, data);

		// Handle triggering a single element
		} else {
			// don't do events on text and comment nodes
			if ( elem.nodeType == 3 || elem.nodeType == 8 )
				return undefined;

			var val, ret, fn = jQuery.isFunction( elem[ type ] || null ),
				// Check to see if we need to provide a fake event, or not
				event = !data[0] || !data[0].preventDefault;
			
			// Pass along a fake event
			if ( event )
				data.unshift( this.fix({ type: type, target: elem }) );

			// Enforce the right trigger type
			data[0].type = type;
			if ( exclusive )
				data[0].exclusive = true;

			// Trigger the event
			if ( jQuery.isFunction( jQuery.data(elem, "handle") ) )
				val = jQuery.data(elem, "handle").apply( elem, data );

			// Handle triggering native .onfoo handlers
			if ( !fn && elem["on"+type] && elem["on"+type].apply( elem, data ) === false )
				val = false;

			// Extra functions don't get the custom event object
			if ( event )
				data.shift();

			// Handle triggering of extra function
			if ( extra && jQuery.isFunction( extra ) ) {
				// call the extra function and tack the current return value on the end for possible inspection
				ret = extra.apply( elem, val == null ? data : data.concat( val ) );
				// if anything is returned, give it precedence and have it overwrite the previous value
				if (ret !== undefined)
					val = ret;
			}

			// Trigger the native events (except for clicks on links)
			if ( fn && donative !== false && val !== false && !(jQuery.nodeName(elem, 'a') && type == "click") ) {
				this.triggered = true;
				try {
					elem[ type ]();
				// prevent IE from throwing an error for some hidden elements
				} catch (e) {}
			}

			this.triggered = false;
		}

		return val;
	},

	handle: function(event) {
		// returned undefined or false
		var val;

		// Empty object is for triggered events with no data
		event = jQuery.event.fix( event || window.event || {} ); 

		// Namespaced event handlers
		var parts = event.type.split(".");
		event.type = parts[0];

		var handlers = jQuery.data(this, "events") && jQuery.data(this, "events")[event.type], args = Array.prototype.slice.call( arguments, 1 );
		args.unshift( event );

		for ( var j in handlers ) {
			var handler = handlers[j];
			// Pass in a reference to the handler function itself
			// So that we can later remove it
			args[0].handler = handler;
			args[0].data = handler.data;

			// Filter the functions by class
			if ( !parts[1] && !event.exclusive || handler.type == parts[1] ) {
				var ret = handler.apply( this, args );

				if ( val !== false )
					val = ret;

				if ( ret === false ) {
					event.preventDefault();
					event.stopPropagation();
				}
			}
		}

		// Clean up added properties in IE to prevent memory leak
		if (jQuery.browser.msie)
			event.target = event.preventDefault = event.stopPropagation =
				event.handler = event.data = null;

		return val;
	},

	fix: function(event) {
		// store a copy of the original event object 
		// and clone to set read-only properties
		var originalEvent = event;
		event = jQuery.extend({}, originalEvent);
		
		// add preventDefault and stopPropagation since 
		// they will not work on the clone
		event.preventDefault = function() {
			// if preventDefault exists run it on the original event
			if (originalEvent.preventDefault)
				originalEvent.preventDefault();
			// otherwise set the returnValue property of the original event to false (IE)
			originalEvent.returnValue = false;
		};
		event.stopPropagation = function() {
			// if stopPropagation exists run it on the original event
			if (originalEvent.stopPropagation)
				originalEvent.stopPropagation();
			// otherwise set the cancelBubble property of the original event to true (IE)
			originalEvent.cancelBubble = true;
		};
		
		// Fix target property, if necessary
		if ( !event.target )
			event.target = event.srcElement || document; // Fixes #1925 where srcElement might not be defined either
				
		// check if target is a textnode (safari)
		if ( event.target.nodeType == 3 )
			event.target = originalEvent.target.parentNode;

		// Add relatedTarget, if necessary
		if ( !event.relatedTarget && event.fromElement )
			event.relatedTarget = event.fromElement == event.target ? event.toElement : event.fromElement;

		// Calculate pageX/Y if missing and clientX/Y available
		if ( event.pageX == null && event.clientX != null ) {
			var doc = document.documentElement, body = document.body;
			event.pageX = event.clientX + (doc && doc.scrollLeft || body && body.scrollLeft || 0) - (doc.clientLeft || 0);
			event.pageY = event.clientY + (doc && doc.scrollTop || body && body.scrollTop || 0) - (doc.clientTop || 0);
		}
			
		// Add which for key events
		if ( !event.which && ((event.charCode || event.charCode === 0) ? event.charCode : event.keyCode) )
			event.which = event.charCode || event.keyCode;
		
		// Add metaKey to non-Mac browsers (use ctrl for PC's and Meta for Macs)
		if ( !event.metaKey && event.ctrlKey )
			event.metaKey = event.ctrlKey;

		// Add which for click: 1 == left; 2 == middle; 3 == right
		// Note: button is not normalized, so don't use it
		if ( !event.which && event.button )
			event.which = (event.button & 1 ? 1 : ( event.button & 2 ? 3 : ( event.button & 4 ? 2 : 0 ) ));
			
		return event;
	},
	
	special: {
		ready: {
			setup: function() {
				// Make sure the ready event is setup
				bindReady();
				return;
			},
			
			teardown: function() { return; }
		},
		
		mouseenter: {
			setup: function() {
				if ( jQuery.browser.msie ) return false;
				jQuery(this).bind("mouseover", jQuery.event.special.mouseenter.handler);
				return true;
			},
		
			teardown: function() {
				if ( jQuery.browser.msie ) return false;
				jQuery(this).unbind("mouseover", jQuery.event.special.mouseenter.handler);
				return true;
			},
			
			handler: function(event) {
				// If we actually just moused on to a sub-element, ignore it
				if ( withinElement(event, this) ) return true;
				// Execute the right handlers by setting the event type to mouseenter
				arguments[0].type = "mouseenter";
				return jQuery.event.handle.apply(this, arguments);
			}
		},
	
		mouseleave: {
			setup: function() {
				if ( jQuery.browser.msie ) return false;
				jQuery(this).bind("mouseout", jQuery.event.special.mouseleave.handler);
				return true;
			},
		
			teardown: function() {
				if ( jQuery.browser.msie ) return false;
				jQuery(this).unbind("mouseout", jQuery.event.special.mouseleave.handler);
				return true;
			},
			
			handler: function(event) {
				// If we actually just moused on to a sub-element, ignore it
				if ( withinElement(event, this) ) return true;
				// Execute the right handlers by setting the event type to mouseleave
				arguments[0].type = "mouseleave";
				return jQuery.event.handle.apply(this, arguments);
			}
		}
	}
};

jQuery.fn.extend({
	bind: function( type, data, fn ) {
		return type == "unload" ? this.one(type, data, fn) : this.each(function(){
			jQuery.event.add( this, type, fn || data, fn && data );
		});
	},
	
	one: function( type, data, fn ) {
		return this.each(function(){
			jQuery.event.add( this, type, function(event) {
				jQuery(this).unbind(event);
				return (fn || data).apply( this, arguments);
			}, fn && data);
		});
	},

	unbind: function( type, fn ) {
		return this.each(function(){
			jQuery.event.remove( this, type, fn );
		});
	},

	trigger: function( type, data, fn ) {
		return this.each(function(){
			jQuery.event.trigger( type, data, this, true, fn );
		});
	},

	triggerHandler: function( type, data, fn ) {
		if ( this[0] )
			return jQuery.event.trigger( type, data, this[0], false, fn );
		return undefined;
	},

	toggle: function() {
		// Save reference to arguments for access in closure
		var args = arguments;

		return this.click(function(event) {
			// Figure out which function to execute
			this.lastToggle = 0 == this.lastToggle ? 1 : 0;
			
			// Make sure that clicks stop
			event.preventDefault();
			
			// and execute the function
			return args[this.lastToggle].apply( this, arguments ) || false;
		});
	},

	hover: function(fnOver, fnOut) {
		return this.bind('mouseenter', fnOver).bind('mouseleave', fnOut);
	},
	
	ready: function(fn) {
		// Attach the listeners
		bindReady();

		// If the DOM is already ready
		if ( jQuery.isReady )
			// Execute the function immediately
			fn.call( document, jQuery );
			
		// Otherwise, remember the function for later
		else
			// Add the function to the wait list
			jQuery.readyList.push( function() { return fn.call(this, jQuery); } );
	
		return this;
	}
});

jQuery.extend({
	isReady: false,
	readyList: [],
	// Handle when the DOM is ready
	ready: function() {
		// Make sure that the DOM is not already loaded
		if ( !jQuery.isReady ) {
			// Remember that the DOM is ready
			jQuery.isReady = true;
			
			// If there are functions bound, to execute
			if ( jQuery.readyList ) {
				// Execute all of them
				jQuery.each( jQuery.readyList, function(){
					this.apply( document );
				});
				
				// Reset the list of functions
				jQuery.readyList = null;
			}
		
			// Trigger any bound ready events
			jQuery(document).triggerHandler("ready");
		}
	}
});

var readyBound = false;

function bindReady(){
	if ( readyBound ) return;
	readyBound = true;

	// Mozilla, Opera (see further below for it) and webkit nightlies currently support this event
	if ( document.addEventListener && !jQuery.browser.opera)
		// Use the handy event callback
		document.addEventListener( "DOMContentLoaded", jQuery.ready, false );
	
	// If IE is used and is not in a frame
	// Continually check to see if the document is ready
	if ( jQuery.browser.msie && window == top ) (function(){
		if (jQuery.isReady) return;
		try {
			// If IE is used, use the trick by Diego Perini
			// http://javascript.nwbox.com/IEContentLoaded/
			document.documentElement.doScroll("left");
		} catch( error ) {
			setTimeout( arguments.callee, 0 );
			return;
		}
		// and execute any waiting functions
		jQuery.ready();
	})();

	if ( jQuery.browser.opera )
		document.addEventListener( "DOMContentLoaded", function () {
			if (jQuery.isReady) return;
			for (var i = 0; i < document.styleSheets.length; i++)
				if (document.styleSheets[i].disabled) {
					setTimeout( arguments.callee, 0 );
					return;
				}
			// and execute any waiting functions
			jQuery.ready();
		}, false);

	if ( jQuery.browser.safari ) {
		var numStyles;
		(function(){
			if (jQuery.isReady) return;
			if ( document.readyState != "loaded" && document.readyState != "complete" ) {
				setTimeout( arguments.callee, 0 );
				return;
			}
			if ( numStyles === undefined )
				numStyles = jQuery("style, link[rel=stylesheet]").length;
			if ( document.styleSheets.length != numStyles ) {
				setTimeout( arguments.callee, 0 );
				return;
			}
			// and execute any waiting functions
			jQuery.ready();
		})();
	}

	// A fallback to window.onload, that will always work
	jQuery.event.add( window, "load", jQuery.ready );
}

jQuery.each( ("blur,focus,load,resize,scroll,unload,click,dblclick," +
	"mousedown,mouseup,mousemove,mouseover,mouseout,change,select," + 
	"submit,keydown,keypress,keyup,error").split(","), function(i, name){
	
	// Handle event binding
	jQuery.fn[name] = function(fn){
		return fn ? this.bind(name, fn) : this.trigger(name);
	};
});

// Checks if an event happened on an element within another element
// Used in jQuery.event.special.mouseenter and mouseleave handlers
var withinElement = function(event, elem) {
	// Check if mouse(over|out) are still within the same parent element
	var parent = event.relatedTarget;
	// Traverse up the tree
	while ( parent && parent != elem ) try { parent = parent.parentNode; } catch(error) { parent = elem; }
	// Return true if we actually just moused on to a sub-element
	return parent == elem;
};

// Prevent memory leaks in IE
// And prevent errors on refresh with events like mouseover in other browsers
// Window isn't included so as not to unbind existing unload events
jQuery(window).bind("unload", function() {
	jQuery("*").add(document).unbind();
});
jQuery.fn.extend({
	load: function( url, params, callback ) {
		if ( jQuery.isFunction( url ) )
			return this.bind("load", url);

		var off = url.indexOf(" ");
		if ( off >= 0 ) {
			var selector = url.slice(off, url.length);
			url = url.slice(0, off);
		}

		callback = callback || function(){};

		// Default to a GET request
		var type = "GET";

		// If the second parameter was provided
		if ( params )
			// If it's a function
			if ( jQuery.isFunction( params ) ) {
				// We assume that it's the callback
				callback = params;
				params = null;

			// Otherwise, build a param string
			} else {
				params = jQuery.param( params );
				type = "POST";
			}

		var self = this;

		// Request the remote document
		jQuery.ajax({
			url: url,
			type: type,
			dataType: "html",
			data: params,
			complete: function(res, status){
				// If successful, inject the HTML into all the matched elements
				if ( status == "success" || status == "notmodified" )
					// See if a selector was specified
					self.html( selector ?
						// Create a dummy div to hold the results
						jQuery("<div/>")
							// inject the contents of the document in, removing the scripts
							// to avoid any 'Permission Denied' errors in IE
							.append(res.responseText.replace(/<script(.|\s)*?\/script>/g, ""))

							// Locate the specified elements
							.find(selector) :

						// If not, just inject the full result
						res.responseText );

				self.each( callback, [res.responseText, status, res] );
			}
		});
		return this;
	},

	serialize: function() {
		return jQuery.param(this.serializeArray());
	},
	serializeArray: function() {
		return this.map(function(){
			return jQuery.nodeName(this, "form") ?
				jQuery.makeArray(this.elements) : this;
		})
		.filter(function(){
			return this.name && !this.disabled && 
				(this.checked || /select|textarea/i.test(this.nodeName) || 
					/text|hidden|password/i.test(this.type));
		})
		.map(function(i, elem){
			var val = jQuery(this).val();
			return val == null ? null :
				val.constructor == Array ?
					jQuery.map( val, function(val, i){
						return {name: elem.name, value: val};
					}) :
					{name: elem.name, value: val};
		}).get();
	}
});

// Attach a bunch of functions for handling common AJAX events
jQuery.each( "ajaxStart,ajaxStop,ajaxComplete,ajaxError,ajaxSuccess,ajaxSend".split(","), function(i,o){
	jQuery.fn[o] = function(f){
		return this.bind(o, f);
	};
});

var jsc = (new Date).getTime();

jQuery.extend({
	get: function( url, data, callback, type ) {
		// shift arguments if data argument was ommited
		if ( jQuery.isFunction( data ) ) {
			callback = data;
			data = null;
		}
		
		return jQuery.ajax({
			type: "GET",
			url: url,
			data: data,
			success: callback,
			dataType: type
		});
	},

	getScript: function( url, callback ) {
		return jQuery.get(url, null, callback, "script");
	},

	getJSON: function( url, data, callback ) {
		return jQuery.get(url, data, callback, "json");
	},

	post: function( url, data, callback, type ) {
		if ( jQuery.isFunction( data ) ) {
			callback = data;
			data = {};
		}

		return jQuery.ajax({
			type: "POST",
			url: url,
			data: data,
			success: callback,
			dataType: type
		});
	},

	ajaxSetup: function( settings ) {
		jQuery.extend( jQuery.ajaxSettings, settings );
	},

	ajaxSettings: {
		global: true,
		type: "GET",
		timeout: 0,
		contentType: "application/x-www-form-urlencoded",
		processData: true,
		async: true,
		data: null,
		username: null,
		password: null,
		accepts: {
			xml: "application/xml, text/xml",
			html: "text/html",
			script: "text/javascript, application/javascript",
			json: "application/json, text/javascript",
			text: "text/plain",
			_default: "*/*"
		}
	},
	
	// Last-Modified header cache for next request
	lastModified: {},

	ajax: function( s ) {
		var jsonp, jsre = /=\?(&|$)/g, status, data;

		// Extend the settings, but re-extend 's' so that it can be
		// checked again later (in the test suite, specifically)
		s = jQuery.extend(true, s, jQuery.extend(true, {}, jQuery.ajaxSettings, s));

		// convert data if not already a string
		if ( s.data && s.processData && typeof s.data != "string" )
			s.data = jQuery.param(s.data);

		// Handle JSONP Parameter Callbacks
		if ( s.dataType == "jsonp" ) {
			if ( s.type.toLowerCase() == "get" ) {
				if ( !s.url.match(jsre) )
					s.url += (s.url.match(/\?/) ? "&" : "?") + (s.jsonp || "callback") + "=?";
			} else if ( !s.data || !s.data.match(jsre) )
				s.data = (s.data ? s.data + "&" : "") + (s.jsonp || "callback") + "=?";
			s.dataType = "json";
		}

		// Build temporary JSONP function
		if ( s.dataType == "json" && (s.data && s.data.match(jsre) || s.url.match(jsre)) ) {
			jsonp = "jsonp" + jsc++;

			// Replace the =? sequence both in the query string and the data
			if ( s.data )
				s.data = (s.data + "").replace(jsre, "=" + jsonp + "$1");
			s.url = s.url.replace(jsre, "=" + jsonp + "$1");

			// We need to make sure
			// that a JSONP style response is executed properly
			s.dataType = "script";

			// Handle JSONP-style loading
			window[ jsonp ] = function(tmp){
				data = tmp;
				success();
				complete();
				// Garbage collect
				window[ jsonp ] = undefined;
				try{ delete window[ jsonp ]; } catch(e){}
				if ( head )
					head.removeChild( script );
			};
		}

		if ( s.dataType == "script" && s.cache == null )
			s.cache = false;

		if ( s.cache === false && s.type.toLowerCase() == "get" ) {
			var ts = (new Date()).getTime();
			// try replacing _= if it is there
			var ret = s.url.replace(/(\?|&)_=.*?(&|$)/, "$1_=" + ts + "$2");
			// if nothing was replaced, add timestamp to the end
			s.url = ret + ((ret == s.url) ? (s.url.match(/\?/) ? "&" : "?") + "_=" + ts : "");
		}

		// If data is available, append data to url for get requests
		if ( s.data && s.type.toLowerCase() == "get" ) {
			s.url += (s.url.match(/\?/) ? "&" : "?") + s.data;

			// IE likes to send both get and post data, prevent this
			s.data = null;
		}

		// Watch for a new set of requests
		if ( s.global && ! jQuery.active++ )
			jQuery.event.trigger( "ajaxStart" );

		// If we're requesting a remote document
		// and trying to load JSON or Script with a GET
		if ( (!s.url.indexOf("http") || !s.url.indexOf("//")) && s.dataType == "script" && s.type.toLowerCase() == "get" ) {
			var head = document.getElementsByTagName("head")[0];
			var script = document.createElement("script");
			script.src = s.url;
			if (s.scriptCharset)
				script.charset = s.scriptCharset;

			// Handle Script loading
			if ( !jsonp ) {
				var done = false;

				// Attach handlers for all browsers
				script.onload = script.onreadystatechange = function(){
					if ( !done && (!this.readyState || 
							this.readyState == "loaded" || this.readyState == "complete") ) {
						done = true;
						success();
						complete();
						head.removeChild( script );
					}
				};
			}

			head.appendChild(script);

			// We handle everything using the script element injection
			return undefined;
		}

		var requestDone = false;

		// Create the request object; Microsoft failed to properly
		// implement the XMLHttpRequest in IE7, so we use the ActiveXObject when it is available
		var xml = window.ActiveXObject ? new ActiveXObject("Microsoft.XMLHTTP") : new XMLHttpRequest();

		// Open the socket
		xml.open(s.type, s.url, s.async, s.username, s.password);

		// Need an extra try/catch for cross domain requests in Firefox 3
		try {
			// Set the correct header, if data is being sent
			if ( s.data )
				xml.setRequestHeader("Content-Type", s.contentType);

			// Set the If-Modified-Since header, if ifModified mode.
			if ( s.ifModified )
				xml.setRequestHeader("If-Modified-Since",
					jQuery.lastModified[s.url] || "Thu, 01 Jan 1970 00:00:00 GMT" );

			// Set header so the called script knows that it's an XMLHttpRequest
			xml.setRequestHeader("X-Requested-With", "XMLHttpRequest");

			// Set the Accepts header for the server, depending on the dataType
			xml.setRequestHeader("Accept", s.dataType && s.accepts[ s.dataType ] ?
				s.accepts[ s.dataType ] + ", */*" :
				s.accepts._default );
		} catch(e){}

		// Allow custom headers/mimetypes
		if ( s.beforeSend )
			s.beforeSend(xml);
			
		if ( s.global )
			jQuery.event.trigger("ajaxSend", [xml, s]);

		// Wait for a response to come back
		var onreadystatechange = function(isTimeout){
			// The transfer is complete and the data is available, or the request timed out
			if ( !requestDone && xml && (xml.readyState == 4 || isTimeout == "timeout") ) {
				requestDone = true;
				
				// clear poll interval
				if (ival) {
					clearInterval(ival);
					ival = null;
				}
				
				status = isTimeout == "timeout" && "timeout" ||
					!jQuery.httpSuccess( xml ) && "error" ||
					s.ifModified && jQuery.httpNotModified( xml, s.url ) && "notmodified" ||
					"success";

				if ( status == "success" ) {
					// Watch for, and catch, XML document parse errors
					try {
						// process the data (runs the xml through httpData regardless of callback)
						data = jQuery.httpData( xml, s.dataType );
					} catch(e) {
						status = "parsererror";
					}
				}

				// Make sure that the request was successful or notmodified
				if ( status == "success" ) {
					// Cache Last-Modified header, if ifModified mode.
					var modRes;
					try {
						modRes = xml.getResponseHeader("Last-Modified");
					} catch(e) {} // swallow exception thrown by FF if header is not available
	
					if ( s.ifModified && modRes )
						jQuery.lastModified[s.url] = modRes;

					// JSONP handles its own success callback
					if ( !jsonp )
						success();	
				} else
					jQuery.handleError(s, xml, status);

				// Fire the complete handlers
				complete();

				// Stop memory leaks
				if ( s.async )
					xml = null;
			}
		};
		
		if ( s.async ) {
			// don't attach the handler to the request, just poll it instead
			var ival = setInterval(onreadystatechange, 13); 

			// Timeout checker
			if ( s.timeout > 0 )
				setTimeout(function(){
					// Check to see if the request is still happening
					if ( xml ) {
						// Cancel the request
						xml.abort();
	
						if( !requestDone )
							onreadystatechange( "timeout" );
					}
				}, s.timeout);
		}
			
		// Send the data
		try {
			xml.send(s.data);
		} catch(e) {
			jQuery.handleError(s, xml, null, e);
		}
		
		// firefox 1.5 doesn't fire statechange for sync requests
		if ( !s.async )
			onreadystatechange();

		function success(){
			// If a local callback was specified, fire it and pass it the data
			if ( s.success )
				s.success( data, status );

			// Fire the global callback
			if ( s.global )
				jQuery.event.trigger( "ajaxSuccess", [xml, s] );
		}

		function complete(){
			// Process result
			if ( s.complete )
				s.complete(xml, status);

			// The request was completed
			if ( s.global )
				jQuery.event.trigger( "ajaxComplete", [xml, s] );

			// Handle the global AJAX counter
			if ( s.global && ! --jQuery.active )
				jQuery.event.trigger( "ajaxStop" );
		}
		
		// return XMLHttpRequest to allow aborting the request etc.
		return xml;
	},

	handleError: function( s, xml, status, e ) {
		// If a local callback was specified, fire it
		if ( s.error ) s.error( xml, status, e );

		// Fire the global callback
		if ( s.global )
			jQuery.event.trigger( "ajaxError", [xml, s, e] );
	},

	// Counter for holding the number of active queries
	active: 0,

	// Determines if an XMLHttpRequest was successful or not
	httpSuccess: function( r ) {
		try {
			// IE error sometimes returns 1223 when it should be 204 so treat it as success, see #1450
			return !r.status && location.protocol == "file:" ||
				( r.status >= 200 && r.status < 300 ) || r.status == 304 || r.status == 1223 ||
				jQuery.browser.safari && r.status == undefined;
		} catch(e){}
		return false;
	},

	// Determines if an XMLHttpRequest returns NotModified
	httpNotModified: function( xml, url ) {
		try {
			var xmlRes = xml.getResponseHeader("Last-Modified");

			// Firefox always returns 200. check Last-Modified date
			return xml.status == 304 || xmlRes == jQuery.lastModified[url] ||
				jQuery.browser.safari && xml.status == undefined;
		} catch(e){}
		return false;
	},

	httpData: function( r, type ) {
		var ct = r.getResponseHeader("content-type");
		var xml = type == "xml" || !type && ct && ct.indexOf("xml") >= 0;
		var data = xml ? r.responseXML : r.responseText;

		if ( xml && data.documentElement.tagName == "parsererror" )
			throw "parsererror";

		// If the type is "script", eval it in global context
		if ( type == "script" )
			jQuery.globalEval( data );

		// Get the JavaScript object, if JSON is used.
		if ( type == "json" )
			data = eval("(" + data + ")");

		return data;
	},

	// Serialize an array of form elements or a set of
	// key/values into a query string
	param: function( a ) {
		var s = [];

		// If an array was passed in, assume that it is an array
		// of form elements
		if ( a.constructor == Array || a.jquery )
			// Serialize the form elements
			jQuery.each( a, function(){
				s.push( encodeURIComponent(this.name) + "=" + encodeURIComponent( this.value ) );
			});

		// Otherwise, assume that it's an object of key/value pairs
		else
			// Serialize the key/values
			for ( var j in a )
				// If the value is an array then the key names need to be repeated
				if ( a[j] && a[j].constructor == Array )
					jQuery.each( a[j], function(){
						s.push( encodeURIComponent(j) + "=" + encodeURIComponent( this ) );
					});
				else
					s.push( encodeURIComponent(j) + "=" + encodeURIComponent( a[j] ) );

		// Return the resulting serialization
		return s.join("&").replace(/%20/g, "+");
	}

});
jQuery.fn.extend({
	show: function(speed,callback){
		return speed ?
			this.animate({
				height: "show", width: "show", opacity: "show"
			}, speed, callback) :
			
			this.filter(":hidden").each(function(){
				this.style.display = this.oldblock || "";
				if ( jQuery.css(this,"display") == "none" ) {
					var elem = jQuery("<" + this.tagName + " />").appendTo("body");
					this.style.display = elem.css("display");
					// handle an edge condition where css is - div { display:none; } or similar
					if (this.style.display == "none")
						this.style.display = "block";
					elem.remove();
				}
			}).end();
	},
	
	hide: function(speed,callback){
		return speed ?
			this.animate({
				height: "hide", width: "hide", opacity: "hide"
			}, speed, callback) :
			
			this.filter(":visible").each(function(){
				this.oldblock = this.oldblock || jQuery.css(this,"display");
				this.style.display = "none";
			}).end();
	},

	// Save the old toggle function
	_toggle: jQuery.fn.toggle,
	
	toggle: function( fn, fn2 ){
		return jQuery.isFunction(fn) && jQuery.isFunction(fn2) ?
			this._toggle( fn, fn2 ) :
			fn ?
				this.animate({
					height: "toggle", width: "toggle", opacity: "toggle"
				}, fn, fn2) :
				this.each(function(){
					jQuery(this)[ jQuery(this).is(":hidden") ? "show" : "hide" ]();
				});
	},
	
	slideDown: function(speed,callback){
		return this.animate({height: "show"}, speed, callback);
	},
	
	slideUp: function(speed,callback){
		return this.animate({height: "hide"}, speed, callback);
	},

	slideToggle: function(speed, callback){
		return this.animate({height: "toggle"}, speed, callback);
	},
	
	fadeIn: function(speed, callback){
		return this.animate({opacity: "show"}, speed, callback);
	},
	
	fadeOut: function(speed, callback){
		return this.animate({opacity: "hide"}, speed, callback);
	},
	
	fadeTo: function(speed,to,callback){
		return this.animate({opacity: to}, speed, callback);
	},
	
	animate: function( prop, speed, easing, callback ) {
		var optall = jQuery.speed(speed, easing, callback);

		return this[ optall.queue === false ? "each" : "queue" ](function(){
			if ( this.nodeType != 1)
				return false;

			var opt = jQuery.extend({}, optall);
			var hidden = jQuery(this).is(":hidden"), self = this;
			
			for ( var p in prop ) {
				if ( prop[p] == "hide" && hidden || prop[p] == "show" && !hidden )
					return jQuery.isFunction(opt.complete) && opt.complete.apply(this);

				if ( p == "height" || p == "width" ) {
					// Store display property
					opt.display = jQuery.css(this, "display");

					// Make sure that nothing sneaks out
					opt.overflow = this.style.overflow;
				}
			}

			if ( opt.overflow != null )
				this.style.overflow = "hidden";

			opt.curAnim = jQuery.extend({}, prop);
			
			jQuery.each( prop, function(name, val){
				var e = new jQuery.fx( self, opt, name );

				if ( /toggle|show|hide/.test(val) )
					e[ val == "toggle" ? hidden ? "show" : "hide" : val ]( prop );
				else {
					var parts = val.toString().match(/^([+-]=)?([\d+-.]+)(.*)$/),
						start = e.cur(true) || 0;

					if ( parts ) {
						var end = parseFloat(parts[2]),
							unit = parts[3] || "px";

						// We need to compute starting value
						if ( unit != "px" ) {
							self.style[ name ] = (end || 1) + unit;
							start = ((end || 1) / e.cur(true)) * start;
							self.style[ name ] = start + unit;
						}

						// If a +=/-= token was provided, we're doing a relative animation
						if ( parts[1] )
							end = ((parts[1] == "-=" ? -1 : 1) * end) + start;

						e.custom( start, end, unit );
					} else
						e.custom( start, val, "" );
				}
			});

			// For JS strict compliance
			return true;
		});
	},
	
	queue: function(type, fn){
		if ( jQuery.isFunction(type) || ( type && type.constructor == Array )) {
			fn = type;
			type = "fx";
		}

		if ( !type || (typeof type == "string" && !fn) )
			return queue( this[0], type );

		return this.each(function(){
			if ( fn.constructor == Array )
				queue(this, type, fn);
			else {
				queue(this, type).push( fn );
			
				if ( queue(this, type).length == 1 )
					fn.apply(this);
			}
		});
	},

	stop: function(clearQueue, gotoEnd){
		var timers = jQuery.timers;

		if (clearQueue)
			this.queue([]);

		this.each(function(){
			// go in reverse order so anything added to the queue during the loop is ignored
			for ( var i = timers.length - 1; i >= 0; i-- )
				if ( timers[i].elem == this ) {
					if (gotoEnd)
						// force the next step to be the last
						timers[i](true);
					timers.splice(i, 1);
				}
		});

		// start the next in the queue if the last step wasn't forced
		if (!gotoEnd)
			this.dequeue();

		return this;
	}

});

var queue = function( elem, type, array ) {
	if ( !elem )
		return undefined;

	type = type || "fx";

	var q = jQuery.data( elem, type + "queue" );

	if ( !q || array )
		q = jQuery.data( elem, type + "queue", 
			array ? jQuery.makeArray(array) : [] );

	return q;
};

jQuery.fn.dequeue = function(type){
	type = type || "fx";

	return this.each(function(){
		var q = queue(this, type);

		q.shift();

		if ( q.length )
			q[0].apply( this );
	});
};

jQuery.extend({
	
	speed: function(speed, easing, fn) {
		var opt = speed && speed.constructor == Object ? speed : {
			complete: fn || !fn && easing || 
				jQuery.isFunction( speed ) && speed,
			duration: speed,
			easing: fn && easing || easing && easing.constructor != Function && easing
		};

		opt.duration = (opt.duration && opt.duration.constructor == Number ? 
			opt.duration : 
			{ slow: 600, fast: 200 }[opt.duration]) || 400;
	
		// Queueing
		opt.old = opt.complete;
		opt.complete = function(){
			if ( opt.queue !== false )
				jQuery(this).dequeue();
			if ( jQuery.isFunction( opt.old ) )
				opt.old.apply( this );
		};
	
		return opt;
	},
	
	easing: {
		linear: function( p, n, firstNum, diff ) {
			return firstNum + diff * p;
		},
		swing: function( p, n, firstNum, diff ) {
			return ((-Math.cos(p*Math.PI)/2) + 0.5) * diff + firstNum;
		}
	},
	
	timers: [],
	timerId: null,

	fx: function( elem, options, prop ){
		this.options = options;
		this.elem = elem;
		this.prop = prop;

		if ( !options.orig )
			options.orig = {};
	}

});

jQuery.fx.prototype = {

	// Simple function for setting a style value
	update: function(){
		if ( this.options.step )
			this.options.step.apply( this.elem, [ this.now, this ] );

		(jQuery.fx.step[this.prop] || jQuery.fx.step._default)( this );

		// Set display property to block for height/width animations
		if ( this.prop == "height" || this.prop == "width" )
			this.elem.style.display = "block";
	},

	// Get the current size
	cur: function(force){
		if ( this.elem[this.prop] != null && this.elem.style[this.prop] == null )
			return this.elem[ this.prop ];

		var r = parseFloat(jQuery.css(this.elem, this.prop, force));
		return r && r > -10000 ? r : parseFloat(jQuery.curCSS(this.elem, this.prop)) || 0;
	},

	// Start an animation from one number to another
	custom: function(from, to, unit){
		this.startTime = (new Date()).getTime();
		this.start = from;
		this.end = to;
		this.unit = unit || this.unit || "px";
		this.now = this.start;
		this.pos = this.state = 0;
		this.update();

		var self = this;
		function t(gotoEnd){
			return self.step(gotoEnd);
		}

		t.elem = this.elem;

		jQuery.timers.push(t);

		if ( jQuery.timerId == null ) {
			jQuery.timerId = setInterval(function(){
				var timers = jQuery.timers;
				
				for ( var i = 0; i < timers.length; i++ )
					if ( !timers[i]() )
						timers.splice(i--, 1);

				if ( !timers.length ) {
					clearInterval( jQuery.timerId );
					jQuery.timerId = null;
				}
			}, 13);
		}
	},

	// Simple 'show' function
	show: function(){
		// Remember where we started, so that we can go back to it later
		this.options.orig[this.prop] = jQuery.attr( this.elem.style, this.prop );
		this.options.show = true;

		// Begin the animation
		this.custom(0, this.cur());

		// Make sure that we start at a small width/height to avoid any
		// flash of content
		if ( this.prop == "width" || this.prop == "height" )
			this.elem.style[this.prop] = "1px";
		
		// Start by showing the element
		jQuery(this.elem).show();
	},

	// Simple 'hide' function
	hide: function(){
		// Remember where we started, so that we can go back to it later
		this.options.orig[this.prop] = jQuery.attr( this.elem.style, this.prop );
		this.options.hide = true;

		// Begin the animation
		this.custom(this.cur(), 0);
	},

	// Each step of an animation
	step: function(gotoEnd){
		var t = (new Date()).getTime();

		if ( gotoEnd || t > this.options.duration + this.startTime ) {
			this.now = this.end;
			this.pos = this.state = 1;
			this.update();

			this.options.curAnim[ this.prop ] = true;

			var done = true;
			for ( var i in this.options.curAnim )
				if ( this.options.curAnim[i] !== true )
					done = false;

			if ( done ) {
				if ( this.options.display != null ) {
					// Reset the overflow
					this.elem.style.overflow = this.options.overflow;
				
					// Reset the display
					this.elem.style.display = this.options.display;
					if ( jQuery.css(this.elem, "display") == "none" )
						this.elem.style.display = "block";
				}

				// Hide the element if the "hide" operation was done
				if ( this.options.hide )
					this.elem.style.display = "none";

				// Reset the properties, if the item has been hidden or shown
				if ( this.options.hide || this.options.show )
					for ( var p in this.options.curAnim )
						jQuery.attr(this.elem.style, p, this.options.orig[p]);
			}

			// If a callback was provided, execute it
			if ( done && jQuery.isFunction( this.options.complete ) )
				// Execute the complete function
				this.options.complete.apply( this.elem );

			return false;
		} else {
			var n = t - this.startTime;
			this.state = n / this.options.duration;

			// Perform the easing function, defaults to swing
			this.pos = jQuery.easing[this.options.easing || (jQuery.easing.swing ? "swing" : "linear")](this.state, n, 0, 1, this.options.duration);
			this.now = this.start + ((this.end - this.start) * this.pos);

			// Perform the next step of the animation
			this.update();
		}

		return true;
	}

};

jQuery.fx.step = {
	scrollLeft: function(fx){
		fx.elem.scrollLeft = fx.now;
	},

	scrollTop: function(fx){
		fx.elem.scrollTop = fx.now;
	},

	opacity: function(fx){
		jQuery.attr(fx.elem.style, "opacity", fx.now);
	},

	_default: function(fx){
		fx.elem.style[ fx.prop ] = fx.now + fx.unit;
	}
};
// The Offset Method
// Originally By Brandon Aaron, part of the Dimension Plugin
// http://jquery.com/plugins/project/dimensions
jQuery.fn.offset = function() {
	var left = 0, top = 0, elem = this[0], results;
	
	if ( elem ) with ( jQuery.browser ) {
		var parent       = elem.parentNode, 
		    offsetChild  = elem,
		    offsetParent = elem.offsetParent, 
		    doc          = elem.ownerDocument,
		    safari2      = safari && parseInt(version) < 522 && !/adobeair/i.test(userAgent),
		    fixed        = jQuery.css(elem, "position") == "fixed";
	
		// Use getBoundingClientRect if available
		if ( elem.getBoundingClientRect ) {
			var box = elem.getBoundingClientRect();
		
			// Add the document scroll offsets
			add(box.left + Math.max(doc.documentElement.scrollLeft, doc.body.scrollLeft),
				box.top  + Math.max(doc.documentElement.scrollTop,  doc.body.scrollTop));
		
			// IE adds the HTML element's border, by default it is medium which is 2px
			// IE 6 and 7 quirks mode the border width is overwritable by the following css html { border: 0; }
			// IE 7 standards mode, the border is always 2px
			// This border/offset is typically represented by the clientLeft and clientTop properties
			// However, in IE6 and 7 quirks mode the clientLeft and clientTop properties are not updated when overwriting it via CSS
			// Therefore this method will be off by 2px in IE while in quirksmode
			add( -doc.documentElement.clientLeft, -doc.documentElement.clientTop );
	
		// Otherwise loop through the offsetParents and parentNodes
		} else {
		
			// Initial element offsets
			add( elem.offsetLeft, elem.offsetTop );
			
			// Get parent offsets
			while ( offsetParent ) {
				// Add offsetParent offsets
				add( offsetParent.offsetLeft, offsetParent.offsetTop );
			
				// Mozilla and Safari > 2 does not include the border on offset parents
				// However Mozilla adds the border for table or table cells
				if ( mozilla && !/^t(able|d|h)$/i.test(offsetParent.tagName) || safari && !safari2 )
					border( offsetParent );
					
				// Add the document scroll offsets if position is fixed on any offsetParent
				if ( !fixed && jQuery.css(offsetParent, "position") == "fixed" )
					fixed = true;
			
				// Set offsetChild to previous offsetParent unless it is the body element
				offsetChild  = /^body$/i.test(offsetParent.tagName) ? offsetChild : offsetParent;
				// Get next offsetParent
				offsetParent = offsetParent.offsetParent;
			}
		
			// Get parent scroll offsets
			while ( parent && parent.tagName && !/^body|html$/i.test(parent.tagName) ) {
				// Remove parent scroll UNLESS that parent is inline or a table to work around Opera inline/table scrollLeft/Top bug
				if ( !/^inline|table.*$/i.test(jQuery.css(parent, "display")) )
					// Subtract parent scroll offsets
					add( -parent.scrollLeft, -parent.scrollTop );
			
				// Mozilla does not add the border for a parent that has overflow != visible
				if ( mozilla && jQuery.css(parent, "overflow") != "visible" )
					border( parent );
			
				// Get next parent
				parent = parent.parentNode;
			}
		
			// Safari <= 2 doubles body offsets with a fixed position element/offsetParent or absolutely positioned offsetChild
			// Mozilla doubles body offsets with a non-absolutely positioned offsetChild
			if ( (safari2 && (fixed || jQuery.css(offsetChild, "position") == "absolute")) || 
				(mozilla && jQuery.css(offsetChild, "position") != "absolute") )
					add( -doc.body.offsetLeft, -doc.body.offsetTop );
			
			// Add the document scroll offsets if position is fixed
			if ( fixed )
				add(Math.max(doc.documentElement.scrollLeft, doc.body.scrollLeft),
					Math.max(doc.documentElement.scrollTop,  doc.body.scrollTop));
		}

		// Return an object with top and left properties
		results = { top: top, left: left };
	}

	function border(elem) {
		add( jQuery.curCSS(elem, "borderLeftWidth", true), jQuery.curCSS(elem, "borderTopWidth", true) );
	}

	function add(l, t) {
		left += parseInt(l) || 0;
		top += parseInt(t) || 0;
	}

	return results;
};
})();



/***************************************************************************

								 JQUERY.DIMENSIONS.JS

****************************************************************************/




/* Copyright (c) 2007 Paul Bakaus (paul.bakaus@googlemail.com) and Brandon Aaron (brandon.aaron@gmail.com || http://brandonaaron.net)
 * Dual licensed under the MIT (http://www.opensource.org/licenses/mit-license.php)
 * and GPL (http://www.opensource.org/licenses/gpl-license.php) licenses.
 *
 * $LastChangedDate$
 * $Rev$
 *
 * Version: @VERSION
 *
 * Requires: jQuery 1.2+
 */

(function($){
	
$.dimensions = {
	version: '@VERSION'
};

// Create innerHeight, innerWidth, outerHeight and outerWidth methods
$.each( [ 'Height', 'Width' ], function(i, name){
	
	// innerHeight and innerWidth
	$.fn[ 'inner' + name ] = function() {
		if (!this[0]) return;
		
		var torl = name == 'Height' ? 'Top'    : 'Left',  // top or left
		    borr = name == 'Height' ? 'Bottom' : 'Right'; // bottom or right
		
		return this.is(':visible') ? this[0]['client' + name] : num( this, name.toLowerCase() ) + num(this, 'padding' + torl) + num(this, 'padding' + borr);
	};
	
	// outerHeight and outerWidth
	$.fn[ 'outer' + name ] = function(options) {
		if (!this[0]) return;
		
		var torl = name == 'Height' ? 'Top'    : 'Left',  // top or left
		    borr = name == 'Height' ? 'Bottom' : 'Right'; // bottom or right
		
		options = $.extend({ margin: false }, options || {});
		
		var val = this.is(':visible') ? 
				this[0]['offset' + name] : 
				num( this, name.toLowerCase() )
					+ num(this, 'border' + torl + 'Width') + num(this, 'border' + borr + 'Width')
					+ num(this, 'padding' + torl) + num(this, 'padding' + borr);
		
		return val + (options.margin ? (num(this, 'margin' + torl) + num(this, 'margin' + borr)) : 0);
	};
});

// Create scrollLeft and scrollTop methods
$.each( ['Left', 'Top'], function(i, name) {
	$.fn[ 'scroll' + name ] = function(val) {
		if (!this[0]) return;
		
		return val != undefined ?
		
			// Set the scroll offset
			this.each(function() {
				this == window || this == document ?
					window.scrollTo( 
						name == 'Left' ? val : $(window)[ 'scrollLeft' ](),
						name == 'Top'  ? val : $(window)[ 'scrollTop'  ]()
					) :
					this[ 'scroll' + name ] = val;
			}) :
			
			// Return the scroll offset
			this[0] == window || this[0] == document ?
				self[ (name == 'Left' ? 'pageXOffset' : 'pageYOffset') ] ||
					$.boxModel && document.documentElement[ 'scroll' + name ] ||
					document.body[ 'scroll' + name ] :
				this[0][ 'scroll' + name ];
	};
});

$.fn.extend({
	position: function() {
		var left = 0, top = 0, elem = this[0], offset, parentOffset, offsetParent, results;
		
		if (elem) {
			// Get *real* offsetParent
			offsetParent = this.offsetParent();
			
			// Get correct offsets
			offset       = this.offset();
			parentOffset = offsetParent.offset();
			
			// Subtract element margins
			offset.top  -= num(elem, 'marginTop');
			offset.left -= num(elem, 'marginLeft');
			
			// Add offsetParent borders
			parentOffset.top  += num(offsetParent, 'borderTopWidth');
			parentOffset.left += num(offsetParent, 'borderLeftWidth');
			
			// Subtract the two offsets
			results = {
				top:  offset.top  - parentOffset.top,
				left: offset.left - parentOffset.left
			};
		}
		
		return results;
	},
	
	offsetParent: function() {
		var offsetParent = this[0].offsetParent;
		while ( offsetParent && (!/^body|html$/i.test(offsetParent.tagName) && $.css(offsetParent, 'position') == 'static') )
			offsetParent = offsetParent.offsetParent;
		return $(offsetParent);
	}
});

function num(el, prop) {
	return parseInt($.curCSS(el.jquery?el[0]:el,prop,true))||0;
};

})(jQuery);



/***************************************************************************

								 UI.MOUSE.JS

****************************************************************************/


(function($) {
	
	//If the UI scope is not available, add it
	$.ui = $.ui || {};
	
	//Add methods that are vital for all mouse interaction stuff (plugin registering)
	$.extend($.ui, {
		plugin: {
			add: function(module, option, set) {
				var proto = $.ui[module].prototype;
				for(var i in set) {
					proto.plugins[i] = proto.plugins[i] || [];
					proto.plugins[i].push([option, set[i]]);
				}
			},
			call: function(instance, name, arguments) {
				var set = instance.plugins[name]; if(!set) return;
				for (var i = 0; i < set.length; i++) {
					if (instance.options[set[i][0]]) set[i][1].apply(instance.element, arguments);
				}
			}	
		},
		cssCache: {},
		css: function(name) {
			if ($.ui.cssCache[name]) return $.ui.cssCache[name];
			
			var tmp = $('<div class="ui-resizable-gen">').addClass(name).css(
				{position:'absolute', top:'-5000px', left:'-5000px', display:'block'}
			).appendTo('body');
			
			//Opera and Safari set width and height to 0px instead of auto
			//Safari returns rgba(0,0,0,0) when bgcolor is not set
			$.ui.cssCache[name] = !!(
				((/^[1-9]/).test(tmp.css('height')) || (/^[1-9]/).test(tmp.css('width')) || 
				!(/none/).test(tmp.css('backgroundImage')) || !(/transparent|rgba\(0, 0, 0, 0\)/).test(tmp.css('backgroundColor')))
			);
			try { $('body').get(0).removeChild(tmp.get(0));	} catch(e){}
			return $.ui.cssCache[name];
		},
		disableSelection: function(e) {
			if (!e) return;
			e.unselectable = "on";
			e.onselectstart = function() {	return false; };
			if (e.style) e.style.MozUserSelect = "none";
		},
		enableSelection: function(e) {
			if (!e) return;
			e.unselectable = "off";
			e.onselectstart = function() { return true; };
			if (e.style) e.style.MozUserSelect = "";
		}
	});
	
	/********************************************************************************************************/

	$.fn.extend({
		mouseInteraction: function(o) {
			return this.each(function() {
				new $.ui.mouseInteraction(this, o);
			});
		},
		removeMouseInteraction: function(o) {
			return this.each(function() {
				if($.data(this, "ui-mouse"))
					$.data(this, "ui-mouse").destroy();
			});
		}
	});
	
	/********************************************************************************************************/
	
	$.ui.mouseInteraction = function(element, options) {
	
		var self = this;
		this.element = element;
		$.data(this.element, "ui-mouse", this);
		this.options = $.extend({}, options);
		
		$(element).bind('mousedown.draggable', function() { return self.click.apply(self, arguments); });
		if($.browser.msie) $(element).attr('unselectable', 'on'); //Prevent text selection in IE
		
	};
	
	$.extend($.ui.mouseInteraction.prototype, {
		
		destroy: function() { $(this.element).unbind('mousedown.draggable'); },
		trigger: function() { return this.click.apply(this, arguments); },
		click: function(e) {
			
			if(
				   e.which != 1 //only left click starts dragging
				|| $.inArray(e.target.nodeName.toLowerCase(), this.options.dragPrevention) != -1 // Prevent execution on defined elements
				|| (this.options.condition && !this.options.condition.apply(this.options.executor || this, [e, this.element])) //Prevent execution on condition
			) return true;
			
			var self = this;
			var initialize = function() {
				self._MP = { left: e.pageX, top: e.pageY }; // Store the click mouse position
				$(document).bind('mouseup.draggable', function() { return self.stop.apply(self, arguments); });
				$(document).bind('mousemove.draggable', function() { return self.drag.apply(self, arguments); });
			};

			if(this.options.delay) {
				if(this.timer) clearInterval(this.timer);
				this.timer = setTimeout(initialize, this.options.delay);
			} else {
				initialize();
			}
			
			return false;
			
		},
		stop: function(e) {			
			
			var o = this.options;
			if(!this.initialized) return $(document).unbind('mouseup.draggable').unbind('mousemove.draggable');

			if(this.options.stop) this.options.stop.call(this.options.executor || this, e, this.element);
			$(document).unbind('mouseup.draggable').unbind('mousemove.draggable');
			this.initialized = false;
			return false;
			
		},
		drag: function(e) {

			var o = this.options;
			if ($.browser.msie && !e.button) return this.stop.apply(this, [e]); // IE mouseup check
			
			if(!this.initialized && (Math.abs(this._MP.left-e.pageX) >= o.distance || Math.abs(this._MP.top-e.pageY) >= o.distance)) {
				if(this.options.start) this.options.start.call(this.options.executor || this, e, this.element);
				this.initialized = true;
			} else {
				if(!this.initialized) return false;
			}

			if(o.drag) o.drag.call(this.options.executor || this, e, this.element);
			return false;
			
		}
	});

 })(jQuery);




/***************************************************************************

								 UI.DRAGGABLE.JS

****************************************************************************/



(function($) {

	$.fn.extend({
		draggable: function(options) {
			var args = Array.prototype.slice.call(arguments, 1);

			return this.each(function() {
				if (typeof options == "string") {
					var drag = $.data(this, "ui-draggable");
					drag[options].apply(drag, args);

				} else if(!$.data(this, "ui-draggable"))
					new $.ui.draggable(this, options);
			});
		}
	});

	$.ui.draggable = function(element, options) {
		//Initialize needed constants
		var self = this;

		this.element = $(element);

		$.data(element, "ui-draggable", this);
		this.element.addClass("ui-draggable");

		//Prepare the passed options
		this.options = $.extend({}, options);
		var o = this.options;
		$.extend(o, {
			helper: o.ghosting == true ? 'clone' : (o.helper || 'original'),
			handle : o.handle ? ($(o.handle, element)[0] ? $(o.handle, element) : this.element) : this.element,
			appendTo: o.appendTo || 'parent'		
		});

		$(element).bind("setData.draggable", function(event, key, value){
			self.options[key] = value;
		}).bind("getData.draggable", function(event, key){
			return self.options[key];
		});

		//Initialize mouse events for interaction
		$(o.handle).mouseInteraction({
			executor: this,
			delay: o.delay,
			distance: o.distance || 0,
			dragPrevention: o.prevention ? o.prevention.toLowerCase().split(',') : ['input','textarea','button','select','option'],
			start: this.start,
			stop: this.stop,
			drag: this.drag,
			condition: function(e) { return !(e.target.className.indexOf("ui-resizable-handle") != -1 || this.disabled); }
		});

		//Position the node
		if(o.helper == 'original' && (this.element.css('position') == 'static' || this.element.css('position') == ''))
			this.element.css('position', 'relative');

	};

	$.extend($.ui.draggable.prototype, {
		plugins: {},
		ui: function(e) {
			return {
				helper: this.helper,
				position: this.position,
				absolutePosition: this.positionAbs,
				instance: this,
				options: this.options					
			};
		},
		propagate: function(n,e) {
			$.ui.plugin.call(this, n, [e, this.ui()]);
			return this.element.triggerHandler(n == "drag" ? n : "drag"+n, [e, this.ui()], this.options[n]);
		},
		destroy: function() {
			this.handle.removeMouseInteraction();
			this.element
				.removeClass("ui-draggable ui-draggable-disabled")
				.removeData("ui-draggable")
				.unbind(".draggable");
		},
		enable: function() {
			this.element.removeClass("ui-draggable-disabled");
			this.disabled = false;
		},
		disable: function() {
			this.element.addClass("ui-draggable-disabled");
			this.disabled = true;
		},
		recallOffset: function(e) {

			var elementPosition = { left: this.elementOffset.left - this.offsetParentOffset.left, top: this.elementOffset.top - this.offsetParentOffset.top };
			var r = this.helper.css('position') == 'relative';

			//Generate the original position
			this.originalPosition = {
				left: (r ? parseInt(this.helper.css('left'),10) || 0 : elementPosition.left + (this.offsetParent[0] == document.body ? 0 : this.offsetParent[0].scrollLeft)),
				top: (r ? parseInt(this.helper.css('top'),10) || 0 : elementPosition.top + (this.offsetParent[0] == document.body ? 0 : this.offsetParent[0].scrollTop))
			};

			//Generate a flexible offset that will later be subtracted from e.pageX/Y
			this.offset = {left: this._pageX - this.originalPosition.left, top: this._pageY - this.originalPosition.top };

		},
		start: function(e) {

			var o = this.options;
			if($.ui.ddmanager) $.ui.ddmanager.current = this;

			//Create and append the visible helper
			this.helper = typeof o.helper == 'function' ? $(o.helper.apply(this.element[0], [e])) : (o.helper == 'clone' ? this.element.clone().appendTo((o.appendTo == 'parent' ? this.element[0].parentNode : o.appendTo)) : this.element);
			if(this.helper[0] != this.element[0]) this.helper.css('position', 'absolute');
			if(!this.helper.parents('body').length) this.helper.appendTo((o.appendTo == 'parent' ? this.element[0].parentNode : o.appendTo));

			//Find out the next positioned parent
			this.offsetParent = (function(cp) {
				while(cp) {
					if(cp.style && (/(absolute|relative|fixed)/).test($.css(cp,'position'))) return $(cp);
					cp = cp.parentNode ? cp.parentNode : null;
				}; return $("body");		
			})(this.helper[0].parentNode);

			//Prepare variables for position generation
			this.elementOffset = this.element.offset();
			this.offsetParentOffset = this.offsetParent.offset();
			var elementPosition = { left: this.elementOffset.left - this.offsetParentOffset.left, top: this.elementOffset.top - this.offsetParentOffset.top };
			this._pageX = e.pageX; this._pageY = e.pageY;
			this.clickOffset = { left: e.pageX - this.elementOffset.left, top: e.pageY - this.elementOffset.top };
			var r = this.helper.css('position') == 'relative';

			//Generate the original position
			this.originalPosition = {
				left: (r ? parseInt(this.helper.css('left'),10) || 0 : elementPosition.left + (this.offsetParent[0] == document.body ? 0 : this.offsetParent[0].scrollLeft)),
				top: (r ? parseInt(this.helper.css('top'),10) || 0 : elementPosition.top + (this.offsetParent[0] == document.body ? 0 : this.offsetParent[0].scrollTop))
			};

			//If we have a fixed element, we must subtract the scroll offset again
			if(this.element.css('position') == 'fixed') {
				this.originalPosition.top -= this.offsetParent[0] == document.body ? $(document).scrollTop() : this.offsetParent[0].scrollTop;
				this.originalPosition.left -= this.offsetParent[0] == document.body ? $(document).scrollLeft() : this.offsetParent[0].scrollLeft;
			}

			//Generate a flexible offset that will later be subtracted from e.pageX/Y
			this.offset = {left: e.pageX - this.originalPosition.left, top: e.pageY - this.originalPosition.top };

			//Call plugins and callbacks
			this.propagate("start", e);

			this.helperProportions = { width: this.helper.outerWidth(), height: this.helper.outerHeight() };
			if ($.ui.ddmanager && !o.dropBehaviour) $.ui.ddmanager.prepareOffsets(this, e);

			//If we have something in cursorAt, we'll use it
			if(o.cursorAt) {
				if(o.cursorAt.top != undefined || o.cursorAt.bottom != undefined) {
					this.offset.top -= this.clickOffset.top - (o.cursorAt.top != undefined ? o.cursorAt.top : (this.helperProportions.height - o.cursorAt.bottom));
					this.clickOffset.top = (o.cursorAt.top != undefined ? o.cursorAt.top : (this.helperProportions.height - o.cursorAt.bottom));
				}
				if(o.cursorAt.left != undefined || o.cursorAt.right != undefined) {
					this.offset.left -= this.clickOffset.left - (o.cursorAt.left != undefined ? o.cursorAt.left : (this.helperProportions.width - o.cursorAt.right));
					this.clickOffset.left = (o.cursorAt.left != undefined ? o.cursorAt.left : (this.helperProportions.width - o.cursorAt.right));
				}
			}

			return false;

		},
		clear: function() {
			if($.ui.ddmanager) $.ui.ddmanager.current = null;
			this.helper = null;
		},
		stop: function(e) {

			//If we are using droppables, inform the manager about the drop
			if ($.ui.ddmanager && !this.options.dropBehaviour)
				$.ui.ddmanager.drop(this, e);

			//Call plugins and trigger callbacks
			this.propagate("stop", e);

			if(this.cancelHelperRemoval) return false;			
			if(this.options.helper != 'original') this.helper.remove();
			this.clear();

			return false;
		},
		drag: function(e) {

			//Compute the helpers position
			this.position = { top: e.pageY - this.offset.top, left: e.pageX - this.offset.left };
			this.positionAbs = { left: e.pageX - this.clickOffset.left, top: e.pageY - this.clickOffset.top };

			//Call plugins and callbacks			
			this.position = this.propagate("drag", e) || this.position;

			this.helper.css({ left: this.position.left+'px', top: this.position.top+'px' }); // Stick the helper to the cursor
			if($.ui.ddmanager) $.ui.ddmanager.drag(this, e);
			return false;

		}
	});

})(jQuery);






/***************************************************************************

								 UI.DRAGGABLE.EXT.JS

****************************************************************************/



/*
 * 'this' -> original element
 * 1. argument: browser event
 * 2.argument: ui object
 */

(function($) {

	$.ui.plugin.add("draggable", "cursor", {
		start: function(e,ui) {
			var t = $('body');
			if (t.css("cursor")) ui.options._cursor = t.css("cursor");
			t.css("cursor", ui.options.cursor);
		},
		stop: function(e,ui) {
			if (ui.options._cursor) $('body').css("cursor", ui.options._cursor);
		}
	});

	$.ui.plugin.add("draggable", "zIndex", {
		start: function(e,ui) {
			var t = $(ui.helper);
			if(t.css("zIndex")) ui.options._zIndex = t.css("zIndex");
			t.css('zIndex', ui.options.zIndex);
		},
		stop: function(e,ui) {
			if(ui.options._zIndex) $(ui.helper).css('zIndex', ui.options._zIndex);
		}
	});

	$.ui.plugin.add("draggable", "opacity", {
		start: function(e,ui) {
			var t = $(ui.helper);
			if(t.css("opacity")) ui.options._opacity = t.css("opacity");
			t.css('opacity', ui.options.opacity);
		},
		stop: function(e,ui) {
			if(ui.options._opacity) $(ui.helper).css('opacity', ui.options._opacity);
		}
	});


	$.ui.plugin.add("draggable", "revert", {
		stop: function(e,ui) {
			var self = ui.instance;
			self.cancelHelperRemoval = true;
			$(ui.helper).animate({ left: self.originalPosition.left, top: self.originalPosition.top }, parseInt(ui.options.revert, 10) || 500, function() {
				if(ui.options.helper != 'original') self.helper.remove();
				self.clear();
			});
		}
	});

	$.ui.plugin.add("draggable", "iframeFix", {
		start: function(e,ui) {

			var o = ui.options;
			if(ui.instance.slowMode) return; // Make clones on top of iframes (only if we are not in slowMode)
			
			if(o.iframeFix.constructor == Array) {
				for(var i=0;i<o.iframeFix.length;i++) {
					var co = $(o.iframeFix[i]).offset({ border: false });
					$('<div class="DragDropIframeFix"" style="background: #fff;"></div>').css("width", $(o.iframeFix[i])[0].offsetWidth+"px").css("height", $(o.iframeFix[i])[0].offsetHeight+"px").css("position", "absolute").css("opacity", "0.001").css("z-index", "1000").css("top", co.top+"px").css("left", co.left+"px").appendTo("body");
				}		
			} else {
				$("iframe").each(function() {					
					var co = $(this).offset({ border: false });
					$('<div class="DragDropIframeFix" style="background: #fff;"></div>').css("width", this.offsetWidth+"px").css("height", this.offsetHeight+"px").css("position", "absolute").css("opacity", "0.001").css("z-index", "1000").css("top", co.top+"px").css("left", co.left+"px").appendTo("body");
				});							
			}

		},
		stop: function(e,ui) {
			if(ui.options.iframeFix) $("div.DragDropIframeFix").each(function() { this.parentNode.removeChild(this); }); //Remove frame helpers	
		}
	});
	
	$.ui.plugin.add("draggable", "containment", {
		start: function(e,ui) {

			var o = ui.options;
			if((o.containment.left != undefined || o.containment.constructor == Array) && !o._containment) return;
			if(!o._containment) o._containment = o.containment;

			if(o._containment == 'parent') o._containment = this[0].parentNode;
			if(o._containment == 'document') {
				o.containment = [
					0,
					0,
					$(document).width(),
					($(document).height() || document.body.parentNode.scrollHeight)
				];
			} else { //I'm a node, so compute top/left/right/bottom

				var ce = $(o._containment)[0];
				var co = $(o._containment).offset();

				o.containment = [
					co.left,
					co.top,
					co.left+(ce.offsetWidth || ce.scrollWidth),
					co.top+(ce.offsetHeight || ce.scrollHeight)
				];
			}

		},
		drag: function(e,ui) {

			var o = ui.options;
			var h = ui.helper;
			var c = o.containment;
			var self = ui.instance;
			
			if(c.constructor == Array) {
				if((ui.absolutePosition.left < c[0])) self.position.left = c[0] - (self.offset.left - self.clickOffset.left);
				if((ui.absolutePosition.top < c[1])) self.position.top = c[1] - (self.offset.top - self.clickOffset.top);
				if(ui.absolutePosition.left - c[2] + self.helperProportions.width >= 0) self.position.left = c[2] - (self.offset.left - self.clickOffset.left) - self.helperProportions.width;
				if(ui.absolutePosition.top - c[3] + self.helperProportions.height >= 0) self.position.top = c[3] - (self.offset.top - self.clickOffset.top) - self.helperProportions.height;
			} else {
				if((ui.position.left < c.left)) self.position.left = c.left;
				if((ui.position.top < c.top)) self.position.top = c.top;
				if(ui.position.left - self.offsetParent.innerWidth() + self.helperProportions.width + c.right + (parseInt(self.offsetParent.css("borderLeftWidth"), 10) || 0) + (parseInt(self.offsetParent.css("borderRightWidth"), 10) || 0) >= 0) self.position.left = self.offsetParent.innerWidth() - self.helperProportions.width - c.right - (parseInt(self.offsetParent.css("borderLeftWidth"), 10) || 0) - (parseInt(self.offsetParent.css("borderRightWidth"), 10) || 0);
				if(ui.position.top - self.offsetParent.innerHeight() + self.helperProportions.height + c.bottom + (parseInt(self.offsetParent.css("borderTopWidth"), 10) || 0) + (parseInt(self.offsetParent.css("borderBottomWidth"), 10) || 0) >= 0) self.position.top = self.offsetParent.innerHeight() - self.helperProportions.height - c.bottom - (parseInt(self.offsetParent.css("borderTopWidth"), 10) || 0) - (parseInt(self.offsetParent.css("borderBottomWidth"), 10) || 0);
			}

		}
	});

	$.ui.plugin.add("draggable", "grid", {
		drag: function(e,ui) {
			var o = ui.options;
			ui.instance.position.left = ui.instance.originalPosition.left + Math.round((e.pageX - ui.instance._pageX) / o.grid[0]) * o.grid[0];
			ui.instance.position.top = ui.instance.originalPosition.top + Math.round((e.pageY - ui.instance._pageY) / o.grid[1]) * o.grid[1];
		}
	});

	$.ui.plugin.add("draggable", "axis", {
		drag: function(e,ui) {
			var o = ui.options;
			if(o.constraint) o.axis = o.constraint; //Legacy check
			o.axis == 'x' ? ui.instance.position.top = ui.instance.originalPosition.top : ui.instance.position.left = ui.instance.originalPosition.left;
		}
	});

	$.ui.plugin.add("draggable", "scroll", {
		start: function(e,ui) {
			var o = ui.options;
			o.scrollSensitivity	= o.scrollSensitivity || 20;
			o.scrollSpeed		= o.scrollSpeed || 20;

			ui.instance.overflowY = function(el) {
				do { if(/auto|scroll/.test(el.css('overflow')) || (/auto|scroll/).test(el.css('overflow-y'))) return el; el = el.parent(); } while (el[0].parentNode);
				return $(document);
			}(this);
			ui.instance.overflowX = function(el) {
				do { if(/auto|scroll/.test(el.css('overflow')) || (/auto|scroll/).test(el.css('overflow-x'))) return el; el = el.parent(); } while (el[0].parentNode);
				return $(document);
			}(this);
		},
		drag: function(e,ui) {
			
			var o = ui.options;
			var i = ui.instance;

			if(i.overflowY[0] != document && i.overflowY[0].tagName != 'HTML') {
				if(i.overflowY[0].offsetHeight - (ui.position.top - i.overflowY[0].scrollTop + i.clickOffset.top) < o.scrollSensitivity)
					i.overflowY[0].scrollTop = i.overflowY[0].scrollTop + o.scrollSpeed;
				if((ui.position.top - i.overflowY[0].scrollTop + i.clickOffset.top) < o.scrollSensitivity)
					i.overflowY[0].scrollTop = i.overflowY[0].scrollTop - o.scrollSpeed;				
			} else {
				//$(document.body).append('<p>'+(e.pageY - $(document).scrollTop())+'</p>');
				if(e.pageY - $(document).scrollTop() < o.scrollSensitivity)
					$(document).scrollTop($(document).scrollTop() - o.scrollSpeed);
				if($(window).height() - (e.pageY - $(document).scrollTop()) < o.scrollSensitivity)
					$(document).scrollTop($(document).scrollTop() + o.scrollSpeed);
			}
			
			if(i.overflowX[0] != document && i.overflowX[0].tagName != 'HTML') {
				if(i.overflowX[0].offsetWidth - (ui.position.left - i.overflowX[0].scrollLeft + i.clickOffset.left) < o.scrollSensitivity)
					i.overflowX[0].scrollLeft = i.overflowX[0].scrollLeft + o.scrollSpeed;
				if((ui.position.top - i.overflowX[0].scrollLeft + i.clickOffset.left) < o.scrollSensitivity)
					i.overflowX[0].scrollLeft = i.overflowX[0].scrollLeft - o.scrollSpeed;				
			} else {
				if(e.pageX - $(document).scrollLeft() < o.scrollSensitivity)
					$(document).scrollLeft($(document).scrollLeft() - o.scrollSpeed);
				if($(window).width() - (e.pageX - $(document).scrollLeft()) < o.scrollSensitivity)
					$(document).scrollLeft($(document).scrollLeft() + o.scrollSpeed);
			}
			
			ui.instance.recallOffset(e);

		}
	});
	
	$.ui.plugin.add("draggable", "snap", {
		start: function(e,ui) {
			
			ui.instance.snapElements = [];
			$(ui.options.snap === true ? '.ui-draggable' : ui.options.snap).each(function() {
				var $t = $(this); var $o = $t.offset();
				if(this != ui.instance.element[0]) ui.instance.snapElements.push({
					item: this,
					width: $t.outerWidth(),
					height: $t.outerHeight(),
					top: $o.top,
					left: $o.left
				});
			});
			
		},
		drag: function(e,ui) {

			var d = ui.options.snapTolerance || 20;
			var x1 = ui.absolutePosition.left, x2 = x1 + ui.instance.helperProportions.width,
			    y1 = ui.absolutePosition.top, y2 = y1 + ui.instance.helperProportions.height;

			for (var i = ui.instance.snapElements.length - 1; i >= 0; i--){

				var l = ui.instance.snapElements[i].left, r = l + ui.instance.snapElements[i].width, 
				    t = ui.instance.snapElements[i].top,  b = t + ui.instance.snapElements[i].height;

				//Yes, I know, this is insane ;)
				if(!((l-d < x1 && x1 < r+d && t-d < y1 && y1 < b+d) || (l-d < x1 && x1 < r+d && t-d < y2 && y2 < b+d) || (l-d < x2 && x2 < r+d && t-d < y1 && y1 < b+d) || (l-d < x2 && x2 < r+d && t-d < y2 && y2 < b+d))) continue;

				if(ui.options.snapMode != 'inner') {
					var ts = Math.abs(t - y2) <= 20;
					var bs = Math.abs(b - y1) <= 20;
					var ls = Math.abs(l - x2) <= 20;
					var rs = Math.abs(r - x1) <= 20;
					if(ts) ui.position.top = t - ui.instance.offset.top + ui.instance.clickOffset.top - ui.instance.helperProportions.height;
					if(bs) ui.position.top = b - ui.instance.offset.top + ui.instance.clickOffset.top;
					if(ls) ui.position.left = l - ui.instance.offset.left + ui.instance.clickOffset.left - ui.instance.helperProportions.width;
					if(rs) ui.position.left = r - ui.instance.offset.left + ui.instance.clickOffset.left;
				}
				
				if(ui.options.snapMode != 'outer') {
					var ts = Math.abs(t - y1) <= 20;
					var bs = Math.abs(b - y2) <= 20;
					var ls = Math.abs(l - x1) <= 20;
					var rs = Math.abs(r - x2) <= 20;
					if(ts) ui.position.top = t - ui.instance.offset.top + ui.instance.clickOffset.top;
					if(bs) ui.position.top = b - ui.instance.offset.top + ui.instance.clickOffset.top - ui.instance.helperProportions.height;
					if(ls) ui.position.left = l - ui.instance.offset.left + ui.instance.clickOffset.left;
					if(rs) ui.position.left = r - ui.instance.offset.left + ui.instance.clickOffset.left - ui.instance.helperProportions.width;
				}

			};
		}
	});

	//TODO: wrapHelper, snap

})(jQuery);



/***************************************************************************

								 UI.DROPPABLE.JS

****************************************************************************/


(function($) {

	$.fn.extend({
		droppable: function(options) {
			var args = Array.prototype.slice.call(arguments, 1);
			
			return this.each(function() {
				if (typeof options == "string") {
					var drop = $.data(this, "ui-droppable");
					drop[options].apply(drop, args);

				} else if(!$.data(this, "ui-droppable"))
					new $.ui.droppable(this, options);
			});
		}
	});

	
	$.ui.droppable = function(element, options) {

		//Initialize needed constants			
		this.element = $(element);
		$.data(element, "ui-droppable", this);
		this.element.addClass("ui-droppable");		
		
		//Prepare the passed options
		this.options = $.extend({}, options);
		var o = this.options; var accept = o.accept;
		$.extend(o, {
			accept: o.accept && o.accept.constructor == Function ? o.accept : function(d) {
				return $(d).is(accept);	
			},
			tolerance: o.tolerance || 'intersect'		
		});
		
		$(element).bind("setData.draggable", function(event, key, value){
			o[key] = value;
		}).bind("getData.draggable", function(event, key){
			return o[key];
		});
		
		//Store the droppable's proportions
		this.proportions = { width: this.element.outerWidth(), height: this.element.outerHeight() };
		
		// Add the reference and positions to the manager
		$.ui.ddmanager.droppables.push({ item: this, over: 0, out: 1 });
			
	};
	
	$.extend($.ui.droppable.prototype, {
		plugins: {},
		ui: function(c) {
			return {
				instance: this,
				draggable: c.element,
				helper: c.helper,
				position: c.position,
				absolutePosition: c.positionAbs,
				options: this.options	
			};		
		},
		destroy: function() {
			var drop = $.ui.ddmanager.droppables;
			for ( var i = 0; i < drop.length; i++ )
				if ( drop[i].item == this )
					drop.splice(i, 1);
			
			this.element
				.removeClass("ui-droppable ui-droppable-disabled")
				.removeData("ui-droppable")
				.unbind(".droppable");
		},
		enable: function() {
			this.element.removeClass("ui-droppable-disabled");
			this.disabled = false;
		},
		disable: function() {
			this.element.addClass("ui-droppable-disabled");
			this.disabled = true;
		},
		over: function(e) {

			var draggable = $.ui.ddmanager.current;
			if (!draggable || draggable.element[0] == this.element[0]) return; // Bail if draggable and droppable are same element
			
			if (this.options.accept.call(this.element,draggable.element)) {
				$.ui.plugin.call(this, 'over', [e, this.ui(draggable)]);
				this.element.triggerHandler("dropover", [e, this.ui(draggable)], this.options.over);
			}
			
		},
		out: function(e) {

			var draggable = $.ui.ddmanager.current;
			if (!draggable || draggable.element[0] == this.element[0]) return; // Bail if draggable and droppable are same element

			if (this.options.accept.call(this.element,draggable.element)) {
				$.ui.plugin.call(this, 'out', [e, this.ui(draggable)]);
				this.element.triggerHandler("dropout", [e, this.ui(draggable)], this.options.out);
			}
			
		},
		drop: function(e) {

			var draggable = $.ui.ddmanager.current;
			if (!draggable || draggable.element[0] == this.element[0]) return; // Bail if draggable and droppable are same element
			
			if(this.options.accept.call(this.element,draggable.element)) {
				$.ui.plugin.call(this, 'drop', [e, this.ui(draggable)]);
				this.element.triggerHandler("drop", [e, this.ui(draggable)], this.options.drop);
			}
			
		},
		activate: function(e) {

			var draggable = $.ui.ddmanager.current;
			$.ui.plugin.call(this, 'activate', [e, this.ui(draggable)]);
			if(draggable) this.element.triggerHandler("dropactivate", [e, this.ui(draggable)], this.options.activate);
				
		},
		deactivate: function(e) {
			
			var draggable = $.ui.ddmanager.current;
			$.ui.plugin.call(this, 'deactivate', [e, this.ui(draggable)]);
			if(draggable) this.element.triggerHandler("dropdeactivate", [e, this.ui(draggable)], this.options.deactivate);
			
		}
	});
	
	$.ui.intersect = function(draggable, droppable, toleranceMode) {

		if (!droppable.offset) return false;
		
		var x1 = draggable.positionAbs.left, x2 = x1 + draggable.helperProportions.width,
		    y1 = draggable.positionAbs.top, y2 = y1 + draggable.helperProportions.height;
		var l = droppable.offset.left, r = l + droppable.item.proportions.width, 
		    t = droppable.offset.top,  b = t + droppable.item.proportions.height;

		switch (toleranceMode) {
			case 'fit':
				return (   l < x1 && x2 < r
					&& t < y1 && y2 < b);
				break;
			case 'intersect':
				return (   l < x1 + (draggable.helperProportions.width  / 2)        // Right Half
					&&     x2 - (draggable.helperProportions.width  / 2) < r    // Left Half
					&& t < y1 + (draggable.helperProportions.height / 2)        // Bottom Half
					&&     y2 - (draggable.helperProportions.height / 2) < b ); // Top Half
				break;
			case 'pointer':
				return (   l < (draggable.positionAbs.left + draggable.clickOffset.left) && (draggable.positionAbs.left + draggable.clickOffset.left) < r
					&& t < (draggable.positionAbs.top + draggable.clickOffset.top) && (draggable.positionAbs.top + draggable.clickOffset.top) < b);
				break;
			case 'touch':
				return (   (l < x1 && x1 < r && t < y1 && y1 < b)    // Top-Left Corner
					|| (l < x1 && x1 < r && t < y2 && y2 < b)    // Bottom-Left Corner
					|| (l < x2 && x2 < r && t < y1 && y1 < b)    // Top-Right Corner
					|| (l < x2 && x2 < r && t < y2 && y2 < b) ); // Bottom-Right Corner
				break;
			default:
				return false;
				break;
			}
		
	};
	
	/*
		This manager tracks offsets of draggables and droppables
	*/
	$.ui.ddmanager = {
		current: null,
		droppables: [],
		prepareOffsets: function(t, e) {

			var m = $.ui.ddmanager.droppables;
			for (var i = 0; i < m.length; i++) {
				
				if(m[i].item.disabled || (t && !m[i].item.options.accept.call(m[i].item.element,t.element))) continue;
				m[i].offset = $(m[i].item.element).offset();
				
				if(t) m[i].item.activate.call(m[i].item, e); //Activate the droppable if used directly from draggables
					
			}
			
		},
		drop: function(draggable, e) {
			
			$.each($.ui.ddmanager.droppables, function() {
				
				if (!this.item.disabled && $.ui.intersect(draggable, this, this.item.options.tolerance))
					this.item.drop.call(this.item, e);
					
				if (!this.item.disabled && this.item.options.accept.call(this.item.element,draggable.element)) {
					this.out = 1; this.over = 0;
					this.item.deactivate.call(this.item, e);
				}
				
			});
			
		},
		drag: function(draggable, e) {
			
			//If you have a highly dynamic page, you might try this option. It renders positions every time you move the mouse.
			if(draggable.options.refreshPositions) $.ui.ddmanager.prepareOffsets();
		
			//Run through all draggables and check their positions based on specific tolerance options
			$.each($.ui.ddmanager.droppables, function() {

				if(this.item.disabled) return false; 
				var intersects = $.ui.intersect(draggable, this, this.item.options.tolerance);

				var c = !intersects && this.over == 1 ? 'out' : (intersects && this.over == 0 ? 'over' : null);
				if(!c) return;
					
				this[c] = 1; this[c == 'out' ? 'over' : 'out'] = 0;
				this.item[c].call(this.item, e);
					
			});
			
		}
	};
	
})(jQuery);




/***************************************************************************

								 UI.DROPPABLE.EXT.JS

****************************************************************************/




(function($) {

	$.ui.plugin.add("droppable", "activeClass", {
		activate: function(e,ui) {
			$(this).addClass(ui.options.activeClass);
		},
		deactivate: function(e,ui) {
			$(this).removeClass(ui.options.activeClass);
		},
		drop: function(e,ui) {
			$(this).removeClass(ui.options.activeClass);
		}
	});

	$.ui.plugin.add("droppable", "hoverClass", {
		over: function(e,ui) {
			$(this).addClass(ui.options.hoverClass);
		},
		out: function(e,ui) {
			$(this).removeClass(ui.options.hoverClass);
		},
		drop: function(e,ui) {
			$(this).removeClass(ui.options.hoverClass);
		}
	});	

})(jQuery);



/***************************************************************************

								 UI.RESIZABLE.JS

****************************************************************************/


(function($) {
  
  $.fn.resizable = function(options) {
    return this.each(function() {
      var args = Array.prototype.slice.call(arguments, 1);
      
      if (typeof options == "string") {
        var resize = $.data(this, "ui-resizable");
        resize[options].apply(resize, args);

      } else if(!$(this).is(".ui-resizable"))
        new $.ui.resizable(this, options);
        
    });
  };
  
  $.ui.resizable = function(element, options) {
    //Initialize needed constants
    var self = this;
    
    this.element = $(element);
    
    $.data(element, "ui-resizable", this);
    this.element.addClass("ui-resizable");
    
    //Prepare the passed options
    this.options = $.extend({
      preventDefault: true,
      transparent: false,
      minWidth: 10,
      minHeight: 10,
      aspectRatio: false,
      disableSelection: true,
      preserveCursor: true,
      autohide: false
    }, options);
    
		this.options._aspectRatio = !!(this.options.aspectRatio);
		
    $(element).bind("setData.resizable", function(event, key, value){
      self.options[key] = value;
    }).bind("getData.resizable", function(event, key){
      return self.options[key];
    });
    
    var o = this.options;
    
    //Default Theme
    var aBorder = '1px solid #DEDEDE';
    
    o.defaultTheme = {
      'ui-resizable': { display: 'block' },
      'ui-resizable-handle': { position: 'absolute', background: '#F5F5F5' },
      'ui-resizable-n': { cursor: 'n-resize', height: '4px', left: '0px', right: '0px', borderTop: aBorder },
      'ui-resizable-s': { cursor: 's-resize', height: '4px', left: '0px', right: '0px', borderBottom: aBorder },
      'ui-resizable-e': { cursor: 'e-resize', width: '4px', top: '0px', bottom: '0px', borderRight: aBorder },
      'ui-resizable-w': { cursor: 'w-resize', width: '4px', top: '0px', bottom: '0px', borderLeft: aBorder },
      'ui-resizable-se': { cursor: 'se-resize', width: '4px', height: '4px', borderRight: aBorder, borderBottom: aBorder },
      'ui-resizable-sw': { cursor: 'sw-resize', width: '4px', height: '4px', borderBottom: aBorder, borderLeft: aBorder },
      'ui-resizable-ne': { cursor: 'ne-resize', width: '4px', height: '4px', borderRight: aBorder, borderTop: aBorder },
      'ui-resizable-nw': { cursor: 'nw-resize', width: '4px', height: '4px', borderLeft: aBorder, borderTop: aBorder }
    };
    
    //Position the node
    if(!o.proxy && (this.element.css('position') == 'static' || this.element.css('position') == ''))
      this.element.css('position', 'relative');
    
    var nodeName = element.nodeName;
    
    //Wrap the element if it cannot hold child nodes
    if(nodeName.match(/textarea|input|select|button|img/i)) {
      
      //Create a wrapper element and set the wrapper to the new current internal element
      this.element.wrap('<div class="ui-wrapper"  style="overflow: hidden; position: relative; width: '+this.element.outerWidth()+'px; height: '+this.element.outerHeight()+';"></div>');
      var oel = this.element; element = element.parentNode; this.element = $(element);
      
      //Move margins to the wrapper
      this.element.css({ marginLeft: oel.css("marginLeft"), marginTop: oel.css("marginTop"),
        marginRight: oel.css("marginRight"), marginBottom: oel.css("marginBottom")
      });
      
      oel.css({ marginLeft: 0, marginTop: 0, marginRight: 0, marginBottom: 0});
      
      //Prevent Safari textarea resize
      if ($.browser.safari && o.preventDefault) oel.css('resize', 'none');
      
      o.proportionallyResize = oel.css('position', 'static');
			
			// fix handlers offset
			this._proportionallyResize();
    }
    
    if(!o.handles) o.handles = !$('.ui-resizable-handle', element).length ? "e,s,se" : { n: '.ui-resizable-n', e: '.ui-resizable-e', s: '.ui-resizable-s', w: '.ui-resizable-w', se: '.ui-resizable-se', sw: '.ui-resizable-sw', ne: '.ui-resizable-ne', nw: '.ui-resizable-nw' };
    if(o.handles.constructor == String) {

      if(o.handles == 'all')
        o.handles = 'n,e,s,w,se,sw,ne,nw';
        
      var n = o.handles.split(","); o.handles = {};
      
      o.zIndex = o.zIndex || 1000;
      
      var insertions = {
				handle: 'overflow:hidden; position:absolute;',
        n: 'top: 0pt; width:100%;',
        e: 'right: 0pt; height:100%;',
        s: 'bottom: 0pt; width:100%;',
        w: 'left: 0pt; height:100%;',
        se: 'bottom: 0pt; right: 0px;',
        sw: 'bottom: 0pt; left: 0px;',
        ne: 'top: 0pt; right: 0px;',
        nw: 'top: 0pt; left: 0px;'
      };
      
      for(var i = 0; i < n.length; i++) {
        var d = jQuery.trim(n[i]), t = o.defaultTheme, hname = 'ui-resizable-'+d;
        
        var rcss = $.extend(t[hname], t['ui-resizable-handle']), 
            axis = $(["<div class=\"",hname," ui-resizable-handle\" style=\"",insertions[d], insertions.handle,"\"></div>"].join("")).css(/sw|se|ne|nw/.test(d) ? { zIndex: ++o.zIndex } : {});
        
        o.handles[d] = '.ui-resizable-'+d;
          
        this.element.append(
          //Theme detection, if not loaded, load o.defaultTheme
          axis.css( !$.ui.css(hname) ? rcss : {} )
        );
      }
    }
    
    this._renderAxis = function(target) {
      target = target || this.element;
      
      for(var i in o.handles) {
        if(o.handles[i].constructor == String) 
          o.handles[i] = $(o.handles[i], element).show();
        
        if (o.transparent)
          o.handles[i].css({opacity:0});
        
        //Apply pad to wrapper element, needed to fix axis position (textarea, inputs, scrolls)
        if (this.element.is('.ui-wrapper') && 
          nodeName.match(/textarea|input|select|button/i)) {
            
          var axis = $(o.handles[i], element), padWrapper = 0;
          
          //Checking the correct pad and border
          padWrapper = /sw|ne|nw|se|n|s/.test(i) ? axis.outerHeight() : axis.outerWidth();
          
          //The padding type i have to apply...
          var padPos = [ 'padding', 
            /ne|nw|n/.test(i) ? 'Top' :
            /se|sw|s/.test(i) ? 'Bottom' : 
            /^e$/.test(i) ? 'Right' : 'Left' ].join(""); 
          
          if (!o.transparent)
            target.css(padPos, padWrapper);
        }
        if(!$(o.handles[i]).length) continue;
      }
    };
    
    this._renderAxis(this.element);
    var handlers = $('.ui-resizable-handle', self.element);
    
    if (o.disableSelection)
      handlers.each(function(i, e) { $.ui.disableSelection(e); });
    
    //Matching axis name
    handlers.mouseover(function() {
      if (!o.resizing) {
        if (this.className) 
          var axis = this.className.match(/ui-resizable-(se|sw|ne|nw|n|e|s|w)/i);
        //Axis, default = se
        o.axis = axis && axis[1] ? axis[1] : 'se';
      }
    });
        
    //If we want to auto hide the elements
    if (o.autohide) {
      var tLoaded = $.ui.css('ui-resizable-s') || $.ui.css('ui-resizable-e');
      if (!tLoaded) handlers.hide();
      
      $(self.element).addClass("ui-resizable-autohide").hover(function(){
        if (!tLoaded) handlers.show();
        $(this).removeClass("ui-resizable-autohide");
      }, function(){
        if (!o.resizing) {
          if (!tLoaded) handlers.hide();
          $(this).addClass("ui-resizable-autohide");
        }
      });
    }
  
    //Initialize mouse events for interaction
    this.element.mouseInteraction({
      executor: this,
      delay: 0,
      distance: 0,
      dragPrevention: ['input','textarea','button','select','option'],
      start: this.start,
      stop: this.stop,
      drag: this.drag,
      condition: function(e) {
        if(this.disabled) return false;
        for(var i in this.options.handles) {
          if($(this.options.handles[i])[0] == e.target) return true;
        }
        return false;
      }
    });
  };
  
  $.extend($.ui.resizable.prototype, {
    plugins: {},
    ui: function() {
      return {
        instance: this,
        axis: this.options.axis,
        options: this.options
      };      
    },
    _proportionallyResize: function() {
			var o = this.options;

			if (!o.proportionallyResize)
				return;
			
			var prel = o.proportionallyResize;
			
      var b = [ prel.css('borderTopWidth'), prel.css('borderRightWidth'), prel.css('borderBottomWidth'), prel.css('borderLeftWidth') ];
      var p = [ prel.css('paddingTop'), prel.css('paddingRight'), prel.css('paddingBottom'), prel.css('paddingLeft') ];
      
      o.borderDif = o.borderDif || $.map(b, function(v, i) {
        var border = parseInt(v,10)||0, padding = parseInt(p[i],10)||0;
        return border + padding; 
      });
			
      prel.css({
        display: 'block', //Needed to fix height autoincrement
        height: (this.element.height() - o.borderDif[0] - o.borderDif[2]) + "px",
        width: (this.element.width() - o.borderDif[1] - o.borderDif[3]) + "px"
      });
    },
    _renderProxy: function() {
      var el = this.element, o = this.options;
      this.offset = el.offset();
      
      if(o.proxy) {
        this.helper = this.helper || $('<div style="overflow:hidden;"></div>');
				
				// fix ie6 offset
				var ie6offset = ($.browser.msie && $.browser.version < 7 ? 3 : 0);
				
        this.helper.addClass(o.proxy).css({
          width: el.outerWidth(),
          height: el.outerHeight(),
          position: 'absolute',
          left: this.offset.left - ie6offset +'px',
          top: this.offset.top - ie6offset +'px',
          zIndex: ++o.zIndex
        });
        
        this.helper.appendTo("body");
        
        if (o.disableSelection)
          $.ui.disableSelection(this.helper.get(0));
            
      } else {
        this.helper = el; 
      }
    },
    propagate: function(n,e) {
      $.ui.plugin.call(this, n, [e, this.ui()]);
      this.element.triggerHandler(n == "resize" ? n : "resize"+n, [e, this.ui()], this.options[n]);
    },
    destroy: function() {
      this.element
        .removeClass("ui-resizable ui-resizable-disabled")
        .removeMouseInteraction()
        .removeData("ui-resizable")
        .unbind(".resizable");
    },
    enable: function() {
      this.element.removeClass("ui-resizable-disabled");
      this.disabled = false;
    },
    disable: function() {
      this.element.addClass("ui-resizable-disabled");
      this.disabled = true;
    },
    start: function(e) {
      var o = this.options, iniPos = this.element.position(), el = this.element;
      o.resizing = true;
			o.documentScroll = { top: $(document).scrollTop(),	left: $(document).scrollLeft() };
			
			// buf fix #1749
      if (el.is('.ui-draggable') || (/absolute/).test(el.css('position'))) {
				// sOffset decides if document scrollOffset will be added to the top/left of the resizable element
				var sOffset = $.browser.msie && !o.containment && (/absolute/).test(el.css('position')) && !(/relative/).test(el.parent().css('position'));
				var dscrollt = sOffset ? o.documentScroll.top : 0, dscrolll = sOffset ? o.documentScroll.left : 0;
	  		el.css({ position: 'absolute', top: (iniPos.top + dscrollt),	left: (iniPos.left + dscrolll)	});
	  	}
      
      //Opera fixing relative position
      if (/relative/.test(el.css('position')) && $.browser.opera)
        el.css({ position: 'relative', top: 'auto', left: 'auto' });
      
      this._renderProxy();
      
			var curleft = parseInt(this.helper.css('left'),10) || 0, curtop = parseInt(this.helper.css('top'),10) || 0;
			
      //Store needed variables
      $.extend(o, {
        currentSize: { width: el.outerWidth(), height: el.outerHeight() },
        currentSizeDiff: { width: el.outerWidth() - el.width(), height: el.outerHeight() - el.height() },
        startMousePosition: { left: e.pageX, top: e.pageY },
        startPosition: { left: curleft, top: curtop },
        currentPosition: { left: curleft,top: curtop }
      });

			//Aspect Ratio
			var iswlt = o.currentSize.width < o.currentSize.height;
 			o.aspectRatio = (typeof o.aspectRatio == 'number') ? o.aspectRatio : Math.pow(o.currentSize.width / o.currentSize.height, iswlt ? 1 : -1);
			o.aspectRatioTarget = iswlt ? "width" : "height";
			
      if (o.preserveCursor)
        $('body').css('cursor', o.axis + '-resize');
      
      if (o.containment) {
        var oc = o.containment,
           ce = (oc instanceof jQuery) ? oc.get(0) : 
              (/parent/.test(oc)) ? el.parent().get(0) : null;
        if (ce) {
          
          var scroll = function(e, a) {
            var scroll = /top/.test(a||"top") ? 'scrollTop' : 'scrollLeft', has = false;
            if (e[scroll] > 0) return true; e[scroll] = 1;
            has = e[scroll] > 0 ? true : false; e[scroll] = 0;
            return has; 
          };
          
          var co = $(ce).offset(), ch = $(ce).innerHeight(), cw = $(ce).innerWidth();
          o.cdata = { e: ce, l: co.left, t: co.top, w: (scroll(ce, "left") ? ce.scrollWidth : cw ), h: (scroll(ce) ? ce.scrollHeight : ch) };
        }
        if (/document/.test(oc) || oc == document) o.cdata = { e: document, l: 0, t: 0, w: $(document).width(), h: $(document).height() };
      }
      this.propagate("start", e);   
      return false;
      
    },
    stop: function(e) {
      this.options.resizing = false;
      var o = this.options;
      
      if(o.proxy) {
        var style = { 
          width: (this.helper.width() - o.currentSizeDiff.width) + "px",
          height: (this.helper.height() - o.currentSizeDiff.height) + "px",
          top: ((parseInt(this.element.css('top'),10) || 0) + ((parseInt(this.helper.css('top'),10) - this.offset.top)||0)),
          left: ((parseInt(this.element.css('left'),10) || 0) + ((parseInt(this.helper.css('left'),10) - this.offset.left)||0))
        };
       	this.element.css(style);
        if (o.proxy) this._proportionallyResize();
        this.helper.remove();
      }
      
      if (o.preserveCursor)
        $('body').css('cursor', 'auto');
      
      this.propagate("stop", e);  
      return false;
      
    },
    drag: function(e) {
      //Increase performance, avoid regex
      var el = this.helper, o = this.options, props = {}, 
						self = this, pRatio = o._aspectRatio || e.shiftKey;
			
      var change = function(a,b) {
        var isth = (a=="top"||a=="height"), ishw = (a=="width"||a=="height"),
							defAxis = (o.axis=="se"||o.axis=="s"||o.axis=="e");
								 
        var mod = (e[isth ? 'pageY' : 'pageX'] - o.startMousePosition[isth ? 'top' : 'left']) * (b ? -1 : 1);
        var val = o[ishw ? 'currentSize' : 'startPosition'][a] - mod - (!o.proxy && defAxis ? o.currentSizeDiff.width : 0);
				
				//Preserve ratio
        if (pRatio) {
					var v = val * Math.pow(o.aspectRatio, (isth ? -1 : 1) * (o.aspectRatioTarget == 'height' ? 1 : -1)), locked = false;
					
					if (isth && v >= o.maxWidth || !isth && v >= o.maxHeight)	locked = true;
					if (isth && v <= o.minWidth || !isth && v <= o.minHeight)	locked = true;
						
					if (ishw && !locked)	el.css(isth ? "width" : "height", v);
					
					if (a == "top" && (o.axis == "ne" || o.axis == "nw")) {
						//el.css('top', o.startPosition['top'] - (el.outerHeight() - o.currentSize.height) );
						/*TODO*/ return;
					};
				}
				el.css(a, val);
      };
      
			var a = o.axis, tminval = 0, tmaxval;
			
      //Change the height
      if(a=="n"||a=="ne"||a=="nw") change("height");
      if(a=="s"||a=="se"||a=="sw") change("height", 1);
      
      //Measure the new height and correct against min/maxHeight
			var curheight = parseInt(el.css('height'),10)||0;
      if(o.minHeight && curheight <= o.minHeight) el.css('height', o.minHeight);  
      if(o.maxHeight && curheight >= o.maxHeight) el.css('height', o.maxHeight);
      
      //Change the top position when picking a handle at north
      if(a=="n"||a=="ne"||a=="nw") change("top", 1);
	  
      //Measure the new top position and correct against min/maxHeight
			var curtop = parseInt(el.css('top'),10)||0;
			
			tminval = (o.startPosition.top + (o.currentSize.height - o.minHeight));
			tmaxval = (o.startPosition.top + (o.currentSize.height - o.maxHeight));
      if(o.minHeight && curtop >= tminval) el.css('top', tminval);
      if(o.maxHeight && curtop <= tmaxval) el.css('top', tmaxval);
		
      //Change the width
      if(a=="e"||a=="se"||a=="ne") change("width", 1);
      if(a=="sw"||a=="w"||a=="nw") change("width");
      
      //Measure the new width and correct against min/maxWidth
			var curwidth = parseInt(el.css('width'),10)||0;
      if(o.minWidth && curwidth <= o.minWidth) el.css('width', o.minWidth);  
      if(o.maxWidth && curwidth >= o.maxWidth) el.css('width', o.maxWidth);
      
      //Change the left position when picking a handle at west
      if(a=="sw"||a=="w"||a=="nw") change("left", 1);
      
      //Measure the new left position and correct against min/maxWidth
			var curleft = parseInt(el.css('left'),10)||0;
			
			tminval = (o.startPosition.left + (o.currentSize.width - o.minWidth));
			tmaxval = (o.startPosition.left + (o.currentSize.width - o.maxWidth));
      if(o.minWidth && curleft >= tminval) el.css('left', tminval);
      if(o.maxWidth && curleft <= tmaxval) el.css('left', tmaxval);
      
      if (o.containment && o.cdata.e) {
        if (curleft < 0) {
          el.css('left', 0);
          el.css('width', curwidth + curleft);
        }
        if (curtop < 0) {
          el.css('top', 0);
          el.css('height', curheight + curtop);
        }
        if (curwidth + o.currentSizeDiff.width + curleft >= o.cdata.w) 
					el.css('width', o.cdata.w - o.currentSizeDiff.width - (curleft < 0 ? 0 : curleft));
        if (curheight + o.currentSizeDiff.height + curtop >= o.cdata.h)
					el.css('height', o.cdata.h - o.currentSizeDiff.height - (curtop < 0 ? 0 : curtop));
      }
			o.currentPosition = { left: curleft, top: curtop };
      if (!o.proxy) this._proportionallyResize();
      this.propagate("resize", e);  
      return false;
    }
  });

})(jQuery);











/***************************************************************************

								 UI.DIALOG.JS

****************************************************************************/



;(function($) {
	
	//If the UI scope is not available, add it
	$.ui = $.ui || {};

	$.fn.extend({
		dialog: function(options, data) {
			var args = Array.prototype.slice.call(arguments, 1);

			return this.each(function() {
				if (typeof options == "string") {
					var dialog = $.data(this, "ui-dialog") ||
						$.data($(this).parents(".ui-dialog:first").find(".ui-dialog-content")[0], "ui-dialog");
					dialog[options].apply(dialog, args);

				// INIT with optional options
				} else if (!$(this).is(".ui-dialog-content"))
					new $.ui.dialog(this, options);
			});
		}
	});

	$.ui.dialog = function(el, options) {
		
		this.options = options = $.extend({},
			$.ui.dialog.defaults,
			options && options.modal ? {resizable: false} : {},
			options);
		this.element = el;
		var self = this; //Do bindings

		$.data(this.element, "ui-dialog", this);
		
		$(el).bind("setData.dialog", function(event, key, value){
			options[key] = value;
		}).bind("getData.dialog", function(event, key){
			return options[key];
		});

		var uiDialogContent = $(el).addClass('ui-dialog-content');

		if (!uiDialogContent.parent().length) {
			uiDialogContent.appendTo('body');
		}
		uiDialogContent
			.wrap(document.createElement('div'))
			.wrap(document.createElement('div'));
		var uiDialogContainer = uiDialogContent.parent().addClass('ui-dialog-container').css({position: 'relative'});
		var uiDialog = this.uiDialog = uiDialogContainer.parent().hide()
			.addClass('ui-dialog')
			.css({position: 'absolute', width: options.width, height: options.height, overflow: 'hidden'}); 

		var classNames = uiDialogContent.attr('className').split(' ');

		// Add content classes to dialog, to inherit theme at top level of element
		$.each(classNames, function(i, className) {
			if (className != 'ui-dialog-content')
				uiDialog.addClass(className);
		});
		
		if (options.resizable && $.fn.resizable) {
			uiDialog.append('<div class="ui-resizable-n ui-resizable-handle"></div>')
				.append('<div class="ui-resizable-s ui-resizable-handle"></div>')
				.append('<div class="ui-resizable-e ui-resizable-handle"></div>')
				.append('<div class="ui-resizable-w ui-resizable-handle"></div>')
				.append('<div class="ui-resizable-ne ui-resizable-handle"></div>')
				.append('<div class="ui-resizable-se ui-resizable-handle"></div>')
				.append('<div class="ui-resizable-sw ui-resizable-handle"></div>')
				.append('<div class="ui-resizable-nw ui-resizable-handle"></div>');
			uiDialog.resizable({ maxWidth: options.maxWidth, maxHeight: options.maxHeight, minWidth: options.minWidth, minHeight: options.minHeight });
		}

		uiDialogContainer.prepend('<div class="ui-dialog-titlebar"></div>');
		var uiDialogTitlebar = $('.ui-dialog-titlebar', uiDialogContainer);
		var title = (options.title) ? options.title : (uiDialogContent.attr('title')) ? uiDialogContent.attr('title') : '';
		uiDialogTitlebar.append('<span class="ui-dialog-title">' + title + '</span>');
		uiDialogTitlebar.append('<a href="#" class="ui-dialog-titlebar-close"><span>X</span></a>');
		this.uiDialogTitlebarClose = $('.ui-dialog-titlebar-close', uiDialogTitlebar)
			.hover(function() { $(this).addClass('ui-dialog-titlebar-close-hover'); }, 
			       function() { $(this).removeClass('ui-dialog-titlebar-close-hover'); })
			.mousedown(function(ev) {
				ev.stopPropagation();
			})
			.click(function() {
				self.close();
				return false;
			})
			.keydown(function(ev) {
				var ESC = 27;
				ev.keyCode && ev.keyCode == ESC && self.close(); 
			});

		var l = 0;
		$.each(options.buttons, function() { l = 1; return false; });
		if (l == 1) {
			uiDialog.append('<div class="ui-dialog-buttonpane"></div>');
			var uiDialogButtonPane = $('.ui-dialog-buttonpane', uiDialog);
			$.each(options.buttons, function(name, value) {
				var btn = $(document.createElement('button')).text(name).click(value);
				uiDialogButtonPane.append(btn);
			});
		}
	
		if (options.draggable && $.fn.draggable) {
			uiDialog.draggable({
				handle: '.ui-dialog-titlebar',
				start: function() {
					self.activate();
				}
			});
		}
		uiDialog.mousedown(function() {
			self.activate();
		});
		uiDialogTitlebar.click(function() {
			self.activate();
		});
		
		// TODO: determine if this is necessary for modal dialogs
		options.bgiframe && $.fn.bgiframe && uiDialog.bgiframe();
		
		this.open = function() {
			options.modal && overlay.show(self, options.overlay);
			uiDialog.appendTo('body');
			var wnd = $(window), doc = $(document), top = doc.scrollTop(), left = doc.scrollLeft();
			if (options.position.constructor == Array) {
				// [x, y]
				top += options.position[1];
				left += options.position[0];
			} else {
				switch (options.position) {
					case 'center':
						top += (wnd.height() / 2) - (uiDialog.height() / 2);
						left += (wnd.width() / 2) - (uiDialog.width() / 2);
						break;
					case 'top':
						top += 0;
						left += (wnd.width() / 2) - (uiDialog.width() / 2);
						break;
					case 'right':
						top += (wnd.height() / 2) - (uiDialog.height() / 2);
						left += (wnd.width()) - (uiDialog.width());
						break;
					case 'bottom':
						top += (wnd.height()) - (uiDialog.height());
						left += (wnd.width() / 2) - (uiDialog.width() / 2);
						break;
					case 'left':
						top += (wnd.height() / 2) - (uiDialog.height() / 2);
						left += 0;
						break;
					default:
						//center
						top += (wnd.height() / 2) - (uiDialog.height() / 2);
						left += (wnd.width() / 2) - (uiDialog.width() / 2);
				}
			}
			top = top < doc.scrollTop() ? doc.scrollTop() : top;
			uiDialog.css({top: top, left: left});
			uiDialog.show();
			self.activate();

			// CALLBACK: open
			var openEV = null;
			var openUI = {
				options: options
			};
			this.uiDialogTitlebarClose.focus();
			$(this.element).triggerHandler("dialogopen", [openEV, openUI], options.open);
		};

		this.activate = function() {
			var maxZ = 0;
			$('.ui-dialog:visible').each(function() {
				maxZ = Math.max(maxZ, parseInt($(this).css("z-index"),10));
			});
			overlay.$el && overlay.$el.css('z-index', ++maxZ);
			uiDialog.css("z-index", ++maxZ);
		};

		this.close = function() {
			options.modal && overlay.hide();
			uiDialog.hide();

			// CALLBACK: close
			var closeEV = null;
			var closeUI = {
				options: options
			};
			$(this.element).triggerHandler("dialogclose", [closeEV, closeUI], options.close);
		};
		
		if (options.autoOpen)
			this.open();
	};
	
	$.extend($.ui.dialog, {
		defaults: {
			autoOpen: true,
			bgiframe: false,
			buttons: [],
			draggable: true,
			height: 200,
			minHeight: 100,
			minWidth: 150,
			modal: false,
			overlay: {},
			position: 'center',
			resizable: true,
			width: 300
		}
	});
	
	// This is a port of relevant pieces of Mike Alsup's blockUI plugin (http://www.malsup.com/jquery/block/)
	// duplicated here for minimal overlay functionality and no dependency on a non-UI plugin
	var overlay = {
		$el: null,
		events: $.map('focus,mousedown,mouseup,keydown,keypress,click'.split(','),
			function(e) { return e + '.ui-dialog-overlay'; }).join(' '),
		
		show: function(dialog, css) {
			if (this.$el) return;
			
			this.dialog = dialog;
			this.selects = this.ie6 && $('select:visible').css('visibility', 'hidden');
			var width = this.width();
			var height = this.height();
			this.$el = $('<div/>').appendTo(document.body)
				.addClass('ui-dialog-overlay').css($.extend({
					borderWidth: 0, margin: 0, padding: 0,
					position: 'absolute', top: 0, left: 0,
					width: width,
					height: height
				}, css));
			
			// prevent use of anchors and inputs
			$('a, :input').bind(this.events, function() {
				if ($(this).parents('.ui-dialog').length == 0) {
					dialog.uiDialogTitlebarClose.focus();
					return false;
				}
			});
			
			// allow closing by pressing the escape key
			$(document).bind('keydown.ui-dialog-overlay', function(e) {
				var ESC = 27;
				e.keyCode && e.keyCode == ESC && dialog.close(); 
			});
			
			// handle window resizing
			$overlay = this.$el;
			function resize() {
				// If the dialog is draggable and the user drags it past the
				// right edge of the window, the document becomes wider so we
				// need to stretch the overlay.  If the user then drags the
				// dialog back to the left, the document will become narrower,
				// so we need to shrink the overlay to the appropriate size.
				// This is handled by resetting the overlay to its original
				// size before setting it to the full document size.
				$overlay.css({
					width: width,
					height: height
				}).css({
					width: overlay.width(),
					height: overlay.height()
				});
			};
			$(window).bind('resize.ui-dialog-overlay', resize);
			dialog.uiDialog.is('.ui-draggable') && dialog.uiDialog.data('stop.draggable', resize);
			dialog.uiDialog.is('.ui-resizable') && dialog.uiDialog.data('stop.resizable', resize);
		},
		
		hide: function() {
			$('a, :input').add([document, window]).unbind('.ui-dialog-overlay');
			this.ie6 && this.selects.css('visibility', 'visible');
			this.$el = null;
			$('.ui-dialog-overlay').remove();
		},
		
		height: function() {
			var height;
			if (this.ie6
				// body is smaller than window
				&& ($(document.body).height() < $(window).height())
				// dialog is above the fold
				&& !(document.documentElement.scrollTop
					|| (this.dialog.uiDialog.offset().top
						+ this.dialog.uiDialog.height())
						> $(window).height())) {
				height = $(window).height();
			} else {
				height = $(document).height();
			}
			return height + 'px';
		},
		
		width: function() {
			var width;
			if (this.ie6
				// body is smaller than window
				&& ($(document.body).width() < $(window).width())
				// dialog is off to the right
				&& !(document.documentElement.scrollLeft
					|| (this.dialog.uiDialog.offset().left
						+ this.dialog.uiDialog.width())
						> $(window).width())) {
				width = $(window).width();
			} else {
				width = $(document).width();
			}
			return width + 'px';
		},
		
		// IE 6 compatibility
		ie6: $.browser.msie && $.browser.version < 7,
		selects: null
	};

})(jQuery);






/***************************************************************************

								 UI.ACCORDIAN.JS

****************************************************************************/


/*
 * jQuery UI Accordion 1.5
 * 
 * Copyright (c) 2007 Jörn Zaefferer
 *
 * http://docs.jquery.com/UI/Accordion
 *
 * Dual licensed under the MIT and GPL licenses:
 *   http://www.opensource.org/licenses/mit-license.php
 *   http://www.gnu.org/licenses/gpl.html
 *
 * Revision: $Id$
 *
 */

;(function($) {
	
// If the UI scope is not available, add it
$.ui = $.ui || {};

$.fn.extend({
	accordion: function(options, data) {
		var args = Array.prototype.slice.call(arguments, 1);

		return this.each(function() {
			if (typeof options == "string") {
				var accordion = $.data(this, "ui-accordion");
				accordion[options].apply(accordion, args);
			// INIT with optional options
			} else if (!$(this).is(".ui-accordion"))
				$.data(this, "ui-accordion", new $.ui.accordion(this, options));
		});
	},
	// deprecated, use accordion("activate", index) instead
	activate: function(index) {
		return this.accordion("activate", index);
	}
});

$.ui.accordion = function(container, options) {
	
	// setup configuration
	this.options = options = $.extend({}, $.ui.accordion.defaults, options);
	this.element = container;
	
	$(container).addClass("ui-accordion");
	
	if ( options.navigation ) {
		var current = $(container).find("a").filter(options.navigationFilter);
		if ( current.length ) {
			if ( current.filter(options.header).length ) {
				options.active = current;
			} else {
				options.active = current.parent().parent().prev();
				current.addClass("current");
			}
		}
	}
	
	// calculate active if not specified, using the first header
	options.headers = $(container).find(options.header);
	options.active = findActive(options.headers, options.active);

	if ( options.fillSpace ) {
		var maxHeight = $(container).parent().height();
		options.headers.each(function() {
			maxHeight -= $(this).outerHeight();
		});
		var maxPadding = 0;
		options.headers.next().each(function() {
			maxPadding = Math.max(maxPadding, $(this).innerHeight() - $(this).height());
		}).height(maxHeight - maxPadding);
	} else if ( options.autoheight ) {
		var maxHeight = 0;
		options.headers.next().each(function() {
			maxHeight = Math.max(maxHeight, $(this).outerHeight());
		}).height(maxHeight);
	}

	options.headers
		.not(options.active || "")
		.next()
		.hide();
	options.active.parent().andSelf().addClass(options.selectedClass);
	
	if (options.event)
		$(container).bind((options.event || "") + ".ui-accordion", clickHandler);
};

$.ui.accordion.prototype = {
	activate: function(index) {
		// call clickHandler with custom event
		clickHandler.call(this.element, {
			target: findActive( this.options.headers, index )[0]
		});
	},
	
	enable: function() {
		this.options.disabled = false;
	},
	disable: function() {
		this.options.disabled = true;
	},
	destroy: function() {
		this.options.headers.next().css("display", "");
		if ( this.options.fillSpace || this.options.autoheight ) {
			this.options.headers.next().css("height", "");
		}
		$.removeData(this.element, "ui-accordion");
		$(this.element).removeClass("ui-accordion").unbind(".ui-accordion");
	}
};

function scopeCallback(callback, scope) {
	return function() {
		return callback.apply(scope, arguments);
	};
};

function completed(cancel) {
	// if removed while animated data can be empty
	if (!$.data(this, "ui-accordion"))
		return;
	var instance = $.data(this, "ui-accordion");
	var options = instance.options;
	options.running = cancel ? 0 : --options.running;
	if ( options.running )
		return;
	if ( options.clearStyle ) {
		options.toShow.add(options.toHide).css({
			height: "",
			overflow: ""
		});
	}
	$(this).triggerHandler("change.ui-accordion", [options.data], options.change);
}

function toggle(toShow, toHide, data, clickedActive, down) {
	var options = $.data(this, "ui-accordion").options;
	options.toShow = toShow;
	options.toHide = toHide;
	options.data = data;
	var complete = scopeCallback(completed, this);
	
	// count elements to animate
	options.running = toHide.size() == 0 ? toShow.size() : toHide.size();
	
	if ( options.animated ) {
		if ( !options.alwaysOpen && clickedActive ) {
			$.ui.accordion.animations[options.animated]({
				toShow: jQuery([]),
				toHide: toHide,
				complete: complete,
				down: down,
				autoheight: options.autoheight
			});
		} else {
			$.ui.accordion.animations[options.animated]({
				toShow: toShow,
				toHide: toHide,
				complete: complete,
				down: down,
				autoheight: options.autoheight
			});
		}
	} else {
		if ( !options.alwaysOpen && clickedActive ) {
			toShow.toggle();
		} else {
			toHide.hide();
			toShow.show();
		}
		complete(true);
	}
}

function clickHandler(event) {
	var options = $.data(this, "ui-accordion").options;
	if (options.disabled)
		return false;
	
	// called only when using activate(false) to close all parts programmatically
	if ( !event.target && !options.alwaysOpen ) {
		options.active.parent().andSelf().toggleClass(options.selectedClass);
		var toHide = options.active.next(),
			data = {
				instance: this,
				options: options,
				newHeader: jQuery([]),
				oldHeader: options.active,
				newContent: jQuery([]),
				oldContent: toHide
			},
			toShow = options.active = $([]);
		toggle.call(this, toShow, toHide, data );
		return false;
	}
	// get the click target
	var clicked = $(event.target);
	
	// due to the event delegation model, we have to check if one
	// of the parent elements is our actual header, and find that
	if ( clicked.parents(options.header).length )
		while ( !clicked.is(options.header) )
			clicked = clicked.parent();
	
	var clickedActive = clicked[0] == options.active[0];
	
	// if animations are still active, or the active header is the target, ignore click
	if (options.running || (options.alwaysOpen && clickedActive))
		return false;
	if (!clicked.is(options.header))
		return;

	// switch classes
	options.active.parent().andSelf().toggleClass(options.selectedClass);
	if ( !clickedActive ) {
		clicked.parent().andSelf().addClass(options.selectedClass);
	}

	// find elements to show and hide
	var toShow = clicked.next(),
		toHide = options.active.next(),
		//data = [clicked, options.active, toShow, toHide],
		data = {
			instance: this,
			options: options,
			newHeader: clicked,
			oldHeader: options.active,
			newContent: toShow,
			oldContent: toHide
		},
		down = options.headers.index( options.active[0] ) > options.headers.index( clicked[0] );
	
	options.active = clickedActive ? $([]) : clicked;
	toggle.call(this, toShow, toHide, data, clickedActive, down );

	return false;
};

function findActive(headers, selector) {
	return selector != undefined
		? typeof selector == "number"
			? headers.filter(":eq(" + selector + ")")
			: headers.not(headers.not(selector))
		: selector === false
			? $([])
			: headers.filter(":eq(0)");
}

$.extend($.ui.accordion, {
	defaults: {
		selectedClass: "selected",
		alwaysOpen: true,
		animated: 'slide',
		event: "click",
		header: "a",
		autoheight: true,
		running: 0,
		navigationFilter: function() {
			return this.href.toLowerCase() == location.href.toLowerCase();
		}
	},
	animations: {
		slide: function(options, additions) {
			options = $.extend({
				easing: "swing",
				duration: 300
			}, options, additions);
			if ( !options.toHide.size() ) {
				options.toShow.animate({height: "show"}, options);
				return;
			}
			var hideHeight = options.toHide.height(),
				showHeight = options.toShow.height(),
				difference = showHeight / hideHeight;
			options.toShow.css({ height: 0, overflow: 'hidden' }).show();
			options.toHide.filter(":hidden").each(options.complete).end().filter(":visible").animate({height:"hide"},{
				step: function(now) {
					var current = (hideHeight - now) * difference;
					if ($.browser.msie || $.browser.opera) {
						current = Math.ceil(current);
					}
					options.toShow.height( current );
				},
				duration: options.duration,
				easing: options.easing,
				complete: function() {
					if ( !options.autoheight ) {
						options.toShow.css("height", "auto");
					}
					options.complete();
				}
			});
		},
		bounceslide: function(options) {
			this.slide(options, {
				easing: options.down ? "bounceout" : "swing",
				duration: options.down ? 1000 : 200
			});
		},
		easeslide: function(options) {
			this.slide(options, {
				easing: "easeinout",
				duration: 700
			});
		}
	}
});

})(jQuery);








/***************************************************************************

								 UI.CALENDER.JS

****************************************************************************/



/* jQuery Calendar v2.7
   Written by Marc Grabanski (m@marcgrabanski.com) and enhanced by Keith Wood (kbwood@iprimus.com.au).

   Copyright (c) 2007 Marc Grabanski (http://marcgrabanski.com/code/jquery-calendar)
   Dual licensed under the GPL (http://www.gnu.org/licenses/gpl-3.0.txt) and 
   CC (http://creativecommons.org/licenses/by/3.0/) licenses. "Share or Remix it but please Attribute the authors."
   Date: 09-03-2007  */

/* PopUp Calendar manager.
   Use the singleton instance of this class, popUpCal, to interact with the calendar.
   Settings for (groups of) calendars are maintained in an instance object
   (PopUpCalInstance), allowing multiple different settings on the same page. */
function PopUpCal() {
	this._nextId = 0; // Next ID for a calendar instance
	this._inst = []; // List of instances indexed by ID
	this._curInst = null; // The current instance in use
	this._disabledInputs = []; // List of calendar inputs that have been disabled
	this._popUpShowing = false; // True if the popup calendar is showing , false if not
	this._inDialog = false; // True if showing within a "dialog", false if not
	this.regional = []; // Available regional settings, indexed by language code
	this.regional[''] = { // Default regional settings
		clearText: 'Clear', // Display text for clear link
		closeText: 'Close', // Display text for close link
		prevText: '&lt;Prev', // Display text for previous month link
		nextText: 'Next&gt;', // Display text for next month link
		currentText: 'Today', // Display text for current month link
		dayNames: ['Su','Mo','Tu','We','Th','Fr','Sa'], // Names of days starting at Sunday
		monthNames: ['January','February','March','April','May','June',
			'July','August','September','October','November','December'], // Names of months
		dateFormat: 'DMY/' // First three are day, month, year in the required order,
			// fourth (optional) is the separator, e.g. US would be 'MDY/', ISO would be 'YMD-'
	};
	this._defaults = { // Global defaults for all the calendar instances
		autoPopUp: 'focus', // 'focus' for popup on focus,
			// 'button' for trigger button, or 'both' for either
		defaultDate: null, // Used when field is blank: actual date,
			// +/-number for offset from today, null for today
		appendText: '', // Display text following the input box, e.g. showing the format
		buttonText: '...', // Text for trigger button
		buttonImage: '', // URL for trigger button image
		buttonImageOnly: false, // True if the image appears alone, false if it appears on a button
		closeAtTop: true, // True to have the clear/close at the top,
			// false to have them at the bottom
		hideIfNoPrevNext: false, // True to hide next/previous month links
			// if not applicable, false to just disable them
		changeMonth: true, // True if month can be selected directly, false if only prev/next
		changeYear: true, // True if year can be selected directly, false if only prev/next
		yearRange: '-10:+10', // Range of years to display in drop-down,
			// either relative to current year (-nn:+nn) or absolute (nnnn:nnnn)
		firstDay: 0, // The first day of the week, Sun = 0, Mon = 1, ...
		changeFirstDay: true, // True to click on day name to change, false to remain as set
		showOtherMonths: false, // True to show dates in other months, false to leave blank
		minDate: null, // The earliest selectable date, or null for no limit
		maxDate: null, // The latest selectable date, or null for no limit
		speed: 'medium', // Speed of display/closure
		customDate: null, // Function that takes a date and returns an array with
			// [0] = true if selectable, false if not,
			// [1] = custom CSS class name(s) or '', e.g. popUpCal.noWeekends
		fieldSettings: null, // Function that takes an input field and
			// returns a set of custom settings for the calendar
		onSelect: null // Define a callback function when a date is selected
	};
	$.extend(this._defaults, this.regional['']);
	this._calendarDiv = $('<div id="calendar_div"></div>');
	$(document.body).append(this._calendarDiv);
	$(document.body).mousedown(this._checkExternalClick);
}

$.extend(PopUpCal.prototype, {
	/* Class name added to elements to indicate already configured with a calendar. */
	markerClassName: 'hasCalendar',
	
	/* Register a new calendar instance - with custom settings. */
	_register: function(inst) {
		var id = this._nextId++;
		this._inst[id] = inst;
		return id;
	},

	/* Retrieve a particular calendar instance based on its ID. */
	_getInst: function(id) {
		return this._inst[id] || id;
	},

	/* Override the default settings for all instances of the calendar. 
	   @param  settings  object - the new settings to use as defaults (anonymous object)
	   @return void */
	setDefaults: function(settings) {
		extendRemove(this._defaults, settings || {});
	},

	/* Handle keystrokes. */
	_doKeyDown: function(e) {
		var inst = popUpCal._getInst(this._calId);
		if (popUpCal._popUpShowing) {
			switch (e.keyCode) {
				case 9:  popUpCal.hideCalendar(inst, '');
						break; // hide on tab out
				case 13: popUpCal._selectDate(inst);
						break; // select the value on enter
				case 27: popUpCal.hideCalendar(inst, inst._get('speed'));
						break; // hide on escape
				case 33: popUpCal._adjustDate(inst, -1, (e.ctrlKey ? 'Y' : 'M'));
						break; // previous month/year on page up/+ ctrl
				case 34: popUpCal._adjustDate(inst, +1, (e.ctrlKey ? 'Y' : 'M'));
						break; // next month/year on page down/+ ctrl
				case 35: if (e.ctrlKey) popUpCal._clearDate(inst);
						break; // clear on ctrl+end
				case 36: if (e.ctrlKey) popUpCal._gotoToday(inst);
						break; // current on ctrl+home
				case 37: if (e.ctrlKey) popUpCal._adjustDate(inst, -1, 'D');
						break; // -1 day on ctrl+left
				case 38: if (e.ctrlKey) popUpCal._adjustDate(inst, -7, 'D');
						break; // -1 week on ctrl+up
				case 39: if (e.ctrlKey) popUpCal._adjustDate(inst, +1, 'D');
						break; // +1 day on ctrl+right
				case 40: if (e.ctrlKey) popUpCal._adjustDate(inst, +7, 'D');
						break; // +1 week on ctrl+down
			}
		}
		else if (e.keyCode == 36 && e.ctrlKey) { // display the calendar on ctrl+home
			popUpCal.showFor(this);
		}
	},

	/* Filter entered characters. */
	_doKeyPress: function(e) {
		var inst = popUpCal._getInst(this._calId);
		var chr = String.fromCharCode(e.charCode == undefined ? e.keyCode : e.charCode);
		return (chr < ' ' || chr == inst._get('dateFormat').charAt(3) ||
			(chr >= '0' && chr <= '9')); // only allow numbers and separator
	},

	/* Attach the calendar to an input field. */
	_connectCalendar: function(target, inst) {
		var input = $(target);
		if (this._hasClass(input, this.markerClassName)) {
			return;
		}
		var appendText = inst._get('appendText');
		if (appendText) {
			input.after('<span class="calendar_append">' + appendText + '</span>');
		}
		var autoPopUp = inst._get('autoPopUp');
		if (autoPopUp == 'focus' || autoPopUp == 'both') { // pop-up calendar when in the marked field
			input.focus(this.showFor);
		}
		if (autoPopUp == 'button' || autoPopUp == 'both') { // pop-up calendar when button clicked
			var buttonText = inst._get('buttonText');
			var buttonImage = inst._get('buttonImage');
			var buttonImageOnly = inst._get('buttonImageOnly');
			var trigger = $(buttonImageOnly ? '<img class="calendar_trigger" src="' +
				buttonImage + '" alt="' + buttonText + '" title="' + buttonText + '"/>' :
				'<button type="button" class="calendar_trigger">' + (buttonImage != '' ?
				'<img src="' + buttonImage + '" alt="' + buttonText + '" title="' + buttonText + '"/>' :
				buttonText) + '</button>');
			input.wrap('<span class="calendar_wrap"></span>').after(trigger);
			trigger.click(this.showFor);
		}
		input.addClass(this.markerClassName).keydown(this._doKeyDown).keypress(this._doKeyPress);
		input[0]._calId = inst._id;
	},

	/* Attach an inline calendar to a div. */
	_inlineCalendar: function(target, inst) {
		var input = $(target);
		if (this._hasClass(input, this.markerClassName)) {
			return;
		}
		input.addClass(this.markerClassName).append(inst._calendarDiv);
		input[0]._calId = inst._id;
	},

	/* Does this element have a particular class? */
	_hasClass: function(element, className) {
		var classes = element.attr('class');
		return (classes && classes.indexOf(className) > -1);
	},

	/* Pop-up the calendar in a "dialog" box.
	   @param  dateText  string - the initial date to display (in the current format)
	   @param  onSelect  function - the function(dateText) to call when a date is selected
	   @param  settings  object - update the dialog calendar instance's settings (anonymous object)
	   @param  pos       int[2] - coordinates for the dialog's position within the screen
			leave empty for default (screen centre)
	   @return void */
	dialogCalendar: function(dateText, onSelect, settings, pos) {
		var inst = this._dialogInst; // internal instance
		if (!inst) {
			inst = this._dialogInst = new PopUpCalInstance({}, false);
			this._dialogInput = $('<input type="text" size="1" style="position: absolute; top: -100px;"/>');
			this._dialogInput.keydown(this._doKeyDown);
			$('body').append(this._dialogInput);
			this._dialogInput[0]._calId = inst._id;
		}
		extendRemove(inst._settings, settings || {});
		this._dialogInput.val(dateText);
		
		/*	Cross Browser Positioning */
		if (self.innerHeight) { // all except Explorer
			windowWidth = self.innerWidth;
			windowHeight = self.innerHeight;
		} else if (document.documentElement && document.documentElement.clientHeight) { // Explorer 6 Strict Mode
			windowWidth = document.documentElement.clientWidth;
			windowHeight = document.documentElement.clientHeight;
		} else if (document.body) { // other Explorers
			windowWidth = document.body.clientWidth;
			windowHeight = document.body.clientHeight;
		} 
		this._pos = pos || // should use actual width/height below
			[(windowWidth / 2) - 100, (windowHeight / 2) - 100];

		// move input on screen for focus, but hidden behind dialog
		this._dialogInput.css('left', this._pos[0] + 'px').css('top', this._pos[1] + 'px');
		inst._settings.onSelect = onSelect;
		this._inDialog = true;
		this._calendarDiv.addClass('calendar_dialog');
		this.showFor(this._dialogInput[0]);
		if ($.blockUI) {
			$.blockUI(this._calendarDiv);
		}
	},

	/* Enable the input field(s) for entry.
	   @param  inputs  element/object - single input field or jQuery collection of input fields
	   @return void */
	enableFor: function(inputs) {
		inputs = (inputs.jquery ? inputs : $(inputs));
		inputs.each(function() {
			this.disabled = false;
			$('../button.calendar_trigger', this).each(function() { this.disabled = false; });
			$('../img.calendar_trigger', this).css({opacity:'1.0',cursor:''});
			var $this = this;
			popUpCal._disabledInputs = $.map(popUpCal._disabledInputs,
				function(value) { return (value == $this ? null : value); }); // delete entry
		});
	},

	/* Disable the input field(s) from entry.
	   @param  inputs  element/object - single input field or jQuery collection of input fields
	   @return void */
	disableFor: function(inputs) {
		inputs = (inputs.jquery ? inputs : $(inputs));
		inputs.each(function() {
			this.disabled = true;
			$('../button.calendar_trigger', this).each(function() { this.disabled = true; });
			$('../img.calendar_trigger', this).css({opacity:'0.5',cursor:'default'});
			var $this = this;
			popUpCal._disabledInputs = $.map(popUpCal._disabledInputs,
				function(value) { return (value == $this ? null : value); }); // delete entry
			popUpCal._disabledInputs[popUpCal._disabledInputs.length] = this;
		});
	},

	/* Update the settings for a calendar attached to an input field or division.
	   @param  control   element - the input field or div/span attached to the calendar or
	                     string - the ID or other jQuery selector of the input field
	   @param  settings  object - the new settings to update
	   @return void */
	reconfigureFor: function(control, settings) {
		control = (typeof control == 'string' ? $(control)[0] : control);
		var inst = this._getInst(control._calId);
		if (inst) {
			extendRemove(inst._settings, settings || {});
			this._updateCalendar(inst);
		}
	},

	/* Set the date for a calendar attached to an input field or division.
	   @param  control  element - the input field or div/span attached to the calendar
	   @param  date     Date - the new date
	   @return void */
	setDateFor: function(control, date) {
		var inst = this._getInst(control._calId);
		if (inst) {
			inst._setDate(date);
		}
	},

	/* Retrieve the date for a calendar attached to an input field or division.
	   @param  control  element - the input field or div/span attached to the calendar
	   @return Date - the current date */
	getDateFor: function(control) {
		var inst = this._getInst(control._calId);
		return (inst ? inst._getDate() : null);
	},

	/* Pop-up the calendar for a given input field.
	   @param  target  element - the input field attached to the calendar
	   @return void */
	showFor: function(target) {
		var input = (target.nodeName && target.nodeName.toLowerCase() == 'input' ? target : this);
		if (input.nodeName.toLowerCase() != 'input') { // find from button/image trigger
			input = $('input', input.parentNode)[0];
		}
		if (popUpCal._lastInput == input) { // already here
			return;
		}
		for (var i = 0; i < popUpCal._disabledInputs.length; i++) {  // check not disabled
			if (popUpCal._disabledInputs[i] == input) {
				return;
			}
		}
		var inst = popUpCal._getInst(input._calId);
		var fieldSettings = inst._get('fieldSettings');
		extendRemove(inst._settings, (fieldSettings ? fieldSettings(input) : {}));
		popUpCal.hideCalendar(inst, '');
		popUpCal._lastInput = input;
		inst._setDateFromField(input);
		if (popUpCal._inDialog) { // hide cursor
			input.value = '';
		}
		if (!popUpCal._pos) { // position below input
			popUpCal._pos = popUpCal._findPos(input);
			popUpCal._pos[1] += input.offsetHeight;
		}
		inst._calendarDiv.css('position', (popUpCal._inDialog && $.blockUI ? 'static' : 'absolute')).
			css('left', popUpCal._pos[0] + 'px').css('top', popUpCal._pos[1] + 'px');
		popUpCal._pos = null;
		popUpCal._showCalendar(inst);
	},

	/* Construct and display the calendar. */
	_showCalendar: function(id) {
		var inst = this._getInst(id);
		popUpCal._updateCalendar(inst);
		if (!inst._inline) {
			var speed = inst._get('speed');
			inst._calendarDiv.show(speed, function() {
				popUpCal._popUpShowing = true;
				popUpCal._afterShow(inst);
			});
			if (speed == '') {
				popUpCal._popUpShowing = true;
				popUpCal._afterShow(inst);
			}
			if (inst._input[0].type != 'hidden') {
				inst._input[0].focus();
			}
			this._curInst = inst;
		}
	},

	/* Generate the calendar content. */
	_updateCalendar: function(inst) {
		inst._calendarDiv.empty().append(inst._generateCalendar());
		if (inst._input && inst._input[0].type != 'hidden') {
			inst._input[0].focus();
		}
	},

	/* Tidy up after displaying the calendar. */
	_afterShow: function(inst) {
		if ($.browser.msie) { // fix IE < 7 select problems
			$('#calendar_cover').css({width: inst._calendarDiv[0].offsetWidth + 4,
				height: inst._calendarDiv[0].offsetHeight + 4});
		}
		// re-position on screen if necessary
		var calDiv = inst._calendarDiv[0];
		var pos = popUpCal._findPos(inst._input[0]);
		// Get browser width and X value (IE6+, FF, Safari, Opera)
		if( typeof( window.innerWidth ) == 'number' ) {
			browserWidth = window.innerWidth;
		} else {
			browserWidth = document.documentElement.clientWidth;
		}
		if ( document.documentElement && (document.documentElement.scrollLeft)) {
			browserX = document.documentElement.scrollLeft;	
		} else {
			browserX = document.body.scrollLeft;
		}
		// Reposition calendar if outside the browser window.
		if ((calDiv.offsetLeft + calDiv.offsetWidth) >
				(browserWidth + browserX) ) {
			inst._calendarDiv.css('left', (pos[0] + inst._input[0].offsetWidth - calDiv.offsetWidth) + 'px');
		}
		// Get browser height and Y value (IE6+, FF, Safari, Opera)
		if( typeof( window.innerHeight ) == 'number' ) {
			browserHeight = window.innerHeight;
		} else {
			browserHeight = document.documentElement.clientHeight;
		}
		if ( document.documentElement && (document.documentElement.scrollTop)) {
			browserTopY = document.documentElement.scrollTop;
		} else {
			browserTopY = document.body.scrollTop;
		}
		// Reposition calendar if outside the browser window.
		if ((calDiv.offsetTop + calDiv.offsetHeight) >
				(browserTopY + browserHeight) ) {
			inst._calendarDiv.css('top', (pos[1] - calDiv.offsetHeight) + 'px');
		}
	},

	/* Hide the calendar from view.
	   @param  id     string/object - the ID of the current calendar instance,
			or the instance itself
	   @param  speed  string - the speed at which to close the calendar
	   @return void */
	hideCalendar: function(id, speed) {
		var inst = this._getInst(id);
		if (popUpCal._popUpShowing) {
			speed = (speed != null ? speed : inst._get('speed'));
			inst._calendarDiv.hide(speed, function() {
				popUpCal._tidyDialog(inst);
			});
			if (speed == '') {
				popUpCal._tidyDialog(inst);
			}
			popUpCal._popUpShowing = false;
			popUpCal._lastInput = null;
			inst._settings.prompt = null;
			if (popUpCal._inDialog) {
				popUpCal._dialogInput.css('position', 'absolute').
					css('left', '0px').css('top', '-100px');
				if ($.blockUI) {
					$.unblockUI();
					$('body').append(this._calendarDiv);
				}
			}
			popUpCal._inDialog = false;
		}
		popUpCal._curInst = null;
	},

	/* Tidy up after a dialog display. */
	_tidyDialog: function(inst) {
		inst._calendarDiv.removeClass('calendar_dialog');
		$('.calendar_prompt', inst._calendarDiv).remove();
	},

	/* Close calendar if clicked elsewhere. */
	_checkExternalClick: function(event) {
		if (!popUpCal._curInst) {
			return;
		}
		var target = $(event.target);
		if( (target.parents("#calendar_div").length == 0)
			&& (target.attr('class') != 'calendar_trigger')
			&& popUpCal._popUpShowing 
			&& !(popUpCal._inDialog && $.blockUI) )
		{
			popUpCal.hideCalendar(popUpCal._curInst, '');
		}
	},

	/* Adjust one of the date sub-fields. */
	_adjustDate: function(id, offset, period) {
		var inst = this._getInst(id);
		inst._adjustDate(offset, period);
		this._updateCalendar(inst);
	},

	/* Action for current link. */
	_gotoToday: function(id) {
		var date = new Date();
		var inst = this._getInst(id);
		inst._selectedDay = date.getDate();
		inst._selectedMonth = date.getMonth();
		inst._selectedYear = date.getFullYear();
		this._adjustDate(inst);
	},

	/* Action for selecting a new month/year. */
	_selectMonthYear: function(id, select, period) {
		var inst = this._getInst(id);
		inst._selectingMonthYear = false;
		inst[period == 'M' ? '_selectedMonth' : '_selectedYear'] =
			select.options[select.selectedIndex].value - 0;
		this._adjustDate(inst);
	},

	/* Restore input focus after not changing month/year. */
	_clickMonthYear: function(id) {
		var inst = this._getInst(id);
		if (inst._input && inst._selectingMonthYear && !$.browser.msie) {
			inst._input[0].focus();
		}
		inst._selectingMonthYear = !inst._selectingMonthYear;
	},

	/* Action for changing the first week day. */
	_changeFirstDay: function(id, a) {
		var inst = this._getInst(id);
		var dayNames = inst._get('dayNames');
		var value = a.firstChild.nodeValue;
		for (var i = 0; i < 7; i++) {
			if (dayNames[i] == value) {
				inst._settings.firstDay = i;
				break;
			}
		}
		this._updateCalendar(inst);
	},

	/* Action for selecting a day. */
	_selectDay: function(id, td) {
		var inst = this._getInst(id);
		inst._selectedDay = $("a", td).html();
		this._selectDate(id);
	},

	/* Erase the input field and hide the calendar. */
	_clearDate: function(id) {
		this._selectDate(id, '');
	},

	/* Update the input field with the selected date. */
	_selectDate: function(id, dateStr) {
		var inst = this._getInst(id);
		dateStr = (dateStr != null ? dateStr : inst._formatDate());
		if (inst._input) {
			inst._input.val(dateStr);
		}
		var onSelect = inst._get('onSelect');
		if (onSelect) {
			onSelect(dateStr, inst);  // trigger custom callback
		}
		else {
			inst._input.trigger('change'); // fire the change event
		}
		if (inst._inline) {
			this._updateCalendar(inst);
		}
		else {
			this.hideCalendar(inst, inst._get('speed'));
		}
	},

	/* Set as customDate function to prevent selection of weekends.
	   @param  date  Date - the date to customise
	   @return [boolean, string] - is this date selectable?, what is its CSS class? */
	noWeekends: function(date) {
		var day = date.getDay();
		return [(day > 0 && day < 6), ''];
	},

	/* Find an object's position on the screen. */
	_findPos: function(obj) {
		while (obj && (obj.type == 'hidden' || obj.nodeType != 1)) {
			obj = obj.nextSibling;
		}
		var curleft = curtop = 0;
		if (obj && obj.offsetParent) {
			curleft = obj.offsetLeft;
			curtop = obj.offsetTop;
			while (obj = obj.offsetParent) {
				var origcurleft = curleft;
				curleft += obj.offsetLeft;
				if (curleft < 0) {
					curleft = origcurleft;
				}
				curtop += obj.offsetTop;
			}
		}
		return [curleft,curtop];
	}
});

/* Individualised settings for calendars applied to one or more related inputs.
   Instances are managed and manipulated through the PopUpCal manager. */
function PopUpCalInstance(settings, inline) {
	this._id = popUpCal._register(this);
	this._selectedDay = 0;
	this._selectedMonth = 0; // 0-11
	this._selectedYear = 0; // 4-digit year
	this._input = null; // The attached input field
	this._inline = inline; // True if showing inline, false if used in a popup
	this._calendarDiv = (!inline ? popUpCal._calendarDiv :
		$('<div id="calendar_div_' + this._id + '" class="calendar_inline"></div>'));
	// customise the calendar object - uses manager defaults if not overridden
	this._settings = extendRemove({}, settings || {}); // clone
	if (inline) {
		this._setDate(this._getDefaultDate());
	}
}

$.extend(PopUpCalInstance.prototype, {
	/* Get a setting value, defaulting if necessary. */
	_get: function(name) {
		return (this._settings[name] != null ? this._settings[name] : popUpCal._defaults[name]);
	},

	/* Parse existing date and initialise calendar. */
	_setDateFromField: function(input) {
		this._input = $(input);
		var dateFormat = this._get('dateFormat');
		var currentDate = this._input.val().split(dateFormat.charAt(3));
		if (currentDate.length == 3) {
			this._currentDay = parseInt(currentDate[dateFormat.indexOf('D')], 10);
			this._currentMonth = parseInt(currentDate[dateFormat.indexOf('M')], 10) - 1;
			this._currentYear = parseInt(currentDate[dateFormat.indexOf('Y')], 10);
		}
		else {
			var date = this._getDefaultDate();
			this._currentDay = date.getDate();
			this._currentMonth = date.getMonth();
			this._currentYear = date.getFullYear();
		}
		this._selectedDay = this._currentDay;
		this._selectedMonth = this._currentMonth;
		this._selectedYear = this._currentYear;
		this._adjustDate();
	},
	
	/* Retrieve the default date shown on opening. */
	_getDefaultDate: function() {
		var offsetDate = function(offset) {
			var date = new Date();
			date.setDate(date.getDate() + offset);
			return date;
		};
		var defaultDate = this._get('defaultDate');
		return (defaultDate == null ? new Date() :
			(typeof defaultDate == 'number' ? offsetDate(defaultDate) : defaultDate));
	},

	/* Set the date directly. */
	_setDate: function(date) {
		this._selectedDay = this._currentDay = date.getDate();
		this._selectedMonth = this._currentMonth = date.getMonth();
		this._selectedYear = this._currentYear = date.getFullYear();
		this._adjustDate();
	},

	/* Retrieve the date directly. */
	_getDate: function() {
		return new Date(this._currentYear, this._currentMonth, this._currentDay);
	},

	/* Generate the HTML for the current state of the calendar. */
	_generateCalendar: function() {
		var today = new Date();
		today = new Date(today.getFullYear(), today.getMonth(), today.getDate()); // clear time
		// build the calendar HTML
		var controls = '<div class="calendar_control">' +
			'<a class="calendar_clear" onclick="popUpCal._clearDate(' + this._id + ');">' +
			this._get('clearText') + '</a>' +
			'<a class="calendar_close" onclick="popUpCal.hideCalendar(' + this._id + ');">' +
			this._get('closeText') + '</a></div>';
		var prompt = this._get('prompt');
		var closeAtTop = this._get('closeAtTop');
		var hideIfNoPrevNext = this._get('hideIfNoPrevNext');
		// controls and links
		var html = (prompt ? '<div class="calendar_prompt">' + prompt + '</div>' : '') +
			(closeAtTop && !this._inline ? controls : '') + '<div class="calendar_links">' +
			(this._canAdjustMonth(-1) ? '<a class="calendar_prev" ' +
			'onclick="popUpCal._adjustDate(' + this._id + ', -1, \'M\');">' + this._get('prevText') + '</a>' :
			(hideIfNoPrevNext ? '' : '<label class="calendar_prev">' + this._get('prevText') + '</label>')) +
			(this._isInRange(today) ? '<a class="calendar_current" ' +
			'onclick="popUpCal._gotoToday(' + this._id + ');">' + this._get('currentText') + '</a>' : '') +
			(this._canAdjustMonth(+1) ? '<a class="calendar_next" ' +
			'onclick="popUpCal._adjustDate(' + this._id + ', +1, \'M\');">' + this._get('nextText') + '</a>' :
			(hideIfNoPrevNext ? '' : '<label class="calendar_next">' + this._get('nextText') + '</label>')) +
			'</div><div class="calendar_header">';
		var minDate = this._get('minDate');
		var maxDate = this._get('maxDate');
		// month selection
		var monthNames = this._get('monthNames');
		if (!this._get('changeMonth')) {
			html += monthNames[this._selectedMonth] + '&nbsp;';
		}
		else {
			var inMinYear = (minDate && minDate.getFullYear() == this._selectedYear);
			var inMaxYear = (maxDate && maxDate.getFullYear() == this._selectedYear);
			html += '<select class="calendar_newMonth" ' +
				'onchange="popUpCal._selectMonthYear(' + this._id + ', this, \'M\');" ' +
				'onclick="popUpCal._clickMonthYear(' + this._id + ');">';
			for (var month = 0; month < 12; month++) {
				if ((!inMinYear || month >= minDate.getMonth()) &&
						(!inMaxYear || month <= maxDate.getMonth())) {
					html += '<option value="' + month + '"' +
						(month == this._selectedMonth ? ' selected="selected"' : '') +
						'>' + monthNames[month] + '</option>';
				}
			}
			html += '</select>';
		}
		// year selection
		if (!this._get('changeYear')) {
			html += this._selectedYear;
		}
		else {
			// determine range of years to display
			var years = this._get('yearRange').split(':');
			var year = 0;
			var endYear = 0;
			if (years.length != 2) {
				year = this._selectedYear - 10;
				endYear = this._selectedYear + 10;
			}
			else if (years[0].charAt(0) == '+' || years[0].charAt(0) == '-') {
				year = this._selectedYear + parseInt(years[0], 10);
				endYear = this._selectedYear + parseInt(years[1], 10);
			}
			else {
				year = parseInt(years[0], 10);
				endYear = parseInt(years[1], 10);
			}
			year = (minDate ? Math.max(year, minDate.getFullYear()) : year);
			endYear = (maxDate ? Math.min(endYear, maxDate.getFullYear()) : endYear);
			html += '<select class="calendar_newYear" onchange="popUpCal._selectMonthYear(' +
				this._id + ', this, \'Y\');" ' + 'onclick="popUpCal._clickMonthYear(' +
				this._id + ');">';
			for (; year <= endYear; year++) {
				html += '<option value="' + year + '"' +
					(year == this._selectedYear ? ' selected="selected"' : '') +
					'>' + year + '</option>';
			}
			html += '</select>';
		}
		html += '</div><table class="calendar" cellpadding="0" cellspacing="0"><thead>' +
			'<tr class="calendar_titleRow">';
		var firstDay = this._get('firstDay');
		var changeFirstDay = this._get('changeFirstDay');
		var dayNames = this._get('dayNames');
		for (var dow = 0; dow < 7; dow++) { // days of the week
			html += '<td>' + (!changeFirstDay ? '' : '<a onclick="popUpCal._changeFirstDay(' +
				this._id + ', this);">') + dayNames[(dow + firstDay) % 7] +
				(changeFirstDay ? '</a>' : '') + '</td>';
		}
		html += '</tr></thead><tbody>';
		var daysInMonth = this._getDaysInMonth(this._selectedYear, this._selectedMonth);
		this._selectedDay = Math.min(this._selectedDay, daysInMonth);
		var leadDays = (this._getFirstDayOfMonth(this._selectedYear, this._selectedMonth) - firstDay + 7) % 7;
		var currentDate = new Date(this._currentYear, this._currentMonth, this._currentDay);
		var selectedDate = new Date(this._selectedYear, this._selectedMonth, this._selectedDay);
		var printDate = new Date(this._selectedYear, this._selectedMonth, 1 - leadDays);
		var numRows = Math.ceil((leadDays + daysInMonth) / 7); // calculate the number of rows to generate
		var customDate = this._get('customDate');
		var showOtherMonths = this._get('showOtherMonths');
		for (var row = 0; row < numRows; row++) { // create calendar rows
			html += '<tr class="calendar_daysRow">';
			for (var dow = 0; dow < 7; dow++) { // create calendar days
				var customSettings = (customDate ? customDate(printDate) : [true, '']);
				var otherMonth = (printDate.getMonth() != this._selectedMonth);
				var unselectable = otherMonth || !customSettings[0] ||
					(minDate && printDate < minDate) || (maxDate && printDate > maxDate);
				html += '<td class="calendar_daysCell' +
					((dow + firstDay + 6) % 7 >= 5 ? ' calendar_weekEndCell' : '') + // highlight weekends
					(otherMonth ? ' calendar_otherMonth' : '') + // highlight days from other months
					(printDate.getTime() == selectedDate.getTime() ? ' calendar_daysCellOver' : '') + // highlight selected day
					(unselectable ? ' calendar_unselectable' : '') +  // highlight unselectable days
					(otherMonth && !showOtherMonths ? '' : ' ' + customSettings[1] + // highlight custom dates
					(printDate.getTime() == currentDate.getTime() ? ' calendar_currentDay' : // highlight current day
					(printDate.getTime() == today.getTime() ? ' calendar_today' : ''))) + '"' + // highlight today (if different)
					(unselectable ? '' : ' onmouseover="$(this).addClass(\'calendar_daysCellOver\');"' +
					' onmouseout="$(this).removeClass(\'calendar_daysCellOver\');"' +
					' onclick="popUpCal._selectDay(' + this._id + ', this);"') + '>' + // actions
					(otherMonth ? (showOtherMonths ? printDate.getDate() : '&nbsp;') : // display for other months
					(unselectable ? printDate.getDate() : '<a>' + printDate.getDate() + '</a>')) + '</td>'; // display for this month
				printDate.setDate(printDate.getDate() + 1);
			}
			html += '</tr>';
		}
		html += '</tbody></table>' + (!closeAtTop && !this._inline ? controls : '') +
			'<div style="clear: both;"></div>' + (!$.browser.msie ? '' :
			'<!--[if lte IE 6.5]><iframe src="javascript:false;" class="calendar_cover"></iframe><![endif]-->');
		return html;
	},

	/* Adjust one of the date sub-fields. */
	_adjustDate: function(offset, period) {
		var date = new Date(this._selectedYear + (period == 'Y' ? offset : 0),
			this._selectedMonth + (period == 'M' ? offset : 0),
			this._selectedDay + (period == 'D' ? offset : 0));
		// ensure it is within the bounds set
		var minDate = this._get('minDate');
		var maxDate = this._get('maxDate');
		date = (minDate && date < minDate ? minDate : date);
		date = (maxDate && date > maxDate ? maxDate : date);
		this._selectedDay = date.getDate();
		this._selectedMonth = date.getMonth();
		this._selectedYear = date.getFullYear();
	},

	/* Find the number of days in a given month. */
	_getDaysInMonth: function(year, month) {
		return 32 - new Date(year, month, 32).getDate();
	},

	/* Find the day of the week of the first of a month. */
	_getFirstDayOfMonth: function(year, month) {
		return new Date(year, month, 1).getDay();
	},

	/* Determines if we should allow a "next/prev" month display change. */
	_canAdjustMonth: function(offset) {
		var date = new Date(this._selectedYear, this._selectedMonth + offset, 1);
		if (offset < 0) {
			date.setDate(this._getDaysInMonth(date.getFullYear(), date.getMonth()));
		}
		return this._isInRange(date);
	},

	/* Is the given date in the accepted range? */
	_isInRange: function(date) {
		var minDate = this._get('minDate');
		var maxDate = this._get('maxDate');
		return ((!minDate || date >= minDate) && (!maxDate || date <= maxDate));
	},

	/* Format the given date for display. */
	_formatDate: function() {
		var day = this._currentDay = this._selectedDay;
		var month = this._currentMonth = this._selectedMonth;
		var year = this._currentYear = this._selectedYear;
		month++; // adjust javascript month
		var dateFormat = this._get('dateFormat');
		var dateString = '';
		for (var i = 0; i < 3; i++) {
			dateString += dateFormat.charAt(3) +
				(dateFormat.charAt(i) == 'D' ? (day < 10 ? '0' : '') + day :
				(dateFormat.charAt(i) == 'M' ? (month < 10 ? '0' : '') + month :
				(dateFormat.charAt(i) == 'Y' ? year : '?')));
		}
		return dateString.substring(dateFormat.charAt(3) ? 1 : 0);
	}
});

/* jQuery extend now ignores nulls! */
function extendRemove(target, props) {
	$.extend(target, props);
	for (var name in props) {
		if (props[name] == null) {
			target[name] = null;
		}
	}
	return target;
}

/* Attach the calendar to a jQuery selection.
   @param  settings  object - the new settings to use for this calendar instance (anonymous)
   @return jQuery object - for chaining further calls */
$.fn.calendar = function(settings) {
	return this.each(function() {
		// check for settings on the control itself - in namespace 'cal:'
		var inlineSettings = null;
		for (attrName in popUpCal._defaults) {
			var attrValue = this.getAttribute('cal:' + attrName);
			if (attrValue) {
				inlineSettings = inlineSettings || {};
				try {
					inlineSettings[attrName] = eval(attrValue);
				}
				catch (err) {
					inlineSettings[attrName] = attrValue;
				}
			}
		}
		var nodeName = this.nodeName.toLowerCase();
		if (nodeName == 'input') {
			var instSettings = (inlineSettings ? $.extend($.extend({}, settings || {}),
				inlineSettings || {}) : settings); // clone and customise
			var inst = (inst && !inlineSettings ? inst :
				new PopUpCalInstance(instSettings, false));
			popUpCal._connectCalendar(this, inst);
		} 
		else if (nodeName == 'div' || nodeName == 'span') {
			var instSettings = $.extend($.extend({}, settings || {}),
				inlineSettings || {}); // clone and customise
			var inst = new PopUpCalInstance(instSettings, true);
			popUpCal._inlineCalendar(this, inst);
		}
	});
};

/* Initialise the calendar. */
$(document).ready(function() {
   popUpCal = new PopUpCal(); // singleton instance
});










/***************************************************************************

								 UI.MAGNIFIER.JS

****************************************************************************/


(function($) {

	//If the UI scope is not availalable, add it
	$.ui = $.ui || {};
	
	//Make nodes selectable by expression
	$.extend($.expr[':'], { magnifier: "(' '+a.className+' ').indexOf(' ui-magnifier ')" });

	//Macros for external methods that support chaining
	var methods = "destroy,enable,disable,reset".split(",");
	for(var i=0;i<methods.length;i++) {
		var cur = methods[i], f;
		eval('f = function() { var a = arguments; return this.each(function() { if(jQuery(this).is(".ui-magnifier")) jQuery.data(this, "ui-magnifier")["'+cur+'"](a); }); }');
		$.fn["magnifier"+cur.substr(0,1).toUpperCase()+cur.substr(1)] = f;
	};

	//get instance method
	$.fn.magnifierInstance = function() {
		if($(this[0]).is(".ui-magnifier")) return $.data(this[0], "ui-magnifier");
		return false;
	};
	
	$.fn.magnifier = function(options) {
		return this.each(function() {
			new $.ui.magnifier(this,options);	
		});
	};
	
	$.ui.magnifier = function(el,options) {
		
		var self = this; this.items = []; this.element = el;
		this.options = options || {}; var o = this.options;
		$.data(el, "ui-magnifier", this);
		$(el).addClass("ui-magnifier");
		
		o.distance = o.distance || 150;
		o.magnification = o.magnification || 2;
		o.baseline = o.baseline || 0;
		o.verticalLine =  o.verticalLine != undefined ? o.verticalLine : -0.5;
		
		this.pp = $(el).offset({ border: false });
		
		$('> *', el).each(function() {
			var co = $(this).offset({ border: false });
			if(self.options.overlap) var cp = $(this).position();
			self.items.push([this, co, [$(this).width(),$(this).height()], (cp || null)]);
			
			if(o.opacity)
				$(this).css('opacity', o.opacity.min);	
		});
	
		if(o.overlap) {
			for(var i=0;i<this.items.length;i++) {
				//Absolute stuff
				$(this.items[i][0]).css({
					position: "absolute",
					top: this.items[i][3].top,
					left: this.items[i][3].left
				});
			};
		}
		
		this.moveEvent = function(e) { if(!self.disabled) self.magnify.apply(self, [e]); }
		$(document).bind("mousemove", this.moveEvent);
		
		if(o.click) { //If onclick callback is available

			this.clickEvent = function(e) { if(!self.disabled) o.click.apply(this, [e, { options: self.options, current: self.current[0], currentOffset: self.current[1] }]); }
			$(el).bind('click', this.clickEvent);
		}
		
	}
	
	$.extend($.ui.magnifier.prototype, {
		destroy: function() {
			$(this.element).removeClass("ui-magnifier").removeClass("ui-magnifier-disabled");
			$(document).unbind("mousemove", this.moveEvent);
			if(this.clickEvent) $(this.element).unbind("click", this.clickEvent);
		},
		enable: function() {
			$(this.element).removeClass("ui-magnifier-disabled");
			this.disabled = false;
		},
		disable: function() {
			$(this.element).addClass("ui-magnifier-disabled");
			this.reset();
			this.disabled = true;
		},
		reset: function(e) {
			
			var o = this.options;
			var c;
			var distance = 1;
			
			for(var i=0;i<this.items.length;i++) {
	
				c = this.items[i];
				
				$(c[0]).css({
					width: c[2][0],
					height: c[2][1],
					top: (c[3] ? c[3].top : 0),
					left: (c[3] ? c[3].left : 0)
				});
				
				if(o.opacity)
					$(c[0]).css('opacity', o.opacity.min);
					
				if(o.zIndex)
					$(c[0]).css("z-index", "");
				
			}
					
		},
		magnify: function(e) {
			var p = [e.pageX,e.pageY];
			var o = this.options;
			var c;
			this.current = this.items[0];
			var distance = 1;
	
			//Compute the parents distance, because we don't need to fire anything if we are not near the parent
	
			var overlap = ((p[0] > this.pp.left-o.distance && p[0] < this.pp.left + this.element.offsetWidth + o.distance) && (p[1] > this.pp.top-o.distance && p[1] < this.pp.top + this.element.offsetHeight + o.distance));
			if(!overlap) return false;
	
			
			for(var i=0;i<this.items.length;i++) {
				c = this.items[i];
				
				var olddistance = distance;
				if(!o.axis) {
					distance = Math.sqrt(
						  Math.pow(p[0] - ((c[3] ? this.pp.left : c[1].left) + parseInt(c[0].style.left)) - (c[0].offsetWidth/2), 2)
						+ Math.pow(p[1] - ((c[3] ? this.pp.top  : c[1].top ) + parseInt(c[0].style.top )) - (c[0].offsetHeight/2), 2)
					);
				} else {
					if(o.axis == "y") {
						distance = Math.abs(p[1] - ((c[3] ? this.pp.top  : c[1].top ) + parseInt(c[0].style.top )) - (c[0].offsetHeight/2));
					} else {
						distance = Math.abs(p[0] - ((c[3] ? this.pp.left : c[1].left) + parseInt(c[0].style.left)) - (c[0].offsetWidth/2));
					}			
				}
				
				if(distance < o.distance) {
	
					this.current = distance < olddistance ? this.items[i] : this.current;
					
					if(!o.axis || o.axis != "y") {
						$(c[0]).css({
							width: c[2][0]+ (c[2][0] * (o.magnification-1)) - (((distance/o.distance)*c[2][0]) * (o.magnification-1)),
							left: (c[3] ? (c[3].left + o.verticalLine * ((c[2][1] * (o.magnification-1)) - (((distance/o.distance)*c[2][1]) * (o.magnification-1)))) : 0)
						});
					}
					
					if(!o.axis || o.axis != "x") {
						$(c[0]).css({
							height: c[2][1]+ (c[2][1] * (o.magnification-1)) - (((distance/o.distance)*c[2][1]) * (o.magnification-1)),
							top: (c[3] ? c[3].top : 0) + (o.baseline-0.5) * ((c[2][0] * (o.magnification-1)) - (((distance/o.distance)*c[2][0]) * (o.magnification-1)))
						});					
					}
					
					if(o.opacity)
						$(c[0]).css('opacity', o.opacity.max-(distance/o.distance) < o.opacity.min ? o.opacity.min : o.opacity.max-(distance/o.distance));
					
				} else {
					
					$(c[0]).css({
						width: c[2][0],
						height: c[2][1],
						top: (c[3] ? c[3].top : 0),
						left: (c[3] ? c[3].left : 0)
					});
					
					if(o.opacity)
						$(c[0]).css('opacity', o.opacity.min);
								
				}
				
				if(o.zIndex)
					$(c[0]).css("z-index", "");
	
			}
			
			if(this.options.zIndex)
				$(this.current[0]).css("z-index", this.options.zIndex);
			
		}
	});

})($);















/***************************************************************************

								 UI.SHADOW.JS

****************************************************************************/



(function($) {

	//If the UI scope is not available, add it
	$.ui = $.ui || {};
	
	//Make nodes selectable by expression
	$.extend($.expr[':'], { shadowed: "(' '+a.className+' ').indexOf(' ui-shadowed ')" });
	
	$.fn.shadowEnable = function() { if($(this[0]).next().is(".ui-shadow")) $(this[0]).next().show(); }
	$.fn.shadowDisable = function() { if($(this[0]).next().is(".ui-shadow")) $(this[0]).next().hide(); }
	
	$.fn.shadow = function(options) {
		
		options = options || {};
		options.offset = options.offset ? options.offset : 0;
		options.opacity = options.opacity ? options.opacity : 0.2;
		
		return this.each(function() {
			
			var cur = $(this);
			
			//Create a shadow element
			var shadow = $("<div class='ui-shadow'></div>"); cur.after(shadow);
			
			//Figure the base height and width
			var baseWidth = cur.outerWidth();
			var baseHeight = cur.outerHeight();
			
			//get the offset
			var position = cur.position();
			
			//Append smooth corners
			$('<div class="ui-shadow-color ui-shadow-layer-1"></div>').css({ opacity: options.opacity-0.05, left: 5+options.offset, top: 5+options.offset, width: baseWidth+1, height: baseHeight+1 }).appendTo(shadow);
			$('<div class="ui-shadow-color ui-shadow-layer-2"></div>').css({ opacity: options.opacity-0.1, left: 7+options.offset, top: 7+options.offset, width: baseWidth, height: baseHeight-3 }).appendTo(shadow);
			$('<div class="ui-shadow-color ui-shadow-layer-3"></div>').css({ opacity: options.opacity-0.1, left: 7+options.offset, top: 7+options.offset, width: baseWidth-3, height: baseHeight }).appendTo(shadow);
			$('<div class="ui-shadow-color ui-shadow-layer-4"></div>').css({ opacity: options.opacity, left: 6+options.offset, top: 6+options.offset, width: baseWidth-1, height: baseHeight-1 }).appendTo(shadow);
			
			//If we have a color, use it
			if(options.color)
				$("div.ui-shadow-color", shadow).css("background-color", options.color);
			
			//Determine the stack order (attention: the zIndex will get one higher!)
			if(!cur.css("zIndex") || cur.css("zIndex") == "auto") {
				var stack = 0;
				cur.css("position", (cur.css("position") == "static" ? "relative" : cur.css("position"))).css("z-index", "1");
			} else {
				var stack = parseInt(cur.css("zIndex"));
				cur.css("zIndex", stack+1);
			}
			
			//Copy the original z-index and position to the clone
			//alert(shadow); If you insert this alert, opera will time correctly!!
			shadow.css({
				position: "absolute",
				zIndex: stack,
				left: position.left,
				top: position.top,
				width: baseWidth,
				height: baseHeight,
				marginLeft: cur.css("marginLeft"),
				marginRight: cur.css("marginRight"),
				marginBottom: cur.css("marginBottom"),
				marginTop: cur.css("marginTop")
			});
			
			
			function rearrangeShadow(el,sh) {
				var $el = $(el);
				$(sh).css($el.position());
				$(sh).children().css({ height: $el.outerHeight()+"px", width: $el.outerWidth()+"px" });
			}
			
			if($.browser.msie) {
				//Add dynamic css expressions
				shadow[0].style.setExpression("left","parseInt(jQuery(this.previousSibling).css('left'))+'px' || jQuery(this.previousSibling).position().left");
				shadow[0].style.setExpression("top","parseInt(jQuery(this.previousSibling).css('top'))+'px' || jQuery(this.previousSibling).position().top");
			} else {
				//Bind events for good browsers
				this.addEventListener("DOMAttrModified",function() { rearrangeShadow(this,shadow); },false);
			}

				
		});
	};
	

})($);











/***************************************************************************

								 UI.SLIDER.JS

****************************************************************************/



(function($) {

	$.fn.extend({
		slider: function(options) {
			var args = Array.prototype.slice.call(arguments, 1);
			
			if ( options == "value" )
				return $.data(this[0], "ui-slider").value(arguments[1]);
			
			return this.each(function() {
				if (typeof options == "string") {
					var slider = $.data(this, "ui-slider");
					slider[options].apply(slider, args);

				} else if(!$.data(this, "ui-slider"))
					new $.ui.slider(this, options);
			});
		}
	});
	
	$.ui.slider = function(element, options) {

		//Initialize needed constants
		var self = this;
		this.element = $(element);
		$.data(element, "ui-slider", this);
		this.element.addClass("ui-slider");
		
		//Prepare the passed options
		this.options = $.extend({}, options);
		var o = this.options;
		$.extend(o, {
			axis: o.axis || (element.offsetWidth < element.offsetHeight ? 'vertical' : 'horizontal'),
			maxValue: !isNaN(parseInt(o.maxValue,10)) ? parseInt(o.maxValue,10) :  100,
			minValue: parseInt(o.minValue,10) || 0,
			startValue: parseInt(o.startValue,10) || 'none'		
		});
		
		//Prepare the real maxValue
		o.realMaxValue = o.maxValue - o.minValue;
		
		//Calculate stepping based on steps
		o.stepping = parseInt(o.stepping,10) || (o.steps ? o.realMaxValue/o.steps : 0);
		
		$(element).bind("setData.slider", function(event, key, value){
			self.options[key] = value;
		}).bind("getData.slider", function(event, key){
			return self.options[key];
		});

		//Initialize mouse and key events for interaction
		this.handle = o.handle ? $(o.handle, element) : $('> *', element);
		$(this.handle)
			.mouseInteraction({
				executor: this,
				delay: o.delay,
				distance: o.distance || 0,
				dragPrevention: o.prevention ? o.prevention.toLowerCase().split(',') : ['input','textarea','button','select','option'],
				start: this.start,
				stop: this.stop,
				drag: this.drag,
				condition: function(e, handle) {
					if(!this.disabled) {
						if(this.currentHandle) this.blur(this.currentHandle);
						this.focus(handle,1);
						return !this.disabled;
					}
				}
			})
			.wrap('<a href="javascript:void(0)"></a>')
			.parent()
				.bind('focus', function(e) { self.focus(this.firstChild); })
				.bind('blur', function(e) { self.blur(this.firstChild); })
				.bind('keydown', function(e) {
					if(/(37|39)/.test(e.keyCode))
						self.moveTo((e.keyCode == 37 ? '-' : '+')+'='+(self.options.stepping ? self.options.stepping : (self.options.realMaxValue / self.size)*5),this.firstChild);
				})
		;
		
		//Position the node
		if(o.helper == 'original' && (this.element.css('position') == 'static' || this.element.css('position') == '')) this.element.css('position', 'relative');
		
		//Prepare dynamic properties for later use
		if(o.axis == 'horizontal') {
			this.size = this.element.outerWidth();
			this.properties = ['left', 'width'];
		} else {
			this.size = this.element.outerHeight();
			this.properties = ['top', 'height'];
		}
		
		//Bind the click to the slider itself
		this.element.bind('click', function(e) { self.click.apply(self, [e]); });
		
		//Move the first handle to the startValue
		if(!isNaN(o.startValue)) this.moveTo(o.startValue, 0);
		
		//If we only have one handle, set the previous handle to this one to allow clicking before selecting the handle
		if(this.handle.length == 1) this.previousHandle = this.handle;
		
		
		if(this.handle.length == 2 && o.range) this.createRange();
	
	};
	
	$.extend($.ui.slider.prototype, {
		plugins: {},
		createRange: function() {
			this.rangeElement = $('<div></div>')
				.addClass('ui-slider-range')
				.css({ position: 'absolute' })
				.css(this.properties[0], parseInt($(this.handle[0]).css(this.properties[0]),10) + this.handleSize(0)/2)
				.css(this.properties[1], parseInt($(this.handle[1]).css(this.properties[0]),10) - parseInt($(this.handle[0]).css(this.properties[0]),10))
				.appendTo(this.element);
		},
		updateRange: function() {
				this.rangeElement.css(this.properties[0], parseInt($(this.handle[0]).css(this.properties[0]),10) + this.handleSize(0)/2);
				this.rangeElement.css(this.properties[1], parseInt($(this.handle[1]).css(this.properties[0]),10) - parseInt($(this.handle[0]).css(this.properties[0]),10));
		},
		getRange: function() {
			return this.rangeElement ? this.convertValue(parseInt(this.rangeElement.css(this.properties[1]),10)) : null;
		},
		ui: function(e) {
			return {
				instance: this,
				options: this.options,
				handle: this.currentHandle,
				value: this.value(),
				range: this.getRange()
			};
		},
		propagate: function(n,e) {
			$.ui.plugin.call(this, n, [e, this.ui()]);
			this.element.triggerHandler(n == "slide" ? n : "slide"+n, [e, this.ui()], this.options[n]);
		},
		destroy: function() {
			this.element
				.removeClass("ui-slider ui-slider-disabled")
				.removeData("ul-slider")
				.unbind(".slider");
			this.handles.removeMouseInteraction();
		},
		enable: function() {
			this.element.removeClass("ui-slider-disabled");
			this.disabled = false;
		},
		disable: function() {
			this.element.addClass("ui-slider-disabled");
			this.disabled = true;
		},
		focus: function(handle,hard) {
			this.currentHandle = $(handle).addClass('ui-slider-handle-active');
			if(hard) this.currentHandle.parent()[0].focus();
		},
		blur: function(handle) {
			$(handle).removeClass('ui-slider-handle-active');
			if(this.currentHandle && this.currentHandle[0] == handle) { this.previousHandle = this.currentHandle; this.currentHandle = null; };
		},
		value: function(handle) {
			if(this.handle.length == 1) this.currentHandle = this.handle;
			return ((parseInt($(handle != undefined ? this.handle[handle] || handle : this.currentHandle).css(this.properties[0]),10) / (this.size - this.handleSize())) * this.options.realMaxValue) + this.options.minValue;
		},
		convertValue: function(value) {
			return (value / (this.size - this.handleSize())) * this.options.realMaxValue;
		},
		translateValue: function(value) {
			return ((value - this.options.minValue) / this.options.realMaxValue) * (this.size - this.handleSize());
		},
		handleSize: function(handle) {
			return $(handle != undefined ? this.handle[handle] : this.currentHandle)['outer'+this.properties[1].substr(0,1).toUpperCase()+this.properties[1].substr(1)]();	
		},
		click: function(e) {
		
			// This method is only used if:
			// - The user didn't click a handle
			// - The Slider is not disabled
			// - There is a current, or previous selected handle (otherwise we wouldn't know which one to move)
			var pointer = [e.pageX,e.pageY];
			var clickedHandle = false; this.handle.each(function() { if(this == e.target) clickedHandle = true;  });
			if(clickedHandle || this.disabled || !(this.currentHandle || this.previousHandle)) return;

			//If a previous handle was focussed, focus it again
			if(this.previousHandle) this.focus(this.previousHandle, 1);
			
			//Move focussed handle to the clicked position
			this.offset = this.element.offset();
			this.moveTo(this.convertValue(e[this.properties[0] == 'top' ? 'pageY' : 'pageX'] - this.offset[this.properties[0]] - this.handleSize()/2));
			
		},
		start: function(e, handle) {
			
			var o = this.options;
			
			this.offset = this.element.offset();
			this.handleOffset = this.currentHandle.offset();
			this.clickOffset = { top: e.pageY - this.handleOffset.top, left: e.pageX - this.handleOffset.left };
			this.firstValue = this.value();
			
			this.propagate('start', e);
			return false;
						
		},
		stop: function(e) {
			this.propagate('stop', e);
			if(this.firstValue != this.value()) this.propagate('change', e);
			return false;
		},
		drag: function(e, handle) {

			var o = this.options;
			var position = { top: e.pageY - this.offset.top - this.clickOffset.top, left: e.pageX - this.offset.left - this.clickOffset.left};

			var modifier = position[this.properties[0]];			
			if(modifier >= this.size - this.handleSize()) modifier = this.size - this.handleSize();
			if(modifier <= 0) modifier = 0;
			
			if(o.stepping) {
				var value = this.convertValue(modifier);
				value = Math.round(value / o.stepping) * o.stepping;
				modifier = this.translateValue(value);	
			}

			if(this.rangeElement) {
				if(this.currentHandle[0] == this.handle[0] && modifier >= this.translateValue(this.value(1))) modifier = this.translateValue(this.value(1));
				if(this.currentHandle[0] == this.handle[1] && modifier <= this.translateValue(this.value(0))) modifier = this.translateValue(this.value(0));
			}	
			
			this.currentHandle.css(this.properties[0], modifier);
			if(this.rangeElement) this.updateRange();
			this.propagate('slide', e);
			return false;
			
		},
		moveTo: function(value, handle) {

			var o = this.options;
			if(handle == undefined && !this.currentHandle && this.handle.length != 1) return false; //If no handle has been passed, no current handle is available and we have multiple handles, return false
			if(handle == undefined && !this.currentHandle) handle = 0; //If only one handle is available, use it
			if(handle != undefined) this.currentHandle = this.previousHandle = $(this.handle[handle] || handle);

			if(value.constructor == String) value = /\-\=/.test(value) ? this.value() - parseInt(value.replace('-=', ''),10) : this.value() + parseInt(value.replace('+=', ''),10);
			if(o.stepping) value = Math.round(value / o.stepping) * o.stepping;
			value = this.translateValue(value);

			if(value >= this.size - this.handleSize()) value = this.size - this.handleSize();
			if(value <= 0) value = 0;
			if(this.rangeElement) {
				if(this.currentHandle[0] == this.handle[0] && value >= this.translateValue(this.value(1))) value = this.translateValue(this.value(1));
				if(this.currentHandle[0] == this.handle[1] && value <= this.translateValue(this.value(0))) value = this.translateValue(this.value(0));
			}
			
			this.currentHandle.css(this.properties[0], value);
			if(this.rangeElement) this.updateRange();
			
			this.propagate('start', null);
			this.propagate('stop', null);
			this.propagate('change', null);

		}
	});

})(jQuery);







/***************************************************************************

								 UI.SORTABLE.JS

****************************************************************************/




(function($) {

	if (window.Node && Node.prototype && !Node.prototype.contains) {
		Node.prototype.contains = function (arg) {
			return !!(this.compareDocumentPosition(arg) & 16);
		};
	}

	$.fn.extend({
		sortable: function(options) {
			
			var args = Array.prototype.slice.call(arguments, 1);
			
			if ( options == "serialize" )
				return $.data(this[0], "ui-sortable").serialize(arguments[1]);
			
			return this.each(function() {
				if (typeof options == "string") {
					var sort = $.data(this, "ui-sortable");
					sort[options].apply(sort, args);

				} else if(!$.data(this, "ui-sortable"))
					new $.ui.sortable(this, options);
			});
		}
	});
	
	$.ui.sortable = function(element, options) {
		//Initialize needed constants
		var self = this;
		
		this.element = $(element);
		
		$.data(element, "ui-sortable", this);
		this.element.addClass("ui-sortable");

		//Prepare the passed options
		this.options = $.extend({}, options);
		var o = this.options;
		$.extend(o, {
			items: this.options.items || '> *',
			zIndex: this.options.zIndex || 1000,
			startCondition: function() {
				return !self.disabled;	
			}		
		});
		
		$(element).bind("setData.sortable", function(event, key, value){
			self.options[key] = value;
		}).bind("getData.sortable", function(event, key){
			return self.options[key];
		});
		
		//Get the items
		this.refresh();

		//Let's determine if the items are floating
		this.floating = /left|right/.test(this.items[0].item.css('float'));
		
		//Let's determine the parent's offset
		if(!(/(relative|absolute|fixed)/).test(this.element.css('position'))) this.element.css('position', 'relative');
		this.offset = this.element.offset({ border: false });

		//Initialize mouse events for interaction
		this.element.mouseInteraction({
			executor: this,
			delay: o.delay,
			distance: o.distance || 0,
			dragPrevention: o.prevention ? o.prevention.toLowerCase().split(',') : ['input','textarea','button','select','option'],
			start: this.start,
			stop: this.stop,
			drag: this.drag,
			condition: function(e) {

				if(this.disabled) return false;

				//Find out if the clicked node (or one of its parents) is a actual item in this.items
				var currentItem = null, nodes = $(e.target).parents().andSelf().each(function() {
					if($.data(this, 'ui-sortable-item')) currentItem = $(this);
				});
				if(currentItem && (!this.options.handle || $(e.target).parents().andSelf().is(this.options.handle))) {
					this.currentItem = currentItem;
					return true;
				} else return false; 

			}
		});

	};
	
	$.extend($.ui.sortable.prototype, {
		plugins: {},
		ui: function() {
			return {
				helper: this.helper,
				placeholder: this.placeholder || $([]),
				position: this.position,
				absolutePosition: this.positionAbs,
				instance: this,
				options: this.options
			};		
		},
		propagate: function(n,e) {
			$.ui.plugin.call(this, n, [e, this.ui()]);
			this.element.triggerHandler(n == "sort" ? n : "sort"+n, [e, this.ui()], this.options[n]);
		},
		serialize: function(o) {
			
			var items = $(this.options.items, this.element).not('.ui-sortable-helper'); //Only the items of the sortable itself
			var str = [];
			o = o || {};
			
			items.each(function() {
				var res = (this.getAttribute(o.attribute || 'id') || '').match(o.expression || (/(.+)[-=_](.+)/));
				if(res) str.push((o.key || res[1])+'[]='+(o.key ? res[1] : res[2]));				
			});
			
			return str.join('&');
			
		},
		intersectsWith: function(item) {
			
			var x1 = this.positionAbs.left, x2 = x1 + this.helperProportions.width,
			    y1 = this.positionAbs.top, y2 = y1 + this.helperProportions.height;
			var l = item.left, r = l + item.width, 
			    t = item.top,  b = t + item.height;
			
			return (   l < x1 + (this.helperProportions.width  / 2)        // Right Half
				&&     x2 - (this.helperProportions.width  / 2) < r    // Left Half
				&& t < y1 + (this.helperProportions.height / 2)        // Bottom Half
				&&     y2 - (this.helperProportions.height / 2) < b ); // Top Half
			
		},
		refresh: function() {
			
			this.items = [];
			var items = this.items;
			var queries = [$(this.options.items, this.element)];
			
			if(this.options.connectWith) {
				for (var i = this.options.connectWith.length - 1; i >= 0; i--){
					var inst = $.data($(this.options.connectWith[i])[0], 'ui-sortable');
					if(inst && !inst.disabled) queries.push($(inst.options.items, inst.element));
				};
			}
			
			for (var i = queries.length - 1; i >= 0; i--){
				queries[i].each(function() {
					$.data(this, 'ui-sortable-item', true); // Data for target checking (mouse manager)
					items.push({
						item: $(this),
						width: 0, height: 0,
						left: 0, top: 0
					});
				});
			};

		},
		refreshPositions: function(fast) {
			for (var i = this.items.length - 1; i >= 0; i--){
				if(!fast) this.items[i].width = this.items[i].item.outerWidth();
				if(!fast) this.items[i].height = this.items[i].item.outerHeight();
				var p = this.items[i].item.offset();
				this.items[i].left = p.left;
				this.items[i].top = p.top;
			};
		},
		destroy: function() {
			this.element
				.removeClass("ui-sortable ui-sortable-disabled")
				.removeData("ui-sortable")
				.unbind(".sortable")
				.removeMouseInteraction();
			
			for ( var i = this.items.length - 1; i >= 0; i-- )
				this.items[i].item.removeData("ui-sortable-item");
		},
		enable: function() {
			this.element.removeClass("ui-sortable-disabled");
			this.disabled = false;
		},
		disable: function() {
			this.element.addClass("ui-sortable-disabled");
			this.disabled = true;
		},
		createPlaceholder: function() {
			this.placeholderElement = this.options.placeholderElement ? $(this.options.placeholderElement, this.currentItem) : this.currentItem;
			this.placeholder = $('<div></div>')
				.addClass(this.options.placeholder)
				.appendTo('body')
				.css({ position: 'absolute' })
				.css(this.placeholderElement.offset())
				.css({ width: this.placeholderElement.outerWidth(), height: this.placeholderElement.outerHeight() })
				;
		},
		recallOffset: function(e) {

			var elementPosition = { left: this.elementOffset.left - this.offsetParentOffset.left, top: this.elementOffset.top - this.offsetParentOffset.top };
			var r = this.helper.css('position') == 'relative';

			//Generate the original position
			this.originalPosition = {
				left: (r ? parseInt(this.helper.css('left'),10) || 0 : elementPosition.left + (this.offsetParent[0] == document.body ? 0 : this.offsetParent[0].scrollLeft)),
				top: (r ? parseInt(this.helper.css('top'),10) || 0 : elementPosition.top + (this.offsetParent[0] == document.body ? 0 : this.offsetParent[0].scrollTop))
			};
			
			//Generate a flexible offset that will later be subtracted from e.pageX/Y
			this.offset = {
				left: this._pageX - this.originalPosition.left + (parseInt(this.currentItem.css('marginLeft'),10) || 0),
				top: this._pageY - this.originalPosition.top + (parseInt(this.currentItem.css('marginTop'),10) || 0)
			};
			
		},
		start: function(e) {
			
			var o = this.options;

			//Refresh the droppable items
			this.refresh(); this.refreshPositions();

			//Create and append the visible helper
			this.helper = typeof o.helper == 'function' ? $(o.helper.apply(this.element[0], [e, this.currentItem])) : this.currentItem.clone();
			this.helper.appendTo(this.currentItem[0].parentNode).css({ position: 'absolute', clear: 'both' }).addClass('ui-sortable-helper');

			//Find out the next positioned parent
			this.offsetParent = (function(cp) {
				while(cp) {
					if(cp.style && (/(absolute|relative|fixed)/).test($.css(cp,'position'))) return $(cp);
					cp = cp.parentNode ? cp.parentNode : null;
				}; return $("body");		
			})(this.helper[0].parentNode);
			
			//Prepare variables for position generation
			this.elementOffset = this.currentItem.offset();
			this.offsetParentOffset = this.offsetParent.offset();
			var elementPosition = { left: this.elementOffset.left - this.offsetParentOffset.left, top: this.elementOffset.top - this.offsetParentOffset.top };
			this._pageX = e.pageX; this._pageY = e.pageY;
			this.clickOffset = { left: e.pageX - this.elementOffset.left, top: e.pageY - this.elementOffset.top };
			var r = this.helper.css('position') == 'relative';
			
			//Generate the original position
			this.originalPosition = {
				left: (r ? parseInt(this.helper.css('left'),10) || 0 : elementPosition.left + (this.offsetParent[0] == document.body ? 0 : this.offsetParent[0].scrollLeft)),
				top: (r ? parseInt(this.helper.css('top'),10) || 0 : elementPosition.top + (this.offsetParent[0] == document.body ? 0 : this.offsetParent[0].scrollTop))
			};
			
			//Generate a flexible offset that will later be subtracted from e.pageX/Y
			//I hate margins - they need to be removed before positioning the element absolutely..
			this.offset = {
				left: e.pageX - this.originalPosition.left + (parseInt(this.currentItem.css('marginLeft'),10) || 0),
				top: e.pageY - this.originalPosition.top + (parseInt(this.currentItem.css('marginTop'),10) || 0)
			};

			//Save the first time position
			this.position = { top: e.pageY - this.offset.top, left: e.pageX - this.offset.left };
			this.positionAbs = { left: e.pageX - this.clickOffset.left, top: e.pageY - this.clickOffset.top };
			this.positionDOM = this.currentItem.prev()[0];

			//If o.placeholder is used, create a new element at the given position with the class
			if(o.placeholder) this.createPlaceholder();

			//Call plugins and callbacks
			this.propagate("start", e);

			//Save and store the helper proportions
			this.helperProportions = { width: this.helper.outerWidth(), height: this.helper.outerHeight() };
			
			//Set the original element visibility to hidden to still fill out the white space	
			$(this.currentItem).css('visibility', 'hidden');

			return false;
						
		},
		stop: function(e) {

			this.propagate("stop", e); //Call plugins and trigger callbacks
			if(this.positionDOM != this.currentItem.prev()[0]) this.propagate("update", e);
			
			if(this.cancelHelperRemoval) return false;			
			$(this.currentItem).css('visibility', '');
			if(this.placeholder) this.placeholder.remove();
			this.helper.remove();

			return false;
			
		},
		drag: function(e) {

			//Compute the helpers position
			this.direction = (this.floating && this.positionAbs.left > e.pageX - this.clickOffset.left) || (this.positionAbs.top > e.pageY - this.clickOffset.top) ? 'down' : 'up';
			this.position = { top: e.pageY - this.offset.top, left: e.pageX - this.offset.left };
			this.positionAbs = { left: e.pageX - this.clickOffset.left, top: e.pageY - this.clickOffset.top };

			//Rearrange
			for (var i = this.items.length - 1; i >= 0; i--) {
				if(this.intersectsWith(this.items[i]) && this.items[i].item[0] != this.currentItem[0] && (this.options.tree ? !this.currentItem[0].contains(this.items[i].item[0]) : true)) {
					//Rearrange the DOM
					this.items[i].item[this.direction == 'down' ? 'before' : 'after'](this.currentItem);
					this.refreshPositions(true); //Precompute after each DOM insertion, NOT on mousemove
					if(this.placeholderElement) this.placeholder.css(this.placeholderElement.offset());
					this.propagate("change", e); //Call plugins and callbacks
					break;
				}
			}

			this.propagate("sort", e); //Call plugins and callbacks
			this.helper.css({ left: this.position.left+'px', top: this.position.top+'px' }); // Stick the helper to the cursor
			return false;
			
		}
	});

})(jQuery);








/***************************************************************************

								 UI.SORTABLE.EXT.JS

****************************************************************************/




/*
 * 'this' -> original element
 * 1. argument: browser event
 * 2.argument: ui object
 */

(function($) {

	$.ui.plugin.add("sortable", "cursor", {
		start: function(e,ui) {
			var t = $('body');
			if (t.css("cursor")) ui.options._cursor = t.css("cursor");
			t.css("cursor", ui.options.cursor);
		},
		stop: function(e,ui) {
			if (ui.options._cursor) $('body').css("cursor", ui.options._cursor);
		}
	});

	$.ui.plugin.add("sortable", "zIndex", {
		start: function(e,ui) {
			var t = ui.helper;
			if(t.css("zIndex")) ui.options._zIndex = t.css("zIndex");
			t.css('zIndex', ui.options.zIndex);
		},
		stop: function(e,ui) {
			if(ui.options._zIndex) $(ui.helper).css('zIndex', ui.options._zIndex);
		}
	});

	$.ui.plugin.add("sortable", "opacity", {
		start: function(e,ui) {
			var t = ui.helper;
			if(t.css("opacity")) ui.options._opacity = t.css("opacity");
			t.css('opacity', ui.options.opacity);
		},
		stop: function(e,ui) {
			if(ui.options._opacity) $(ui.helper).css('opacity', ui.options._opacity);
		}
	});


	$.ui.plugin.add("sortable", "revert", {
		stop: function(e,ui) {
			var self = ui.instance;
			self.cancelHelperRemoval = true;
			var cur = self.currentItem.offset();
			if(ui.instance.options.zIndex) ui.helper.css('zIndex', ui.instance.options.zIndex); //Do the zIndex again because it already was resetted by the plugin above on stop

			//Also animate the placeholder if we have one
			if(ui.instance.placeholder) ui.instance.placeholder.animate({ opacity: 'hide' }, parseInt(ui.options.revert, 10) || 500);
			
			ui.helper.animate({
				left: cur.left - self.offsetParentOffset.left - (parseInt(self.currentItem.css('marginLeft'),10) || 0),
				top: cur.top - self.offsetParentOffset.top - (parseInt(self.currentItem.css('marginTop'),10) || 0)
			}, parseInt(ui.options.revert, 10) || 500, function() {
				self.currentItem.css('visibility', 'visible');
				window.setTimeout(function() {
					if(self.placeholder) self.placeholder.remove();
					self.helper.remove();
					if(ui.options._zIndex) ui.helper.css('zIndex', ui.options._zIndex);
				}, 50);
			});
		}
	});

	
	$.ui.plugin.add("sortable", "containment", {
		start: function(e,ui) {

			var o = ui.options;
			if((o.containment.left != undefined || o.containment.constructor == Array) && !o._containment) return;
			if(!o._containment) o._containment = o.containment;

			if(o._containment == 'parent') o._containment = this[0].parentNode;
			if(o._containment == 'document') {
				o.containment = [
					0,
					0,
					$(document).width(),
					($(document).height() || document.body.parentNode.scrollHeight)
				];
			} else { //I'm a node, so compute top/left/right/bottom

				var ce = $(o._containment)[0];
				var co = $(o._containment).offset();

				o.containment = [
					co.left,
					co.top,
					co.left+(ce.offsetWidth || ce.scrollWidth),
					co.top+(ce.offsetHeight || ce.scrollHeight)
				];
			}

		},
		sort: function(e,ui) {

			var o = ui.options;
			var h = ui.helper;
			var c = o.containment;
			var self = ui.instance;
			
			if(c.constructor == Array) {
				if((ui.absolutePosition.left < c[0])) self.position.left = c[0] - (self.offset.left - self.clickOffset.left);
				if((ui.absolutePosition.top < c[1])) self.position.top = c[1] - (self.offset.top - self.clickOffset.top);
				if(ui.absolutePosition.left - c[2] + self.helperProportions.width >= 0) self.position.left = c[2] - (self.offset.left - self.clickOffset.left) - self.helperProportions.width;
				if(ui.absolutePosition.top - c[3] + self.helperProportions.height >= 0) self.position.top = c[3] - (self.offset.top - self.clickOffset.top) - self.helperProportions.height;
			} else {
				if((ui.position.left < c.left)) self.position.left = c.left;
				if((ui.position.top < c.top)) self.position.top = c.top;
				if(ui.position.left - self.offsetParent.innerWidth() + self.helperProportions.width + c.right + (parseInt(self.offsetParent.css("borderLeftWidth"), 10) || 0) + (parseInt(self.offsetParent.css("borderRightWidth"), 10) || 0) >= 0) self.position.left = self.offsetParent.innerWidth() - self.helperProportions.width - c.right - (parseInt(self.offsetParent.css("borderLeftWidth"), 10) || 0) - (parseInt(self.offsetParent.css("borderRightWidth"), 10) || 0);
				if(ui.position.top - self.offsetParent.innerHeight() + self.helperProportions.height + c.bottom + (parseInt(self.offsetParent.css("borderTopWidth"), 10) || 0) + (parseInt(self.offsetParent.css("borderBottomWidth"), 10) || 0) >= 0) self.position.top = self.offsetParent.innerHeight() - self.helperProportions.height - c.bottom - (parseInt(self.offsetParent.css("borderTopWidth"), 10) || 0) - (parseInt(self.offsetParent.css("borderBottomWidth"), 10) || 0);
			}

		}
	});

	$.ui.plugin.add("sortable", "axis", {
		sort: function(e,ui) {
			var o = ui.options;
			if(o.constraint) o.axis = o.constraint; //Legacy check
			o.axis == 'x' ? ui.instance.position.top = ui.instance.originalPosition.top : ui.instance.position.left = ui.instance.originalPosition.left;
		}
	});

	$.ui.plugin.add("sortable", "scroll", {
		start: function(e,ui) {
			var o = ui.options;
			o.scrollSensitivity	= o.scrollSensitivity || 20;
			o.scrollSpeed		= o.scrollSpeed || 20;

			ui.instance.overflowY = function(el) {
				do { if((/auto|scroll/).test(el.css('overflow')) || (/auto|scroll/).test(el.css('overflow-y'))) return el; el = el.parent(); } while (el[0].parentNode);
				return $(document);
			}(this);
			ui.instance.overflowX = function(el) {
				do { if((/auto|scroll/).test(el.css('overflow')) || (/auto|scroll/).test(el.css('overflow-x'))) return el; el = el.parent(); } while (el[0].parentNode);
				return $(document);
			}(this);
		},
		sort: function(e,ui) {
			
			var o = ui.options;
			var i = ui.instance;

			if(i.overflowY[0] != document && i.overflowY[0].tagName != 'HTML') {
				if(i.overflowY[0].offsetHeight - (ui.position.top - i.overflowY[0].scrollTop + i.clickOffset.top) < o.scrollSensitivity)
					i.overflowY[0].scrollTop = i.overflowY[0].scrollTop + o.scrollSpeed;
				if((ui.position.top - i.overflowY[0].scrollTop + i.clickOffset.top) < o.scrollSensitivity)
					i.overflowY[0].scrollTop = i.overflowY[0].scrollTop - o.scrollSpeed;				
			} else {
				//$(document.body).append('<p>'+(e.pageY - $(document).scrollTop())+'</p>');
				if(e.pageY - $(document).scrollTop() < o.scrollSensitivity)
					$(document).scrollTop($(document).scrollTop() - o.scrollSpeed);
				if($(window).height() - (e.pageY - $(document).scrollTop()) < o.scrollSensitivity)
					$(document).scrollTop($(document).scrollTop() + o.scrollSpeed);
			}
			
			if(i.overflowX[0] != document && i.overflowX[0].tagName != 'HTML') {
				if(i.overflowX[0].offsetWidth - (ui.position.left - i.overflowX[0].scrollLeft + i.clickOffset.left) < o.scrollSensitivity)
					i.overflowX[0].scrollLeft = i.overflowX[0].scrollLeft + o.scrollSpeed;
				if((ui.position.top - i.overflowX[0].scrollLeft + i.clickOffset.left) < o.scrollSensitivity)
					i.overflowX[0].scrollLeft = i.overflowX[0].scrollLeft - o.scrollSpeed;				
			} else {
				if(e.pageX - $(document).scrollLeft() < o.scrollSensitivity)
					$(document).scrollLeft($(document).scrollLeft() - o.scrollSpeed);
				if($(window).width() - (e.pageX - $(document).scrollLeft()) < o.scrollSensitivity)
					$(document).scrollLeft($(document).scrollLeft() + o.scrollSpeed);
			}
			
			ui.instance.recallOffset(e);

		}
	});

})(jQuery);








/***************************************************************************

								 UI.TABS.JS

****************************************************************************/



/*
 * Tabs 3 - New Wave Tabs
 *
 * Copyright (c) 2007 Klaus Hartl (stilbuero.de)
 * Dual licensed under the MIT (MIT-LICENSE.txt)
 * and GPL (GPL-LICENSE.txt) licenses.
 *
 * http://docs.jquery.com/UI/Tabs
 */

(function($) {

    // if the UI scope is not availalable, add it
    $.ui = $.ui || {};

    // tabs API methods
    $.fn.tabs = function() {
        var method = typeof arguments[0] == 'string' && arguments[0];
        var args = method && Array.prototype.slice.call(arguments, 1) || arguments;

        return this.each(function() {
            if (method) {
                var tabs = $.data(this, 'ui-tabs');
                tabs[method].apply(tabs, args);
            } else
                new $.ui.tabs(this, args[0] || {});
        });
    };

    // tabs class
    $.ui.tabs = function(el, options) {
        var self = this;

        this.element = el;

        this.options = $.extend({

            // basic setup
            selected: 0,
            unselect: options.selected === null,
            event: 'click',
            disabled: [],
            cookie: null, // pass options object as expected by cookie plugin: { expires: 7, path: '/', domain: 'jquery.com', secure: true }
            // TODO bookmarkable: $.ajaxHistory ? true : false,

            // Ajax
            spinner: 'Loading&#8230;',
            cache: false,
            idPrefix: 'ui-tabs-',
            ajaxOptions: {},

            // animations
            fx: null, /* e.g. { height: 'toggle', opacity: 'toggle', duration: 200 } */

            // templates
            tabTemplate: '<li><a href="#{href}"><span>#{label}</span></a></li>',
            panelTemplate: '<div></div>',

            // CSS classes
            navClass: 'ui-tabs-nav',
            selectedClass: 'ui-tabs-selected',
            unselectClass: 'ui-tabs-unselect',
            disabledClass: 'ui-tabs-disabled',
            panelClass: 'ui-tabs-panel',
            hideClass: 'ui-tabs-hide',
            loadingClass: 'ui-tabs-loading'

        }, options);

        this.options.event += '.ui-tabs'; // namespace event
        this.options.cookie = $.cookie && $.cookie.constructor == Function && this.options.cookie;

        $(el).bind('setData.ui-tabs', function(event, key, value) {
            self.options[key] = value;
            this.tabify();
        }).bind('getData.ui-tabs', function(event, key) {
            return self.options[key];
        });

        // save instance for later
        $.data(el, 'ui-tabs', this);

        // create tabs
        this.tabify(true);
    };

    // instance methods
    $.extend($.ui.tabs.prototype, {
        tabId: function(a) {
            return a.title && a.title.replace(/\s/g, '_').replace(/[^A-Za-z0-9\-_:\.]/g, '')
                || this.options.idPrefix + $.data(a);
        },
        ui: function(tab, panel) {
            return {
                instance: this,
                options: this.options,
                tab: tab,
                panel: panel
            };
        },
        tabify: function(init) {

            this.$lis = $('li:has(a[href])', this.element);
            this.$tabs = this.$lis.map(function() { return $('a', this)[0]; });
            this.$panels = $([]);

            var self = this, o = this.options;

            this.$tabs.each(function(i, a) {
                // inline tab
                if (a.hash && a.hash.replace('#', '')) // Safari 2 reports '#' for an empty hash
                    self.$panels = self.$panels.add(a.hash);
                // remote tab
                else if ($(a).attr('href') != '#') { // prevent loading the page itself if href is just "#"
                    $.data(a, 'href.ui-tabs', a.href); // required for restore on destroy
                    $.data(a, 'load.ui-tabs', a.href); // mutable
                    var id = self.tabId(a);
                    a.href = '#' + id;
                    var $panel = $('#' + id);
                    if (!$panel.length) {
                        $panel = $(o.panelTemplate).attr('id', id).addClass(o.panelClass)
                            .insertAfter( self.$panels[i - 1] || self.element );
                        $panel.data('destroy.ui-tabs', true);
                    }
                    self.$panels = self.$panels.add( $panel );
                }
                // invalid tab href
                else
                    o.disabled.push(i + 1);
            });

            if (init) {

                // attach necessary classes for styling if not present
                $(this.element).hasClass(o.navClass) || $(this.element).addClass(o.navClass);
                this.$panels.each(function() {
                    var $this = $(this);
                    $this.hasClass(o.panelClass) || $this.addClass(o.panelClass);
                });

                // disabled tabs
                for (var i = 0, index; index = o.disabled[i]; i++)
                    this.disable(index);

                // Try to retrieve selected tab:
                // 1. from fragment identifier in url if present
                // 2. from cookie
                // 3. from selected class attribute on <li>
                // 4. otherwise use given "selected" option
                // 5. check if tab is disabled
                this.$tabs.each(function(i, a) {
                    if (location.hash) {
                        if (a.hash == location.hash) {
                            o.selected = i;
                            // prevent page scroll to fragment
                            //if (($.browser.msie || $.browser.opera) && !o.remote) {
                            if ($.browser.msie || $.browser.opera) {
                                var $toShow = $(location.hash), toShowId = $toShow.attr('id');
                                $toShow.attr('id', '');
                                setTimeout(function() {
                                    $toShow.attr('id', toShowId); // restore id
                                }, 500);
                            }
                            scrollTo(0, 0);
                            return false; // break
                        }
                    } else if (o.cookie) {
                        var index = parseInt($.cookie('ui-tabs' + $.data(self.element)),10);
                        if (index && self.$tabs[index]) {
                            o.selected = index;
                            return false; // break
                        }
                    } else if ( self.$lis.eq(i).hasClass(o.selectedClass) ) {
                        o.selected = i;
                        return false; // break
                    }
                });
                var n = this.$lis.length;
                while (this.$lis.eq(o.selected).hasClass(o.disabledClass) && n) {
                    o.selected = ++o.selected < this.$lis.length ? o.selected : 0;
                    n--;
                }
                if (!n) // all tabs disabled, set option unselect to true
                    o.unselect = true;

                // highlight selected tab
                this.$panels.addClass(o.hideClass);
                this.$lis.removeClass(o.selectedClass);
                if (!o.unselect) {
                    this.$panels.eq(o.selected).show().removeClass(o.hideClass); // use show and remove class to show in any case no matter how it has been hidden before
                    this.$lis.eq(o.selected).addClass(o.selectedClass);
                }

                // load if remote tab
                var href = !o.unselect && $.data(this.$tabs[o.selected], 'load.ui-tabs');
                if (href)
                    this.load(o.selected, href);

                // disable click if event is configured to something else
                if (!(/^click/).test(o.event))
                    this.$tabs.bind('click', function(e) { e.preventDefault(); });

            }

            var hideFx, showFx, baseFx = { 'min-width': 0, duration: 1 }, baseDuration = 'normal';
            if (o.fx && o.fx.constructor == Array)
                hideFx = o.fx[0] || baseFx, showFx = o.fx[1] || baseFx;
            else
                hideFx = showFx = o.fx || baseFx;

            // reset some styles to maintain print style sheets etc.
            var resetCSS = { display: '', overflow: '', height: '' };
            if (!$.browser.msie) // not in IE to prevent ClearType font issue
                resetCSS.opacity = '';

            // Hide a tab, animation prevents browser scrolling to fragment,
            // $show is optional.
            function hideTab(clicked, $hide, $show) {
                $hide.animate(hideFx, hideFx.duration || baseDuration, function() { //
                    $hide.addClass(o.hideClass).css(resetCSS); // maintain flexible height and accessibility in print etc.
                    if ($.browser.msie && hideFx.opacity)
                        $hide[0].style.filter = '';
                    if ($show)
                        showTab(clicked, $show, $hide);
                });
            }

            // Show a tab, animation prevents browser scrolling to fragment,
            // $hide is optional.
            function showTab(clicked, $show, $hide) {
                if (showFx === baseFx)
                    $show.css('display', 'block'); // prevent occasionally occuring flicker in Firefox cause by gap between showing and hiding the tab panels
                $show.animate(showFx, showFx.duration || baseDuration, function() {
                    $show.removeClass(o.hideClass).css(resetCSS); // maintain flexible height and accessibility in print etc.
                    if ($.browser.msie && showFx.opacity)
                        $show[0].style.filter = '';

                    // callback
                    $(self.element).triggerHandler("show.ui-tabs", [self.ui(clicked, $show[0])]);

                });
            }

            // switch a tab
            function switchTab(clicked, $li, $hide, $show) {
                /*if (o.bookmarkable && trueClick) { // add to history only if true click occured, not a triggered click
                    $.ajaxHistory.update(clicked.hash);
                }*/
                $li.addClass(o.selectedClass)
                    .siblings().removeClass(o.selectedClass);
                hideTab(clicked, $hide, $show);
            }

            // attach tab event handler, unbind to avoid duplicates from former tabifying...
            this.$tabs.unbind(o.event).bind(o.event, function() {

                //var trueClick = e.clientX; // add to history only if true click occured, not a triggered click
                var $li = $(this).parents('li:eq(0)'),
                    $hide = self.$panels.filter(':visible'),
                    $show = $(this.hash);

                // If tab is already selected and not unselectable or tab disabled or click callback returns false stop here.
                // Check if click handler returns false last so that it is not executed for a disabled tab!
                if (($li.hasClass(o.selectedClass) && !o.unselect) || $li.hasClass(o.disabledClass)
                    || $(self.element).triggerHandler("select.ui-tabs", [self.ui(this, $show[0])]) === false) {
                    this.blur();
                    return false;
                }

                self.options.selected = self.$tabs.index(this);

                // if tab may be closed
                if (o.unselect) {
                    if ($li.hasClass(o.selectedClass)) {
                        self.options.selected = null;
                        $li.removeClass(o.selectedClass);
                        self.$panels.stop();
                        hideTab(this, $hide);
                        this.blur();
                        return false;
                    } else if (!$hide.length) {
                        self.$panels.stop();
                        var a = this;
                        self.load(self.$tabs.index(this), function() {
                            $li.addClass(o.selectedClass).addClass(o.unselectClass);
                            showTab(a, $show);
                        });
                        this.blur();
                        return false;
                    }
                }

                if (o.cookie)
                    $.cookie('ui-tabs' + $.data(self.element), self.options.selected, o.cookie);

                // stop possibly running animations
                self.$panels.stop();

                // show new tab
                if ($show.length) {

                    // prevent scrollbar scrolling to 0 and than back in IE7, happens only if bookmarking/history is enabled
                    /*if ($.browser.msie && o.bookmarkable) {
                        var showId = this.hash.replace('#', '');
                        $show.attr('id', '');
                        setTimeout(function() {
                            $show.attr('id', showId); // restore id
                        }, 0);
                    }*/

                    var a = this;
                    self.load(self.$tabs.index(this), function() {
                        switchTab(a, $li, $hide, $show);
                    });

                    // Set scrollbar to saved position - need to use timeout with 0 to prevent browser scroll to target of hash
                    /*var scrollX = window.pageXOffset || document.documentElement && document.documentElement.scrollLeft || document.body.scrollLeft || 0;
                    var scrollY = window.pageYOffset || document.documentElement && document.documentElement.scrollTop || document.body.scrollTop || 0;
                    setTimeout(function() {
                        scrollTo(scrollX, scrollY);
                    }, 0);*/

                } else
                    throw 'jQuery UI Tabs: Mismatching fragment identifier.';

                // Prevent IE from keeping other link focussed when using the back button
                // and remove dotted border from clicked link. This is controlled in modern
                // browsers via CSS, also blur removes focus from address bar in Firefox
                // which can become a usability and annoying problem with tabsRotate.
                if ($.browser.msie)
                    this.blur();

                //return o.bookmarkable && !!trueClick; // convert trueClick == undefined to Boolean required in IE
                return false;

            });

        },
        add: function(url, label, index) {
            if (url && label) {
                index = index || this.$tabs.length; // append by default

                var o = this.options;
                var $li = $(o.tabTemplate.replace(/#\{href\}/, url).replace(/#\{label\}/, label));
                $li.data('destroy.ui-tabs', true);

                var id = url.indexOf('#') == 0 ? url.replace('#', '') : this.tabId( $('a:first-child', $li)[0] );

                // try to find an existing element before creating a new one
                var $panel = $('#' + id);
                if (!$panel.length) {
                    $panel = $(o.panelTemplate).attr('id', id)
                        .addClass(o.panelClass).addClass(o.hideClass);
                    $panel.data('destroy.ui-tabs', true);
                }
                if (index >= this.$lis.length) {
                    $li.appendTo(this.element);
                    $panel.appendTo(this.element.parentNode);
                } else {
                    $li.insertBefore(this.$lis[index]);
                    $panel.insertBefore(this.$panels[index]);
                }

                this.tabify();

                if (this.$tabs.length == 1) {
                     $li.addClass(o.selectedClass);
                     $panel.removeClass(o.hideClass);
                     var href = $.data(this.$tabs[0], 'load.ui-tabs');
                     if (href)
                         this.load(index, href);
                }

                // callback
                $(this.element).triggerHandler("add.ui-tabs",
                    [this.ui(this.$tabs[index], this.$panels[index])]
                );

            } else
                throw 'jQuery UI Tabs: Not enough arguments to add tab.';
        },
        remove: function(index) {
            if (index && index.constructor == Number) {
                var o = this.options, $li = this.$lis.eq(index).remove(),
                    $panel = this.$panels.eq(index).remove();

                // If selected tab was removed focus tab to the right or
                // tab to the left if last tab was removed.
                if ($li.hasClass(o.selectedClass) && this.$tabs.length > 1)
                    this.click(index + (index < this.$tabs.length ? 1 : -1));
                this.tabify();

                // callback
                $(this.element).triggerHandler("remove.ui-tabs",
                    [this.ui($li.find('a')[0], $panel[0])]
                );

            }
        },
        enable: function(index) {
            var self = this, o = this.options, $li = this.$lis.eq(index);
            $li.removeClass(o.disabledClass);
            if ($.browser.safari) { // fix disappearing tab (that used opacity indicating disabling) after enabling in Safari 2...
                $li.css('display', 'inline-block');
                setTimeout(function() {
                    $li.css('display', 'block');
                }, 0);
            }

            o.disabled = $.map(this.$lis.filter('.' + o.disabledClass),
                function(n, i) { return self.$lis.index(n); } );

            // callback
            $(this.element).triggerHandler("enable.ui-tabs",
                [this.ui(this.$tabs[index], this.$panels[index])]
            );

        },
        disable: function(index) {
            var self = this, o = this.options;
            this.$lis.eq(index).addClass(o.disabledClass);

            o.disabled = $.map(this.$lis.filter('.' + o.disabledClass),
                function(n, i) { return self.$lis.index(n); } );

            // callback
            $(this.element).triggerHandler("disable.ui-tabs",
                [this.ui(this.$tabs[index], this.$panels[index])]
            );

        },
        select: function(index) {
            if (typeof index == 'string')
                index = this.$tabs.index( this.$tabs.filter('[href$=' + index + ']')[0] );
            this.$tabs.eq(index).trigger(this.options.event);
        },
        load: function(index, callback) { // callback is for internal usage only
            var self = this, o = this.options,
                $a = this.$tabs.eq(index), a = $a[0];

            var url = $a.data('load.ui-tabs');

            // no remote - just finish with callback
            if (!url) {
                typeof callback == 'function' && callback();
                return;
            }

            // load remote from here on
            if (o.spinner) {
                var $span = $('span', a), label = $span.html();
                $span.html('<em>' + o.spinner + '</em>');
            }
            var finish = function() {
                self.$tabs.filter('.' + o.loadingClass).each(function() {
                    $(this).removeClass(o.loadingClass);
                    if (o.spinner)
                        $('span', this).html(label);
                });
                self.xhr = null;
            };
            var ajaxOptions = $.extend({}, o.ajaxOptions, {
                url: url,
                success: function(r, s) {
                    $(a.hash).html(r);
                    finish();
                    // This callback is required because the switch has to take
                    // place after loading has completed.
                    typeof callback == 'function' && callback();

                    if (o.cache)
                        $.removeData(a, 'load.ui-tabs'); // if loaded once do not load them again

                    // callback
                    $(self.element).triggerHandler("load.ui-tabs",
                        [self.ui(self.$tabs[index], self.$panels[index])]
                    );

                    o.ajaxOptions.success && o.ajaxOptions.success(r, s);
                }
            });
            if (this.xhr) {
                // terminate pending requests from other tabs and restore tab label
                this.xhr.abort();
                finish();
            }
            $a.addClass(o.loadingClass);
            setTimeout(function() { // timeout is again required in IE, "wait" for id being restored
                self.xhr = $.ajax(ajaxOptions);
            }, 0);

        },
        url: function(index, url) {
            this.$tabs.eq(index).data('load.ui-tabs', url);
        },
        destroy: function() {
            var o = this.options;
            $(this.element).unbind('.ui-tabs')
                .removeClass(o.navClass).removeData('ui-tabs');
            this.$tabs.each(function() {
                var href = $.data(this, 'href.ui-tabs');
                if (href)
                    this.href = href;
                $(this).unbind('.ui-tabs')
                    .removeData('href.ui-tabs').removeData('load.ui-tabs');
            });
            this.$lis.add(this.$panels).each(function() {
                if ($.data(this, 'destroy.ui-tabs'))
                    $(this).remove();
                else
                    $(this).removeClass([o.selectedClass, o.unselectClass,
                        o.disabledClass, o.panelClass, o.hideClass].join(' '));
            });
        }
    });

})(jQuery);





/***************************************************************************

								 UI.TABS.EXT.JS

****************************************************************************/


/*
 * Tabs 3 extensions
 *
 * Copyright (c) 2007 Klaus Hartl (stilbuero.de)
 * Dual licensed under the MIT (MIT-LICENSE.txt)
 * and GPL (GPL-LICENSE.txt) licenses.
 *
 * http://docs.jquery.com/UI/TabsExtensions
 */
 
(function($) {
    
    /*
     * Rotate
     */
    $.extend($.ui.tabs.prototype, {
        rotation: null,
        rotate: function(ms) {
            var self = this;
            function stop(e) {
                if (e.clientX) { // only in case of a true click
                    clearInterval(self.rotation);
                }
            }
            // start interval
            if (ms) {
                var t = this.options.selected;
                this.rotation = setInterval(function() {
                    t = ++t < self.$tabs.length ? t : 0;
                    self.click(t);
                }, ms);
                this.$tabs.bind(this.options.event, stop);
            }
            // stop interval
            else {
                clearInterval(this.rotation);
                this.$tabs.unbind(this.options.event, stop);
            }
        }
    });

})(jQuery);





/***************************************************************************

								 UI.TREE.JS

****************************************************************************/



(function($) {

	// Plugin to wrap html around all non-empty text nodes within an element: (ignores text in child elements)
	// By George Adamson, SoftwareUnity.com, March 2007. 
	$.fn.wrapText = function(html){
		return this.each(function(){
			$(this.childNodes).filter("[@nodeType=3]").each(function(){
				if($.trim(this.nodeValue).length > 0)
					$(this).wrap(html);
			})
		});
	};
  
  
	//If the UI scope is not availalable, add it
	$.ui = $.ui || {};

	$.fn.tree = function(o) {
		return this.each(function() {
			new $.ui.tree(this,o);	
		});
	}
	
	
	$.ui.tree = function(el, o) {

		var self = this;

		self.options = {};
		$.extend(self.options, o);
		
		var SHIFT = false;
		var CTRL = false;
		var ALT = false;
		var CUT = false;
		var shiftSelPos = 0;
		var dragging = false;
		var dragNode = $([]);
		var dragNodes = $([]);
		var dragStartX = -1;
		var dragStartY = -1;
		var dragIndented = false;
		var selecting = false;
		var selectStartNode = $([]);
		var selectStartX = -1;
		var selectStartY = -1;
	
		var tree = el;
		var outline = $(document.createElement('a'));
		outline.attr('href', '#').addClass('ui-tree-outline').css({display: 'block !important', 'text-decoration': 'none' });
		$(tree).wrap(outline);
		outline = $(tree).parent('a.ui-tree-outline');
		$(tree).addClass('ui-tree').addClass('ui-tree-nodes').children('li').addClass('ui-tree-node');
		outline.css('MozUserSelect', 'none').attr('unselectable', 'on');
		var nodes = $('ul',tree).addClass('ui-tree-nodes')
			.css('MozUserSelect', 'none').attr('unselectable', 'on');
		var node = $('li',tree).addClass('ui-tree-node')
			.css('MozUserSelect', 'none').attr('unselectable', 'on');

		$("<span class='ui-tree-node-button'>&bull;&nbsp;</span>")
			.prependTo(node);
		$('.ui-tree-node-button')
			.click(function() {
				toggle(parent($(this)));
				select($('.ui-tree-node-selected',tree));
				return false;
			});
	
		node
			.wrapText("<span class='ui-tree-node-text'></span>")
			.each(function() {
				var node = $(this);
				if (node.children('.ui-tree-nodes').length) {
					node.addClass('ui-tree-node-expanded');
					node.children('.ui-tree-node-button').text('- ');
				}
			})
			.click(function() {
				return false;
			});
	
		$(".ui-tree-node-text", tree)
			.mousedown(function(ev) {
				var node = parent($(this));
				if (node.is('.ui-tree-node-selected')) {
					if (node.is('.ui-tree-node-moving')) {
						$('.ui-tree-node-moving').removeClass('ui-tree-node-moving');
					} else {
						dragNode = node;
						dragNodes = dragNode.siblings('.ui-tree-node-selected').add(node);
						dragNodes.addClass('ui-tree-node-moving');
						dragging = true;
						dragStartX = ev.pageX;
						dragStartY = ev.pageY;
					}
					return false;
				} else {
					outline.focus();
					if (!ev.ctrlKey && !ev.shiftKey)
						unselect($('.ui-tree-node-selected'));
					select(node);
					selectStartNode = node;
					selecting = true;
					return false;
				}
			})
			.mousemove(function(ev) {
				var target = parent($(this));
				if (dragging) {
					if (target.parents('.ui-tree-node-moving').length)
						return false;
					if (target.is('.ui-tree-node-moving')) {
						if (ev.pageX - dragStartX > 15) {
							if (!dragIndented) {
								indent(dragNodes);
								dragIndented = true;
							}
						} else {
							if (dragIndented) {
								unindent(dragNodes);
								dragIndented = false;
							}
						}
					} else {
						if (dragIndented) {
							unindent(dragNodes);
							dragIndented = false;
						}
						var oldParent = parent(dragNodes);
						dragNodes.insertBefore(target);
						update(oldParent);
						dragStartX = ev.pageX;
						dragStartY = ev.pageY;
					}
				} else {
					if (!ev.altKey) {
						ALT = false;
						$('.ui-tree-node-moving').removeClass('ui-tree-node-moving');
					}
					if (selecting) {
						var parentNode = parent(selectStartNode);
						var nodes = parentNode.children('.ui-tree-nodes').children('.ui-tree-node');
						var a, b;
						var on = false;
						nodes.each(function(i) {
							if ($(this)[0] == selectStartNode[0]) a = i;
							if ($(this)[0] == target[0]) b = i;
						});
						if (a && b) {
							if (b > a)
								{ var swp = a; a = b; b = swp; }
							//TODO: Shift-Select
						}
						if (parent(selectStartNode)[0] == parent(target)[0]) {
							//target.addClass('ui-tree-node-selected');
							andSelect(target);
							return false;
						}
					}
				}
			})
			.mouseup(function(ev) {
				if (dragging) {
					dragNodes = $('.ui-tree-node-moving');
					dragNodes.removeClass('ui-tree-node-moving');
					dragNode = dragNodes = $([]);
					dragging = false;
					dragStartX = dragStartY = -1;
				} else if (selecting) {
					selecting = false;
					selectStartNode = $([]);
					selectStartX = selectStartY = -1;
				} else {
					var node = parent($(this));
					if (node.is('.ui-tree-node-selected')) {
						var selected = $('.ui-tree-node-selected');
						selected.addClass('ui-tree-node-moving');
					} else {
						if (ev.ctrlKey) {
							andSelect(node);
						} else {
							select(node);
						}
					}
				}
				return false;
			})
			.dblclick(function() {
				toggle(parent($(this)));
				return false;
			});

		outline
		.keydown(function(ev) {
			if (ev.keyCode == 16) { SHIFT = true; }
			if (ev.keyCode == 17) { CTRL = true; }
			if (ev.keyCode == 18) {
				ALT = true;
				$('.ui-tree-node-selected').addClass('ui-tree-node-moving');
			}
			if (ALT && !ev.altKey) {
				ALT = false;
				if (!CUT)
					$('.ui-tree-node-moving').removeClass('ui-tree-node-moving');
			}
			var TAB = (ev.keyCode == 9), HOME = (ev.keyCode == 36), END = (ev.keyCode == 35); var SPACE = (ev.keyCode == 32);
			var LEFT = (ev.keyCode == 37), UP = (ev.keyCode == 38), RIGHT = (ev.keyCode == 39), DOWN = (ev.keyCode == 40);
			var DEL = (ev.keyCode == 46);
			var X = (ev.charCode == 88 || ev.charCode == 120 || ev.keyCode == 88 || ev.keyCode == 120);
			var V = (ev.charCode == 86 || ev.charCode == 118 || ev.keyCode == 86 || ev.keyCode == 118);
			var node = $('.ui-tree-node-selected',this);
			var prevNode = prev(node.filter(':first'));
			var nextNode = next(node.filter(':last'));
			var parentNode = parent(node);
			var firstNode = node.siblings('.ui-tree-node:first');
			var lastNode = node.siblings('.ui-tree-node:last');
			var isFirst = (prevNode.length == 0);
			var isLast = (nextNode.length == 0);
			var upNode;// = (isFirst) ? parentNode : (expanded(prevNode)) ? last(prevNode) : prevNode;
			if (isFirst) {
				upNode = parentNode;
			} else {
				if (expanded(prevNode)) {
					upNode = last(prevNode);
					while (expanded(upNode)) {
						upNode = last(upNode);
					}
				} else {
					upNode = prevNode;
				}
			}
			var downNode; // = (isLast && !expanded(node)) ? next(parentNode) : (expanded(node)) ? first(node) : nextNode ;
			if (isLast && !expanded(node.filter(':last'))) {
				var firstNonLastAncestor = node.filter(':last').parents('.ui-tree-node').filter(function() {
					return (next($(this)).length);
				}).eq(0);
				downNode = next(firstNonLastAncestor);
			} else {
				downNode = (expanded(node.filter(':last'))) ? first(node.filter(':last')) : nextNode;
			}

			if (DEL) {
				node.each(function() {
					var eachNode = $(this);
					var eachParentNode = parent(eachNode);
					eachNode.remove();
					update(eachParentNode);
				});
			}

			if (CTRL && X) {
				CUT = true;
				node.addClass('ui-tree-node-moving');
			}

			if (CTRL && V) {
				CUT = false;
				node.removeClass('ui-tree-node-moving');
			}

			if ((CUT || ALT) && HOME) {
				node.insertBefore(firstNode);
			} else if (HOME) {
				if (!isFirst) select(firstNode);
			}

			if ((CUT || ALT) && END) {
				node.insertAfter(lastNode);
			} else if (END) {
				if (!isLast) select(lastNode);
			}

			if ((CUT || ALT) && LEFT) {
				unindent(node);
			} else if (SHIFT && LEFT) {

			} else if (LEFT) {
				if (expanded(node)) {
					collapse(node);
				} else {
					select(parentNode);
				}
			}

			if ((CUT || ALT) && UP) {
				prevNode.insertAfter(node.filter(':last'));
			} else if (SHIFT && UP) {
				if (!isFirst) {
					shiftSelPos--;
					if (shiftSelPos < 0) {
						andSelect(prevNode);
					} else {
						unselect(node.filter(':last'));
					}
				}
			} else if (UP) {
				shiftSelPos = 0;
				select(upNode.filter(':first'));
			}

			if ((CUT || ALT) && RIGHT) {
				indent(node);
			} else if (SHIFT && RIGHT) {

			} else if (RIGHT) {
				if (expanded(node)) {
					select(downNode)
				} else {
					expand(node);
				}
			}

			if ((CUT || ALT) && DOWN) {
				nextNode.insertBefore(node.filter(':first'));
			} else if (SHIFT && DOWN) {
				if (!isLast) {
					shiftSelPos++;
					if (shiftSelPos > 0) {
						andSelect(nextNode);
					} else {
						unselect(node.filter(':first'));
					}
				}
			} else if (DOWN) {
				shiftSelPos = 0;
				select(downNode.filter(':last'));
			}

			if (UP || DOWN || HOME || END) { // Prevent Page/container scroll
				return false;
			}
			if (SHIFT && LEFT || RIGHT) { // Prevent text selection
				return false;
			}
			if (ALT && LEFT || RIGHT) { // Prevent forward and back navigation
				ev.preventDefault();
			}
		})
		.keyup(function(ev) {
			if (ev.keyCode == 16) {
				SHIFT = false;
			}
			if (ev.keyCode == 17) {
				CTRL = false;
			}
			if (ev.keyCode == 18) {
				ALT = false;
				if (!CUT)
					$('.ui-tree-node-moving').removeClass('ui-tree-node-moving');
				return false;
			}
		})
		.focus(function(ev) {
			if ($('.ui-tree-node-selected', this).length == 0)
				select($('.ui-tree-node:first', this));
			$('.ui-tree', this).addClass('ui-tree-active');
		})
		.blur(function(ev) {
			$('.ui-tree', this).removeClass('ui-tree-active');
			$('.ui-tree-node-moving', this).removeClass('ui-tree-node-moving');
			CUT = false;
			SHIFT = false;
		})
		;

		function collapse(node) {
			var nodes = node.children('.ui-tree-nodes');
			if (nodes.length) {
				node.removeClass('ui-tree-node-expanded').addClass('ui-tree-node-collapsed');
				node.children('.ui-tree-nodes').hide();
				node.children('.ui-tree-node-button').text('+ ');
				node.each(function() {
					$(this).triggerHandler("treecollapse", [null, {node:this}], self.options.collapse);
				})
			}
		}

		function expand(node) {
			var nodes = node.children('.ui-tree-nodes');
			if (nodes.length) {
				node.removeClass('ui-tree-node-collapsed').addClass('ui-tree-node-expanded');
				nodes.show();
				node.children('.ui-tree-node-button').text('- ');
			}
		}

		function indent(node) {
			var prevNode = prev(node.filter(':first'));
			if (!prevNode.children('.ui-tree-nodes').length) {
				prevNode.append($(document.createElement('ul')).addClass('ui-tree-nodes'));
			} else if (!expanded(prevNode)) {
				expand(prevNode);
			}
			prevNode.children('.ui-tree-nodes').append(node);
			update(prevNode);
			andSelect(node);
		}

		function unindent(node) {
			var parentNode = parent(node.filter(':last'));
			var dummy = $(document.createElement('li'));
			dummy.insertAfter(parentNode);
			node.insertBefore(dummy);
			dummy.remove();
			update(parentNode);
		}

		function toggle(node) {
			if (expanded(node)) {
				collapse(node);
			} else {
				expand(node);
			}
		}

		function unselect(node) {
			node.removeClass('ui-tree-node-selected');
			node.children('.ui-tree-node-text').css('outline', 'none');
		}

		function select(node) {
			if (node.length) {
				$('.ui-tree-node-selected').removeClass('ui-tree-node-selected').removeClass('ui-tree-node-moving')
					.children('.ui-tree-node-text').css('outline', 'none');
				shiftSelPos = 0;
				node.addClass('ui-tree-node-selected');
				node.children('.ui-tree-node-text').css('outline', '1px dotted black');
			}
		}

		function andSelect(node) {
			node.addClass('ui-tree-node-selected')
				.children('.ui-tree-node-text').css('outline', '1px dotted black');
		}

		function update(node) {
			var nodes = node.children('.ui-tree-nodes');
			if (nodes.length && nodes.children('.ui-tree-node').length) { 
				node.addClass('ui-tree-node-expanded');
				node.children('.ui-tree-node-button').text('- ');
			} else {
				node.removeClass('ui-tree-node-expanded').removeClass('ui-tree-node-collapsed');
				node.children('.ui-tree-node-button').html('&bull;&nbsp;');
				nodes.remove();
			}
		}

		function expanded(node) { return !!node.children('.ui-tree-nodes:visible').length; }
		function first(node)	{ return node.children('.ui-tree-nodes').children('.ui-tree-node:first'); }
		function last(node)	 { return node.children('.ui-tree-nodes').children('.ui-tree-node:last'); }
		function next(node)	 { return node.next('.ui-tree-node'); }
		function parent(node)   { return node.parents('.ui-tree-node:first') }
		function prev(node)	 { return node.prev('.ui-tree-node'); }
		
	}

 })($);



// *****************************************************************
// *** DONE WITH UI ************************************************
// *****************************************************************

window.$ui = jQuery.noConflict(true);


