<?php

/**
 * Implementation of hook_views_handlers
 */
function darwincore_views_handlers(){
  return array(
    'info' => array(
      'path' => drupal_get_path('module','darwincore') . '/views'
    ),
    'handlers' => array(
      'darwincore_handler_field_tid' => array(
        'parent' => 'views_handler_field'
      )
    )
  );
}

/**
 * Implementation of hook_views_data()
 */
function darwincore_views_data() {
  $data = array(
    'darwincore' => array(
      'table' => array(
        'base' => array(
          'field' => 'nid',
          'title' => t('Darwincore specimen'),
          'help' => t('Darwincore specimen')
        ),
        'group' => 'Darwincore - Specimen',
        'join' => array(
          'node' => array(
            'left_field' => 'vid',
            'field' => 'vid'
          )
        )
      ),
      'sex' => array(
        'title' => t('Sex'),
        'help' => t('The sex of the specimen'),
        'field' => array(
          'handler' => 'darwincore_handler_field_tid'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_term_node_tid'
        )
      ),
      'basisofrecord' => array(
        'title' => t('Basis of record'),
        'help' => t('A descriptive term indicating whether the record represents an object or observation.'),
        'field' => array(
          'handler' => 'darwincore_handler_field_tid'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_term_node_tid'
        )
      ),
      'typestatus' => array(
        'title' => t('Type status'),
        'help' => t('A list of one or more nomenclatural types represented by the object.'),
        'field' => array(
          'handler' => 'darwincore_handler_field_tid'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_term_node_tid'
        )
      ),  
      'taxonomicname' => array(
        'title' => t('Taxonomic name'),
        'help' => t('The full name of the lowest level taxon to which the organism has been identified in the most recent accepted determination, specified as precisely as possible'),
        'field' => array(
          'handler' => 'darwincore_handler_field_tid'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_term_node_tid'
        )
      ), 
      'collectioncode' => array(
        'title' => t('Collection code'),
        'help' => t("The code (or acronym) identifying the collection within the institution in which the organism record is cataloged."),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'institutioncode' => array(
        'title' => t('Institution code'),
        'help' => t("The code (or acronym) identifying the institution administering the collection in which the organism record is cataloged. No global registry exists for institutional codes; use the code that is \"standard\" at your institution or in your discipline."),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),    
      'cataloguenumbertext' => array(
        'title' => t('Catalogue number text'),
        'help' => t("The catalogue number (text) for this specimen"),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ), 
      'identificationqualifier' => array(
        'title' => t('Identification qualifier/modifier'),
        'help' => t('A standard term to qualify the identification of the organism when doubts have arisen as to its identity. Examples: "cf.", "aff.", "subspecies in question"'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ), 
      'identifiedby' => array(
        'title' => t('Identified by'),
        'help' => t('The name(s) of the person(s) who applied the ScientificName to the object or observation. Example: James Earl Jones.'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'dateidentifiedyear' => array(
        'title' => t('Date identified year'),
        'help' => t('The year in which the object or observation was identified as being a member of the taxon given in the ScientificName.'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'dateidentifiedmonth' => array(
        'title' => t('Date identified month'),
        'help' => t('The month in which the object or observation was identified as being a member of the taxon given in the ScientificName.'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'dateidentifiedday' => array(
        'title' => t('Date identified day'),
        'help' => t('The day in which the object or observation was identified as being a member of the taxon given in the ScientificName.'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'collectornumber' => array(
        'title' => t('Collector number'),
        'help' => t('An identifying string applied to the object or observation at the time of collection. Serves as a link between field notes and the object or observation.'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'fieldnumber' => array(
        'title' => t('Field number'),
        'help' => t('An identifying string applied to a set of material that resulted from a single collecting event.'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'collector' => array(
        'title' => t('Collector'),
        'help' => t('The name(s) of the collector(s) of the original data for the cataloged item.'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'earliestdatecollected' => array(
        'title' => t('Earliest date collected'),
        'help' => t('The earliest date-time (Common Era calendar) in a date-time period during which an organism or group of organisms was collected or observed.'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'latestdatecollected' => array(
        'title' => t('Latest date collected'),
        'help' => t('The latest date-time (Common Era calendar) in a date-time period during which an organism or group of organisms was collected or observed.'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'fieldnotes' => array(
        'title' => t('Field notes'),
        'help' => t('Notes taken in the field for the cataloged item.'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'othercataloguenum' => array(
        'title' => t('Other cataloguenum'),
        'help' => t('A list of previous or alternative fully qualified catalog numbers for the same object or observation, whether in the current collection or in any other.  One per line'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'lifestage' => array(
        'title' => t('Life stage'),
        'help' => t('The age class or life stage of the biological individual represented by the catalogued object or observation. Examples: "adult", "mature", "juvenile", "eft", "nymph", "seedling", "seed", "egg"'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'count' => array(
        'title' => t('Count'),
        'help' => t('The number of individuals present in the lot or container. Not to be used for observations.'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'genbanknum' => array(
        'title' => t('Genbank number'),
        'help' => t('Genbank number'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'remarks' => array(
        'title' => t('Remarks'),
        'help' => t('General comments about the specimen that others may find usefull.'),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      'locationnode' => array(
        'title' => t('Darwincore location node'),
        'help' => t('The Location node that this specimen references'),
        'relationship' => array(
          'base' => 'darwincorelocation',
          'field' => 'nid',
          'handler' => 'views_handler_relationship',
          'label' => t('Darwincore location node')
        )
      )
    ),    
    'darwincorelocation' => array(
      'table' => array(
        'base' => array(
          'field' => 'nid',
          'title' => t('Darwincore location'),
          'help' => t('Darwincore location')
        ),
        'group' => t('Darwincore - Location'),
        'title' => t('Darwincore - Location'),
        'join' => array(
          'node' => array(
            'left_field' => 'vid',
            'field' => 'vid'
          )
        )
      ),
      'island' => array(
        'title' => t('Island'),
        'help' => t('The full, unabbreviated name of the island from which the cataloged item was collected.'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )
      ),
      'locality' => array(
        'title' => t('Locality'),
        'help' => t('The description of the locality from which the cataloged item was collected, sans geographic information provided in other geographic fields.'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )
      ),
      'continentocean' => array(
        'title' => t('Continent/BodyOfWater'),
        'help' => t('The full, unabbreviated name of the continent or ocean from which the cataloged item was collected.'),
        'field' => array(
          'handler' => 'darwincore_handler_field_tid'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_term_node_tid'
        )
      ),
      'stateprovince' => array(
        'title' => t('State province'),
        'help' => t('The full, unabbreviated name of the state, province, or region (i.e., the next smaller political region than Country) from which the cataloged item was collected.'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )        
      ),
      'islandgroup' => array(
        'title' => t('Island group'),
        'help' => t('The full, unabbreviated name of the island group from which the cataloged item was collected.'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )        
      ),
      'county' => array(
        'title' => t('County'),
        'help' => t('The full, unabbreviated name of the county, shire, or municipality (i.e., the next smaller political region than StateProvince) from which the cataloged item was collected.'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )        
      ),
      'geodeticdatum' => array(
        'title' => t('Geodetic Datum'),
        'help' => t('The geodetic datum to which the latitude and longitude refer. If not known, use "not recorded".'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )        
      ),
      'verbatimcoordinatesystem' => array(
        'title' => t('Verbatim Coordinate System '),
        'help' => t('The name of the system in which the verbatim geographic coordinates were recorded. Examples: "decimal degrees", "degrees minutes seconds", "degrees decimal minutes", "UTM"'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )        
      ),
      'georeferenceprotocol' => array(
        'title' => t('Georeference Protocol'),
        'help' => t('A reference to the methods used for determining the coordinates and uncertainties. Example: "http://manisnet.org/GeorefGuide.html".'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )        
      ),
      'coordinateuncertainty' => array(
        'title' => t('Coordinate uncertainty in meters'),
        'help' => t('The upper limit of the distance (in meters) from the given latitude and longitude describing a circle within which the whole of the described locality must lie. Use NULL where the uncertainty is unknown, cannot be estimated, or is not applicable.'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )        
      ),
      'georeferenceremarks' => array(
        'title' => t('Georeference remarks'),
        'help' => t('Comments about the spatial description determination, explaining assumptions made in addition or opposition to the those formalized in the method referred to in GeoreferenceProtocol.'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )        
      ),
      'minelevation' => array(
        'title' => t('Minimum elevation in meters'),
        'help' => t('The minimum distance in meters above (positive) or below sea level of the collecting locality.'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )        
      ),
      'maxelevation' => array(
        'title' => t('Maximum elevation in meters'),
        'help' => t('The maximum distance in meters above (positive) or below sea level of the collecting locality.'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )        
      ),
      'mindepth' => array(
        'title' => t('Minimum depth in meters'),
        'help' => t('The minimum or actual depth at which the collection or observation was made. Use positive values for locations below the surface. Examples: 0 (for a depth of up to 10m). 50 (for a depth between 50m and 100m).'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )        
      ),
      'maxdepth' => array(
        'title' => t('Maximum depth in meters'),
        'help' => t('The maximum or actual depth at which the collection or observation was made. Use positive values for locations below the surface. Examples: 10 (for a depth of up to 10m). 100 (for a depth between 50m and 100m).'),
        'field' => array(
          'handler' => 'views_handler_field'
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        )        
      )
    )
  );
  return $data;
}