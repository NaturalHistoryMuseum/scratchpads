<?php
/**
 * $Id: TpNamespaceManager.php 380 2007-06-05 01:58:52Z rdg $
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
 */

require_once('TpNamespace.php');
require_once('TpDiagnostics.php');

class TpNamespaceManager
{
    var $mData = array();  // parser obj => array( sequence => TpNamespace decl obj )

    // No constructor - this class uses the singleton pattern
    // Use GetInstance instead

    function &GetInstance( )
    {
        static $instance;

        if ( ! isset( $instance ) ) 
        {
            $instance = new TpNamespaceManager();
        }

        return $instance;

    } // end of member function GetInstance

    function AddNamespace( $parser, $prefix, $uri, $flag=null )
    {
        global $g_dlog;

        $g_dlog->debug( 'Adding namespace declaration '.$uri.' for prefix '.$prefix.' ('.$parser.') with flag: '.$flag );

        if ( empty( $prefix ) )
        {
            $prefix = 'default';
        }

        $namespace = new TpNamespace( $prefix, $uri, $flag );

        // Add namespace declaration
        // Note: namespaces that are redeclared will be appended on the list

        $this->mData[$parser][] = $namespace;

    } // end of member function AddNamespace

    function GetNamespace( $parser, $prefix )
    {
        // Note: this function will always retrieve the last declaration for the prefix

        $parser_namespaces = $this->mData[$parser];

        // Search backwards
        for ( $i = count( $parser_namespaces ) - 1; $i >= 0; --$i )
        {
            $ns = $parser_namespaces[$i];

            if ( $ns->GetPrefix() == $prefix )
            {
                return $ns->GetUri();
            }
        }

        $error = 'Could not find namespace declaration for prefix "'.$prefix.'"'.
                 ' ('.$parser.')';
        TpDiagnostics::Append( DC_INVALID_REQUEST, $error, DIAG_ERROR );

        return null;

    } // end of member function GetNamespace

    function GetPrefix( $parser, $namespace )
    {
        // Note: this function will always retrieve the last declaration for the namespace

        $parser_namespaces = $this->mData[$parser];

        // Search backwards
        for ( $i = count( $parser_namespaces ) - 1; $i >= 0; --$i )
        {
            $ns = $parser_namespaces[$i];

            if ( $ns->GetUri() == $namespace )
            {
                return $ns->GetPrefix();
            }
        }

        return null;

    } // end of member function GetPrefix

    function GetFlaggedNamespaces( $parser, $flag )
    {
        global $g_dlog;

        $g_dlog->debug( 'Getting flagged namespaces ('.$parser.')' );

        if ( ! isset( $this->mData[$parser] ) )
        {
            return array();
        }

        $parser_namespaces = $this->mData[$parser];

        $namespaces_to_return = array();

        for ( $i = 0; $i < count( $parser_namespaces ); ++$i )
        {
            $ns = $parser_namespaces[$i];

            if ( $ns->HasFlag( $flag ) )
            {
                array_push( $namespaces_to_return, $ns );
            }
        }

        return $namespaces_to_return;

    } // end of member function GetFlaggedNamespaces

    function RemoveFlag( $parser, $flag )
    {
        $r_parser_namespaces =& $this->mData[$parser];

        for ( $i = 0; $i < count( $r_parser_namespaces ); ++$i )
        {
            $r_parser_namespaces[$i]->RemoveFlag( 'm' );
        }

    } // end of member function RemoveFlag

} // end of TpNamespaceManager
?>
