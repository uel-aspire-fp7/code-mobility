/* This research is supported by the European Union Seventh Framework Programme (FP7/2007-2013), project ASPIRE (Advanced  Software Protection: Integration, Research, and Exploitation), under grant agreement no. 609734; on-line at https://aspire-fp7.eu/. */

#ifndef DOWNLOADER_H
#define DOWNLOADER_H

/* libraries headers inclusion */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CODE_MOBILITY_ERROR 0

#ifdef ENABLE_LOGGING
#define LOG(mesg, ...) printf(mesg, ##__VA_ARGS__)
#else
#define LOG(...)
#endif

/* see accl.h */
extern int acclExchange (
	const int T_ID,
	const int payloadBufferSize,
	const char* pPayloadBuffer,
	unsigned int* returnBufferSize,
	char** 	pReturnBuffer
);

/* see accl.h */
#define ACCL_TID_CODE_MOBILITY			20

/* ACCL Return values */
#define ACCL_SUCCESS							0
#define ACCL_CURL_INITIALIZATION_ERROR			5
#define ACCL_INPUT_BUFFER_ERROR					10
#define ACCL_INPUT_BUFFER_MAX_SIZE_EXCEEDED		11
#define ACCL_OUTPUT_BUFFER_MAX_SIZE_EXCEEDED	12
#define ACCL_OUTPUT_BUFFER_ALLOCATION_ERROR		15
#define ACCL_UNKNOWN_TECHNIQUE_ID				20

#define ACCL_SERVER_ERROR						100

#endif 