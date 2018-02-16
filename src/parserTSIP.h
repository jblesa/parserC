/*
 * parserTSIP.h
 *
 *  Created on: 15/02/2018
 *      Author: Javier Blesa  <javierblesam@gmail.com>
 *
 */
/**
 * @file parserTSIP.h
 */


#ifndef PARSERTSIP_H
#define PARSERTSIP_H


/**
 * Includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Definitions
 */

#define DLE 0x10
#define ETX 0x03

#define CRCSIZE 4
/**
 * Variables
 */

union crc32Received {
	uint32_t crc32;
	uint8_t byte[4];
} ucrc32Received;

enum packetstauts {
	PACKET_OK,
	PACKET_FORMAT_ERROR,
	PACKET_CRC_ERROR,
	OUT_OF_MEMORY
};


uint8_t data[16] = { DLE, 0x01, 0x01, 0x04, 0x03, 0x10, 0x10, 0x04, 0x45, 0x46, 0x47, 0x48, 0x49, 0x50, DLE, ETX};

#endif
