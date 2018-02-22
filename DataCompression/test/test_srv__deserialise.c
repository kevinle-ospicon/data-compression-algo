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
#include "srv__deserialise.h"
#include "data__log.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*----------------------------------------------------------------------------
  manifest constants
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  type definitions
----------------------------------------------------------------------------*/
typedef void ( * test_payload_cb_t ) ( data__log_packet_t * log_packet );

/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/
#define RAW_ADC_SINGLE_ASCII_SAMPLE "Raw:40885\r\n"
#define RAW_ADC_ASCII_SAMPLE "Raw:40885\r\n \
Raw:40487\r\n \
Raw:40431\r\n \
Raw:40216\r\n \
Raw:40027\r\n \
Raw:39880\r\n \
Raw:39800\r\n \
Raw:39823\r\n \
Raw:40098\r\n \
Raw:40350\r\n"
#define CAL_SINGLE_LED_ASCII_SAMPLE "Calibration finish: Single LED, 15.0 mA, 50864\r\n"
#define CAL_PGA_1_ASCII_SAMPLE "Calibration finish: PGA1, 15.0 mA, 50864\r\n"
#define CAL_PGA_2_ASCII_SAMPLE "Calibration finish: PGA2, 15.0 mA, 50864\r\n"
#define CAL_PGA_4_ASCII_SAMPLE "Calibration finish: PGA4, 15.0 mA, 50864\r\n"


/*----------------------------------------------------------------------------
  prototypes
----------------------------------------------------------------------------*/
static void test_raw_adc_payload( data__log_packet_t * log_packet );
static void test_calibraion_payload( data__log_packet_t * log_packet );
static void test_timestamp_payload( data__log_packet_t * log_packet );
static void test_cal_payload_ascii( char * expected );

/*----------------------------------------------------------------------------
  global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  static variables
----------------------------------------------------------------------------*/
//20180119_17:01:01:Temp:23
//20180119_17:01:23:Calibration finish: Single LED, 15.0 mA, 50864

static char log_str[ SRV_DESERIALISE_MAX_STRING_LEN ] = "not empty";

static uint8_t wrong_begin_marker[] = { 0x0D , 0x0A , 0x3E , 0x00 };
static uint8_t begin_marker_wrong_LF[] = { 0x0D , 0x00 , 0x0A , 0x3E , 0x3E };
static uint8_t begin_marker_wrong_begin_1[] = { 0x0D , 0x0A , 0x00 , 0x3E , 0x3E };
static uint8_t begin_marker_wrong_begin_2[] = { 0x0D , 0x0A , 0x3E , 0x00 , 0x3E };

static uint8_t correct_header[] = 
{ 
    0x0D , 0x0A , 0x3E , 0x3E , 0x01 , 0x08 , 0x00 , 0xB0 , 0xC6 , 0x96 , 0x12 , 0x34 , 0x56 , 0x78
};

static uint8_t cal_packet[] = 
{ 
    0x0D , 0x0A , 0x3E , 0x3E , 0x01 , 0x08 , 0x00 , 0xB0 , 0xC6 , 0x96 , 0x12 , 0x34 , 0x56 , 0x78
};

static uint8_t cal_packet_pga_1[] = 
{ 
    0x0D , 0x0A , 0x3E , 0x3E , 0x01 , 0x08 , 0x01 , 0xB0 , 0xC6 , 0x96 , 0x12 , 0x34 , 0x56 , 0x78
};

static uint8_t cal_packet_pga_2[] = 
{ 
    0x0D , 0x0A , 0x3E , 0x3E , 0x01 , 0x08 , 0x02 , 0xB0 , 0xC6 , 0x96 , 0x12 , 0x34 , 0x56 , 0x78
};

static uint8_t cal_packet_pga_4[] = 
{ 
    0x0D , 0x0A , 0x3E , 0x3E , 0x01 , 0x08 , 0x03 , 0xB0 , 0xC6 , 0x96 , 0x12 , 0x34 , 0x56 , 0x78
};

static uint8_t raw_adc_packet[] = 
{ 
    0x0D , 0x0A , 0x3E , 0x3E , 0x00 , 0x15 , 
    0x0A , 
    0xB5 , 0x9F , 0x27 , 0x9E , 0xEF , 0x9D , 0x18 , 0x9D , 0x5B , 0x9C , 
    0xC8 , 0x9B , 0x78 , 0x9B , 0x8F , 0x9B , 0xA2 , 0x9C , 0x9E , 0x9D  
};

static uint8_t raw_adc_single_packet[] = 
{ 
    0x0D , 0x0A , 0x3E , 0x3E , 0x00 , 0x15 , 
    0x01 , 
    0xB5 , 0x9F , 0x27 , 0x9E , 0xEF , 0x9D , 0x18 , 0x9D , 0x5B , 0x9C , 
    0xC8 , 0x9B , 0x78 , 0x9B , 0x8F , 0x9B , 0xA2 , 0x9C , 0x9E , 0x9D 
};

