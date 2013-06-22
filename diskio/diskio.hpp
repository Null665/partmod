

#ifndef DISKIO_HPP
#define DISKIO_HPP

#include <stdint.h>

#if defined(_WIN32)
  #include <windows.h>
  typedef HANDLE DISK_HANDLE;
#else
  typedef int DISK_HANDLE;
#endif



#include "../Partmod/definitions.h"
#include "../Partmod/disk_exception.h"

class DiskIO
{
protected:
  DISK_HANDLE hDisk;
  bool is_open;
  bool disk_image;
  GEOMETRY disk_geometry;

public:
  DiskIO();

  int Open(const char *disk);
  int Open(const char *disk, GEOMETRY geom);

  void Close();
  bool IsOpen();

  const GEOMETRY &GetDiskGeometry();
  int GetDiskSize(uint64_t &length);

  int Seek(uint64_t pos);
  int SeekSector(uint64_t sector);

  int Read(void *buff,uint32_t buffer_size);
  int Write(const void *buff,uint32_t buffer_size);

  void Sync();

protected:
    int  open_handle(const char* disk);
    void close_handle();
    int  get_geometry();

    int seek(uint64_t pos);
    int read(void *buff,uint32_t buffer_size);
    int write(const void *buff,uint32_t buffer_size);

    int sync();

    int get_disk_length(uint64_t&);
};


#endif
