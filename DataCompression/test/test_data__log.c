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
static uint8_t get_header_size(void);

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

void test_data__log_RawAdcPacketLength(void)
{
    int packet_len = sizeof( uint8_t ) +                               // sample_count
                      sizeof( uint16_t ) * MAX_ADC_SAMPLE_COUNT +      // payload length bytes
                      get_header_size();
    TEST_ASSERT_EQUAL_INT( packet_len , data__log_get_packet_len( data__log_type_raw_adc ) );
}

void test_data__log_CalibrationLedPacketLength(void)
{
    int packet_len = sizeof( uint8_t ) +           // pga_level
                      sizeof( uint16_t ) +          // raw_value
                      sizeof( uint8_t ) +            // current
                      sizeof( uint32_t ) +            // timestamp
                      get_header_size();
    TEST_ASSERT_EQUAL_INT( packet_len , data__log_get_packet_len( data__log_type_cal ) );
}

void test_data__log_TimestampPacketLength(void)
{
    int packet_len = sizeof( uint32_t ) +             // value
                     get_header_size();
    TEST_ASSERT_EQUAL_INT( packet_len , data__log_get_packet_len( data__log_type_timestamp ) );
}

void test_data__log_CheckHeaderLength(void)
{
    TEST_ASSERT_EQUAL_INT( get_header_size() , sizeof( data__log_header_t ) );
}

void test_data__log_CheckPacketheader(void)
{
    uint8_t sample_count = 10;
    data__log_raw_adc_payload_t raw_adc_payload = 
    { 
        sample_count,
        { 0x1111 , 0x2222 , 0x3333 , 0x4444 , 0x5555 , 0x6666 , 0x7777 , 0x8888 , 0x9999 , 0xAAAA }
    };
    data__log_packet_t log_packet = data__log_prepare_packet( data__log_type_raw_adc , (uint8_t *) & raw_adc_payload );
    
    TEST_ASSERT_EQUAL_MEMORY( LOG_DATA_BEGIN_MARKER , log_packet.header.log_begin , LOG_DATA_BEGIN_MARKER_LEN );
    TEST_ASSERT_EQUAL_UINT8( (uint8_t) data__log_type_raw_adc , log_packet.header.log_type );
    TEST_ASSERT_EQUAL_UINT8( sizeof( data__log_raw_adc_payload_t ) , log_packet.header.payload_len );

    TEST_ASSERT_EQUAL_UINT8( raw_adc_payload.sample_count , log_packet.raw_adc_payload.sample_count );
    TEST_ASSERT_EQUAL_MEMORY( raw_adc_payload.value , log_packet.raw_adc_payload.value , sizeof( raw_adc_payload.value ) );
}

void test_data__log_CheckNullPayloadPointer(void)
{
    data__log_packet_t log_packet = data__log_prepare_packet( data__log_type_raw_adc , NULL );
}

void test_data__log_CheckCalibrationLedPacket(void)
{
    data__log_cal_payload_t cal_led_payload = 
    {
        data__log_cal_pga_lvl_1,
        12345,
        100
    };
    data__log_packet_t log_packet = data__log_prepare_packet( data__log_type_cal , (uint8_t *) & cal_led_payload );
    
    TEST_ASSERT_EQUAL_UINT8( data__log_cal_pga_lvl_1 , log_packet.cal_payload.pga_level );
    TEST_ASSERT_EQUAL_UINT16( 12345 , log_packet.cal_payload.raw_value );
    TEST_ASSERT_EQUAL_UINT8( 100 , log_packet.cal_payload.current );
}

void test_data__log_CheckTimestampPacket(void)
{
    data__log_timestamp_payload_t timestamp_payload = 
    {
        0x12345678
    };
    data__log_packet_t log_packet = data__log_prepare_packet( data__log_type_timestamp , (uint8_t *) & timestamp_payload );
    
    TEST_ASSERT_EQUAL_INT8( 0x12345678 , log_packet.timestamp_payload.value );
}
/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/
static uint8_t get_header_size(void)
{
    return sizeof( uint8_t ) * LOG_DATA_BEGIN_MARKER_LEN +
           sizeof( uint8_t ) + 
           sizeof( uint8_t );
}

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/