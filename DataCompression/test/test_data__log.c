/*============================================================================
@brief A C source template
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
#include "unity.h"
#include "data__log.h"
#include <stdint.h>

/*----------------------------------------------------------------------------
  manifest constants
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  type definitions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  prototypes
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  static variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  public functions
----------------------------------------------------------------------------*/

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void setUp(void)
{
}

void tearDown(void)
{
}

void test_data__log_CheckHeaderLength(void)
{
    int header_len = sizeof( uint8_t ) * LOG_DATA_BEGIN_MARKER_LEN +    // marker bytes
                     sizeof( uint32_t ) +                               // Unix timestamp bytes
                     sizeof( uint8_t )  +                               // log type bytes
                     sizeof( uint8_t );                                 // payload length bytes
    TEST_ASSERT_EQUAL_INT( header_len , sizeof( data__log_header_t ) );
}

void test_data__log_RawAdcPayloadLength(void)
{
    int payload_len = sizeof( uint8_t ) +                               // sample_count
                      sizeof( uint16_t ) * MAX_ADC_SAMPLE_COUNT;        // payload length bytes
    TEST_ASSERT_EQUAL_INT( payload_len , data__log_get_payload_len( data__log_type_raw_adc ) );
}

void test_data__log_CalibrationLedPayloadLength(void)
{
    int payload_len = sizeof( uint8_t ) +           // event_type | pga_level
                      sizeof( uint16_t ) +          // raw_value
                      sizeof( uint8_t );            // current
    TEST_ASSERT_EQUAL_INT( payload_len , data__log_get_payload_len( data__log_type_cal_led ) );
}

void test_data__log_TemperaturePayloadLength(void)
{
    int payload_len = sizeof( int8_t );            // value
    TEST_ASSERT_EQUAL_INT( payload_len , data__log_get_payload_len( data__log_type_temperature ) );
}

void test_data__log_InitialiseRawAdcPacket(void)
{
    uint32_t timestamp = 0x12345678;
    uint16_t raw_adc [ 10 ] = { 0x1111 , 0x2222 , 0x3333 , 0x4444 , 0x5555 ,
                                0x6666 , 0x7777 , 0x8888 , 0x9999 , 0xAAAA };
    uint8_t sample_count = 10;
    data__log_packet_t log_packet = data__log_prepare_raw_adc_packet( timestamp , raw_adc , sample_count );
    
    TEST_ASSERT_EQUAL_MEMORY( LOG_DATA_BEGIN_MARKER , log_packet.header.log_begin , LOG_DATA_BEGIN_MARKER_LEN );
    TEST_ASSERT_EQUAL_UINT32( timestamp , log_packet.header.timestamp );
    TEST_ASSERT_EQUAL_UINT8( (uint8_t) data__log_type_raw_adc , log_packet.header.log_type );
    TEST_ASSERT_EQUAL_UINT8( data__log_get_payload_len( data__log_type_raw_adc ) , log_packet.header.payload_len );
}
/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/