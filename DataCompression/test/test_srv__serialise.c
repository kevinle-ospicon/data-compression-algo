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
	srv__serialise_init( & log_packet );
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
    TEST_ASSERT_EQUAL_UINT8( 23 , log_packet.temperature_payload.value );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetRawAdcType(void)
{
    srv__serialise_to_bin( raw_adc_str , strlen( raw_adc_str ) );
    TEST_ASSERT_EQUAL_UINT8( data__log_type_raw_adc , log_packet.header.log_type );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__serialise_GetCalibrationType(void)
{
    srv__serialise_to_bin( cal_str_single_led , strlen( cal_str_single_led ) );
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
    TEST_ASSERT_EQUAL_UINT8( 50864 , log_packet.cal_payload.raw_value );
}

/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/