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

require_once('TpServiceUtils.php');
require_once('TpDiagnostics.php');
require_once('TpResponse.php');
require_once('TpSqlBuilder.php');

class TpInventoryResponse extends TpResponse
{
    var $mTotalReturned = 0;
    var $mMainSql = '';

    function TpInventoryResponse( $request )
    {
        $this->TpResponse( $request );

        $this->mCacheLife = TP_INVENTORY_CACHE_LIFE_SECS;

    } // end of member function TpInventoryResponse

    function Body()
    {
        $inventory_parameters = $this->mRequest->GetOperationParameters();

        if ( ! is_object( $inventory_parameters ) )
        {
            $msg = 'No parameters specified';

            echo $this->Error( $msg );
            return;
        }

        $concepts = $inventory_parameters->GetConcepts();

        if ( count( $concepts ) == 0 )
        {
            $msg = 'No concepts specified';

            echo $this->Error( $msg );
            return;
        }

        // Load resource config

        $r_resource =& $this->mRequest->GetResource();

        $r_resource->LoadConfig();

        $r_data_source =& $r_resource->GetDatasource();

        $r_local_mapping =& $r_resource->GetLocalMapping();

        // Prepare SQL builder

        $sql_builder = new TpSqlBuilder();

        $concepts_xml = '';

        foreach ( $concepts as $concept_id => $tag_name )
        {
            $concept = $r_local_mapping->GetConcept( $concept_id );

            if ( $concept == null or ! $concept->IsMapped() )
            {
                $msg = 'Concept "'.$concept_id.'" is not mapped';

                echo $this->Error( $msg );
                return;
            }

            $sql_builder->AddTargetConcept( $concept );

            $concepts_xml .= "\n".'<concept id="'.$concept_id.'" />';
        }

        $r_tables =& $r_resource->GetTables();

        $sql_builder->AddRecordSource( $r_tables->GetStructure() );

        $r_settings =& $r_resource->GetSettings();

        // DB connection
        if ( ! $r_data_source->Validate() )
        {
            $this->Error( 'Failed to connect to database' );
            return;
        }

        $cn = $r_data_source->GetConnection();

        $db_encoding = $r_data_source->GetEncoding();

        // Filter

        $filter = $inventory_parameters->GetFilter();

        if ( ! $filter->IsEmpty() )
        {
            // This verifies only the syntax
            if ( ! $filter->IsValid() )
            {
                $this->Error( 'Invalid filter' );
                return;
            }

            $filter_sql = $filter->GetSql( $r_resource );

            // Need to check if other run time errors were found
            if ( TpDiagnostics::Count( array( DIAG_ERROR, DIAG_FATAL ) ) )
            {
                $this->Error( 'Runtime error' );
                return;
            }

            $sql_builder->AddCondition( $filter_sql );
        }

        // At this point we can save a query template in cache
        // since all possible errors were already checked

        $template = $this->mRequest->GetTemplate();

        if ( TP_USE_CACHE and TP_TEMPLATE_CACHE_LIFE_SECS and 
             ( ! empty( $template ) ) and 
             ( ! $this->mRequest->LoadedTemplateFromCache() ) )
        {
            $cache_options = array( 'cache_dir' => TP_CACHE_DIR );

            $cache = new Cache( 'file', $cache_options );
            $cache_id = $cache->generateID( $template );

            if ( ( ! $cache->isCached( $cache_id, 'templates' ) ) or 
                 (  $cache->isExpired( $cache_id, 'templates' ) ) )
            {
                $cache_expires = TP_TEMPLATE_CACHE_LIFE_SECS;
                $cached_data = serialize( $parameters );

                $cache->save( $cache_id, $cached_data, $cache_expires, 'templates' );

                $g_dlog->debug( 'Caching query template with id generated from "'.
                                $template.'"' );
            }
        }

        // Local filter

        $r_local_filter =& $r_resource->GetLocalFilter();

        if ( ! $r_local_filter->IsEmpty() )
        {
            $local_filter_sql = $r_local_filter->GetSql( $r_resource );

            $sql_builder->AddCondition( $local_filter_sql );
        }

        // Additional settings 

        $start = $this->mRequest->GetStart();

        $limit = $this->mRequest->GetLimit();

        $max_limit = $r_settings->GetMaxElementRepetitions();

        if ( is_null( $limit ) )
        {
            $limit = $max_limit;
        }
        else if ( $limit > $max_limit )
        {
            $msg = 'Parameter "limit" exceeded maximum element repetitions';
            TpDiagnostics::Append( DC_TRUNCATED_RESPONSE, $msg, DIAG_WARN );

            $limit = $max_limit;
        }

        $sql_builder->AddCountColumn( true );
        $sql_builder->GroupAll();

        // Count total matched records, if requested

        $matched = 0;

        if ( $this->mRequest->GetCount() )
        {
            $sql = $sql_builder->GetSql();

            TpDiagnostics::Append( DC_DEBUG_MSG, 'SQL to count: '.$sql, DIAG_DEBUG );

            $encoded_sql = TpServiceUtils::EncodeSql( $sql, $db_encoding );

            $result_set = &$cn->Execute( $encoded_sql );

            if ( ! is_object( $result_set ) )
            {
                $this->Error( 'Failed to count matched records' );

                $r_data_source->ResetConnection();

                return;
            }
            else
            {
                $matched = $result_set->RecordCount();

                $result_set->Close();
            }
        }

        // Retrieve records

        $sql_builder->OrderBy( array() ); // empty array means order by all

        $this->mMainSql = $sql_builder->GetSql();

        TpDiagnostics::Append( DC_DEBUG_MSG, 'SQL to get records: '.$this->mMainSql, DIAG_DEBUG );

        $encoded_sql = TpServiceUtils::EncodeSql( $this->mMainSql, $db_encoding );

        // note: Select one record more just to know if there are further records
        $result_set =& $cn->SelectLimit( $encoded_sql, $limit+1, $start );

        if ( ! is_object( $result_set ) )
        {
            $err = $cn->ErrorMsg();

            $this->Error( 'Failed to select records: '.$err );

            $r_data_source->ResetConnection();

            return;
        }

        // Inventory Header
        echo "\n<inventory>";
        echo "\n<concepts>";
        echo $concepts_xml;
        echo "\n</concepts>";

        // Inventory Records
        $num_recs = 0;
        $num_concepts = count( $concepts );

        $tag_names = array_values( $concepts );

        while ( ( ! $result_set->EOF ) and ( $num_recs < $limit ) )
        {
            $num_recs ++;

            echo "\n<record";

            if ( $this->mRequest->GetCount() )
            {
                echo ' count="'.$result_set->fields[$num_concepts].'"';
            }

            echo '>';

            for ( $i = 0; $i < $num_concepts; ++$i )
            { 
                echo "\n<".$tag_names[$i].'>';
                echo TpServiceUtils::EncodeData( $result_set->fields[$i], $db_encoding );
                echo '</'.$tag_names[$i].'>';
            }

            echo "\n</record>";

            $result_set->MoveNext();
        }

        // Inventory Summary

        echo "\n".'<summary start="'.$start.'"';

        if ( ! $result_set->EOF )
        {
            $next = $start + $limit;

            echo ' next="'.$next.'"';
        }

        $this->mTotalReturned = $num_recs;

        echo ' totalReturned="'.$num_recs.'"';

        if ( $this->mRequest->GetCount() )
        {
            echo ' totalMatched="'.$matched.'"';
        }

        echo ' />';

        echo "\n</inventory>";

        $result_set->Close();

        $r_data_source->ResetConnection();

    } // end of member function Body

    function _GetLogData( )
    {
        $data = array();

        $data['start'] = $this->mRequest->GetStart();
        $data['limit'] = $this->mRequest->GetLimit();

        $data['returned'] = $this->mTotalReturned;

        $parameters = $this->mRequest->GetOperationParameters();

        $data['template'] = $parameters->GetTemplate();

        $data['concepts'] = implode( ',', array_keys( $parameters->GetConcepts() ) );

        $data['sql'] = $this->mMainSql; // note: will be empty for log-only requests

        $filter = $parameters->GetFilter();

        $data['filter'] = $filter->GetLogRepresentation();

        return array_merge( parent::_GetLogData(), $data );

    } // end of member function _GetLogData

} // end of TpInventoryResponse
?>