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

int main(void) {

	static char * temp_str = "20180119_17:01:01:Temp:22,9\r\n";


    srv__serialise_to_bin( temp_str , strlen( temp_str ) );

	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
