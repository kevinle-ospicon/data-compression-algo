/*============================================================================
@brief A C source header template
------------------------------------------------------------------------------
<!-- Written by Kevin (Phuc) Le Dinh -->
<!-- Copyright (C) 2018 All rights reserved -->
============================================================================*/

#ifndef _DATA__LOG_H
#define _DATA__LOG_H

/*----------------------------------------------------------------------------
  @brief
    This file contain definitions of log data for binary encoding scheme
    For more info on the binary encoding scheme, see:
        https://docs.google.com/document/d/19ZK3PVEwRuw18u7qEBL89JhddKySXR0NnTo1SuOWa60/edit?usp=sharing
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  nested include files
----------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/
#define MAX_ADC_SAMPLE_COUNT    10
#define LOG_DATA_BEGIN_MARKER   "\r\n>>"
#define LOG_DATA_BEGIN_MARKER_LEN   4

/*----------------------------------------------------------------------------
  manifest constants
----------------------------------------------------------------------------*/
enum data__log_type_e
{
    data__log_type_raw_adc = 0x00,
    data__log_type_cal_led,
    data__log_type_temperature,
    
    data__log_type_number_of
};

enum data__log_cal_led_event_e
{
    data__log_cal_led_event_on_mat,
    data__log_cal_led_event_on_mat_moving,
    data__log_cal_led_event_uncertain,
    
    data__log_cal_led_event_number_of
};

/*----------------------------------------------------------------------------
  type definitions
----------------------------------------------------------------------------*/

/* Log data header (excluding the begin marker) definitions */

typedef struct data__log_header_s
{
    uint8_t log_begin[ LOG_DATA_BEGIN_MARKER_LEN ];
    uint32_t timestamp;
    uint8_t log_type;
    uint8_t payload_len;
} __attribute__((packed)) data__log_header_t;

/* Log data header definitions */

/* Log data payload definitions */

typedef struct data__log_raw_adc_payload_s
{
    uint8_t sample_count;
    uint16_t value[ MAX_ADC_SAMPLE_COUNT ];
} __attribute__((packed)) data__log_raw_adc_payload_t;

typedef struct data__log_cal_led_payload_s
{
    uint8_t event_type : 4;
    uint8_t pga_level : 4;
    uint16_t raw_value;
    uint8_t current;
} __attribute__((packed)) data__log_cal_led_payload_t;

typedef struct data__log_temperature_payload_s
{
    int8_t value;
} __attribute__((packed)) data__log_temperature_payload_t;

/* End of log data payload definitions */

/* Log data packet definition */

typedef union data__log_packet_s
{
    struct __attribute__((packed))
    {
        data__log_header_t header;        
        union __attribute__((packed))
        {
            data__log_raw_adc_payload_t raw_adc_payload;
            data__log_cal_led_payload_t cal_led_payload;
            data__log_temperature_payload_t temperature_payload;
            uint8_t payload[ 1 ];
        };
    };
    uint8_t img[ 1 ];
    
} __attribute__((packed)) data__log_packet_t;

/* End of log data packet definition */

/*----------------------------------------------------------------------------
  extern variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  prototypes
----------------------------------------------------------------------------*/
uint8_t data__log_get_packet_len( enum data__log_type_e log_type );
data__log_packet_t data__log_prepare_packet( uint32_t timestamp , enum data__log_type_e log_type , uint8_t * payload_ptr );

/*----------------------------------------------------------------------------
  compile time checks
----------------------------------------------------------------------------*/

#endif // _DATA__LOG_H

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/


