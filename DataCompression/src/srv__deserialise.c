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
    data__log_packet_t log_packet;
    uint8_t dummy_count;
} srv__deserialise_context_t;

/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  prototypes
----------------------------------------------------------------------------*/
static bool srv__deserialise_init_line( char * line_str , uint8_t size );
static bool srv__deserialise_detect_begin_marker( srv__deserialise_context_t * ctx , uint8_t byte_value );
static bool srv__deserialise_parse_header( srv__deserialise_context_t * ctx , uint8_t byte_value );
static uint32_t srv__deserialise_shift_byte( uint8_t byte_value , uint8_t position );

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
    srv__deserialise_context.dummy_count = 0;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
bool srv__deserialise_parse( uint8_t byte_value )
{
    bool status = false;
    if( srv__deserialise_context.state < srv__deserialise_state_timestamp )
    {
        //We're detecting the begin marker
        status = srv__deserialise_detect_begin_marker( & srv__deserialise_context , byte_value );
    }
    else if( srv__deserialise_context.state < srv__deserialise_state_payload )
    {
        //We're getting the header
        status = srv__deserialise_parse_header( & srv__deserialise_context , byte_value );
    }
    else
    {
        //We're getting the payload
    }
    return status;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
data__log_packet_t srv__deserialise_get_log_packet( void )
{
    return srv__deserialise_context.log_packet;
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

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static bool srv__deserialise_detect_begin_marker( srv__deserialise_context_t * ctx , uint8_t byte_value )
{
    bool status = false;
    switch( ctx->state )
    {
        case srv__deserialise_state_CR:
            if( byte_value == msg_begin_CR )
            {
                ctx->state = srv__deserialise_state_LF;
            }
            break;
        case srv__deserialise_state_LF:
            if( byte_value == msg_begin_LF )
            {
                ctx->state = srv__deserialise_state_begin_1;
            }
            else
            {
                ctx->state = srv__deserialise_state_CR;
            }
            break;
        case srv__deserialise_state_begin_1:
            if( byte_value == msg_begin_begin_1 )
            {
                ctx->state = srv__deserialise_state_begin_2;
            }
            else
            {
                ctx->state = srv__deserialise_state_CR;
            }
            break;
        case srv__deserialise_state_begin_2:
            if( byte_value == msg_begin_begin_2 )
            {
                ctx->state = srv__deserialise_state_timestamp;
                status = true; // the begin marker has been detected
                ctx->dummy_count = 0;
            }
            else
            {
                ctx->state = srv__deserialise_state_CR;
            }
            break;
        default:
            ctx->state = srv__deserialise_state_CR;
            break;
    }
    return status;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static bool srv__deserialise_parse_header( srv__deserialise_context_t * ctx , uint8_t byte_value )
{
    bool status = false;
    switch( ctx->state )
    {
        case srv__deserialise_state_timestamp:
            ctx->log_packet.header.timestamp |= srv__deserialise_shift_byte( byte_value , ctx->dummy_count );
            if( ( ++ ctx->dummy_count ) >= LOG_DATA_TIMESTAMP_LEN )
            {
                ctx->state = srv__deserialise_state_log_type;
            }
            break;
        case srv__deserialise_state_log_type:
            ctx->state = srv__deserialise_state_payload_len;
            ctx->log_packet.header.log_type = byte_value;
            break;
        case srv__deserialise_state_payload_len:
            ctx->state = srv__deserialise_state_payload;
            ctx->log_packet.header.payload_len = byte_value;
            ctx->dummy_count = 0;
            status = true;
            break;
        default:
            ctx->state = srv__deserialise_state_CR;
            break;
    }
    return status;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static uint32_t srv__deserialise_shift_byte( uint8_t byte_value , uint8_t position )
{
    return ( ( 0x000000FF & byte_value ) << ( position * 8 ) );
}
/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/