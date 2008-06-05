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
require_once('TpSchemaInspector.php');
require_once('TpXmlGenerator.php');

class TpSearchResponse extends TpResponse
{
    var $mTotalReturned = 0;
    var $mMainSql = '';

    function TpSearchResponse( $request )
    {
        $this->TpResponse( $request );

        $this->mCacheLife = TP_SEARCH_CACHE_LIFE_SECS;

    } // end of member function TpSearchResponse

    function Header()
    {
        if ( $this->mRequest->GetEnvelope() )
        {
            parent::Header();
        }
        else
        {
            $this->XmlHeader();
        }

    } // end of member function Header

    function Footer()
    {
        if ( $this->mRequest->GetEnvelope() )
        {
            parent::Footer();
        }

    } // end of member function Footer

    function Body()
    {
        global $g_dlog;

        $g_dlog->debug( '[Search Body]' );

        $parameters = $this->mRequest->GetOperationParameters();

        if ( ! is_object( $parameters ) )
        {
            $msg = 'No parameters specified';

            echo $this->Error( $msg );
            return;
        }

        // Load resource config

        $r_resource =& $this->mRequest->GetResource();

        $r_resource->LoadConfig();

        $r_data_source =& $r_resource->GetDatasource();

        $r_tables = $r_resource->GetTables();

        $r_local_mapping =& $r_resource->GetLocalMapping();

        $r_local_filter =& $r_resource->GetLocalFilter();

        $r_settings =& $r_resource->GetSettings();

        $max_repetitions = $r_settings->GetMaxElementRepetitions();

        $max_levels = $r_settings->GetMaxElementLevels();

        // Output model

        $r_output_model =& $parameters->GetOutputModel();

        if ( ! is_object( $r_output_model ) )
        {
            $this->Error( 'Failed to load output model' );

            return;
        }

        $r_response_structure =& $r_output_model->GetResponseStructure();

        // Report unsupported schema constructs if not in debug mode
        if ( ! _DEBUG )
        {
            if ( is_object( $r_response_structure ) )
            {
                $unsupported_schema_constructs = $r_response_structure->GetUnsupportedConstructs();

                foreach ( $unsupported_schema_constructs as $construct => $schemas )
                {
                    $msg = 'Unsupported schema construct "'.$construct.'" found in '.
                           implode( ',', $schemas );
                    TpDiagnostics::Append( DC_TRUNCATED_RESPONSE, $msg, DIAG_WARN );
                }
            }
        }

        $rejected_paths = array();
        $accepted_paths = array();

        if ( ! $r_response_structure->LoadedFromCache() )
        {
            $partial = $parameters->GetPartial();

            // For memory management and processing reasons, it is better to perform
            // a previous inspection in the response schema. The reason is that certain 
            // checkings can be done per generic path instead of per node instance. 
            $schema_inspector = new TpSchemaInspector( $r_resource, $r_output_model, 
                                                       $r_local_mapping, $max_levels, 
                                                       $partial );

            $schema_inspector->Inspect();

            if ( $schema_inspector->MustAbort() )
            {
                $this->Error( 'Invalid or unsupported response structure' );

                return;
            }

            $rejected_paths = $schema_inspector->GetRejectedPaths();
            $accepted_paths = $schema_inspector->GetAcceptedPaths();

            if ( TP_LOG_DEBUG )
            {
                $g_dlog->debug( 'Paths that were understood from the structure:' );

                foreach ( $accepted_paths as $path => $min_occurs )
                {
                    $g_dlog->debug( 'Accepted path: '.$path );
                }
            }

            $r_response_structure->SetRejectedPaths( $rejected_paths );
            $r_response_structure->SetAcceptedPaths( $accepted_paths );

            $r_response_structure->Cache();
        }
        else 
        {
            $rejected_paths = $r_response_structure->GetRejectedPaths();
            $accepted_paths = $r_response_structure->GetAcceptedPaths();
        }

        if ( $r_output_model->IsValid() )
        {
            if ( ! $r_output_model->LoadedFromCache() )
            {
                $r_output_model->Cache();
            }
        }
        else
        {
            $this->Error( 'Invalid or unsupported output model' );
            return;
        }

        // Prepare SQL builder

        $g_dlog->debug( '--------------' );
        $g_dlog->debug( 'Preparing SQL Builder' );

        $sql_builder = new TpSqlBuilder();

        $num_targets = 0;

        // Add output model mapped concepts to SQL
        foreach ( $r_output_model->GetMapping() as $path => $expressions )
        {
            if ( in_array( $path, array_keys( $accepted_paths ) ) )
            {
                foreach ( $expressions as $expression )
                {
                    if ( $expression->GetType() == EXP_CONCEPT )
                    {
                        $concept_id = $expression->GetReference();

                        $concept = $r_local_mapping->GetConcept( $concept_id );

                        if ( $concept == null or ! $concept->IsMapped() )
                        {
                            $msg = 'Concept "'.$concept_id.'" is not mapped';

                            echo $this->Error( $msg );
                            return;
                        }

                        $sql_builder->AddTargetConcept( $concept );

                        ++$num_targets;

                        $g_dlog->debug( 'Adding target: '.$concept_id );
                    }
                }
            }
        }

        if ( $num_targets == 0 )
        {
            $msg = 'Could not determine any data target';
            echo $this->Error( $msg );
            return;
        }

        // Add "order by" concepts to SQL

        $order_by_concepts = $parameters->GetOrderBy();

        if ( count( $order_by_concepts ) > 0 )
        {
            foreach ( $order_by_concepts as $concept_id => $descend )
            {
                $concept = $r_local_mapping->GetConcept( $concept_id );

                if ( $concept == null or ! $concept->IsMapped() )
                {
                    $msg = 'Concept "'.$concept_id.'" is not mapped';

                    echo $this->Error( $msg );
                    return;
                }

                // Field should usually be present in the output model,
                // but let's add it here just in case...
                $sql_builder->AddTargetConcept( $concept );
            }

            $sql_builder->OrderBy( $order_by_concepts );
        }

        $sql_builder->AddRecordSource( $r_tables->GetStructure() );

        // DB connection

        if ( ! $r_data_source->Validate() )
        {
            $this->Error( 'Failed to connect to database' );
            return;
        }

        $cn = $r_data_source->GetConnection();

        $db_encoding = $r_data_source->GetEncoding();

        // Filter

        $filter = $parameters->GetFilter();

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

        if ( ! $r_local_filter->IsEmpty() )
        {
            $local_filter_sql = $r_local_filter->GetSql( $r_resource );

            $sql_builder->AddCondition( $local_filter_sql );
        }

        // Additional settings 

        $start = $this->mRequest->GetStart();

        $limit = $this->mRequest->GetLimit();

        if ( is_null( $limit ) )
        {
            $limit = $max_repetitions;
        }
        else if ( $limit > $max_repetitions )
        {
            $msg = 'Parameter "limit" exceeded maximum element repetitions';
            TpDiagnostics::Append( DC_TRUNCATED_RESPONSE, $msg, DIAG_WARN );

            $limit = $max_repetitions;
        }

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

        $this->mMainSql = $sql_builder->GetSql();

        TpDiagnostics::Append( DC_DEBUG_MSG, 'SQL to get records: '.$this->mMainSql, DIAG_DEBUG );

        $encoded_sql = TpServiceUtils::EncodeSql( $this->mMainSql, $db_encoding );

        $result_set =& $cn->SelectLimit( $encoded_sql, $limit+1, $start );

        if ( ! is_object( $result_set ) )
        {
            $err = $cn->ErrorMsg();

            $this->Error( 'Failed to select records: '.$err );

            $r_data_source->ResetConnection();

            return;
        }

        // TODO: Get and execute the SQL from inside "Render" guided by class mappings

        $xml_generator = new TpXmlGenerator( $r_output_model, $rejected_paths, 
                                             $sql_builder, $db_encoding,
                                             $max_repetitions, $limit,
                                             $this->mRequest->GetOmitNamespaces() );

        $main_content = $xml_generator->Render( $result_set, $r_resource );

        $g_dlog->debug( 'Finished preparing XML for search result' );

        if ( TpDiagnostics::Count( array( DIAG_ERROR, DIAG_FATAL ) ) )
        {
            $this->Error( 'Runtime error' );
            return;
        }

        if ( $this->mRequest->GetEnvelope() )
        {
            echo "\n<search>";
        }

        echo $main_content;

        // Search Summary

        if ( $this->mRequest->GetEnvelope() )
        {
            $this->mTotalReturned = $xml_generator->GetTotalReturned();

            echo "\n".'<summary start="'.$start.'"';

            if ( ! $result_set->EOF and $this->mTotalReturned > 0 )
            {
                $next = $start + $limit;

                echo ' next="'.$next.'"';
            }

            echo ' totalReturned="'.$this->mTotalReturned.'"';

            if ( $this->mRequest->GetCount() )
            {
                echo ' totalMatched="'.$matched.'"';
            }

            echo ' />';

            echo "\n</search>";
        }

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

        $data['template'] = $this->mRequest->GetTemplate();

        $r_output_model =& $parameters->GetOutputModel();

        if ( is_object( $r_output_model ) )
        {
            $data['output_model'] = $r_output_model->GetLocation();

            $r_response_structure =& $r_output_model->GetResponseStructure();

            if ( is_object( $r_response_structure ) )
            {
                $data['response_structure'] = $r_response_structure->GetLocation();
            }
            else
            {
                $data['response_structure'] = null;
            }
        }
        else
        {
            $data['output_model'] = null;

            $data['response_structure'] = null;
        }

        $data['partial'] = implode( ',', $parameters->GetPartial() );

        $data['sql'] = $this->mMainSql;; // note: will be empty for log-only requests

        $filter = $parameters->GetFilter();

        if ( is_object( $filter ) )
        {
            $data['filter'] = $filter->GetLogRepresentation();
        }
        else
        {
            $data['filter'] = null;
        }

        $order_by_concepts = $parameters->GetOrderBy();

        $order_by_str = '';

        foreach ( $order_by_concepts as $concept_id => $descend )
        {
            if ( strlen( $order_by_str ) > 0 )
            {
                $order_by_str .= ',';
            }

            $order_by_str .= $concept_id;

            $order_by_str .= ( $descend ) ? '(DESC)' : '(ASC)';
        }

        $data['order_by'] = $order_by_str;

        return array_merge( parent::_GetLogData(), $data );

    } // end of member function _GetLogData

} // end of TpSearchResponse
?>