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
    dev__log_handler_init_log_data();
}

void tearDown(void)
{
}
void test_dev__log_handler_AddAndReadCalibrationLedPayload(void)
{
    uint8_t event_type = 1;
    uint8_t pga_level = data__log_cal_pga_lvl_2;
    uint16_t raw_value = 0x9876;
    uint8_t current = 125;
    uint32_t timestamp = 0x12345678;

    dev__log_handler_add_cal_packet( pga_level , raw_value , current , timestamp );
    uint8_t data_size = 0;
    uint8_t * data_ptr = hw__log_io_read( & data_size );

    TEST_ASSERT_EQUAL_UINT8( data__log_get_packet_len( data__log_type_cal ) , data_size );

    data__log_packet_t packet;
    memcpy( & packet , data_ptr , data_size );
    
    TEST_ASSERT_EQUAL_UINT8( pga_level , packet.cal_payload.pga_level );
    TEST_ASSERT_EQUAL_UINT16( raw_value , packet.cal_payload.raw_value );
    TEST_ASSERT_EQUAL_UINT8( current , packet.cal_payload.current );
    TEST_ASSERT_EQUAL_UINT32( timestamp , packet.cal_payload.timestamp );
}

void test_dev__log_handler_AddAndReadRawAdcPayloadWithinOneSecond(void)
{
    uint16_t values[ 6 ] = { 0xAABB  , 0xAABB , 0xAABB , 0xAABB , 0xAABB , 0xAABB };

    for( int idx = 0 ; idx < 6  ; idx ++)
    {
        dev__log_handler_add_raw_adc_value( values[ idx ] );
    }
    dev__log_handler_commit_raw_adc_packet();
    uint8_t data_size = 0;
    uint8_t * data_ptr = hw__log_io_read( & data_size );

    TEST_ASSERT_EQUAL_UINT8( data__log_get_packet_len( data__log_type_raw_adc ) , data_size );

    data__log_packet_t packet;
    memcpy( & packet , data_ptr , data_size );
    
    TEST_ASSERT_EQUAL_UINT8( 6 , packet.raw_adc_payload.sample_count );
    TEST_ASSERT_EQUAL_UINT16_ARRAY( values , packet.raw_adc_payload.value , 6 );
}

void test_dev__log_handler_AddAndReadOufOfBoundRawAdcPayloadWithinOneSecond(void)
{
    uint16_t values[ MAX_ADC_SAMPLE_COUNT + 1 ] = { 0xAABB , 0xAABB , 0xAABB , 0xAABB ,
                                                    0xAABB , 0xAABB , 0xAABB , 0xAABB , 
                                                    0xAABB , 0xAABB , 0xAABB };

    for( int idx = 0 ; idx <= MAX_ADC_SAMPLE_COUNT  ; idx ++)
    {
        dev__log_handler_add_raw_adc_value( values[ idx ] );
    }

    uint8_t data_size = 0;
    uint8_t * data_ptr = hw__log_io_read( & data_size );

    TEST_ASSERT_EQUAL_UINT8( data__log_get_packet_len( data__log_type_raw_adc ) , data_size );

    data__log_packet_t packet;
    memcpy( & packet , data_ptr , data_size );
    
    TEST_ASSERT_EQUAL_UINT8( MAX_ADC_SAMPLE_COUNT , packet.raw_adc_payload.sample_count );
    TEST_ASSERT_EQUAL_UINT16_ARRAY( values , packet.raw_adc_payload.value , MAX_ADC_SAMPLE_COUNT );
}

void test_dev__log_handler_AddAndReadAdcPayloadFromTwoTimestamps(void)
{
    uint16_t values[ 6 ] = { 0xAABB  , 0xAABB , 0xAABB , 0xAABB , 0xAABB , 0xAABB };
    uint16_t values_new[ 6 ] = { 0xBBAA , 0xBBAA , 0xBBAA , 0xBBAA , 0xBBAA , 0xBBAA };

    for( int idx = 0 ; idx < 6  ; idx ++)
    {
        dev__log_handler_add_raw_adc_value( values[ idx ] );
    }
    dev__log_handler_commit_raw_adc_packet();

    uint8_t data_size = 0;
    uint8_t * data_ptr = hw__log_io_read( & data_size );
    data__log_packet_t packet;
    memcpy( & packet , data_ptr , data_size );

    TEST_ASSERT_EQUAL_UINT8( data__log_get_packet_len( data__log_type_raw_adc ) , data_size );
    
    TEST_ASSERT_EQUAL_UINT8( 6 , packet.raw_adc_payload.sample_count );
    TEST_ASSERT_EQUAL_UINT16_ARRAY( values , packet.raw_adc_payload.value , 6 );

    for( int idx = 0 ; idx < 6  ; idx ++)
    {
        dev__log_handler_add_raw_adc_value( values_new[ idx ] );
    }


    //Commit and test for the next packet
    dev__log_handler_commit_raw_adc_packet();

    data_ptr = hw__log_io_read( & data_size );
    memcpy( & packet , data_ptr , data_size );

    TEST_ASSERT_EQUAL_UINT8( data__log_get_packet_len( data__log_type_raw_adc ) , data_size );

    memcpy( & packet , data_ptr , data_size );
    
    TEST_ASSERT_EQUAL_UINT8( 6 , packet.raw_adc_payload.sample_count );
    TEST_ASSERT_EQUAL_UINT16_ARRAY( values_new , packet.raw_adc_payload.value , 6 );

}

void test_dev__log_handler_AddAndReadTimstampPayload(void)
{
    uint32_t value = 0x12345678;

    dev__log_handler_add_timestamp_packet( value );
    uint8_t data_size = 0;
    uint8_t * data_ptr = hw__log_io_read( & data_size );

    TEST_ASSERT_EQUAL_UINT8( data__log_get_packet_len( data__log_type_timestamp ) , data_size );

    data__log_packet_t packet;
    memcpy( & packet , data_ptr , data_size );
    
    TEST_ASSERT_EQUAL_UINT8( data__log_type_timestamp , packet.header.log_type );
    TEST_ASSERT_EQUAL_INT8( value , packet.timestamp_payload.value );
}

