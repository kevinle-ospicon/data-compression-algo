/*============================================================================
@brief Log data handler source file
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
#include "dev__log_handler.h"
#include "hw__log_io.h"
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
static void dev__log_handler_write_packet( uint32_t timestamp , enum data__log_type_e log_type , uint8_t * payload_ptr );
/*----------------------------------------------------------------------------
  global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  static variables
----------------------------------------------------------------------------*/
static data__log_raw_adc_payload_t raw_adc_payload;
static uint32_t data__log_raw_adc_timestamp = 0xFFFFFFFF;

/*----------------------------------------------------------------------------
  public functions
----------------------------------------------------------------------------*/

/*============================================================================
@brief Initialise all internal log payloads
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_init_log_data( void )
{
    raw_adc_payload.sample_count = 0;
    memset( raw_adc_payload.value , 0 , MAX_ADC_SAMPLE_COUNT * sizeof( uint16_t ) );
    data__log_raw_adc_timestamp = 0xFFFFFFFF;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_add_raw_adc_value( uint32_t timestamp , uint16_t value )
{
    if( timestamp != data__log_raw_adc_timestamp && data__log_raw_adc_timestamp != 0xFFFFFFFF )
    {
        dev__log_handler_commit_raw_adc_packet();
        raw_adc_payload.value[ raw_adc_payload.sample_count ++ ] = value;
        data__log_raw_adc_timestamp = timestamp;
    }
    else
    {
        raw_adc_payload.value[ raw_adc_payload.sample_count ++ ] = value;
        data__log_raw_adc_timestamp = timestamp;
        if( raw_adc_payload.sample_count == MAX_ADC_SAMPLE_COUNT )
        {
            dev__log_handler_commit_raw_adc_packet();
        }
    }

}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_commit_raw_adc_packet( void )
{
    dev__log_handler_write_packet( data__log_raw_adc_timestamp , data__log_type_raw_adc , ( uint8_t *) & raw_adc_payload );

    dev__log_handler_init_log_data();   
}
/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_add_cal_packet( uint32_t timestamp , uint8_t pga_level , uint16_t raw_value , uint8_t current )
{
    data__log_cal_payload_t payload = 
    {
        pga_level & 0x0F,
        raw_value,
        current
    };
    dev__log_handler_write_packet( timestamp , data__log_type_cal , ( uint8_t *) & payload );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_add_temperature_packet( uint32_t timestamp , int8_t value )
{
    data__log_temperature_payload_t payload = { value };
    dev__log_handler_write_packet( timestamp , data__log_type_temperature , ( uint8_t *) & payload );
}

/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/
/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void dev__log_handler_write_packet( uint32_t timestamp , enum data__log_type_e log_type , uint8_t * payload_ptr )
{
    data__log_packet_t packet = data__log_prepare_packet( timestamp , log_type , payload_ptr );
    hw__log_io_write( packet.img , data__log_get_packet_len( log_type ) );
}

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/