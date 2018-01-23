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

int main(void) {
	FILE *fp;
	char str[MAXCHAR];
	char * filename = "test_log_1.txt";
	int str_size = 0;
	int line_number = 1;

	fp = fopen(filename, "r");
	if (fp == NULL){
		printf("Could not open file %s",filename);
		return 1;
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

	return EXIT_SUCCESS;
}
