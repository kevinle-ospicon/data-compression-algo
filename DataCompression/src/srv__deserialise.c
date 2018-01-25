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
#include <stdio.h>

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

enum srv__deserialise_raw_adc_payload_state
{
    srv__deserialise_raw_adc_payload_state_sample_number,
    srv__deserialise_raw_adc_payload_state_value
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
    enum srv__deserialise_raw_adc_payload_state raw_adc_payload_state;
    bool valid_packet;
    uint8_t raw_adc_payload_idx;
    char packet_string[ SRV_DESERIALISE_MAX_STRING_LEN ];
} srv__deserialise_context_t;

typedef bool ( * srv__deserialise_parse_payload_cb_t ) ( srv__deserialise_context_t * ctx , uint8_t byte_value );

/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/
#define SRV_DESERIALISE_TIMESTAMP_FORMAT "%Y%m%d_%H:%M:%S:"

/*----------------------------------------------------------------------------
  prototypes
----------------------------------------------------------------------------*/
static void srv__deserialise_detect_begin_marker( srv__deserialise_context_t * ctx , uint8_t byte_value );
static void srv__deserialise_parse_header( srv__deserialise_context_t * ctx , uint8_t byte_value );
static bool srv__deserialise_parse_raw_adc_payload( srv__deserialise_context_t * ctx , uint8_t byte_value );
static bool srv__deserialise_parse_calibration_payload( srv__deserialise_context_t * ctx , uint8_t byte_value );
static bool srv__deserialise_parse_temperature_payload( srv__deserialise_context_t * ctx , uint8_t byte_value );
static int srv__deserialise_get_timestamp_ascii( srv__deserialise_context_t * ctx );
static int srv__deserialise_get_log_type_ascii( char * buf , enum data__log_type_e log_type );
static int srv__deserialise_get_payload_value_ascii( char * buf , data__log_packet_t * log_packet , enum data__log_type_e log_type );
static int srv__deserialise_get_cal_payload_value_ascii( char * buf , data__log_packet_t * log_packet );

/*----------------------------------------------------------------------------
  global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  static variables
----------------------------------------------------------------------------*/
static srv__deserialise_context_t srv__deserialise_context;
static const srv__deserialise_parse_payload_cb_t parse_payload_cb[ data__log_type_number_of ] = 
{
    srv__deserialise_parse_raw_adc_payload,
    srv__deserialise_parse_calibration_payload,
    srv__deserialise_parse_temperature_payload,
};

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
    srv__deserialise_context.raw_adc_payload_state = srv__deserialise_raw_adc_payload_state_sample_number;
    srv__deserialise_context.valid_packet = false;
    srv__deserialise_context.raw_adc_payload_idx = 0;
    memset( & srv__deserialise_context.log_packet , 0 , data__log_get_packet_len( data__log_type_raw_adc ) );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
