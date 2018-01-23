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
#define SRV_SERIALISE_LINE_FORMAT   "%4d%2d%2d_%2d:%2d:%2d:%[^:]:%[^:]\r\n"

/*----------------------------------------------------------------------------
  prototypes
----------------------------------------------------------------------------*/
static uint8_t srv__serialise_parse_log_data_type_to_bin( char * type );
static void srv__serialise_parse_payload_to_bin( enum data__log_type_e log_type , char * value );
static void srv__serialise_parse_calibration_payload_to_bin( char * value );
static void srv__serialise_parse_temperature_payload_to_bin( char * value );

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

    printf( "value string: %s\r\n" , value );
    srv__serialise_packet_ptr->header.timestamp = utils__convert_calendar_time_to_epoch( year , month , day , hour , minute , second);
    srv__serialise_packet_ptr->header.log_type = srv__serialise_parse_log_data_type_to_bin( type );
    srv__serialise_parse_payload_to_bin( srv__serialise_packet_ptr->header.log_type , value );
}

/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static uint8_t srv__serialise_parse_log_data_type_to_bin( char * type )
{
    uint8_t return_type = data__log_type_number_of;

    if ( strstr( type , "Raw" )  != NULL )
    {
        return_type = data__log_type_raw_adc;
    }
    else if( strstr( type , "Calibration finish" )  != NULL )
    {
        return_type = data__log_type_cal;
    }
    else if( strstr( type , "Temp" )  != NULL )
    {
        return_type = data__log_type_temperature;
    }

    return return_type;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void srv__serialise_parse_payload_to_bin( enum data__log_type_e log_type , char * value )
{
    switch( log_type )
    {
        case data__log_type_raw_adc:
            break;
        case data__log_type_cal:
            srv__serialise_parse_calibration_payload_to_bin( value );
            break;
        case data__log_type_temperature:
            srv__serialise_parse_temperature_payload_to_bin( value );
            break;
        default:
            break;
    }
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void srv__serialise_parse_calibration_payload_to_bin( char * value )
{
    char pga_lvl[ 16 ] = "";
    float current = 0;
    uint16_t raw_value = 0;

    sscanf( value , "%[^,], %f mA, %d" , pga_lvl , & current , & raw_value );
    printf( "pga lvl: %s\r\n" , pga_lvl );
    printf( "current: %f\r\n" , current );
    printf( "raw value: %u\r\n" , raw_value );

    if( strstr( pga_lvl , "Single LED" ) != NULL )
    {
        srv__serialise_packet_ptr->cal_payload.pga_level = data__log_cal_pga_lvl_single_led;
    }
    else if ( strstr( pga_lvl , "PGA1" ) != NULL )
    {
        srv__serialise_packet_ptr->cal_payload.pga_level = data__log_cal_pga_lvl_1;
    }
    else if ( strstr( pga_lvl , "PGA2" ) != NULL )
    {
        srv__serialise_packet_ptr->cal_payload.pga_level = data__log_cal_pga_lvl_2;
    }
    else if ( strstr( pga_lvl , "PGA4" ) != NULL )
    {
        srv__serialise_packet_ptr->cal_payload.pga_level = data__log_cal_pga_lvl_4;
    }
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void srv__serialise_parse_temperature_payload_to_bin( char * value )
{
    int tenth , unit;

    sscanf( value , "%2d,%1d" , & unit , & tenth );
    
    if( tenth >= 5)
    {
        unit++;
    }

    srv__serialise_packet_ptr->temperature_payload.value = ( int8_t ) unit;
}
/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/
