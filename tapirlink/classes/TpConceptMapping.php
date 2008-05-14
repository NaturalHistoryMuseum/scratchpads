<?php
/**
 * $Id: TpConceptMapping.php 186 2007-01-29 12:08:36Z rdg $
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

require_once('TpDiagnostics.php');
require_once('TpUtils.php');

define( 'TYPE_TEXT'    , 'text' );
define( 'TYPE_NUMERIC' , 'numeric' );
define( 'TYPE_DATE'    , 'date' );
define( 'TYPE_DATETIME', 'datetime' );

class TpConceptMapping
{
    var $mMappingType = 'AbstractMapping';
    var $mrConcept;
    var $mLocalType;

    function TpConceptMapping( ) 
    {

    } // end of member function TpConceptMapping

    function GetMappingType( ) 
    {
        return $this->mMappingType;

    } // end of member function GetMappingType

    function SetConcept( &$rConcept )
    {
        $this->mrConcept =& $rConcept;

    } // end of member function SetConcept

    function SetLocalType( $localType ) 
    {
        $this->mLocalType = $localType;

    } // end of member function SetLocalType

    function IsMapped( )
    {
        // Must be called by subclasses

        return ! empty( $this->mLocalType );

    } // end of member function IsMapped

    function Refresh( &$rForm )
    {
        // Must be called by subclasses

        $input_name = $this->GetLocalTypeInputName();

        if ( isset( $_REQUEST[$input_name] ) )
        {
            $this->mLocalType = $_REQUEST[$input_name];
        }

    } // end of member function Refresh

    function GetLocalTypeInputName( )
    {
        if ( $this->mrConcept == null ) 
        {
            $error = 'Cannot produce an input name for local type without having '.
                     'an associated concept!';
            TpDiagnostics::Append( CFG_INTERNAL_ERROR, $error, DIAG_ERROR );
            return 'undefined_concept_value';
        }

        return strtr( $this->mrConcept->GetId() . '_type', '.', '_' );

    } // end of member function GetLocalTypeInputName

    function GetHtml( ) 
    {
        // Must be called by subclasses after subclass implementaiton
        include('TpConceptMapping.tmpl.php');

    } // end of member function GetHtml

    function GetXml( ) 
    {
        // Must be overwritten by subclasses
        return '<abstractMapping/>';

    } // end of member function GetXml

    function GetSqlTarget( ) 
    {
        // Must be overwritten by subclasses
        return '';

    } // end of member function GetSqlTarget

    function GetSqlFrom( ) 
    {
        // Usually overwritten by subclasses
        return array();

    } // end of member function GetSqlFrom

    function GetLocalType( ) 
    {
        return $this->mLocalType;

    } // end of member function GetLocalType

    function GetLocalTypes( ) 
    {
        $types = array( ''         => '-- type --',
                        'text'     => TYPE_TEXT,
                        'numeric'  => TYPE_NUMERIC,
                        'date'     => TYPE_DATE,
                        'datetime' => TYPE_DATETIME );

        return $types;

    } // end of member function GetLocalTypes

} // end of TpConceptMapping
?>
