// $Id$
// Contributed by user jmiccolis.
$(document).ready(function() { $("a[@href*='/user/login']").addClass('thickbox').each(function() { this.href = this.href.replace(/user\/login\??/,"thickbox_login?height=220&width=250&") }) });
$(document).ready(function() { $("a[@href*='?q=user/login']").addClass('thickbox').each(function() { this.href = this.href.replace(/user\/login/,"thickbox_login&height=220&width=250") }) });