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
 */

require_once('TpConceptualSchemaHandler.php');
require_once('TpDiagnostics.php');

class DarwinSchemaHandler_v1 extends TpConceptualSchemaHandler
{
    var $mConceptualSchema;

    function DarwinSchemaHandler_v1( ) 
    {

    } // end of member function DarwinSchemaHandler_v1

    function Load( &$conceptualSchema ) 
    {
        $this->mConceptualSchema =& $conceptualSchema;

        // Just a stub. Old Darwin not supported yet.

    } // end of member function Load

} // end of DarwinSchemaHandler_v1
?>