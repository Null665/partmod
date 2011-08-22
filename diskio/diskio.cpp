#include "diskio.hpp"
#include <string.h>


//
// System-independent functions
//

DiskIO::DiskIO()
{
    hDisk=0;
    is_open=false;

    memset(&disk_geometry,0,sizeof(GEOMETRY));
}


int DiskIO::Open(const char* disk)
{
    if(open_handle(disk)!=0)
        return ERR_OPEN_DISK;
    if(get_geometry()!=0)
        return ERR_GEOMETRY_NOT_FOUND;
    is_open=true;
    disk_image=false;
    return 0;
}


int DiskIO::Open(const char *disk, GEOMETRY geom)
{
    if(open_handle(disk)!=0)
        return ERR_OPEN_DISK;
    disk_geometry=geom;
    is_open=true;
    disk_image=true;
    return 0;
}

int  DiskIO::GetDiskSize(uint64_t &length)
{
    int ret = get_disk_length(length);
    if(ret!=0)
        return ERR_GET_DISK_SIZE;
    length/=disk_geometry.bps;
    return ret;
}

bool DiskIO::IsOpen()
{
    return is_open;
}

void DiskIO::Close()
{
    close_handle();
    is_open=false;
    disk_image=false;
}

const GEOMETRY &DiskIO::GetDiskGeometry()
{
    return disk_geometry;
}

int DiskIO::Seek(uint64_t sector)
{
    return seek(sector);
}

int DiskIO::Read(void *buff,uint32_t buffer_size)
{
    return read(buff,buffer_size);
}

int DiskIO::Write(const void *buff,uint32_t buffer_size)
{
    return write(buff,buffer_size);
}


void DiskIO::Sync()
{
    sync();
}


