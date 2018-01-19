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
    int payload_len = sizeof( uint8_t ) +                               // marker bytes
                      sizeof( uint16_t ) * MAX_ADC_SAMPLE_COUNT;        // payload length bytes
    TEST_ASSERT_EQUAL_INT( payload_len , sizeof( data__log_raw_adc_payload_t ) );
}
/*----------------------------------------------------------------------------
  private functions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  End of file
----------------------------------------------------------------------------*/