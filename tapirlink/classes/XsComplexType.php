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
 *
 * ACKNOWLEDGEMENTS
 * 
 * This class has been largely based on the API documentation of 
 * xsom (https://xsom.dev.java.net/) written by Kohsuke Kawaguchi.
 */

require_once('XsType.php');

class XsComplexType extends XsType
{
    var $mDeclaredAttributeUses = array(); // XsAttributeUse objects
    var $mrContentType;
    var $mSimpleTypeDerivation;

    function XsComplexType( $name, $targetNamespace, $isGlobal ) 
    {
        parent::XsType( $name, $targetNamespace, $isGlobal );

        $this->mIsSimple = false;

    } // end of member function XsComplexType

    function AddDeclaredAttributeUse( $xsAttributeUse )
    {
        array_push( $this->mDeclaredAttributeUses, $xsAttributeUse );
        
    } // end of member function AddDeclaredAttributeUse

    function AddContentType( &$rContentType )
    {
        $this->mrContentType =& $rContentType;
        
    } // end of member function AddContentType

    function &GetDeclaredAttributeUses( )
    {
        return $this->mDeclaredAttributeUses;
        
    } // end of member function GetDeclaredAttributeUses

    function &GetContentType( )
    {
        return $this->mrContentType;
        
    } // end of member function GetContentType

    function SetSimpleTypeDerivation( $derivedFromSimpleType ) 
    {
        $this->mSimpleTypeDerivation = $derivedFromSimpleType;
        
    } // end of member function SetSimpleTypeDerivation

    function HasSimpleContent( ) 
    {
        return $this->mSimpleTypeDerivation;
        
    } // end of member function HasSimpleContent

    /**
     * Internal method called before serialization
     *
     * @return array Properties that should be considered during serialization
     */
    function __sleep()
    {
	return array_merge( parent::__sleep(), array( 'mDeclaredAttributeUses',
                                                      'mrContentType',
                                                      'mSimpleTypeDerivation' ) );

    } // end of member function __sleep

} // end of XsComplexType
?>