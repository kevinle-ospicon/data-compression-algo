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
#include <stdbool.h>
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
static char log_str[ SRV_DESERIALISE_MAX_STRING_LEN ] = "not empty";

// Equivalent Ascii string: "20180119_17:01:01:Temp:22,9"
static uint8_t temp_packet[] = { 0x0D , 0x0A , 0x3E , 0x3E , 
                                 0x4D , 0x24 , 0x62 , 0x5A ,
                                 0x02 , 0x01 , 0x17 };
static uint8_t correct_begin_marker[] = { 0x0D , 0x0A , 0x3E , 0x3E };
static uint8_t wrong_begin_marker[] = { 0x0D , 0x0A , 0x3E , 0x00 };
static uint8_t begin_marker_wrong_LF[] = { 0x0D , 0x00 , 0x0A , 0x3E , 0x3E };
static uint8_t begin_marker_wrong_begin_1[] = { 0x0D , 0x0A , 0x00 , 0x3E , 0x3E };
static uint8_t begin_marker_wrong_begin_2[] = { 0x0D , 0x0A , 0x3E , 0x00 , 0x3E };
static uint8_t correct_header[] = { 0x0D , 0x0A , 0x3E , 0x3E , 
                                    0x4D , 0x24 , 0x62 , 0x5A , 0x02 , 0x01 };
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
    srv__deserialise_init( log_str , SRV_DESERIALISE_MAX_STRING_LEN );
}

void tearDown(void)
{
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_InitialiseEmptyString(void)
{
    srv__deserialise_init( log_str , SRV_DESERIALISE_MAX_STRING_LEN );
    TEST_ASSERT_EACH_EQUAL_UINT8( 0 , log_str , SRV_DESERIALISE_MAX_STRING_LEN );
}

/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
void test_srv__deserialise_DetectCorrectBeginMarker(void)
{
    bool parse_result = false;
    for( int idx = 0 ; idx < sizeof( correct_begin_marker ) ; idx ++ )
    {
        parse_result = srv__deserialise_parse( correct_begin_marker[ idx ] );
    }

    TEST_ASSERT_TRUE( parse_result );
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
    data__log_packet_t log_packet = srv__deserialise_get_log_packet();
    TEST_ASSERT_EQUAL_UINT32( 0x5A62244D , log_packet.header.timestamp );
    TEST_ASSERT_EQUAL_UINT8( ( uint8_t ) data__log_type_temperature , log_packet.header.log_type );
    TEST_ASSERT_EQUAL_UINT8( sizeof( data__log_temperature_payload_t ) , log_packet.header.payload_len );
}
/*============================================================================
@brief
------------------------------------------------------------------------------
@note
============================================================================*/
// void test_srv__deserialise_GetTimestampInAscii(void)
// {
//     bool parse_result = false;
//     for( int idx = 0 ; idx < sizeof( temp_packet ) ; idx ++ )
//     {
//         parse_result = srv__deserialise_parse( temp_packet[ idx ] );
//     }
//     char * log_str = srv__deserialise_get_log_string();

//     TEST_ASSERT_TRUE( parse_result );
//     TEST_ASSERT_NOT_NULL( log_str );
//     TEST_ASSERT_EQUAL_STRING( "20180119_17:01:01:Temp:22,9" , log_str );
// }

/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/