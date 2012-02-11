#include "fsid_manager.h"
#include "disk_exception.h"
#include <cstring>
using namespace std;


FsidManager::FsidManager() : vector()
{
  add_fsids();
}

bool FsidManager::Add(uint8_t fsid,
       string description,
       uint64_t min_size,
       uint64_t max_size,
       uint32_t partmod_fsid)
{
    FSID_INFO tmp={fsid,description,min_size,max_size,partmod_fsid};

    for(unsigned i=0;i<this->size();i++)
        if(memcmp(&this->operator[](i),&tmp,sizeof(tmp))==0)
            throw DiskException("Fsid already exists");

    this->push_back(tmp);
    return true;
}

const FSID_INFO &FsidManager::GetByFsid(int fsid) const
{
    for(unsigned i=0;i<this->size();i++)
        if( this->operator [](i).fsid==fsid)
            return this->operator[](i);
    throw DiskException("Unknown File System ID");
}

const FSID_INFO &FsidManager::GetByPartmodFsid(int fsid_partmod) const
{
   return GetByPartmodFsid(fsid_partmod,0);
}

const FSID_INFO &FsidManager::GetByPartmodFsid(int fsid_partmod,unsigned num) const
{
    for(unsigned i=0,j=0;i<this->size();i++)
        if( this->operator [](i).fsid_partmod==fsid_partmod)
            if(j==num)
                return this->operator[](i);
            else
                j++;
    throw DiskException("FsidManager: Not found");
}

uint32_t FsidManager::Count()
{
    return this->size();
}

uint32_t FsidManager::Count(unsigned fsid_partmod)
{
  unsigned c=0;
  for(unsigned i=0;i<this->size();i++)
      if(this->operator[](i).fsid_partmod==fsid_partmod)
          ++c;
  return c;
}

const FSID_INFO &FsidManager::Get(unsigned i) const
{
  return this->at(i);
}


string FsidManager::GetDescription(int i)
{
    return Get(i).description;
}

string FsidManager::GetDescription(int i,int fsid_partmod)
{
    return GetByPartmodFsid(fsid_partmod,i).description;
}