static uint8_t temp_cal_raw_packets[] = 
{ 
    // calibration packet
    0x0D , 0x0A , 0x3E , 0x3E , 0x01 , 0x08 , 
    0x00 , 0xB0 , 0xC6 , 0x96 , 0x12 , 0x34 , 0x56 , 0x78 ,
    // raw adc packet 
    0x0D , 0x0A , 0x3E , 0x3E , 0x00 , 0x15 , 
    0x0A , 
    0xB5 , 0x9F , 0x27 , 0x9E , 0xEF , 0x9D , 0x18 , 0x9D , 0x5B , 0x9C , 
    0xC8 , 0x9B , 0x78 , 0x9B , 0x8F , 0x9B , 0xA2 , 0x9C , 0x9E , 0x9D 
};

static uint8_t mixed_packets[] = 
{ 
    // calibration packet
    0x0D , 0x0A , 0x3E , 0x3E , 0x01 , 0x08 , 
    0x00 , 0xB0 , 0xC6 , 0x96 , 0x12 , 0x34 , 0x56 , 0x78 ,
    // raw adc packet 
    0x0D , 0x0A , 0x3E , 0x3E , 0x00 , 0x15 , 
    0x0A , 
    0xB5 , 0x9F , 0x27 , 0x9E , 0xEF , 0x9D , 0x18 , 0x9D , 0x5B , 0x9C , 
    0xC8 , 0x9B , 0x78 , 0x9B , 0x8F , 0x9B , 0xA2 , 0x9C , 0x9E , 0x9D
};

static const test_payload_cb_t test_payload_cb [ data__log_type_number_of ] = 
{
    test_raw_adc_payload,
    test_calibraion_payload,
    test_timestamp_payload
};

static char * raw_adc_packet_ascii_sample[] = 
{
    "Raw:40885\r\n",
    "Raw:40487\r\n",
    "Raw:40431\r\n",
    "Raw:40216\r\n",
    "Raw:40027\r\n",
    "Raw:39880\r\n",
    "Raw:39800\r\n",
    "Raw:39823\r\n",
    "Raw:40098\r\n",
    "Raw:40350\r\n"
};

static char * mixed_ascii_sample[] = 
{
    CAL_SINGLE_LED_ASCII_SAMPLE,
    "Raw:40885\r\n",
    "Raw:40487\r\n",
    "Raw:40431\r\n",
    "Raw:40216\r\n",
    "Raw:40027\r\n",
    "Raw:39880\r\n",
    "Raw:39800\r\n",
    "Raw:39823\r\n",
    "Raw:40098\r\n",
    "Raw:40350\r\n",
};
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
    srv__deserialise_init();
}

