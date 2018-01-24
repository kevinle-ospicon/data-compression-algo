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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXCHAR 128

static void read_text_file_and_save_bin_file( void );
static void read_bin_file_and_save_text_file( void );

int main(void) {
	read_text_file_and_save_bin_file();

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
	char str[MAXCHAR];
	char * filename = "test.bin";
	int str_size = 0;
	int line_number = 1;

	fp = fopen(filename, "rb");
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
