/*
 * parserTSIP.c
 *
 *  Created on: 15/02/2018
 *      Author: Javier Blesa  <javierblesam@gmail.com>
 *
 */
/**
 * @file parserTSIP.c
 */

#include "parserTSIP.h"


/**
 * \brief Read received raw data from COM port.
 *
 * This is a non-blocking read function. This means that only available received
 * data will be served. User may decide to call this function within a loop
 * until the desired amount of data is received.
 *
 * \param buffer Pointer to destination buffer where to copy received raw data.
 * \param count Maximum number of bytes to be read.
 * \return Number of read bytes. This value will always be <= count.
 */
int32_t uavnComRead(uint8_t * const buffer, const uint32_t count);
/**
 * \brief Process valid data at application level.
 *
 * \param[in] buffer Const pointer where valid data is located. Valid data consists of ID1 + ID2
+ Data
 * \param[in] numberOfBytes It is filled with the number bytes.
 */
void ProcessValidData(const uint8_t * const buffer, const int32_t numberOfBytes);
/**
 * \brief CRC32 calculation. No CRC check is performed.
 *
 * \param buffer Pointer to buffer containing data to process.
 * \param start Offset value of first data byte (within buffer) to be processed.
 * \param end Offset value of last data byte (within buffer) to be processed.
 * \return Calculated CRC value.
 */
uint32_t crc32(const uint8_t * const buffer, const uint32_t start, const uint32_t end);
/**
 * \brief Parse received TSIP packets optimizing the used memory
 * return PACKET_OK when a successful packet is received, or error in the other cases
 */
uint8_t parserTSIP ();

int main () {
	int status;

	while(1) {

		parserTSIP ();
	}

	return status;

}


// Detect the first DLE
uint8_t parserTSIP () {

	int32_t readed;
	uint8_t checkByte;
	uint8_t *commandBuffer;
	uint8_t *tmpBuffer;
	uint8_t commandSize = 0;
	uint32_t crc32Calculated;

	uint8_t status;
	uint8_t i;
	uint8_t endPacket = 0;


	//TEST
	uint8_t dataCounter = 0;

	while ((readed = uavnComRead(&checkByte, dataCounter++)) == 0) {}

	while (checkByte != DLE) {
		readed = uavnComRead(&checkByte, dataCounter++);
	}

	// Detect the ETX

	while ((readed = uavnComRead(&checkByte, dataCounter++)) == 0) {}
	commandBuffer = (uint8_t *) malloc(1);

	while (!endPacket) {

		if (checkByte == DLE) {
			while ((readed = uavnComRead(&checkByte, dataCounter++)) == 0) {}
			if (checkByte == DLE) {
				commandBuffer[commandSize++] = checkByte;
				tmpBuffer = (uint8_t *) realloc(commandBuffer, sizeof(commandBuffer)+1);
				if (!tmpBuffer) {
					status = OUT_OF_MEMORY;
				}
				commandBuffer = tmpBuffer;
			}
			else if (checkByte == ETX) {
				tmpBuffer = (uint8_t *) realloc(commandBuffer, sizeof(commandBuffer)-1); //Remove last empty
				if (!tmpBuffer) {
					status = OUT_OF_MEMORY;
				}
				commandBuffer = tmpBuffer;
				endPacket = 1;
			}
			else {
				endPacket = 1;
				status = PACKET_FORMAT_ERROR;
			}
		}
		else {
			commandBuffer[commandSize++] = checkByte;
			tmpBuffer = (uint8_t *) realloc(commandBuffer, sizeof(commandBuffer)+1);
			if (!tmpBuffer) {
				status = OUT_OF_MEMORY;
			}
			commandBuffer = tmpBuffer;

		}

		while ((readed = uavnComRead(&checkByte, dataCounter++)) == 0) {}
	}

	if (status == PACKET_OK) {

		// Extract CRC32
		for (i=0; i < CRCSIZE; i++){
			ucrc32Received.byte[i] = commandBuffer[commandSize - CRCSIZE + i];
		}

		// Check CRC32
		crc32Calculated = crc32(commandBuffer, 0, commandSize - CRCSIZE);

		if (crc32Calculated != ucrc32Received.crc32) { //TODO-change to equal

			////CRC extraction
			commandBuffer = (uint8_t *) realloc(commandBuffer, sizeof(commandBuffer)-CRCSIZE); //Remove CRC
			ProcessValidData(commandBuffer, commandSize - CRCSIZE);

			status = PACKET_OK;
		}

		else {
			status = PACKET_CRC_ERROR;
		}
	}

	free(commandBuffer);
	return status;

}


int32_t uavnComRead(uint8_t * const buffer, const uint32_t count){


	memcpy (buffer, data+count, 1);


	return 1;

}

uint32_t crc32(const uint8_t * const buffer, const uint32_t start, const uint32_t end){
	int32_t readed = 0;

	return readed;

}


void ProcessValidData(const uint8_t * const buffer, const int32_t numberOfBytes) {


}




