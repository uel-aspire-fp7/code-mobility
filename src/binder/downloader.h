#ifndef DOWNLOADER_H
#define DOWNLOADER_H

/* C-standard headers */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Linux-specific headers */
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef void* t_address;

t_address DIABLO_Mobility_DownloadByIndex(uint32_t index, size_t *len);

#endif