void tearDown(void)
{
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_DetectWrongBeginMarker(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( wrong_begin_marker ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( wrong_begin_marker[ idx ] );
    }

    TEST_ASSERT_FALSE( parse_result );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_DetectWrongBeginMarkerWithEmptyLF(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( begin_marker_wrong_LF ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( begin_marker_wrong_LF[ idx ] );
    }

    TEST_ASSERT_FALSE( parse_result );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_DetectWrongBeginMarkerWithEmptyBegin1(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( begin_marker_wrong_begin_1 ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( begin_marker_wrong_begin_1[ idx ] );
    }

    TEST_ASSERT_FALSE( parse_result );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_DetectWrongBeginMarkerWithEmptyBegin2(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( begin_marker_wrong_begin_2 ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( begin_marker_wrong_begin_2[ idx ] );
    }
    TEST_ASSERT_FALSE( parse_result );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_DetectCorrectHeader(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( correct_header ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( correct_header[ idx ] );
    }
    
    TEST_ASSERT_TRUE( parse_result );

    data__log_packet_t log_packet = srv__deserialise_get_log_packet();

    TEST_ASSERT_EQUAL_UINT8( ( uint8_t ) data__log_type_cal , log_packet.header.log_type );
    TEST_ASSERT_EQUAL_UINT8( sizeof( data__log_cal_payload_t ) , log_packet.header.payload_len );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_GetCalibrationPacket(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( cal_packet ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( cal_packet[ idx ] );
    }

    TEST_ASSERT_TRUE( parse_result );
    
    data__log_packet_t log_packet = srv__deserialise_get_log_packet();

    test_payload_cb[ data__log_type_cal ]( & log_packet );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_GetRawAdcPacket(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( raw_adc_packet ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( raw_adc_packet[ idx ] );
    }

    TEST_ASSERT_TRUE( parse_result );

    data__log_packet_t log_packet = srv__deserialise_get_log_packet();

    test_payload_cb[ data__log_type_raw_adc ]( & log_packet );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_GetMixedackets(void)
{
    for( int idx = 0 ; idx < sizeof( temp_cal_raw_packets ) ; idx ++ )
    {
        if( srv__deserialise_parse( temp_cal_raw_packets[ idx ] ) )
        {
            data__log_packet_t log_packet = srv__deserialise_get_log_packet();

            test_payload_cb[ log_packet.header.log_type ]( & log_packet );
        }
    }
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_GetStringFromCalibrationBinPacket(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( cal_packet ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( cal_packet[ idx ] );
    }
    TEST_ASSERT_TRUE( parse_result );
    test_cal_payload_ascii( CAL_SINGLE_LED_ASCII_SAMPLE );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_GetStringFromCalibrationBinPacketWithPga1(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( cal_packet_pga_1 ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( cal_packet_pga_1[ idx ] );
    }
    TEST_ASSERT_TRUE( parse_result );
    test_cal_payload_ascii( CAL_PGA_1_ASCII_SAMPLE );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_GetStringFromCalibrationBinPacketWithPga2(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( cal_packet_pga_2 ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( cal_packet_pga_2[ idx ] );
    }
    TEST_ASSERT_TRUE( parse_result );
    test_cal_payload_ascii( CAL_PGA_2_ASCII_SAMPLE );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_GetStringFromCalibrationBinPacketWithPga4(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( cal_packet_pga_4 ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( cal_packet_pga_4[ idx ] );
    }
    TEST_ASSERT_TRUE( parse_result );
    test_cal_payload_ascii( CAL_PGA_4_ASCII_SAMPLE );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_GetStringFromRawAdcBinPacketWithSingleSampleToAscii(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( raw_adc_single_packet ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( raw_adc_single_packet[ idx ] );
        if( parse_result ) break;
    }

    test_cal_payload_ascii( RAW_ADC_SINGLE_ASCII_SAMPLE );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_GetStringFromRawAdcBinPacketWithMultipleSamplesToAscii(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( raw_adc_packet ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( raw_adc_packet[ idx ] );
        if( parse_result ) break;
    }

    int total_lines = srv__deserialise_get_pending_raw_adc_lines();
    for( int count = 0 ; count < total_lines ; count ++ )
    {
        test_cal_payload_ascii( raw_adc_packet_ascii_sample[ count ] );
    }
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_GetStringFromMixedPacketsWithToAscii(void)
{
    bool parse_result = false;
    int total_adc_lines = 0;
    int count = 0;
    for( int idx = 0 ; idx < sizeof( mixed_packets ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( mixed_packets[ idx ] );
        if( parse_result )
        {
            total_adc_lines = srv__deserialise_get_pending_raw_adc_lines();
            printf("count: %d\r\n" , count);
            printf("total_adc_lines: %d\r\n" , total_adc_lines);
            printf("expected: %s\r\n" ,  mixed_ascii_sample[ count ]);
            if( total_adc_lines > 0 )
            {
                for( int idx = 0 ; idx < total_adc_lines ; idx ++ )
                {
                    test_cal_payload_ascii( mixed_ascii_sample[ count ++ ] );
                }
            }
            else
            {
                test_cal_payload_ascii( mixed_ascii_sample[ count ++ ] );
            }
        }
    }
}

/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void test_raw_adc_payload( data__log_packet_t * log_packet )
{
    TEST_ASSERT_EQUAL_UINT8( ( uint8_t ) data__log_type_raw_adc , log_packet->header.log_type );
    TEST_ASSERT_EQUAL_UINT8( sizeof( data__log_raw_adc_payload_t ) , log_packet->header.payload_len );

    TEST_ASSERT_EQUAL_UINT8( MAX_ADC_SAMPLE_COUNT , log_packet->raw_adc_payload.sample_count );
    TEST_ASSERT_EQUAL_MEMORY( & raw_adc_packet[ sizeof( data__log_header_t ) + 1 ] , log_packet->raw_adc_payload.value , sizeof( data__log_raw_adc_payload_t ) - 1 );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void test_calibraion_payload( data__log_packet_t * log_packet )
{
    TEST_ASSERT_EQUAL_UINT8( ( uint8_t ) data__log_type_cal , log_packet->header.log_type );
    TEST_ASSERT_EQUAL_UINT8( sizeof( data__log_cal_payload_t ) , log_packet->header.payload_len );

    TEST_ASSERT_EQUAL_UINT8( data__log_cal_pga_lvl_single_led , log_packet->cal_payload.pga_level );
    TEST_ASSERT_EQUAL_UINT16( 50864 , log_packet->cal_payload.raw_value );
    TEST_ASSERT_EQUAL_UINT8( 150 , log_packet->cal_payload.current );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void test_timestamp_payload( data__log_packet_t * log_packet )
{}
/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void test_cal_payload_ascii( char * expected )
{
    uint8_t str_len = 0;
    char * test_str = srv__deserialise_get_log_packet_line( & str_len );
    printf( "actual: %s" , test_str );
    TEST_ASSERT_NOT_NULL( test_str );
    TEST_ASSERT_EQUAL_UINT8( str_len , ( uint8_t ) strlen( test_str ) );
    TEST_ASSERT_EQUAL_STRING( expected , test_str );
}
/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/