void FsidManager::add_fsids()
{
    #define _S(x) (x) // yes, _S does nothing. I am just too lazy to remove it
  //  Add( 0x00, _S("Empty"),             0,         0,         0              );
	Add( 0x01, _S("FAT12"),             MIN_SIZE, 16*MB,      FS_FAT12       );
	Add( 0x02, _S("XENIX root"),        -1,        -1,        0              );
	Add( 0x03, _S("XENIX usr") ,        -1,        -1,        0              );
	Add( 0x04, _S("Small FAT16"),       MIN_SIZE,  32*MB,     FS_FAT16       );
	Add( 0x05, _S("Extended"),          MIN_SIZE,  8.4*GB,    FS_EXTENDED    );
	Add( 0x06, _S("FAT16"),             32*MB,     2*GB,      FS_FAT16       );
	Add( 0x07, _S("HPFS/NTFS"),         MIN_SIZE,  (250*TB-64*KB),   FS_NTFS );
	Add( 0x08, _S("AIX"),               -1,        -1,        0              );
	Add( 0x09, _S("AIX bootable"),      -1,        -1,        0              );
	Add( 0x0a, _S("OS/2 boot mgr"),     -1,        -1,        0              );
	Add( 0x0b, _S("FAT32"),             MIN_SIZE,  2047*GB,   FS_FAT32       );
	Add( 0x0c, _S("FAT32 LBA"),         MIN_SIZE,  2047*GB,   FS_FAT32       );
	Add( 0x0e, _S("FAT16 LBA"),         32*MB,     2*GB,      FS_FAT16       );
	Add( 0x0f, _S("Extended LBA"),      8.5*GB,    MAX_LBA,   FS_EXTENDED    );
	Add( 0x10, _S("OPUS"),              -1,        -1,        0              );
	Add( 0x11, _S("Hidden FAT12"),      MIN_SIZE,  16*MB,     FS_FAT12       );
	Add( 0x12, _S("Compaq diag"),       MIN_SIZE,  100*MB,    0              );
	Add( 0x14, _S("Hidd Sm FAT16"),     MIN_SIZE,  32*MB,     FS_FAT16       );
	Add( 0x16, _S("Hidd FAT16"),        32*MB,     2*GB,      FS_FAT16       );
	Add( 0x17, _S("Hidd HPFS/NTFS"),    MIN_SIZE,  (250*TB-64*KB),   FS_NTFS );
	Add( 0x18, _S("AST SmartSleep"),    -1,        -1,        0              );
	Add( 0x1b, _S("Hidden FAT32"),      MIN_SIZE,  2047*GB,   FS_FAT32       );
	Add( 0x1c, _S("Hidden FAT32 LBA"),  MIN_SIZE,  2047*GB,   FS_FAT32       );
	Add( 0x1e, _S("Hidden FAT16 LBA"),  MIN_SIZE,  2*GB,      FS_FAT16       );
	Add( 0x24, _S("NEC DOS"),           -1,        -1,        0              );
	Add( 0x27, _S("PQservice/WinRE"),   -1,        -1,        0              );// FAT32 (PQService) or NTFS (Windows)
	Add( 0x2a, _S("AFS"),               -1,        -1,        0              );
	Add( 0x2b, _S("SyllableSecure"),    -1,        -1,        0              );
	Add( 0x39, _S("Plan 9"),            -1,        -1,        0              );
	Add( 0x3a, _S("THEOS v4"),          -1,        -1,        0              );
	Add( 0x3b, _S("THEOS v4 extended"), -1,        -1,        0              );
	Add( 0x3c, _S("PMagic recovery"),   -1,        -1,        0              );
	Add( 0x3d, _S("Hidden NetWare"),    -1,        -1,        0              );
	Add( 0x40, _S("Venix 80286"),       -1,        -1,        0              );
	Add( 0x41, _S("PPC PReP Boot"),     -1,        -1,        0              );
	Add( 0x42, _S("Dynamic"),           -1,        -1,        FS_DYNAMIC     );
	Add( 0x44, _S("GoBack"),            -1,        -1,        0              );
	Add( 0x4d, _S("QNX4.x"),            -1,        -1,        0              );
	Add( 0x4e, _S("QNX4.x 2nd part"),   -1,        -1,        0              );
	Add( 0x4f, _S("QNX4.x 3rd part"),   -1,        -1,        0              );
	Add( 0x50, _S("OnTrack DM"),        -1,        -1,        0              );
	Add( 0x51, _S("OnTrackDM6 Aux1"),   -1,        -1,        0              );
	Add( 0x52, _S("CP/M"),              -1,        -1,        0              );
	Add( 0x53, _S("OnTrackDM6 Aux3"),   -1,        -1,        0              );
	Add( 0x54, _S("OnTrack DM6"),       -1,        -1,        0              );
	Add( 0x55, _S("EZ Drive"),          -1,        -1,        0              );
	Add( 0x56, _S("AT&T FAT"),          -1,        -1,        0              );
	Add( 0x5c, _S("Priam Edisk"),       -1,        -1,        0              );
	Add( 0x61, _S("SpeedStor"),         -1,        -1,        0              );
	Add( 0x63, _S("GNU HURD/SysV"),     -1,        -1,        0              );
	Add( 0x64, _S("Netware 286"),       -1,        -1,        0              );
	Add( 0x65, _S("Netware 386"),       -1,        -1,        0              );
	Add( 0x70, _S("DiskSec MltBoot"),   -1,        -1,        0              );
	Add( 0x75, _S("IBM PC/IX"),         -1,        -1,        0              );
	Add( 0x80, _S("Minix <1.4a"),       -1,        -1,        0              );
	Add( 0x81, _S("Minix >1.4b"),       -1,        -1,        0              );
	Add( 0x82, _S("Linux swap"),        -1,        -1,        0              );
	Add( 0x83, _S("Linux"),             -1,        -1,        FS_LINUX       );
	Add( 0x84, _S("OS/2 hidden C:"),    -1,        -1,        0              );
	Add( 0x85, _S("Linux extended"),    -1,        -1,        FS_EXTENDED    ); // Linux Equivalent of 0x05
	Add( 0x86, _S("FAT volume set"),    -1,        -1,        0              );
	Add( 0x87, _S("NTFS volume set"),   -1,        -1,        0              );
	Add( 0x88, _S("Linux plaintext"),   -1,        -1,        0              );
	Add( 0x8e, _S("Linux LVM"),         -1,        -1,        0              );
	Add( 0x93, _S("Amoeba"),            -1,        -1,        0              );
	Add( 0x94, _S("Amoeba BBT"),        -1,        -1,        0              );
	Add( 0x9f, _S("BSD/OS"),            -1,        -1,        0              );
	Add( 0xa0, _S("Hibernation"),       -1,        -1,        0              );
	Add( 0xa1, _S("Hibernation"),       -1,        -1,        0              );
	Add( 0xa5, _S("FreeBSD"),           -1,        -1,        0              );
	Add( 0xa6, _S("OpenBSD"),           -1,        -1,        0              );
	Add( 0xa7, _S("NeXTSTEP"),          -1,        -1,        0              );
	Add( 0xa8, _S("Darwin UFS"),        -1,        -1,        0              );
	Add( 0xa9, _S("NetBSD"),            -1,        -1,        0              );
	Add( 0xab, _S("Darwin boot"),       -1,        -1,        0              );
	Add( 0xb7, _S("BSDI fs"),           -1,        -1,        0              );
	Add( 0xb8, _S("BSDI swap"),         -1,        -1,        0              );
	Add( 0xbb, _S("Boot Wizard Hidd"),  -1,        -1,        0              );
	Add( 0xbe, _S("Solaris boot"),      -1,        -1,        0              );
	Add( 0xbf, _S("Solaris"),           -1,        -1,        0              );
	Add( 0xc1, _S("DRDOS/2 FAT12"),     -1,        -1,        0              );
	Add( 0xc2, _S("Hidden Linux"),      -1,        -1,        0              );
	Add( 0xc3, _S("Hidden Linux swap"), -1,        -1,        0              );
	Add( 0xc4, _S("DRDOS/2 smFAT16"),   -1,        -1,        0              );
	Add( 0xc6, _S("DRDOS/2 FAT16"),     -1,        -1,        0              );
	Add( 0xc7, _S("Syrinx"),            -1,        -1,        0              );
	Add( 0xda, _S("Non-FS data"),       -1,        -1,        0              );
	Add( 0xdb, _S("CP/M / CTOS"),       -1,        -1,        0              );
	Add( 0xde, _S("Dell Utility"),      -1,        -1,        0              );
	Add( 0xdf, _S("BootIt"),            -1,        -1,        0              );
	Add( 0xe1, _S("DOS access"),        -1,        -1,        0              );
	Add( 0xe3, _S("DOS R/O"),           -1,        -1,        0              );
	Add( 0xe4, _S("SpeedStor"),         -1,        -1,        0              );
	Add( 0xeb, _S("BeOS fs"),           -1,        -1,        0              );
	Add( 0xee, _S("GPT"),               -1,        -1,        FS_GPT         );
	Add( 0xef, _S("EFI FS"),            -1,        -1,        0              );
	Add( 0xf0, _S("Lnx/PA-RISC bt"),    -1,        -1,        0              );
	Add( 0xf1, _S("SpeedStor"),         -1,        -1,        0              );
	Add( 0xf2, _S("DOS secondary"),     -1,        -1,        0              );
	Add( 0xf4, _S("SpeedStor"),         -1,        -1,        0              );
	Add( 0xfd, _S("Lnx RAID auto"),     -1,        -1,        0              );
	Add( 0xfe, _S("LANstep"),           -1,        -1,        0              );
	Add( 0xff, _S("XENIX BBT"),         -1,        -1,        0              );
	#undef _S
}
