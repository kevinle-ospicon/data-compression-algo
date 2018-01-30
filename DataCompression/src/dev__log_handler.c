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
static void dev__log_handler_write_packet( enum data__log_type_e log_type , uint8_t * payload_ptr );
static void dev__log_handler_init_raw_adc_packet( void );
static void dev__log_handler_init_sound_packet( void );
/*----------------------------------------------------------------------------
  global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  static variables
----------------------------------------------------------------------------*/
static data__log_raw_adc_payload_t raw_adc_payload;
static data__log_sound_payload_t sound_payload;

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
    dev__log_handler_init_raw_adc_packet();
    dev__log_handler_init_sound_packet();
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_add_raw_adc_value( uint16_t value )
{
    raw_adc_payload.value[ raw_adc_payload.sample_count ++ ] = value;
    if( raw_adc_payload.sample_count == MAX_ADC_SAMPLE_COUNT )
    {
        dev__log_handler_commit_raw_adc_packet();
    }
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_commit_raw_adc_packet( void )
{
    dev__log_handler_write_packet( data__log_type_raw_adc , ( uint8_t *) & raw_adc_payload );

    dev__log_handler_init_raw_adc_packet();   
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_add_sound_value( uint16_t value )
{
    sound_payload.value[ sound_payload.sample_count ++ ] = value;
    if( sound_payload.sample_count == MAX_SOUND_SAMPLE_COUNT )
    {
        dev__log_handler_commit_sound_packet();
    }
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_commit_sound_packet( void )
{
    dev__log_handler_write_packet( data__log_type_sound , ( uint8_t *) & sound_payload );

    dev__log_handler_init_sound_packet();   
}
/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_add_cal_packet( uint8_t pga_level , uint16_t raw_value , uint8_t current )
{
    data__log_cal_payload_t payload;
    payload.pga_level = pga_level;
    payload.raw_value = raw_value;
    payload.current = current;
    dev__log_handler_write_packet( data__log_type_cal , ( uint8_t *) & payload );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_add_temperature_packet( int8_t value )
{
    data__log_temperature_payload_t payload;
    payload.value = value;
    dev__log_handler_write_packet( data__log_type_temperature , ( uint8_t *) & payload );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_add_timestamp_packet( uint32_t value )
{
    data__log_timestamp_payload_t payload;
    payload.value = value;
    dev__log_handler_write_packet( data__log_type_timestamp , ( uint8_t *) & payload );
}
/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/
/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void dev__log_handler_write_packet( enum data__log_type_e log_type , uint8_t * payload_ptr )
{
    data__log_packet_t packet = data__log_prepare_packet( log_type , payload_ptr );
    hw__log_io_write( packet.img , data__log_get_packet_len( log_type ) );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void dev__log_handler_init_raw_adc_packet( void )
{
    raw_adc_payload.sample_count = 0;
    memset( raw_adc_payload.value , 0 , MAX_ADC_SAMPLE_COUNT * sizeof( uint16_t ) );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void dev__log_handler_init_sound_packet( void )
{
    sound_payload.sample_count = 0;
    memset( sound_payload.value , 0 , MAX_SOUND_SAMPLE_COUNT * sizeof( uint16_t ) );
}
/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/