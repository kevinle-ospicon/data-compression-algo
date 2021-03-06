/*============================================================================
@brief A C source for data log definition
------------------------------------------------------------------------------
<!-- Written by Kevin (Phuc) Le Dinh -->
<!-- Copyright (C) 2018 All rights reserved -->
============================================================================*/

/*----------------------------------------------------------------------------
  @brief
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  include files
----------------------------------------------------------------------------*/
#include "data__log.h"
#include <string.h>

/*----------------------------------------------------------------------------
  manifest constants
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  type definitions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  prototypes
----------------------------------------------------------------------------*/
static uint8_t data__log_get_payload_len( enum data__log_type_e log_type );
static void data__log_prepare_header( data__log_packet_t * packet , enum data__log_type_e log_type );

/*----------------------------------------------------------------------------
  global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  static variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  public functions
----------------------------------------------------------------------------*/
/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
uint8_t data__log_get_packet_len( enum data__log_type_e log_type )
{
    return data__log_get_payload_len( log_type ) + sizeof( data__log_header_t );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
data__log_packet_t 
data__log_prepare_packet( enum data__log_type_e log_type , uint8_t * payload_ptr )
{
    data__log_packet_t packet;
    
    data__log_prepare_header( & packet , log_type );
    
    if( payload_ptr != NULL )
    {
        memset( packet.payload , 0x00 , packet.header.payload_len );
        memcpy( packet.payload , payload_ptr , packet.header.payload_len );
    } // else silently ignore
    return packet;
}
/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static 
uint8_t 
data__log_get_payload_len( enum data__log_type_e log_type )
{
    switch( log_type )
    {
        case data__log_type_raw_adc:
            return sizeof( data__log_raw_adc_payload_t );
        case data__log_type_cal:
            return sizeof( data__log_cal_payload_t );
        case data__log_type_timestamp:
            return sizeof( data__log_timestamp_payload_t );
        default:
            return 0;
    }
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static 
void 
data__log_prepare_header( data__log_packet_t * packet , enum data__log_type_e log_type )
{
    memcpy( packet->header.log_begin , LOG_DATA_BEGIN_MARKER , LOG_DATA_BEGIN_MARKER_LEN );
    packet->header.log_type = log_type;
    packet->header.payload_len = data__log_get_payload_len( log_type );
}

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/
