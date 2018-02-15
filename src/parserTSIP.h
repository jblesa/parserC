/**
 * Includes
 */

#include <stdio.h>
#include <stdlib.h>


/**
 * Definitions
 */

#define DLE 0x10
#define ETX 0x03

/**
 * Variables
 */

union crc32Received {
	uint32_t crc32;
	uint8_t byte[4];
} ucrc32Received;


