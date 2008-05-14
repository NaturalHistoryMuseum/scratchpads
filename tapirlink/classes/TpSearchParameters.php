<?php
/**
 * $Id: TpSearchParameters.php 430 2007-08-15 12:27:28Z rdg $
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

require_once('TpOperationParameters.php');
require_once('TpOutputModel.php');
require_once('TpFilter.php');

class TpSearchParameters extends TpOperationParameters
{
    var $mOutputModel;
    var $mPartial = array(); // node ids
    var $mOrderBy = array(); // concept id => descend (boolean)

    function TpSearchParameters( )
    {
        $this->TpOperationParameters();

    } // end of member function TpSearchParameters

    function LoadKvpParameters()
    {
        // Output model
        if ( isset( $_REQUEST['model'] ) )
        {
            $output_model = $_REQUEST['model'];
        }
        else if ( isset( $_REQUEST['m'] ) )
        {
            $output_model = $_REQUEST['m'];
        }

        if ( isset( $output_model ) )
        {
            $this->LoadOutputModel( $output_model );
        }

        // Partial
        if ( isset( $_REQUEST['partial'] ) )
        {
            $partial = $_REQUEST['partial'];
        }
        else if ( isset( $_REQUEST['p'] ) )
        {
            $partial = $_REQUEST['p'];
        }

        if ( isset( $partial ) )
        {
            if ( is_array( $partial ) )
            {
                $this->mPartial = $partial;
            }
            else if ( is_string( $partial ) )
            {
                array_push( $this->mPartial, $partial );
            }
        }

        // Order by
        if ( isset( $_REQUEST['orderby'] ) )
        {
            $orderby = $_REQUEST['orderby'];
        }
        else if ( isset( $_REQUEST['o'] ) )
        {
            $orderby = $_REQUEST['o'];
        }

        // Ascending or descending
        if ( isset( $_REQUEST['descend'] ) )
        {
            $descend = $_REQUEST['descend'];
        }
        else if ( isset( $_REQUEST['d'] ) )
        {
            $descend = $_REQUEST['d'];
        }

        if ( isset( $orderby ) )
        {
            if ( is_array( $orderby ) )
            {
                $i = 0;

                foreach ( $orderby as $concept_id )
                {
                    $desc = false;

                    if ( is_array( $descend ) and isset( $descend[$i] ) )
                    {
                        $desc = $descend[$i];

                        if ( $desc == 'true' or (int)$desc == 1 )
                        {
                            $desc = true;
                        }
                    }

                    $this->mOrderBy[$concept_id] = $desc;

                    ++$i;
                }
            }
            else if ( is_string( $orderby ) )
            {
                $desc = false;

                if ( isset( $descend ) and ! is_array( $descend ) )
                {
                    if ( $descend == 'true' or (int)$descend == 1 )
                    {
                        $desc = true;
                    }
                }

                $this->mOrderBy[$orderby] = $desc;
            }
        }

        return parent::LoadKvpParameters();

    } // end of member function LoadKvpParameters

    function StartElement( $parser, $qualified_name, $attrs ) 
    {
        $name = TpUtils::GetUnqualifiedName( $qualified_name );

        parent::StartElement( $parser, $qualified_name, $attrs );

        $depth = count( $this->mInTags );

        if ( strcasecmp( $name, 'externalOutputModel' ) == 0 )
        {
            if ( isset( $attrs['location'] ) )
            {
                $this->LoadOutputModel( $attrs['location'] );
            }
            else
            {
                $error = 'Missing attribute "location" in <externalOutputModel> element';
                TpDiagnostics::Append( DC_INVALID_REQUEST, $msg, DIAG_ERROR );
            }
        }
        else if ( in_array( 'outputmodel', $this->mInTags ) )
        {
            // Delegate to output model parser
            if ( ! is_object( $this->mOutputModel ) )
            {
                $this->mOutputModel = new TpOutputModel();
            }

            $this->mOutputModel->StartElement( $parser, $qualified_name, $attrs );
        }
        // <node> element whose parent is <partial>
        else if ( $depth > 1 and $this->mInTags[$depth-2] == 'partial' and
                  strcasecmp( $name, 'node' ) == 0 and 
                  isset( $attrs['path'] ) )
        {
            array_push( $this->mPartial, $attrs['path'] );
        }
        // <concept> element whose parent is <orderBy>
        else if ( $depth > 1 and $this->mInTags[$depth-2] == 'orderby' and
                  strcasecmp( $name, 'concept' ) == 0 and 
                  isset( $attrs['id'] ) )
        {
            $descend = TpUtils::GetInArray( $attrs, 'descend', false );

            if ( $descend == 'true' or (int)$descend == 1 )
            {
                $descend = true;
            }
            else
            {
                $descend = false;
            }

            $this->mOrderBy[$attrs['id']] = $descend;
        }

    } // end of member function StartElement

    function EndElement( $parser, $qualified_name ) 
    {
        if ( in_array( 'outputmodel', $this->mInTags ) )
        {
            // Delegate to output model parser
            $this->mOutputModel->EndElement( $parser, $qualified_name );
        }

        parent::EndElement( $parser, $qualified_name );

    } // end of member function EndElement

    function CharacterData( $parser, $data ) 
    {
        if ( in_array( 'outputModel', $this->mInTags ) )
        {
            // Delegate to output model parser
            $this->mOutputModel->CharacterData( $parser, $data );
        }

        parent::CharacterData( $parser, $data );

    } // end of member function CharacterData

    function LoadOutputModel( $location )
    {
        global $g_dlog;

        $g_dlog->debug( '[Output model]' );

        // Here output models must be specified as URLs
        // (it's important to check this also for security reasons since
        // "fopen" is used to read templates!)
        if ( ! TpUtils::IsUrl( $location ) )
        {
            $error = 'Output model is not a URL.';
            TpDiagnostics::Append( DC_INVALID_REQUEST, $error, DIAG_FATAL );

            return false;
        }

        $loaded_from_cache = false;

        // If cache is enabled
        if ( TP_USE_CACHE and TP_OUTPUT_MODEL_CACHE_LIFE_SECS )
        {
            $cache_options = array( 'cache_dir' => TP_CACHE_DIR );

            $cache = new Cache( 'file', $cache_options );
            $cache_id = $cache->generateID( $location );
            $cached_data = $cache->get( $cache_id, 'models' );

            if ( $cached_data and ! $cache->isExpired( $cache_id, 'models' ) )
            {
                 $g_dlog->debug( 'Unserializing output model from cache' );

                 $this->mOutputModel = unserialize( $cached_data );

                 if ( ! $this->mOutputModel )
                 {
                     $error = 'Could not unserialize output model from cache';
                     TpDiagnostics::Append( DC_GENERAL_ERROR, $error, DIAG_FATAL );
                     return false;
                 }

                 $loaded_from_cache = true;
            }
        }

        if ( ! $loaded_from_cache )
        {
            $g_dlog->debug( 'Location '.$location );

            $g_dlog->debug( 'Retrieving and parsing output model' );

            $this->mOutputModel = new TpOutputModel();

            if ( ! $this->mOutputModel->Parse( $location ) )
            {
                return false;
            }
        }
 
        return true;

    } // end of member function LoadOutputModel

    function GetOrderBy( )
    {
        return $this->mOrderBy;

    } // end of member function GetOrderBy

    function GetPartial( )
    {
        return $this->mPartial;

    } // end of member function GetPartial

    function &GetOutputModel( )
    {
        return $this->mOutputModel;

    } // end of member function GetOutputModel

    /**
     * Internal method called before serialization
     *
     * @return array Properties that should be considered during serialization
     */
    function __sleep()
    {
	return array_merge( parent::__sleep(), 
                            array( 'mOutputModel', 'mPartial', 'mOrderBy' ) );

    } // end of member function __sleep

} // end of TpSearchParameters
?>