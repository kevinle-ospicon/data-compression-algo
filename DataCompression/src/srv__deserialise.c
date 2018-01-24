/*============================================================================
@brief A C source for log data de-serialisation service
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
#include "srv__deserialise.h"
#include "data__log.h"
#include <string.h>

/*----------------------------------------------------------------------------
  manifest constants
----------------------------------------------------------------------------*/
enum srv__deserialise_state
{
    srv__deserialise_state_CR,
    srv__deserialise_state_LF,
    srv__deserialise_state_begin_1,
    srv__deserialise_state_begin_2,
    srv__deserialise_state_timestamp,
    srv__deserialise_state_log_type,
    srv__deserialise_state_payload_len,
    srv__deserialise_state_payload,

    srv__deserialise_state_number_of
};

enum srv__deserialise_msg_begin
{
    msg_begin_CR = '\r',
    msg_begin_LF = '\n',
    msg_begin_begin_1 = '>',
    msg_begin_begin_2 = '>',
};
/*----------------------------------------------------------------------------
  type definitions
----------------------------------------------------------------------------*/
typedef struct srv__deserialise_context_s
{
    enum srv__deserialise_state state;
    char * line_str;
} srv__deserialise_context_t;

/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  prototypes
----------------------------------------------------------------------------*/
static bool srv__deserialise_init_line( char * line_str , uint8_t size );

/*----------------------------------------------------------------------------
  global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  static variables
----------------------------------------------------------------------------*/
static srv__deserialise_context_t srv__deserialise_context;

/*----------------------------------------------------------------------------
  public functions
----------------------------------------------------------------------------*/

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void srv__deserialise_init( char * line_str , uint8_t size )
{
    if( ! srv__deserialise_init_line( line_str , size ) )
    {
        return;
    }

    srv__deserialise_context.state = srv__deserialise_state_CR;
    srv__deserialise_context.line_str = line_str;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
bool srv__deserialise_parse( uint8_t byte_value )
{
    bool status = false;
    switch( srv__deserialise_context.state )
    {
        case srv__deserialise_state_CR:
            if( byte_value == msg_begin_CR )
            {
                srv__deserialise_context.state = srv__deserialise_state_LF;
            }
            break;
        case srv__deserialise_state_LF:
            if( byte_value == msg_begin_LF )
            {
                srv__deserialise_context.state = srv__deserialise_state_begin_1;
            }
            break;
        case srv__deserialise_state_begin_1:
            if( byte_value == msg_begin_begin_1 )
            {
                srv__deserialise_context.state = srv__deserialise_state_begin_2;
            }
            break;
        case srv__deserialise_state_begin_2:
            if( byte_value == msg_begin_begin_2 )
            {
                srv__deserialise_context.state = srv__deserialise_state_timestamp;
            }
            memcpy( srv__deserialise_context.line_str , LOG_DATA_BEGIN_MARKER , LOG_DATA_BEGIN_MARKER_LEN );
            status = true;
            break;
        default:
            srv__deserialise_context.state = srv__deserialise_state_CR;
            break;
    }
    return status;
}

/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static bool srv__deserialise_init_line( char * line_str , uint8_t size )
{
    if( line_str == NULL )
    {
        return false;
    }

    if( size > SRV_DESERIALISE_MAX_STRING_LEN )
    {
        memset( line_str , 0  , SRV_DESERIALISE_MAX_STRING_LEN );
    }
    else
    {
        memset( line_str , 0 , size );
    }
    return true;
}

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/