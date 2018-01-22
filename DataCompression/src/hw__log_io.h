/*============================================================================
@brief A C source header file for write/read log file
------------------------------------------------------------------------------
<!-- Written by Kevin (Phuc) Le Dinh -->
<!-- Copyright (C) 2018 All rights reserved -->
============================================================================*/

#ifndef hw__log_io_h
#define hw__log_io_h

/*----------------------------------------------------------------------------
  @brief
  A dummy driver for file IO
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  nested include files
----------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/
#define HW__LOG_IO_MAX_DATA_LEN 64 //make this large to test. Need to rewrite this properly

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
void hw__log_io_init( void );
void hw__log_io_write( uint8_t * data_ptr , uint8_t size );
uint8_t * hw__log_io_read( uint8_t * size );

/*----------------------------------------------------------------------------
  compile time checks
----------------------------------------------------------------------------*/

#endif

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/