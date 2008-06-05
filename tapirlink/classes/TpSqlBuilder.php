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

require_once('TpDiagnostics.php');

class TpSqlBuilder 
{
    var $mDistinct  = false;
    var $mCount     = false;
    var $mGroupAll  = false;
    var $mOrderBy; // null = no order ||
                   // empty array = order by all (ascending) || 
                   // array of concept ids => descend (boolean)

    var $mSelect = array(); // concept id => sql representation (table.column)

    var $mTables = array(); // table name => TpTable object

    var $mConditions = array();

    function TpSqlBuilder( ) 
    {

    } // end of member function TpSqlBuilder

    function SetDistinct( $bool ) 
    {
        $this->mDistinct = $bool;
        
    } // end of member function SetDistinct

    function AddCountColumn( $bool ) 
    {
        $this->mCount = $bool;
        
    } // end of member function AddCountColumn

    function GroupAll( ) 
    {
        $this->mGroupAll = true;
        
    } // end of member function GroupAll

    function OrderBy( $orderBy )
    {
        $this->mOrderBy = $orderBy; // target id => descend
        
    } // end of member function OrderBy

    function AddTargetColumn( $column ) 
    {
        $this->mSelect[$column] = $column;

    } // end of member function AddTargetColumn

    /** $concept here should always correspond to a property
     *  In the future, $concept should also indicate to which 
     *  class it belongs. And the class must also have a mapping.
     */
    function AddTargetConcept( $concept ) 
    {
        $mapping = $concept->GetMapping();

        $sql_target = $mapping->GetSqlTarget();

        $concept_id = $concept->GetId();

        if ( empty( $sql_target ) )
        {
            $msg = 'Could not find a valid local mapping for concept "'.$concept_id.'". It will be ignored.';
            TpDiagnostics::Append( DC_UNMAPPED_CONCEPT, $msg, DIAG_WARN );

            return;
        }

        $this->mSelect[$concept_id] = $sql_target;

    } // end of member function AddTargetConcept

    function GetTargetIndex( $targetId )
    {
        $i = -1;

        foreach ( $this->mSelect as $target_id => $sql )
        {
            ++$i;

            if ( strcasecmp( $targetId, $target_id ) == 0 )
            {
                break;
            }
        }

        return $i;

    } // end of member function GetTargetIndex

    function GetTargetSql( $targetId )
    {
        return $this->mSelect[$targetId];

    } // end of member function GetTargetSql

    /** In the future this method should receive as a parameter 
     *  a class that maps to one or more linked tables. It could 
     *  then be called a single time, and then subsequent calls
     *  to AddLinkToClass() could be made.
     */
    function AddRecordSource( $tables ) 
    {
        $this->mTables = $tables;
        
    } // end of member function AddRecordSource

    function AddCondition( $sql ) 
    {
        array_push( $this->mConditions, $sql );
        
    } // end of member function AddCondition

    function GetSql( ) 
    {
        $sql = 'SELECT ';

        if ( $this->mDistinct ) {

            $sql .= 'DISTINCT ';
        }

        // Targets clause

        $i = 0;

        foreach ( $this->mSelect as $target_id => $target_sql )
        {
            ++$i;

            if ( $i > 1 )
            {
                $sql .= ', ';
            }

            $target_index = $this->GetTargetIndex( $target_id );

            if ( $target_index >= 0 )
            {
                $sql .= $target_sql . " AS c" . $target_index;
            }
        }

        if ( $this->mCount ) {

            if ( $i > 0 )
            {
                $sql .= ', ';
            }

            $sql .= 'count(*) as cnt';
        }

        // FROM clause

        if ( count( $this->mTables ) )
        {
            $keys = array_keys( $this->mTables );

            $n_tab = 0;

            for ( $i = 0; $i < count( $keys ); $i++ )
            {
                if ( $this->mTables[$keys[$i]] !== NULL )
                {
                    $n_tab ++;
                }
            }

            $from = $this->mTables[$keys[0]]->GetName();

            if ( $n_tab > 1 )
            {
                $from .= "\n";

                for ( $i = 1; $i < $n_tab; $i++ )
                {
                    $from = '(' . $from;
                    $from .= ' LEFT JOIN ';
                    $from .= $this->mTables[$keys[$i]]->GetName();
                    $from .= ' ON ';
                    $from .= $this->mTables[$keys[$i]]->GetParentName().'.'.
                             $this->mTables[$keys[$i]]->GetJoin();
                    $from .= ' = ';
                    $from .= $this->mTables[$keys[$i]]->GetName().'.'.
                             $this->mTables[$keys[$i]]->GetKey();
                    $from .= ")\n";
                }
            }

            $sql .= " FROM " . $from;
        }

        // WHERE

        $num_conditions = count( $this->mConditions );

        if ( $num_conditions )
        {
            $sql .= ' WHERE ';

            for ( $i = 0; $i < $num_conditions; ++$i )
            {
                if ( $i > 0 )
                {
                    $sql .= ' AND ';
                }

                $sql .= $this->mConditions[$i];
            }
        }

        // GROUP BY

        if ( $this->mGroupAll ) {

            $sql .= ' GROUP BY ';

            $i = 0;

            foreach ( $this->mSelect as $target_id => $target_sql )
            {
                ++$i;

                if ( $i > 1 )
                {
                    $sql .= ', ';
                }

                $sql .= ( TP_SQL_USE_COLUMN_REF ) ? $i : $target_sql;
            }
        }

        // ORDER BY

        if ( is_array( $this->mOrderBy ) )
        {
            $sql .= ' ORDER BY ';

            if ( count( $this->mOrderBy ) == 0 ) // order by all
            {
                $i = 0;

                foreach ( $this->mSelect as $target_id => $target_sql )
                {
                    ++$i;

                    if ( $i > 1 )
                    {
                        $sql .= ', ';
                    }

                    $sql .= ( TP_SQL_USE_COLUMN_REF ) ? $i : $target_sql;
                }
            }
            else
            {
                $i = 1;

                foreach ( $this->mOrderBy as $concept_id => $descend )
                {
                    if ( $i > 1 )
                    {
                        $sql .= ', ';
                    }

                    if ( TP_SQL_USE_COLUMN_REF )
                    {
                        $sql .= $this->GetTargetIndex( $concept_id ) + 1;
                    }
                    else
                    {
                        $sql .= $this->GetTargetSql( $concept_id );
                    }

                    if ( $descend )
                    {
                        $sql .= ' DESC';
                    }

                    ++$i;
                }
            }
        }

        return $sql;
        
    } // end of member function GetSql

} // end of TpSqlBuilder
?>