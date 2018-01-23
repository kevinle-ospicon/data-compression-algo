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
bool srv__deserialise_parse( uint8_t byte_value );

/*----------------------------------------------------------------------------
  compile time checks
----------------------------------------------------------------------------*/

#endif // _SRV__DESERIALISE_H

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/