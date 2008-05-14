<?php
/**
 * $Id: TpServiceUtils.php 6 2007-01-06 01:38:13Z rdg $
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

require_once('TpUtils.php');

class TpServiceUtils 
{
    /**
     * Converts an associative array into a string to be used in the log.
     *
     * @param $data array Data to be logged.
     * @return string Log formatted string.
     */
    function GetLogString( $data )
    {
        $spacer = "\t";

        $log_str = '';

        foreach ( $data as $key => $value )
        {
            if ( is_numeric( $value ) )
            {
                $log_value = "$value";
            }
            elseif ( is_bool( $value ) )
            {
                $log_value = ( $value == false ) ? 'false' : 'true';
            }
            elseif ( $value == null )
            {
                $log_value = 'NULL';
            }
            else
            {
                $log_value = str_replace( "\n", '', str_replace( "\t", '', $value ) );
            }

            $log_str .= "$spacer$key=".$log_value;
        }

        return $log_str;

    } // end of GetLogString

    /**
     * Converts a SQL statement to the current database charset encoding 
     * (if different from UTF-8).
     *
     * @param $sql string SQL statement.
     * @param $encoding string Database charset encoding.
     * @return string SQL statement to be sent to database
     */
    function EncodeSql( $sql, $encoding )
    {
        if ( strcasecmp( $encoding, 'UTF-8' ) )
        {
            if ( function_exists( 'mb_convert_encoding' ) )
            {
                $sql = mb_convert_encoding( $sql, $encoding, 'UTF-8' );
            }
        }

        return $sql;

    } // end of EncodeSql

    function EncodeData( $data, $encoding )
    {
        if ( $data == null )
        {
            return null;
        }

        // If data encoding is different from UTF-8 and conversion function exists,
        // convert values to UTF-8
        if ( strcasecmp( $encoding, 'UTF-8' ) and 
             function_exists( 'mb_convert_encoding' ) )
        {
            $data = mb_convert_encoding( $data, 'UTF-8', $encoding );
        }

        return TpUtils::EscapeXmlSpecialChars( $data );

    } // end of member function EncodeData

    /**
     * Indicates is $path1 contains $path2 (offset = 0) 
     * (note: not using substr_count because offset was only added in PHP5).
     *
     * @param $path1 string haystack.
     * @param $path2 string needle.
     * @return boolean True if haystack contains needle
     */
    function Contains( &$rPath1, &$rPath2 )
    {
        $size = strlen( $rPath2 );

        if ( strlen( $rPath1 ) >= $size )
        {
            if ( substr( $rPath1, 0, $size ) == $rPath2 )
            {
                return true;
            }
        }

        return false;

    } // end of member function Contains

} // end of TpServiceUtils
?>