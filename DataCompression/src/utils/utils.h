/*============================================================================
@brief A C source header for utilities
------------------------------------------------------------------------------
<!-- Written by Kevin (Phuc) Le Dinh -->
<!-- Copyright (C) 2018 All rights reserved -->
============================================================================*/

#ifndef utils_h
#define utils_h

/*----------------------------------------------------------------------------
  @brief
Misc utils
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  nested include files
----------------------------------------------------------------------------*/
#include <stdint.h>

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
uint32_t utils__convert_calendar_time_to_epoch( int year , int month , int day , int hour , int minute , int second );
uint32_t utils__shift_byte_left( uint8_t byte_value , uint8_t position );

/*----------------------------------------------------------------------------
  compile time checks
----------------------------------------------------------------------------*/

#endif

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/