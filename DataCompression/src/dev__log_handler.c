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
static void dev__log_handler_init_calibration_led_payload( void );

/*----------------------------------------------------------------------------
  global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  static variables
----------------------------------------------------------------------------*/
static data__log_packet_t raw_adc_pkt;

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
    dev__log_handler_init_calibration_led_payload();
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_add_raw_adc_value( uint32_t timestamp , uint16_t value )
{

}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void dev__log_handler_add_cal_led_packet( uint32_t timestamp , uint8_t event_type , uint8_t pga_level , uint16_t raw_value , uint8_t current )
{
    data__log_cal_led_payload_t payload = 
    {
        event_type & 0x0F,
        pga_level & 0x0F,
        raw_value,
        current
    };
    data__log_packet_t packet = data__log_prepare_packet( timestamp , data__log_type_cal_led , ( uint8_t *) & payload );
    hw__log_io_write( packet.img , data__log_get_packet_len( data__log_type_cal_led ) );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
data__log_cal_led_payload_t * dev__log_handler_get_packet_ptr( void  )
{
    return NULL;
}

/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/
/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void dev__log_handler_init_calibration_led_payload( void )
{

}

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/