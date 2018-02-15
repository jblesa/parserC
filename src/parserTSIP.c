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

int main () {

	int32_t readed;
	uint8_t tempBuffer;
	uint8_t *commandBuffer;
	uint8_t commandSize = 0;
	int8_t crcByte = 3;
	uint32_t crc32Calculated;

	int status;
	uint8_t i;
	uint8_t j;

	while(1) {

		// Detect the first DLE

		while ((readed = uavnComRead(&tempBuffer, 1)) == 0) {
		}

		while (tempBuffer != DLE) {
			readed = uavnComRead(&tempBuffer, 1);
		}

		// Detect the ETX

		while ((readed = uavnComRead(&tempBuffer, 1)) == 0) {
		}
		commandBuffer = (uint8_t *) malloc(1);
		//commandBuffer[commandSize++] = tempBuffer; //First DLE in not valid data

		while(tempBuffer != ETX) {
			commandBuffer[commandSize++] = tempBuffer;
			while ((readed = uavnComRead(&tempBuffer, 1)) == 0) {
			}
			commandBuffer = (uint8_t *) realloc(commandBuffer, sizeof(commandBuffer)+1); //Last byte always empty
		}

		//DLE extraction

		commandBuffer = (uint8_t *) realloc(commandBuffer, sizeof(commandBuffer)-2); //Last byte always empty and last DLE
		commandSize -= 2;


		for (i = commandSize -1; i >= 0; i--) {
			if(commandBuffer[i] == DLE) {
				//Deletes the others DLE
				for (j = i; j >= commandSize; j++) {
					commandBuffer[j-1] = commandBuffer[j];
				}
				commandSize--;
			}
			if (crcByte >= 0) {
				ucrc32Received.byte[crcByte--]; //little-endiand
			}

		}


		// Check CRC32
		crc32Calculated = crc32(commandBuffer, 0, commandSize);

		if (crc32Calculated == ucrc32Received.crc32) {
			ProcessValidData(commandBuffer, commandSize);
		}

	}

	return status;

}


int32_t uavnComRead(uint8_t * const buffer, const uint32_t count){
	int32_t readed = 0;

	return readed;

}

uint32_t crc32(const uint8_t * const buffer, const uint32_t start, const uint32_t end){
	int32_t readed = 0;

		return readed;

}

