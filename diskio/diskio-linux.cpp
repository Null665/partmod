//
// A very simple disk io for linux
//


#include "diskio.hpp"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include <linux/fs.h>

#include <iostream>
using std::cout;


int DiskIO::open_handle(const char* disk)
{
hDisk=open(disk, O_RDWR|O_LARGEFILE);
if(hDisk==-1)
    return ERR_OPEN_DISK;
return 0;

}
void DiskIO::close_handle()
{
close(hDisk);
}


int DiskIO::get_geometry()
{
struct hd_geometry dg;
if(ioctl(hDisk,HDIO_GETGEO,&dg)!=0)
    return ERR_GET_DISK_GEOMETRY;

disk_geometry.cylinders=dg.cylinders;
disk_geometry.spt=dg.sectors;
disk_geometry.tpc=dg.heads;

unsigned long bps;
if(ioctl(hDisk,BLKSSZGET,&bps)!=0)
    return ERR_GET_DISK_GEOMETRY;

disk_geometry.bps=bps;
return 0;
}

int DiskIO::seek(uint64_t sector)
{
  lseek64(hDisk,sector,SEEK_SET);
  return 0;
}

int DiskIO::write(const void *buff,uint32_t buffer_size)
{
  uint64_t res=::write(hDisk,buff,buffer_size);
  if(res!=buffer_size)
      return ERR_READ;
  return 0;
}
int DiskIO::read(void *buff,uint32_t buffer_size)
{
  uint64_t res=::read(hDisk,buff,buffer_size);
  if(res!=buffer_size)
      return ERR_READ;
  return 0;
}

int DiskIO::sync()
{

return 0;
}
int DiskIO::get_disk_length(uint64_t &ret)
{
if(disk_image==false)
  {
      uint64_t res;
      ret=1;
      if(ioctl(hDisk,BLKGETSIZE64,&res)!=0)
          return ERR_GET_DISK_SIZE;
      ret=res;
  }
else
  {
     ret=lseek(hDisk,0,SEEK_END);
     lseek(hDisk,0,SEEK_SET);

  }
return 0;
}
