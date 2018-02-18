/*
 * parserTSIP.c
 *
 *  Created on: 17 feb. 2018
 *      Author: jblesa
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
/**
 * \brief Resize with realloc the RX buffer.
 * \param buffer Pointer to buffer containing data to process
 * \param change Change of the buffer size.
 */
uint8_t updateBufferSize (uint8_t *buffer, int8_t change);



int main() {

	uint8_t error = 0;

	while (!error) {

		if ((error = parserTSIP()) != PACKET_OK) {

			//printf("Error: %d\n", error);

		}

	}
	return error;
}

// Detect the first DLE
uint8_t parserTSIP () {

	int32_t readed;
	uint8_t checkByte;
	static uint8_t commandSize;
	uint32_t crc32Calculated;
	uint8_t * tmpBuffer;
	uint8_t i;

	static uint8_t machineState;
	static int bEvenDLE;
	uint8_t status;

	//TEST
	static uint8_t dataCounter;


	status = PACKET_OK;
	while ((readed = uavnComRead(&checkByte, dataCounter++)) == 0) {} //Read 1

	switch (machineState) {
	case WAIT_FOR_START:
		if (checkByte == DLE) {
			commandSize = 0;
			machineState = WAIT_FOR_ID;
		}
		else if (checkByte != ETX) {
			machineState = WAIT_FOR_DLE_ETX;
			commandSize = 0;
		}
		bEvenDLE = 0;
		break;

	case WAIT_FOR_DLE_ETX:
		if (checkByte == DLE)
			machineState = WAIT_FOR_ETX;
		else
			machineState = WAIT_FOR_START;
		break;

	case WAIT_FOR_ETX:
	if (checkByte == ETX) {
		// found end of a message
		machineState = WAIT_FOR_DLE;
	}
	else {
		machineState = WAIT_FOR_START;
	}
	break;

	case WAIT_FOR_DLE:
		if (checkByte == DLE) {
			commandSize = 0;
			//commandBuffer[commandSize++] = checkByte;
			machineState = WAIT_FOR_ID;
		}
		else {
			machineState = WAIT_FOR_START;
		}
		bEvenDLE = 0;
		break;

	case WAIT_FOR_ID:
	if (checkByte == DLE || checkByte == ETX) {
		machineState = WAIT_FOR_START;
		commandSize = 0;
	}
	else {
		machineState = WAIT_FOR_END_MSG;
		//First time we save a data
		commandBuffer = (uint8_t *) malloc (1);
		if(commandBuffer == NULL) {
			status = OUT_OF_MEMORY;
		}
		commandBuffer[commandSize++] = checkByte;

	}
	break;

	case WAIT_FOR_END_MSG:
		if (checkByte == DLE && commandBuffer[commandSize-1] == DLE && !bEvenDLE) {
			// byte stuffing
			bEvenDLE = 1;
		}
		else if (checkByte == ETX && commandBuffer[commandSize-1] == DLE && !bEvenDLE) {
			// complete message received, in buffer
			machineState = WAIT_FOR_DLE;


			// Extract CRC32
			for (i=0; i < CRCSIZE; i++){
				ucrc32Received.byte[i] = commandBuffer[commandSize - CRCSIZE - 1 + i];
			}

			// Check CRC32
			crc32Calculated = crc32(commandBuffer, 0, commandSize - CRCSIZE -1); //Last DLE continues there
			if (crc32Calculated == ucrc32Received.crc32) { //TODO-change to equal

				tmpBuffer = (uint8_t *) malloc(commandSize - CRCSIZE -1);
				if (!tmpBuffer) {

				}
				memcpy(tmpBuffer, commandBuffer, commandSize -CRCSIZE -1);
				ProcessValidData(tmpBuffer, commandSize - CRCSIZE -1);
				free(tmpBuffer);

			}

			else {
				status = PACKET_CRC_ERROR;
			}

		}
		else {

			if (updateBufferSize (commandBuffer, 1) == 0) {
				status = OUT_OF_MEMORY;
			}
			commandBuffer[commandSize++] = checkByte;
			bEvenDLE = 0;
		}
		break;

	default:
		machineState = WAIT_FOR_START;
		break;
	}

	return status;
}


uint8_t updateBufferSize (uint8_t *buffer, int8_t change) {
	uint8_t *tmpBuffer;
	uint8_t val;

	tmpBuffer = (uint8_t *) realloc(buffer, sizeof(buffer) + change);
	if (!tmpBuffer) {
		val = 0;
	}
	else {
		buffer = tmpBuffer;
		val = 1;
	}
	return val;
}

//For testing
int32_t uavnComRead(uint8_t * const buffer, const uint32_t count){

	memcpy (buffer, datab+count, 1);
	return 1;

}

uint32_t crc32(const uint8_t * const buffer, const uint32_t start, const uint32_t end){

	int32_t readed = 0;
	return readed;

}

void ProcessValidData(const uint8_t * const buffer, const int32_t numberOfBytes) {

}
