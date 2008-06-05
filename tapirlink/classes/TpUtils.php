<?php
/**
 * $Id$
 * 
 * LICENSE INFORMATION
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details:
 * 
 * http://www.gnu.org/copyleft/gpl.html
 * 
 * 
 * @author Renato De Giovanni <renato [at] cria . org . br>
 * @author Dave Vieglais (Biodiversity Research Center, University of Kansas)
 * 
 */

require_once('TpDiagnostics.php');

class TpUtils 
{
    /** Instantiate two log objects: the main one called g_log and another one
     *  just for detailed debugging called g_dlog. 
     *
     */
    function InitializeLogs( ) 
    {
        // Main log
        $log_file_name = TP_LOG_DIR.'/'.TP_LOG_NAME;

        if ( ! file_exists( $log_file_name ) )
        {
            touch( $log_file_name );
        }

        // Create the log singleton
        // log entries have ID = client (portal) IP address
        $GLOBALS['g_log'] =& Log::singleton( TP_LOG_TYPE,
                                             $log_file_name,
                                             $_SERVER['REMOTE_ADDR'],
                                             unserialize( TP_LOG_OPTIONS ),
                                             TP_LOG_LEVEL );
        global $g_log;

        if ( PEAR::isError( $g_log ) ) 
        {
            $msg = 'The main log file could not be opened';
            TpDiagnostics::Append( DC_LOG_ERROR, $msg, DC_WARN );
        }

        // Separate log for detailed debugging
        $debug_file_name = TP_DEBUG_DIR.'/'.TP_DEBUG_LOGFILE;

        $debug_logtype = 'null';

        if ( TP_LOG_DEBUG )
        {
            if ( touch( $debug_file_name ) )
            {
                $debug_logtype = 'file';
            }
        }

        $GLOBALS['g_dlog'] = &Log::singleton( $debug_logtype, $debug_file_name, 'debug', 
                                              array( 'append'     => false,
                                                     'timeFormat' => '', 
                                                     'lineFormat' => '%4$s' ), 
                                              PEAR_LOG_DEBUG );
        global $g_dlog;

        if ( PEAR::isError( $g_dlog ) ) 
        {
            $msg = 'The debug log file could not be opened';
            TpDiagnostics::Append( DC_LOG_ERROR, $msg, DC_WARN );
        }

    } // end of InitializeLogs

    /** Get value from post/get environment variables or return a default 
     *
     * @param string name Parameter name
     * @param mixed defaultVal Default value to be used if parameter was not passed
     *
     * @return mixed Parameter value
     */
    function GetVar( $name, $defaultVal=false ) 
    {
        // Note: If tapir_globals.php is included, then all $_REQUEST keys
        //       are changed to lower case!

        return ( isset( $_REQUEST[$name] ) ? $_REQUEST[$name] : $defaultVal );

    } // end of GetVar

    /** Get value from array using case insensitive comparison with a given key
     *  
     * @param array targetArray Array to search for key
     * @param string searchKey Key to be searched
     * @param mixed defaultVal Default value to be used if key is not present
     *
     * @return mixed Key value (if found) or default value
     */
    function GetInArray( $targetArray, $searchKey, $defaultVal=false )
    {
        foreach ( $targetArray as $key => $value )
        {
            if ( strcasecmp( $searchKey, $key ) == 0 )
            {
                return $value;
            }
        }

        return $defaultVal;
    }

    /**
     * Returns the unqualified name from a 'namespace:name' string.
     */
    function GetUnqualifiedName( $fullName ) {

        $last_colon = strrpos( $fullName, ':' );

        if ( $last_colon === false ) {

            return $fullName;
        }

        return substr( $fullName, $last_colon + 1 );

    } // end of GetUnqualifiedName

    /**
     * Simple check to determine if the supplied string is a URL
     *
     * @param string tst String to test
     *
     * returns TRUE or FALSE
     */
    function IsUrl( $tst ) {

        //simple check to see if a string is a URL.
        //just looks at the first few characters to see if the scheme is http or ftp
        //add a single char so we don't get confused with a false answer
        //being the same as a zero
        $tst = ' ' . substr( $tst, 0, 8 );

        $tst = strtolower( $tst );

        if ( strpos( $tst, 'http://' ) == 1 )
        {
            return true;
        }
	else if ( strpos( $tst, 'ftp://' ) == 1 )
        {
            return true;
        }
	else if ( strpos( $tst, 'php://' ) == 1 )
        {
            return true;
        }

        return false;

    } // end of IsUrl

    /** Strip slashes from strings and array elements
     *  
     * @param string or array reference
     */
    function StripMagicSlashes( &$rVar ) 
    {
        if ( is_string( $rVar ) )
        {
            $rVar = stripslashes( $rVar );
        }
        elseif ( is_array( $rVar ) )
        {
            foreach( $rVar as $key => $value ) 
            {
                TpUtils::StripMagicSlashes( $rVar[$key] );
            }
        }

    } // end of StripMagicSlashes

