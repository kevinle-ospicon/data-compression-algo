/*============================================================================
@brief A C source to run unit test for log data serialisation service
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
#include "hw__log_io.h"
#include "data__log.h"
#include "dev__log_handler.h"
#include "srv__serialise.h"
#include "utils/utils.h"
#include <string.h>
#include <time.h>

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
static void read_test_data_from_fw(void);

/*----------------------------------------------------------------------------
  global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  static variables
----------------------------------------------------------------------------*/
static data__log_packet_t log_packet;
static char * temp_str = "20180119_17:01:01:Temp:22,9\r\n";
static char * raw_adc_str = "20180119_17:01:01:Raw:65535\r\n";
static char * cal_str_single_led = "20180119_17:01:23:Calibration finish: Single LED, 15.0 mA, 50864\r\n";
static char * cal_str_pga_1 = "20180119_17:01:23:Calibration finish: PGA1, 15.0 mA, 50864\r\n";
static char * cal_str_pga_2 = "20180119_17:01:23:Calibration finish: PGA2, 15.0 mA, 50864\r\n";
static char * cal_str_pga_4 = "20180119_17:01:23:Calibration finish: PGA4, 15.0 mA, 50864\r\n";

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
	hw__log_io_init();
    dev__log_handler_init_log_data();
    memset( & log_packet , 0 , data__log_get_packet_len( data__log_type_raw_adc ) );
}

void tearDown(void)
{
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetEpochTime(void)
{
    srv__serialise_to_bin( temp_str , strlen( temp_str ) );
    read_test_data_from_fw();
    TEST_ASSERT_EQUAL_UINT32( 1516381261 , log_packet.header.timestamp );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetTemperatureType(void)
{
    srv__serialise_to_bin( temp_str , strlen( temp_str ) );
    read_test_data_from_fw();
    TEST_ASSERT_EQUAL_UINT8( data__log_type_temperature , log_packet.header.log_type );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetTemperatureValue(void)
{
    srv__serialise_to_bin( temp_str , strlen( temp_str ) );
    read_test_data_from_fw();
    TEST_ASSERT_EQUAL_UINT8( 23 , log_packet.temperature_payload.value );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetRawAdcType(void)
{
    log_packet.header.log_type = data__log_type_temperature;
    srv__serialise_to_bin( raw_adc_str , strlen( raw_adc_str ) );
    srv__serialise_commit_all();
    read_test_data_from_fw();
    TEST_ASSERT_EQUAL_UINT8( data__log_type_raw_adc , log_packet.header.log_type );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetRawAdcSingleValue(void)
{
    srv__serialise_to_bin( raw_adc_str , strlen( raw_adc_str ) );
    srv__serialise_commit_all();
    read_test_data_from_fw();
    TEST_ASSERT_EQUAL_UINT16( 65535 , log_packet.raw_adc_payload.value[ 0 ] );
    TEST_ASSERT_EQUAL_UINT8( 1 , log_packet.raw_adc_payload.sample_count );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetRawAdcMultipleValuesWithinBound(void)
{
    srv__serialise_to_bin( raw_adc_str , strlen( raw_adc_str ) );
    srv__serialise_to_bin( raw_adc_str , strlen( raw_adc_str ) );
    srv__serialise_to_bin( raw_adc_str , strlen( raw_adc_str ) );
    srv__serialise_to_bin( raw_adc_str , strlen( raw_adc_str ) );
    srv__serialise_to_bin( raw_adc_str , strlen( raw_adc_str ) );
    srv__serialise_to_bin( raw_adc_str , strlen( raw_adc_str ) );
    srv__serialise_commit_all();
    read_test_data_from_fw();
    uint16_t test_array[ 6 ] = { 65535 , 65535 , 65535 , 65535 , 65535 , 65535 };
    TEST_ASSERT_EQUAL_UINT16_ARRAY( test_array , log_packet.raw_adc_payload.value , 6 );
    TEST_ASSERT_EQUAL_UINT8( 6 , log_packet.raw_adc_payload.sample_count );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetRawAdcMultipleValuesOufOfBoundReturnMaxElement(void)
{
    uint16_t test_array[ MAX_ADC_SAMPLE_COUNT + 1 ] ;
	for( int idx = 0 ; idx < MAX_ADC_SAMPLE_COUNT + 1 ; idx ++ )
	{
		srv__serialise_to_bin( raw_adc_str , strlen( raw_adc_str ) );
		test_array[ idx ] = 65535;
	}
    read_test_data_from_fw();

    TEST_ASSERT_EQUAL_UINT16_ARRAY( test_array , log_packet.raw_adc_payload.value , MAX_ADC_SAMPLE_COUNT );
    TEST_ASSERT_EQUAL_UINT8( MAX_ADC_SAMPLE_COUNT , log_packet.raw_adc_payload.sample_count );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetCalibrationType(void)
{
    srv__serialise_to_bin( cal_str_single_led , strlen( cal_str_single_led ) );
    read_test_data_from_fw();
    TEST_ASSERT_EQUAL_UINT8( data__log_type_cal , log_packet.header.log_type );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetCalibrationPgaLvlSingleLed(void)
{
	log_packet.cal_payload.pga_level = data__log_cal_pga_lvl_2;
    srv__serialise_to_bin( cal_str_single_led , strlen( cal_str_single_led ) );
    read_test_data_from_fw();
    TEST_ASSERT_EQUAL_UINT8( data__log_cal_pga_lvl_single_led , log_packet.cal_payload.pga_level );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetCalibrationPgaLvlPga1(void)
{
    srv__serialise_to_bin( cal_str_pga_1 , strlen( cal_str_pga_1 ) );
    read_test_data_from_fw();
    TEST_ASSERT_EQUAL_UINT8( data__log_cal_pga_lvl_1 , log_packet.cal_payload.pga_level );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetCalibrationPgaLvlPga2(void)
{
    srv__serialise_to_bin( cal_str_pga_2 , strlen( cal_str_pga_2 ) );
    read_test_data_from_fw();
    TEST_ASSERT_EQUAL_UINT8( data__log_cal_pga_lvl_2 , log_packet.cal_payload.pga_level );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetCalibrationPgaLvlPga3(void)
{
    srv__serialise_to_bin( cal_str_pga_4 , strlen( cal_str_pga_4 ) );
    read_test_data_from_fw();
    TEST_ASSERT_EQUAL_UINT8( data__log_cal_pga_lvl_4 , log_packet.cal_payload.pga_level );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetCalibrationCurrent(void)
{
    srv__serialise_to_bin( cal_str_pga_4 , strlen( cal_str_pga_4 ) );
    read_test_data_from_fw();
    TEST_ASSERT_EQUAL_UINT8( 150 , log_packet.cal_payload.current );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetCalibrationRawValue(void)
{
    srv__serialise_to_bin( cal_str_pga_4 , strlen( cal_str_pga_4 ) );
    read_test_data_from_fw();
    TEST_ASSERT_EQUAL_UINT8( 50864 , log_packet.cal_payload.raw_value );
}

/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/


/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
static void read_test_data_from_fw(void)
{
    uint8_t data_size = 0;
    uint8_t * data_ptr;
    data_ptr = hw__log_io_read( & data_size );
    memcpy( & log_packet , data_ptr , data_size );
}
/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/