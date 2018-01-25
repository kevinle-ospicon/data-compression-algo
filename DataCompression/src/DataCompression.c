/*
 ============================================================================
 Name        : DataCompression.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "srv__serialise.h"
#include "srv__deserialise.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define MAXCHAR 128

static char * deserialised_filename = "deserialised.txt";
static char * bin_filename = "test.bin";
static int deserialised_lines = 0;

static void read_text_file_and_save_bin_file( void );
static void read_bin_file_and_save_text_file( void );
static void start_deserialise_process( int byte_from_file );
static void write_string_to_file( char * file_name , char * string );

int main(void) {

	//read_text_file_and_save_bin_file();
	read_bin_file_and_save_text_file();

	return EXIT_SUCCESS;
}

static void read_text_file_and_save_bin_file( void )
{
	FILE *fp;
	char str[MAXCHAR];
	char * filename = "test_log_1.txt";
	int str_size = 0;
	int line_number = 1;

	fp = fopen(filename, "r");
	if (fp == NULL){
		printf("Could not open file %s",filename);
		return;
	}
	// reads text until newline
	while ( fgets( str , MAXCHAR , fp ) != NULL )
	{
		printf( "line number %d: %s", line_number ++ , str );
		str_size = strlen( str );
		srv__serialise_to_bin( str , str_size );
	}
	srv__serialise_commit_all();
	fclose(fp);
}

static void read_bin_file_and_save_text_file( void )
{
	FILE *fp;
	int c;

	fp = fopen( bin_filename , "rb" );
	if (fp == NULL){
		printf("Could not open file %s", bin_filename );
		return;
	}
	srv__deserialise_init();
	// note: int, not char, required to handle EOF
	while ( ( c = fgetc( fp ) ) != EOF )
	{
		start_deserialise_process( c );
	}
	printf("total deserialized lines: %d\r\n" , deserialised_lines );
	fclose( fp );
}

static void start_deserialise_process( int byte_from_file )
{
	bool status = false;
	int count = 0;
	uint8_t line_size = 0;
	char * line_ptr;

	status = srv__deserialise_parse( ( uint8_t ) ( byte_from_file & 0x000000FF ) );
	if( status == true )
	{

		int raw_adc_lines = srv__deserialise_get_pending_raw_adc_lines();
		if( raw_adc_lines > 0 )
		{
			for( count = 0 ; count < raw_adc_lines ; count ++ )
			{
				//Write each raw ADC log value to a line.
				//There could be multiple values.
				//write_string_to_file( deserialised_filename , srv__deserialise_get_log_packet_line( & line_size ) );
				line_ptr = srv__deserialise_get_log_packet_line( & line_size );
				printf("%s" , line_ptr );
				deserialised_lines++;
			}
		}
		else
		{
			// Write other log packet to a single line.
			//write_string_to_file( deserialised_filename , srv__deserialise_get_log_packet_line( & line_size ) );
			line_ptr = srv__deserialise_get_log_packet_line( & line_size );
			printf("%s" , line_ptr );
			deserialised_lines++;
		}
	}
}

static void write_string_to_file( char * file_name , char * string )
{
	FILE *fp;
	fp = fopen( file_name , "a");
	if (fp == NULL){
		printf("Could not open file %s",file_name);
		return;
	}
	fputs( string , file_name );
	fclose(fp);
}
