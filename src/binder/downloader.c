#include "downloader.h"

t_address DIABLO_Mobility_DownloadByIndex(uint32_t index, size_t *len)
{
  t_address target_address;
  struct stat s;
  int fd;
  char buffer[strlen("mobile_dump_") + 9];
  sprintf(buffer, "mobile_dump_%08x", index);

  /* Open the file */
  fd = open(buffer, O_RDONLY, 0);

  /* Find the size of the file */
  fstat (fd, & s);
  *len = s.st_size;

  /* Map it into memory */
  target_address = mmap(NULL, *len, PROT_WRITE, MAP_PRIVATE, fd, 0);

  /* Close file */
  close(fd);

  return target_address;
}
