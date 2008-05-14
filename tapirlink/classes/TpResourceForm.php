<?php
/**
 * $Id: TpResourceForm.php 6 2007-01-06 01:38:13Z rdg $
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

require_once('TpPage.php');
require_once('TpResources.php');
require_once('TpUtils.php');
require_once('TpDiagnostics.php');

class TpResourceForm extends TpPage
{
    var $mrResource;
    var $mRemoved = false;

    function TpResourceForm( $rResource )
    {
        $this->mrResource =& $rResource;

    } // end of member function TpResourceForm

    function HandleEvents( ) 
    {
        $r_resources =& TpResources::GetInstance();

        if ( isset( $_REQUEST['remove'] ) )
        {
            if ( $r_resources->RemoveResource( $this->mrResource->GetCode() ) )
            {
                $this->mRemoved = true;
            }
        }

    } // end of member function HandleEvents

    function RemovedResource( ) 
    {
        return $this->mRemoved;

    } // end of member function RemovedResource

    function DisplayHtml( ) 
    {
        $errors = TpDiagnostics::GetMessages();

        // Expose $resource variable to template
        $resource = $this->mrResource;

        include('TpResourceForm.tmpl.php');

    } // end of member function DisplayHtml

} // end of TpResourceForm
?>
