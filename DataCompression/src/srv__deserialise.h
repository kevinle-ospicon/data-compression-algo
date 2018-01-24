/*============================================================================
@brief A C source header for log data de-serialisation service
------------------------------------------------------------------------------
<!-- Written by Kevin (Phuc) Le Dinh -->
<!-- Copyright (C) 2018 All rights reserved -->
============================================================================*/

#ifndef _SRV__DESERIALISE_H
#define _SRV__DESERIALISE_H

/*----------------------------------------------------------------------------
  @brief
Conversion from binary encoded log data to ASCII UTF-8 format
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  nested include files
----------------------------------------------------------------------------*/
#include "data__log.h"
#include <stdbool.h>
#include <stdint.h>

/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/
#define SRV_DESERIALISE_MAX_STRING_LEN 100
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
void srv__deserialise_init( void );
bool srv__deserialise_parse( uint8_t byte_value );
data__log_packet_t srv__deserialise_get_log_packet( void );

/*----------------------------------------------------------------------------
  compile time checks
----------------------------------------------------------------------------*/

#endif // _SRV__DESERIALISE_H

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/