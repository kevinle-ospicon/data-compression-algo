/*============================================================================
@brief Log data handler source file
------------------------------------------------------------------------------
<!-- Written by Kevin (Phuc) Le Dinh -->
<!-- Copyright (C) 2018 All rights reserved -->
============================================================================*/

#ifndef _DEV__LOG_HANDLER_H
#define _DEV__LOG_HANDLER_H

/*----------------------------------------------------------------------------
  @brief
This driver provides APIs to accept log data and append to the memory
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  nested include files
----------------------------------------------------------------------------*/
#include "data__log.h"

/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  manifest constants
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  type definitions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  extern variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  prototypes
----------------------------------------------------------------------------*/
void dev__log_handler_init_log_data( void );
void dev__log_handler_add_raw_adc_value( uint16_t value );
void dev__log_handler_commit_raw_adc_packet( void );
void dev__log_handler_add_sound_value( uint16_t value );
void dev__log_handler_commit_sound_packet( void );
void dev__log_handler_add_cal_packet( uint8_t pga_level , uint16_t raw_value , uint8_t current , uint32_t timestamp );
void dev__log_handler_add_temperature_packet( int8_t value );
void dev__log_handler_add_timestamp_packet( uint32_t value );

/*----------------------------------------------------------------------------
  compile time checks
----------------------------------------------------------------------------*/

#endif // _DEV__LOG_HANDLER_H

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/