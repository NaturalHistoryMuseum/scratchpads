<?php 
/**
 * $Id: tapir_client.php 170 2007-01-24 22:26:25Z rdg $
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

require_once('tapir_globals.php');
require_once('TpUtils.php');
require_once('TpHtmlUtils.php');
require_once('TpResources.php');
require_once('TpResource.php');
require_once('HTTP/Request.php'); // pear package

// Show form when user didn't click on submit button
if ( ! TpUtils::GetVar( 'send' ) ) 
{
    // Accesspoint
    $local_accesspoints = array();

    $rResourcesManager =& TpResources::GetInstance();

    $resources = $rResourcesManager->GetActiveResources();

    foreach ( $resources as $resource ) 
    {
        $accesspoint = $resource->GetAccessPoint();
        $local_accesspoints[$accesspoint] = $accesspoint;
    }

    // Operation
    $operations = array( 'ping'         => 'Ping',
                         'capabilities' => 'Capabilities',
                         'metadata'     => 'Metadata',
                         'inventory'    => 'Inventory',
                         'search'       => 'Search' );

    // Encodings
    $encodings = array( 'rawpost' => 'RAW POST',
                        'get'     => 'GET w/ request parameter',
                        'post'    => 'POST w/ request parameter' );

    // Include HTML template
    include_once('tapir_client.tmpl.php');

}
// Process request if user clicked on submit
else
{
    $url = $_REQUEST['local_accesspoint'];

    $body = str_replace( '\"', '"', $_REQUEST['request'] );

    $http_request = new HTTP_Request();

    if ( $_REQUEST['encoding'] == 'get' )
    {
        $http_request->setMethod( 'GET' );

        $body = urlencode( $body );

        $url .= "?request=$body";
    }
    else
    {
        $http_request->setMethod( 'POST' );

        if ( $_REQUEST['encoding'] == 'rawpost' )
        {
            $http_request->addHeader('Content-Type', 'text/xml');

            $http_request->addRawPostData( $body );
        }
        else
        {
            $http_request->addHeader('Content-Type', 'application/x-www-form-urlencoded');
            $http_request->addPostData( 'request', $body );
        }
    }

    $http_request->setURL( $url );

    $res = $http_request->sendRequest();

    $response = $http_request->getResponseBody();

    // This can be used to see the entire request
    //$raw_request = $http_request->_buildRequest();

    if ( ! headers_sent() ) 
    {
        header ( 'Content-type: text/xml' );
        echo $response;
    }
}
?>