#include "unity.h"
#include "dev__log_handler.h"
#include "data__log.h"
#include <stdint.h>

static data__log_packet_t raw_adc_pkt; 
static data__log_packet_t cal_led_pkt; 

void setUp(void)
{
}

void tearDown(void)
{
}
void test_dev__log_handler_InitialiseCalibrationLedPayload(void)
{
    data__log_cal_led_payload_t * payload_ptr;
    dev__log_handler_init_log_data();
    payload_ptr = dev__log_handler_get_packet_ptr();

    TEST_ASSERT_NOT_NULL( payload_ptr );

    // Test the payload    
    TEST_ASSERT_EQUAL_UINT8( 0 , payload_ptr->event_type );
    TEST_ASSERT_EQUAL_UINT8( 0 , payload_ptr->pga_level );
    TEST_ASSERT_EQUAL_UINT8( 0 , payload_ptr->raw_value );
    TEST_ASSERT_EQUAL_UINT8( 0 , payload_ptr->current );
}

void test_dev__log_handler_InitialiseTemperaturePayload(void)
{
    data__log_temperature_payload_t * payload_ptr;
    payload_ptr = dev__log_handler_get_packet_ptr();

    TEST_ASSERT_NOT_NULL( payload_ptr );
    TEST_ASSERT_EQUAL_UINT8( 0 , payload_ptr->value );
    
}
