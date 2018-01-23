/*============================================================================
@brief A C source for log data serialisation service
------------------------------------------------------------------------------
<!-- Written by Kevin (Phuc) Le Dinh -->
<!-- Copyright (C) 2018 All rights reserved -->
============================================================================*/

/*----------------------------------------------------------------------------
  @brief
Provide API to convert between ASCII and binary encoded format 
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  include files
----------------------------------------------------------------------------*/
#include "srv__serialise.h"
#include "utils/utils.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

/*----------------------------------------------------------------------------
  manifest constants
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  type definitions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/
#define SRV_SERIALISE_LINE_FORMAT   "%4d%2d%2d_%2d:%2d:%2d:%[^:]:%s\r\n"

/*----------------------------------------------------------------------------
  prototypes
----------------------------------------------------------------------------*/
uint8_t srv__serialise_parse_log_data_type_to_bin( char * type );
int8_t srv__serialise_parse_temperature_payload_to_bin( char * value );

/*----------------------------------------------------------------------------
  global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  static variables
----------------------------------------------------------------------------*/
static data__log_packet_t * srv__serialise_packet_ptr;
/*----------------------------------------------------------------------------
  public functions
----------------------------------------------------------------------------*/

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void srv__serialise_init( data__log_packet_t * packet_ptr )
{
    srv__serialise_packet_ptr = packet_ptr;
    memset( packet_ptr , 0 , data__log_get_packet_len( data__log_type_raw_adc ) );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void srv__serialise_to_bin( char * line_str , int line_size )
{
    int year , month , day;
    int hour , minute , second;
    char type[ 16 ] = "";
    char value[ 32 ] = "";

    sscanf( line_str , SRV_SERIALISE_LINE_FORMAT , & year , & month , & day ,
                                                    & hour , & minute , & second ,
                                                    type , value );

    srv__serialise_packet_ptr->header.timestamp = utils__convert_calendar_time_to_epoch( year , month , day , hour , minute , second);
    srv__serialise_packet_ptr->header.log_type = srv__serialise_parse_log_data_type_to_bin( type );
    srv__serialise_packet_ptr->temperature_payload.value = srv__serialise_parse_temperature_payload_to_bin( value );
}

/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
uint8_t srv__serialise_parse_log_data_type_to_bin( char * type )
{
    return data__log_type_temperature;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
int8_t srv__serialise_parse_temperature_payload_to_bin( char * value )
{
    int tenth , unit;

    sscanf( value , "%2d,%1d" , & unit , & tenth );
    
    if( tenth >= 5)
    {
        unit++;
    }
    
    return ( int8_t ) unit;
}
/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/
