/**********************************************************************
 *
 * Filename:    main.c
 * 
 * Description: A simple test program for the CRC implementations.
 *
 * Notes:       To test a different CRC standard, modify crc.h.
 *
 * 
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/
/***********************************************************************
 * 
 * TCE version modified by Otto Esko
 *
 * Notes: Only crc-32 is used
 *
 **********************************************************************/
#ifdef _DEBUG
#include <stdio.h>
#endif /* _DEBUG */

#include "crc.h"

/* Test string is global so that it can be accessed from ttasim */
volatile crc result = 0;
unsigned char  test[] = "TCE rocks!";
#define LENGTH 10

int
main(void) {

#ifdef _DEBUG
	/*
	 * Print the check value for the selected CRC algorithm.
	 */
	printf("The check value for the %s standard is 0x%X\n", CRC_NAME, CHECK_VALUE);
#endif /* _DEBUG */
	
	/*
	 * Compute the CRC of the test message, more efficiently.
	 */
 	/* crcInit(); */
    result = crcFast(test, LENGTH);

#ifdef _DEBUG
	printf("The crcFast() of \"TCE rocks!\" is 0x%X\n", result);
#endif /* _DEBUG */
    
    return 0;
}
