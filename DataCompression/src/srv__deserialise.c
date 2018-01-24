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
#include "utils.h"
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

enum srv__deserialise_cal_payload_state
{
    srv__deserialise_cal_payload_state_pga_level,
    srv__deserialise_cal_payload_state_raw_value,
    srv__deserialise_cal_payload_state_current
};


/*----------------------------------------------------------------------------
  type definitions
----------------------------------------------------------------------------*/
typedef struct srv__deserialise_context_s
{
    enum srv__deserialise_state state;
    data__log_packet_t log_packet;
    uint8_t dummy_count;
    enum srv__deserialise_cal_payload_state cal_payload_state;
} srv__deserialise_context_t;

typedef bool ( * srv__deserialise_parse_payload_cb_t ) ( srv__deserialise_context_t * ctx , uint8_t byte_value );

/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  prototypes
----------------------------------------------------------------------------*/
static bool srv__deserialise_detect_begin_marker( srv__deserialise_context_t * ctx , uint8_t byte_value );
static bool srv__deserialise_parse_header( srv__deserialise_context_t * ctx , uint8_t byte_value );
static bool srv__deserialise_parse_raw_adc_payload( srv__deserialise_context_t * ctx , uint8_t byte_value );
static bool srv__deserialise_parse_calibration_payload( srv__deserialise_context_t * ctx , uint8_t byte_value );
static bool srv__deserialise_parse_temperature_payload( srv__deserialise_context_t * ctx , uint8_t byte_value );

static const srv__deserialise_parse_payload_cb_t parse_payload_cb[ data__log_type_number_of ] = 
{
    srv__deserialise_parse_raw_adc_payload,
    srv__deserialise_parse_calibration_payload,
    srv__deserialise_parse_temperature_payload,
};
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
void srv__deserialise_init( void )
{
    srv__deserialise_context.state = srv__deserialise_state_CR;
    srv__deserialise_context.dummy_count = 0;
    srv__deserialise_context.cal_payload_state = srv__deserialise_cal_payload_state_pga_level;
    memset( & srv__deserialise_context.log_packet , 0 , data__log_get_packet_len( data__log_type_raw_adc ) );
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
        uint8_t log_type = srv__deserialise_context.log_packet.header.log_type;
        if( log_type < data__log_type_number_of )
        {
            status = parse_payload_cb[ log_type ]( & srv__deserialise_context , byte_value );
        }
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
static bool srv__deserialise_detect_begin_marker( srv__deserialise_context_t * ctx , uint8_t byte_value )
{
    bool status = false;
    switch( ctx->state )
    {
        case srv__deserialise_state_CR:
            if( byte_value == msg_begin_CR )
            {
                srv__deserialise_init();
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
            ctx->log_packet.header.timestamp |= utils__shift_byte_left( byte_value , ctx->dummy_count );
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
static bool srv__deserialise_parse_raw_adc_payload( srv__deserialise_context_t * ctx , uint8_t byte_value )
{
    return true;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static bool srv__deserialise_parse_calibration_payload( srv__deserialise_context_t * ctx , uint8_t byte_value )
{
    bool status = false;
    
    switch( ctx->cal_payload_state )
    {
        case srv__deserialise_cal_payload_state_pga_level:
            ctx->log_packet.cal_payload.pga_level = byte_value;
            ctx->cal_payload_state = srv__deserialise_cal_payload_state_raw_value;
            break;
        case srv__deserialise_cal_payload_state_raw_value:
            ctx->log_packet.cal_payload.raw_value |= (uint16_t) ( utils__shift_byte_left( byte_value , ctx->dummy_count ) & 0xFFFF );
            if( ( ++ ctx->dummy_count ) >= LOG_DATA_RAW_ADC_SIZE_BYTES )
            {
                ctx->cal_payload_state = srv__deserialise_cal_payload_state_current;
            }
            break;
        case srv__deserialise_cal_payload_state_current:
            ctx->log_packet.cal_payload.current = byte_value;
            status = true;
            ctx->state = srv__deserialise_state_CR;
            break;
        default:
            break;
    }
    return status;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static bool srv__deserialise_parse_temperature_payload( srv__deserialise_context_t * ctx , uint8_t byte_value )
{
    ctx->log_packet.temperature_payload.value = ( int8_t ) byte_value;
    ctx->state = srv__deserialise_state_CR;
    return true;
}

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/