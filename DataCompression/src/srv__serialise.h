/*============================================================================
@brief A C source header for log data serialisation service
------------------------------------------------------------------------------
<!-- Written by Kevin (Phuc) Le Dinh -->
<!-- Copyright (C) 2018 All rights reserved -->
============================================================================*/

#ifndef _SRV__SERIALISE_H
#define _SRV__SERIALISE_H

/*----------------------------------------------------------------------------
  @brief
Provide API to convert between ASCII and binary encoded format 
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
void srv__serialise_to_bin( char * line_str , int line_size );
/*----------------------------------------------------------------------------
  compile time checks
----------------------------------------------------------------------------*/

#endif // _SRV__SERIALISE_H

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/