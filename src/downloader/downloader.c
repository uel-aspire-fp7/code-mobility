/* This research is supported by the European Union Seventh Framework Programme (FP7/2007-2013), project ASPIRE (Advanced  Software Protection: Integration, Research, and Exploitation), under grant agreement no. 609734; on-line at https://aspire-fp7.eu/. */

#include "downloader.h"
//#include <accl.h>

/*

	Code Mobility Downloader
		[in] unsigned int block_index
		[out] unsigned int *block_length

	Returns a pointer to the downloaded mobile block. The first 4 bytes of
		this blocks represent the program base address.

*/
void* DIABLO_Mobility_DownloadByIndex(
	unsigned int block_index,
	unsigned int *block_length) {

	LOG("Downloader[index: %d]: Block requested\n", block_index);

	unsigned int page_size, buffer_size, ret, full_pages;
	void *temp_buffer, *out_buffer;

	page_size = getpagesize();
	buffer_size = page_size * 2;	

	// invoke ACCL
	ret = acclExchange (
		ACCL_TID_CODE_MOBILITY,
		(int)sizeof(unsigned int),
		(char*)&block_index,
		&buffer_size,
		(char**)&temp_buffer);

	if (ACCL_SUCCESS == ret) {
		LOG("Downloader[index: %d]: Block retrieved (%d bytes) from server.\n", block_index, buffer_size);

		// if the return size is valid the downloader returns received data
		if (buffer_size > 0) {
			// see https://aspire-fp7.eu/bugtracker/view.php?id=73
			// each mobile block should take one or more complete memory
			// pages so that subsequent heap memory allocations are not
			// affected by writing restrictions
			full_pages = buffer_size / page_size
					+ (buffer_size % page_size > 0 ? 1 : 0);

			LOG("Downloader[index: %d]: %d memory pages of %d bytes are being allocated.\n", block_index, full_pages, page_size);

			// allocates a page aligned buffer
			#ifndef __ANDROID__
				if (0 != posix_memalign(&out_buffer, 
					page_size, full_pages * page_size)) {
					free(temp_buffer);
					return CODE_MOBILITY_ERROR;
				}
			#else
				// since posix_memalign is not available on Android we need to
				// rely on the obsolete function memalign
				out_buffer = memalign(page_size, full_pages * page_size);

				if (NULL == out_buffer){
					free(temp_buffer);
					return CODE_MOBILITY_ERROR;
				}
			#endif

			// pass actual buffer length
			memcpy(block_length, &buffer_size, sizeof(unsigned int));

			// this is not so efficient... is it possible to avoid this copy?
			memcpy(out_buffer, temp_buffer, buffer_size);

			LOG("Downloader[index: %d]: mobile code block trasferred and allocated at %p.\n", block_index, out_buffer);

		} else {
			free(temp_buffer);

			return CODE_MOBILITY_ERROR;
		}
	} else {
		free(temp_buffer);

		return CODE_MOBILITY_ERROR;
	}

	free(temp_buffer);

	return out_buffer;
}