    /**
     * Escapes XML special chars in a string
     *
     * @param $s string String to be escaped (assumed to be in utf-8)
     * @return string Escaped string
     */
    function EscapeXmlSpecialChars( $s )
    {
        // Since "htmlspecialchars" does not work with utf-8 in versions
        // prior than 4.3.0 (stable!), we need to use mb_ereg_replace as an alternative
        if ( version_compare( phpversion(), "4.3.0", ">=" ) > 0 )
        {
            $s = htmlspecialchars( $s, ENT_COMPAT, 'UTF-8' );
        }
        else
        {
            if ( function_exists( 'mb_regex_encoding' ) )
            {
                mb_regex_encoding('UTF-8');
                $s = mb_ereg_replace('&', '&amp;' , $s);
                $s = mb_ereg_replace('>', '&gt;'  , $s);
                $s = mb_ereg_replace('<', '&lt;'  , $s);
                $s = mb_ereg_replace('"', '&quot;', $s);
            }
            else
            { 
                // TODO: If $s contains any xml special char, we should raise an error here!
                $s = htmlspecialchars( $s );
            }
        }

        return $s;

    } // end of EscapeXmlSpecialChars

    /**
     * Returns current time measured in the number of seconds since the Unix 
     * Epoch (0:00:00 January 1, 1970 GMT) including microseconds.
     *
     * @return float number of seconds with microseconds since the Unix Epoch
     */
    function MicrotimeFloat()
    {
        list( $usec, $sec ) = explode( ' ', microtime() );

        return ( (float)$usec + (float)$sec );

    } // end of MicrotimeFloat

    /**
     * Returns an xsd:dateTime value from a timestamp.
     * xsd:dateTime values follow this rule: [-]CCYY-MM-DDThh:mm:ss[Z|(+|-)hh:mm]
     *
     * @return string xsd:dateTime
     */
    function TimestampToXsdDateTime( $timestamp )
    {
        $date = strftime( '%Y-%m-%d', $timestamp );

        if ( strtoupper( substr( PHP_OS, 0, 3) ) == 'WIN' )
        {
            $time = strftime( '%X', $timestamp );
        }
        else
        {
            $time = strftime( '%T', $timestamp );
        }

        $time_zone = strftime( '%z', $timestamp );

        if ( preg_match("/^[+-]{1}\d{4}$/", $time_zone ) )
        {
            $time_zone = substr( $time_zone, 0, 3 ) . ':'. substr( $time_zone, 3 );
        }
        else
        {
            $time_zone = '';
        }

        return $date.'T'.$time.$time_zone;

    } // end of TimestampToXsdDateTime

    /**
     * Returns this script URL
     */
    function GetServiceId()
    {
        $sn = isset($_SERVER['SERVER_NAME'])?$_SERVER['SERVER_NAME']:'localhost';
        $sp = isset($_SERVER['SERVER_PORT'])?$_SERVER['SERVER_PORT']:'80';
        $ss = isset($_SERVER['SCRIPT_NAME'])?$_SERVER['SCRIPT_NAME']:'/DiGIR.php';

        $s = 'http://'.$sn.':'.$sp.$ss;

        return $s;
    }

    /** Simple function that returns a hash out of a simple array 
     *  using each array value as both the key and the value of the hash.
     *  
     * @param array elements
     *
     * @return hash
     */
    function GetHash( $elements ) 
    {
        $ret_array = array();
  
        foreach ( $elements as $value )
        {
            $ret_array[$value] = $value;
        }

        return $ret_array;

    } // end of GetHash

    /**
     * Returns an opening XML tag for the specified element.
     *
     * @param string $nsPrefix Namespace prefix.
     * @param string $elementName Element name.
     * @param string $indent Optional indentation characters.
     * @param array $attrs Optional array with key value pairs to be added as attributes.
     * @return string An opening tag for the specified element
     */
    function OpenTag( $nsPrefix, $elementName, $indent='', $attrs=array() )
    {
        $ns_sep = ( $nsPrefix ) ? ':' : '';

        $xml_attrs = '';

        if ( count( $attrs ) )
        {
            foreach ( $attrs as $attr_key => $attr_value )
            {
                $xml_attrs .= ' ' . $attr_key .'="'. $attr_value .'"';
            }
        }

        return sprintf( "%s<%s%s%s%s>\n", $indent, $nsPrefix, $ns_sep, 
                                          $elementName, $xml_attrs );

    } // end of OpenTag

    /**
     * Returns a closing XML tag for the specified element.
     *
     * @param string $nsPrefix Namespace prefix.
     * @param string $elementName Element name.
     * @param string $indent Optional indentation characters.
     * @return string A closing tag for the specified element
     */
    function CloseTag( $nsPrefix, $elementName, $indent='' )
    {
        $ns_sep = ( $nsPrefix ) ? ':' : '';

        return sprintf( "%s</%s%s%s>\n", $indent, $nsPrefix, $ns_sep, $elementName );

    } // end of CloseTag

