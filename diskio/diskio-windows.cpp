#include "diskio.hpp"
#include <winioctl.h>
using namespace std;

int DiskIO::open_handle(const char* disk)
{
    hDisk=CreateFile(disk,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
    if(hDisk==INVALID_HANDLE_VALUE)
        return ERR_OPEN_DISK;
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
      return ERR_GEOMETRY_NOT_FOUND;

  disk_geometry.cylinders=dg.Cylinders.QuadPart;
  disk_geometry.spt=dg.SectorsPerTrack;
  disk_geometry.tpc=dg.TracksPerCylinder;
  disk_geometry.bps=dg.BytesPerSector;
  return 0;
}


int DiskIO::seek(uint64_t sector)
{
LARGE_INTEGER li;
li.QuadPart=sector;
if(SetFilePointerEx(hDisk,li,0,FILE_BEGIN)==INVALID_SET_FILE_POINTER)
    return ERR_INVALID_SEEK;
return 0;
}

int DiskIO::write(void *buff,uint32_t buffer_size)
{
DWORD dwWritten=0;

if(!WriteFile(hDisk,buff,buffer_size,&dwWritten,0) || dwWritten!=buffer_size)
  {
     //
     // Temorary bug fix: sometimes WriteFile fails with GetLastError() code 87(ERROR_INVALID_PARAMETER)
     // Interesting part is that making copy of a buffer fixes the problem
     uint8_t *buf=new uint8_t[buffer_size];
     memcpy(buf,buff,buffer_size);
     if(!WriteFile(hDisk,buf,buffer_size,&dwWritten,0) || dwWritten!=buffer_size)
       {
         delete[] buf;
         return ERR_WRITE;
       }

      delete[] buf;
  }


return 0;
}

int DiskIO::read(void *buff,uint32_t buffer_size)
{
DWORD dwRead;

if(!ReadFile(hDisk,buff,buffer_size,&dwRead,0) || dwRead!=buffer_size)
    return ERR_READ;

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
