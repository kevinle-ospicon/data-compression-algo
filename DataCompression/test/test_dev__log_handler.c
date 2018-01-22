#include "unity.h"
#include "dev__log_handler.h"
#include "data__log.h"
#include "hw__log_io.h"
#include <stdint.h>
#include <string.h>


//static data__log_packet_t raw_adc_pkt; 
//static data__log_packet_t cal_led_pkt; 

void setUp(void)
{
    hw__log_io_init();
}

void tearDown(void)
{
}
void test_dev__log_handler_AddAndReadCalibrationLedPayload(void)
{
    uint32_t timestamp = 0x12345678;
    uint8_t event_type = 1;
    uint8_t pga_level = 2;
    uint16_t raw_value = 0x9876;
    uint8_t current = 125;

    dev__log_handler_add_cal_led_packet( timestamp , event_type , pga_level , raw_value , current );
    uint8_t data_size = 0;
    uint8_t * data_ptr = hw__log_io_read( & data_size );

    TEST_ASSERT_EQUAL_UINT8( data__log_get_packet_len( data__log_type_cal_led ) , data_size );

    data__log_packet_t packet;
    memcpy( & packet , data_ptr , data_size );
    
    TEST_ASSERT_EQUAL_UINT32( timestamp , packet.header.timestamp );
    TEST_ASSERT_EQUAL_UINT8( event_type , packet.cal_led_payload.event_type );
    TEST_ASSERT_EQUAL_UINT8( pga_level , packet.cal_led_payload.pga_level );
    TEST_ASSERT_EQUAL_UINT16( raw_value , packet.cal_led_payload.raw_value );
    TEST_ASSERT_EQUAL_UINT8( current , packet.cal_led_payload.current );
}

void test_dev__log_handler_InitialiseTemperaturePayload(void)
{
    uint32_t timestamp = 0x12345678;
    int8_t value = 29;

    dev__log_handler_add_temperature_packet( timestamp , value );
    uint8_t data_size = 0;
    uint8_t * data_ptr = hw__log_io_read( & data_size );

    TEST_ASSERT_EQUAL_UINT8( data__log_get_packet_len( data__log_type_temperature ) , data_size );

    data__log_packet_t packet;
    memcpy( & packet , data_ptr , data_size );
    
    TEST_ASSERT_EQUAL_UINT32( timestamp , packet.header.timestamp );
    TEST_ASSERT_EQUAL_UINT8( data__log_type_temperature , packet.header.log_type );
    TEST_ASSERT_EQUAL_INT8( value , packet.temperature_payload.value );
}
