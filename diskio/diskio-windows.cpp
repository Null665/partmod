// FIXME/CHECK: read() and write() functions

#include "diskio.hpp"
#include <winioctl.h>
#include <memory>
using namespace std;

int DiskIO::open_handle(const char* disk)
{
    read_only=false;

    hDisk=CreateFile(disk,GENERIC_READ|GENERIC_WRITE,7,0,OPEN_EXISTING,FILE_FLAG_WRITE_THROUGH,0);
    DWORD dwRead; // value not used but still required for internal use by DeviceIOCotol
    int is_writable=DeviceIoControl(hDisk,IOCTL_DISK_IS_WRITABLE,NULL,0,NULL,0,&dwRead,0);

// The following block is executed in two situations:
// 1: Failed to open the disk; try with less access rights
// 2: media is not writable; reopen with less access rights because we do not need them anyway
    if(hDisk==INVALID_HANDLE_VALUE || is_writable==0)
    {
      CloseHandle(hDisk);
      hDisk=CreateFile(disk,GENERIC_READ,7,0,OPEN_EXISTING,FILE_FLAG_WRITE_THROUGH,0);
      if(hDisk==INVALID_HANDLE_VALUE)
          return ERR_OPEN_DISK;
      read_only=true;

    }
    return 0;
}


void DiskIO::close_handle()
{
    CloseHandle(hDisk);

}

int DiskIO::get_geometry()
{
  DWORD dwRead;
  DISK_GEOMETRY dg;
  memset(&dg,0,sizeof(DISK_GEOMETRY));

  if(!DeviceIoControl(hDisk,IOCTL_DISK_GET_DRIVE_GEOMETRY,NULL,0,&dg,sizeof(DISK_GEOMETRY),&dwRead,NULL))
      return ERR_GET_DISK_GEOMETRY;

  disk_geometry.cylinders=dg.Cylinders.QuadPart;
  disk_geometry.spt=dg.SectorsPerTrack;
  disk_geometry.tpc=dg.TracksPerCylinder;
  disk_geometry.bps=dg.BytesPerSector;
  return 0;
}


int DiskIO::seek(uint64_t pos)
{
LARGE_INTEGER li;
li.QuadPart=pos;
if(SetFilePointerEx(hDisk,li,0,FILE_BEGIN)==INVALID_SET_FILE_POINTER)
    return ERR_INVALID_SEEK;
return 0;
}

// FIXME: WriteFile() still fails sometimes.
// It's somehow related to seek position and/or size of the data buffer
int DiskIO::write(const void *buff,uint32_t buffer_size)
{
DWORD dwWritten=0;
/*
if(buffer_size%disk_geometry.bps==0)
  {
    if(!WriteFile(hDisk,buff,buffer_size,&dwWritten,0) || dwWritten!=buffer_size)
        return ERR_WRITE;

  }
else*/
  {
    DWORD dwRead;

    // create a new buffer where buffer_size%bytes_per_sector==0
    unsigned size_new=disk_geometry.bps*( (buffer_size/disk_geometry.bps)+1 );
    unique_ptr<uint8_t[]> buff_new(new uint8_t[size_new]);

    LARGE_INTEGER seek_pos;
    LARGE_INTEGER tmp;

    memset(&seek_pos,0,sizeof(LARGE_INTEGER));
    memset(&tmp,0,sizeof(LARGE_INTEGER));

    if(!SetFilePointerEx(hDisk,tmp,&seek_pos,FILE_CURRENT))
          return ERR_INVALID_SEEK;

    if(!ReadFile(hDisk,buff_new.get(),size_new,&dwRead,0) || dwRead!=size_new)
          return ERR_READ;
    memcpy(buff_new.get()+seek_pos.QuadPart%disk_geometry.bps,buff,buffer_size);

    if(!SetFilePointerEx(hDisk,seek_pos,0,FILE_BEGIN))
          return ERR_INVALID_SEEK;

    if(!WriteFile(hDisk,buff_new.get(),size_new,&dwWritten,0) || dwWritten!=size_new)
          return ERR_WRITE;

  }

return 0;
}

int DiskIO::read(void *buff,uint32_t buffer_size)
{
DWORD dwRead;

// Size of the data buffer must be divisible by bytes-per-sector
unsigned size_new=disk_geometry.bps*( (buffer_size/disk_geometry.bps)+1 );
unique_ptr<uint8_t[]>buff_new(new uint8_t[size_new]);

if(!ReadFile(hDisk,buff_new.get(),size_new,&dwRead,0) || dwRead!=size_new)
    return ERR_READ;

memcpy(buff,buff_new.get(),buffer_size);

return 0;
}


int DiskIO::sync()
{
DWORD ret;
DeviceIoControl(hDisk, IOCTL_DISK_UPDATE_PROPERTIES, NULL,0,NULL, 0, &ret, 0 );
return 0;
}


int DiskIO::get_disk_length(uint64_t &ret)
{
if(disk_image==false)
  {
      GET_LENGTH_INFORMATION gli;
      DWORD bytes_ret;

      if(!DeviceIoControl(hDisk,IOCTL_DISK_GET_LENGTH_INFO,0,0,&gli,sizeof(gli),&bytes_ret,0))
          return ERR_READ;

      ret=gli.Length.QuadPart;
      return 0;
  }
else
  {
      DWORD size_hi=0,size_lo=0;
      size_lo=GetFileSize(hDisk,&size_hi);

      ret=size_lo|(size_hi<<16);
  }

return 0;
}