    /**
     * Returns an XML tag enclosing the specified value.
     *
     * @param string $nsPrefix Namespace prefix.
     * @param string $elementName Element name.
     * @param string $value Element value.
     * @param string $indent Optional indentation characters.
     * @param array $attrs Optional array with key value pairs to be added as attributes.
     * @return string Value (with XML characters escaped) enclosed by the element
     */
    function MakeTag( $nsPrefix, $elementName, $value, $indent='', $attrs=array() )
    {
        $ns_sep = ( $nsPrefix ) ? ':' : '';

        $xml_attrs = '';

        if ( count( $attrs ) )
        {
            foreach ( $attrs as $attr_key => $attr_value )
            {
                $xml_attrs = ' ' . $attr_key .'="'. $attr_value .'"';
            }
        }

        $s = sprintf( '%s<%s%s%s%s>', $indent, $nsPrefix, $ns_sep, 
                                      $elementName, $xml_attrs );
        $s .= TpUtils::EscapeXmlSpecialChars( $value );
        $s .= sprintf( "</%s%s%s>\n", $nsPrefix, $ns_sep, $elementName );

        return $s;

    } // end of MakeTag

    /**
     * Returns an XML tag enclosing the specified value and having an xml:lang attribute.
     *
     * @param string $nsPrefix Namespace prefix.
     * @param string $elementName Element name.
     * @param string $value Element value.
     * @param string $lang Language code.
     * @param string $indent Optional indentation characters.
     * @return string Value (with XML characters escaped) enclosed by the specified 
     *                element with a lang attribute
     */
    function MakeLangTag( $nsPrefix, $elementName, $value, $lang, $indent='' )
    {
        $ns_sep = ( $nsPrefix ) ? ':' : '';

        if ( $lang )
        {
            $s = sprintf( '%s<%s%s%s %s:lang="%s">', $indent, $nsPrefix, $ns_sep, 
                          $elementName, TP_XML_PREFIX, $lang );
        }
        else 
        {
            $s = sprintf( '%s<%s%s%s>', $indent, $nsPrefix, $ns_sep, $elementName );
        }
        
        $s .= TpUtils::EscapeXmlSpecialChars( $value );
        $s .= sprintf( "</%s%s%s>\n", $nsPrefix, $ns_sep, $elementName );

        return $s;

    } // end of MakeLangTag

    /** Returns a default XML header 
     *
     * @return Default XML header
     */
    function GetXmlHeader( ) 
    {
        return '<?xml version="1.0" encoding="utf-8" ?>';

    } // end of GetXmlHeader

    /**
     * Loads the specified library if not already, trying to be platform independent.
     * Note that dl() is only available when the PHP interpreter is running
     * in CGI mode.  
     *
     * @param $libName string Name of the library to load
     */
    function LoadLibrary( $libName )
    {
        $res = true;

        if ( ! extension_loaded( $libName ) )
        {
    	    if ( strtoupper( substr( PHP_OS, 0, 3 ) ) == 'WIN' )
            {
    	        $res = @dl( 'php_'.$libName.'.dll' );
            }
    	    elseif ( PHP_OS == 'HP-UX' )
            {
    	        $res = @dl( $libName.'.sl' );
            }
    	    elseif ( PHP_OS == 'AIX' )
            {
    	        $res = @dl( $libName.'.a' );
            }
    	    else
            {
    	        $res = @dl( $libName.'.so' );
            }
        }

        return $res;

    } // end of LoadLibrary

    /**
     * Simple array dumper. For debugging stuff.
     */
    function DumpArray( $a )
    {
        if ( ! is_array( $a ) ) 
        {
            return;
        }

        $s = '';

        foreach ( $a as $key => $val ) 
        {
            $s .= "\n(".$key.')='.$val;
        }

        return $s;

    } // end of DumpArray

    function GetFileHandle( $location ) // Remember to close the file handle!!
    {
        if ( ini_get('allow_url_fopen') || ! TpUtils::IsUrl( $location ) )
        {
            if ( !( $fp = fopen( $location, 'r' ) ) )
            {
                $error = "Could not open file: $location";
                TpDiagnostics::Append( DC_IO_ERROR, $error, DIAG_ERROR );

                return null;
            }
        }
        else
        {
            // This is a URL and we are not permitted to fopen urls, so use cURL.
            // Open a temporary file to write curl session results to.
            $fp = tmpfile();
            $ch = curl_init( $location );
            curl_setopt( $ch, CURLOPT_FILE, $fp );
            curl_exec( $ch );
            curl_close( $ch );
            rewind( $fp );
        }

        return $fp;

    } // end of member function GetFileHandle

} // end of TpUtils
?>