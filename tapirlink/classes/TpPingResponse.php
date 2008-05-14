<?php
/**
 * $Id: TpPingResponse.php 6 2007-01-06 01:38:13Z rdg $
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

require_once('TpResponse.php');

class TpPingResponse extends TpResponse
{
    function TpPingResponse( $request )
    {
        $this->TpResponse( $request );

        $this->mCacheable = false;

    } // end of member function TpPingResponse

    function Body()
    {
        echo "\n<pong/>";

    } // end of member function Body

} // end of TpPingResponse
?>