bool srv__deserialise_parse( uint8_t byte_value )
{
    if( srv__deserialise_context.state < srv__deserialise_state_timestamp )
    {
        //We're detecting the begin marker
        srv__deserialise_detect_begin_marker( & srv__deserialise_context , byte_value );
    }
    else if( srv__deserialise_context.state < srv__deserialise_state_payload )
    {
        //We're getting the header
        srv__deserialise_parse_header( & srv__deserialise_context , byte_value );
    }
    else
    {
        //We're getting the payload
        uint8_t log_type = srv__deserialise_context.log_packet.header.log_type;
        if( log_type < data__log_type_number_of )
        {
            srv__deserialise_context.valid_packet = parse_payload_cb[ log_type ]( & srv__deserialise_context , byte_value );
        }
    }
    return srv__deserialise_context.valid_packet;
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

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
char * srv__deserialise_get_log_packet_line( uint8_t * size )
{
    int size_so_far;
    char * buf = srv__deserialise_context.packet_string;
    enum data__log_type_e log_type = srv__deserialise_context.log_packet.header.log_type;

    memset( srv__deserialise_context.packet_string , 0 , SRV_DESERIALISE_MAX_STRING_LEN );
    size_so_far = srv__deserialise_get_timestamp_ascii( & srv__deserialise_context );
    size_so_far += srv__deserialise_get_log_type_ascii( & buf[ size_so_far ] , log_type );
    size_so_far += srv__deserialise_get_payload_value_ascii( & buf[ size_so_far ] , & srv__deserialise_context.log_packet , log_type );
    
    * size = size_so_far;
    return srv__deserialise_context.packet_string;
}
/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void srv__deserialise_detect_begin_marker( srv__deserialise_context_t * ctx , uint8_t byte_value )
{
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
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void srv__deserialise_parse_header( srv__deserialise_context_t * ctx , uint8_t byte_value )
{
    switch( ctx->state )
    {
        case srv__deserialise_state_timestamp:
            ctx->log_packet.header.timestamp |= utils__shift_byte_left( byte_value , ctx->dummy_count );
            if( ( ++ ctx->dummy_count ) >= LOG_DATA_TIMESTAMP_LEN )
            {
                ctx->dummy_count = 0;
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
            break;
        default:
            ctx->state = srv__deserialise_state_CR;
            break;
    }
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static bool srv__deserialise_parse_raw_adc_payload( srv__deserialise_context_t * ctx , uint8_t byte_value )
{
    bool status = false;
    switch( ctx->raw_adc_payload_state )
    {
        case srv__deserialise_raw_adc_payload_state_sample_number:
            ctx->log_packet.raw_adc_payload.sample_count = byte_value;
            ctx->raw_adc_payload_state = srv__deserialise_raw_adc_payload_state_value;
            break;
        case srv__deserialise_raw_adc_payload_state_value:
            ctx->log_packet.raw_adc_payload.value[ ctx->raw_adc_payload_idx ] |= (uint16_t) ( utils__shift_byte_left( byte_value , ctx->dummy_count ) & 0xFFFF );
            if( ( ++ ctx->dummy_count ) >= LOG_DATA_RAW_ADC_SIZE_BYTES )
            {
                ctx->dummy_count = 0;
                if( ++ ctx->raw_adc_payload_idx >= ctx->log_packet.raw_adc_payload.sample_count )
                {
                    status = true;
                    ctx->state = srv__deserialise_state_CR;
                }
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
                ctx->dummy_count = 0;
                ctx->cal_payload_state = srv__deserialise_cal_payload_state_current;
            }
            break;
        case srv__deserialise_cal_payload_state_current:
            ctx->log_packet.cal_payload.current = byte_value;
            status = true;
            ctx->state = srv__deserialise_state_CR;
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
static bool srv__deserialise_parse_temperature_payload( srv__deserialise_context_t * ctx , uint8_t byte_value )
{
    ctx->log_packet.temperature_payload.value = ( int8_t ) byte_value;
    ctx->state = srv__deserialise_state_CR;
    return true;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static int srv__deserialise_get_timestamp_ascii( srv__deserialise_context_t * ctx )
{
    uint32_t timestamp = ctx->log_packet.header.timestamp;
    struct tm timeinfo = utils__convert_epoch_to_calendar_time( timestamp );
    return strftime( ctx->packet_string , SRV_DESERIALISE_MAX_STRING_LEN , SRV_DESERIALISE_TIMESTAMP_FORMAT , & timeinfo);
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static int srv__deserialise_get_log_type_ascii( char * buf , enum data__log_type_e log_type )
{
    int return_val = 0;
    switch( log_type )
    {
        case data__log_type_raw_adc:
            return_val = sprintf( buf , "%s:" , LOG_DATA_TYPE_RAW_ADC );
            break;
        case data__log_type_cal:
            return_val = sprintf( buf , "%s:" , LOG_DATA_TYPE_CAL );
            break;
        case data__log_type_temperature:
            return_val = sprintf( buf , "%s:" , LOG_DATA_TYPE_TEMPERATURE );
            break;
        default:
            break;
    }

    return return_val;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static int srv__deserialise_get_payload_value_ascii( char * buf , data__log_packet_t * log_packet , enum data__log_type_e log_type )
{
    int return_val = 0;
    switch( log_type )
    {
        case data__log_type_raw_adc:
            return_val = sprintf( buf , "%u\r\n" , log_packet->raw_adc_payload.value[ 0 ] );
            break;
        case data__log_type_cal:
            return_val = srv__deserialise_get_cal_payload_value_ascii( buf , log_packet );
            break;
        case data__log_type_temperature:
            return_val = sprintf( buf , "%d\r\n" , log_packet->temperature_payload.value );
            break;
        default:
            break;
    }
    return return_val;
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static int srv__deserialise_get_cal_payload_value_ascii( char * buf , data__log_packet_t * log_packet )
{
    int return_val = 0;
    if( log_packet->cal_payload.pga_level == data__log_cal_pga_lvl_single_led )
    {
        return_val += sprintf( buf , " %s," , LOG_DATA_CAL_SINGLE_LED );
    }
    else if( log_packet->cal_payload.pga_level == data__log_cal_pga_lvl_1 )
    {
        return_val += sprintf( buf , " %s," , LOG_DATA_CAL_PGA_1 );
    }
    else if( log_packet->cal_payload.pga_level == data__log_cal_pga_lvl_2 )
    {
        return_val += sprintf( buf , " %s," , LOG_DATA_CAL_PGA_2 );
    }
    else if( log_packet->cal_payload.pga_level == data__log_cal_pga_lvl_4 )
    {
        return_val += sprintf( buf , " %s," , LOG_DATA_CAL_PGA_4 );
    }
    return_val += sprintf( buf + return_val , " %.1f mA," , ( float ) log_packet->cal_payload.current / 10.0 );
    return_val += sprintf( buf + return_val , " %u\r\n" , log_packet->cal_payload.raw_value );
    
    return return_val;
}
/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/