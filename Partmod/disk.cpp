//
// Disk class implementation
//
//4899

#include "disk.h"
#include "definitions.h"
#include "disk_exception.h"
#include "chs.hpp"
#include "crc.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>
using namespace std;



void Sync(PartitionManager *partman,PendingActionManager *pending_man)
{
bool found=false;
uint32_t which;
uint32_t uid_two;
for(unsigned i=0;i<partman->CountPartitions();i++)
  {
      for(unsigned j=0;j<pending_man->CountPendingActions();j++)
        {
            pending_man->GetUID(j,uid_two);
            if(partman->GetPartitionUID(i)==pending_man->GetUID(j) && partman->GetPartitionUID(i)==uid_two)
              {
                  found=true;
                  which=j;
              }
        }
      if(found)
            pending_man->DeleteAction(which);
      found=false;
  }




}




//
//  initialize variables, constructors must call this function
//
void Disk::init()
{
  is_open=false;

  memset(&clbk,0,sizeof(clbk));

  ebr_parser=new EBRParser(this);
  gpt_parser=new GPTParser(this);

  diskio=new DiskIO;

  part_man=new PartitionManager;
  frs_man=new FreeSpaceManager;
  pending_man=new PendingActionManager;
  guid_man=new GuidManager;
  fsid_man=new FsidManager;
}


Disk::Disk()
{
  init();
}

Disk::Disk(string dsk)
{
  init();
  Open(dsk.c_str());
}

Disk::Disk(int dsk)
{
  init();

  stringstream ss;
  ss <<"\\\\.\\PhycicalDrive"<<dsk;
  pd_str=ss.str();
  Open(pd_str.c_str());
}




Disk::~Disk()
{
Close();
delete ebr_parser;
delete gpt_parser;

delete part_man;
delete frs_man;
delete pending_man;
}





void Disk::Open(string _dsk)
{
if(IsOpen())
    throw DiskException(ERR_ALREADY_OPEN);

pd_str=_dsk;

diskio->Open(_dsk.c_str());
if(!diskio->IsOpen())
    throw(DiskException(ERR_OPEN_DISK));

verify_geometry();
load();

find_free_space();
}


void Disk::Open(string _dsk,GEOMETRY _geom)
{
if(IsOpen())
    throw DiskException(ERR_ALREADY_OPEN);

pd_str=_dsk;

diskio->Open(_dsk.c_str(),_geom);
if(!diskio->IsOpen())
    throw(DiskException(ERR_OPEN_DISK));

verify_geometry();
load();
find_free_space();

}


bool Disk::IsOpen()
{
   return diskio->IsOpen();
}



void Disk::Close()
{

diskio->Close();
part_man->DeleteAll();
pending_man->DeleteAll();


is_open=false;
}


GEN_HANDLE Disk::GetDiskHandle()
{
GEN_HANDLE gh;
memset(&gh,0,sizeof(gh));

gh.hDisk=this->diskio;
gh.begin_sector=0;
gh.length=this->LastSector();

memcpy(&gh.disk_geometry,&GetDiskGeometry(),sizeof(gh.disk_geometry));
return gh;
}

void Disk::CheckDisk(vector<DISK_ERROR> &error_list)
{
if(CountPartitions()==0)
    return;

GEN_PART part_curr,part_next;
DISK_ERROR de;

// Check for overlapping
for(int i=0;i<CountPartitions()-1;i++)
  {
     part_curr=GetPartition(i);
     part_next=GetPartition(i+1);

     if(part_curr.begin_sector+part_curr.length>part_next.begin_sector)
       {
           if(part_curr.flags&PART_EXTENDED && part_next.flags&PART_LOGICAL)
               break;
           if(part_curr.flags&PART_MBR_GPT && part_next.flags&PART_GPT)
               break;

           de.error_code=ERR_PARTITIONS_OVERLAP;
           de.partition=i;
           de.partition_nxt=i+1;
           error_list.push_back(de);
       }
  }

// Make sure that no partition is outside disk
for(int i=0;i<CountPartitions();i++)
  {
     part_curr=GetPartition(i);
     if(part_curr.begin_sector+part_curr.length>this->LastSector())
       {
           de.error_code=ERR_OUTSIDE_DISK;
           de.partition=CountPartitions()-1;
           de.partition_nxt=-1;
           error_list.push_back(de);
       }

  }

// Check for logical volumes outside the extended partition
GEN_PART part_extended;
try
  {
    part_extended=part_man->GetExtendedPartition();
    for(int i=0;i<CountPartitions();i++)
      {
          part_curr=GetPartition(i);
          if( !(part_curr.flags&PART_LOGICAL))
              continue;
          if(part_curr.begin_sector+part_curr.length>part_extended.begin_sector+part_extended.length ||
             part_curr.begin_sector<part_extended.begin_sector)
            {
                de.error_code=ERR_OUTSIDE_EXTENDED;
                de.partition=i;
                de.partition_nxt=-1;
                error_list.push_back(de);
            }

      }
  }
catch(DiskException &de)
  {
       // ???
  }


// Check the disk geometry
try
  {
     verify_geometry();
  }
catch(DiskException &e)
  {
    if(e.error_code==ERR_BPS_NOT_POW_OF_TWO)
      {
          de.error_code=e.error_code;
          de.partition=-1;
          de.partition_nxt=-1;
          error_list.push_back(de);
      }

  }
catch(...)
  {
      // TODO: do something here
  }


// what else?


}



void Disk::CreatePartitionPrimary(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid)
{
  GEN_PART new_part;
  MBR_SPECIFIC mspec;
  const FREE_SPACE &frs=GetFreeSpace(which_frs);

  if(frs.type!=FREE_UNALLOCATED)
      throw DiskException(ERR_UNKNOWN_ERROR);

  if(frs.length<size+sect_before)
      throw DiskException(ERR_PART_TOO_BIG);

  new_part.begin_sector=frs.begin_sector+sect_before;
  new_part.length=size;
  new_part.fsid=fsid_man->GetByFsid(fsid).fsid_partmod;
  new_part.flags=PART_PRIMARY;

  mspec.begin_sector_rel=0;
  mspec.fsid=fsid;

  memcpy(new_part.data,&mspec,sizeof(mspec));

  add_partition(new_part);

}

void Disk::CreatePartitionExtended(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid)
{
  GEN_PART new_part;
  MBR_SPECIFIC mspec;
  const FREE_SPACE &frs=GetFreeSpace(which_frs);

  if(frs.type!=FREE_UNALLOCATED)
      throw DiskException(ERR_UNKNOWN_ERROR);

  if(frs.length<size+sect_before)
      throw DiskException(ERR_PART_TOO_BIG);

  if(fsid_man->GetByFsid(fsid).fsid_partmod!=FS_EXTENDED)
      throw DiskException("File system id "+U64ToStr(fsid)+" is not compatible with selected partition type");

  new_part.begin_sector=frs.begin_sector+sect_before;
  new_part.length=size;
  new_part.fsid=fsid_man->GetByFsid(fsid).fsid_partmod;
  new_part.flags=PART_EXTENDED;

  mspec.begin_sector_rel=0;
  mspec.fsid=fsid;

  memcpy(new_part.data,&mspec,sizeof(mspec));

  add_partition(new_part);

}

void Disk::CreatePartitionLogical(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid)
{
  GEN_PART new_part;
  MBR_SPECIFIC mspec;
  const FREE_SPACE &frs=GetFreeSpace(which_frs);

  if(frs.type!=FREE_EXTENDED)
      throw DiskException(ERR_UNKNOWN_ERROR);

  if(frs.length<size+sect_before)
      throw DiskException(ERR_PART_TOO_BIG);

  new_part.begin_sector=frs.begin_sector+sect_before;
  new_part.length=size;
  new_part.fsid=fsid_man->GetByFsid(fsid).fsid_partmod;
  new_part.flags=PART_LOGICAL;

  mspec.begin_sector_rel=0;
  mspec.fsid=fsid;

  memcpy(new_part.data,&mspec,sizeof(mspec));

  add_partition(new_part);
}


void Disk::CreatePartitionGPT(int which_frs,uint64_t size,uint64_t sect_before,__GUID type_guid)
{
  GEN_PART new_part;
  GPT_SPECIFIC gspec;
  const FREE_SPACE &frs=GetFreeSpace(which_frs);

  if(frs.type!=FREE_GPT)
      throw DiskException(ERR_UNKNOWN_ERROR);

  if(frs.length<size+sect_before)
      throw DiskException(ERR_PART_TOO_BIG);

  new_part.begin_sector=frs.begin_sector+sect_before;
  new_part.length=size;
  new_part.fsid=FS_FAT16;
  new_part.flags=PART_GPT;

  gspec.type_guid=type_guid;
  generate_guid(gspec.unique_guid);
  gspec.flags=0;

  wchar_t name[]={L"PartmodPartition"};
  memcpy(&gspec.name,name,sizeof(name));

  memcpy(new_part.data,&gspec,sizeof(gspec));

  add_partition(new_part);
}




void Disk::CreatePartition(FREE_SPACE frs,int part_type,uint64_t size)
{
find_free_space();

GEN_PART new_part;
GEN_PART mbr_extended_part;

new_part.begin_sector=frs.begin_sector;
new_part.length=size;
new_part.flags=0;
new_part.fsid=FS_FAT16;

MBR_SPECIFIC mspec;


if(frs.length<size)
    throw(DiskException(ERR_PART_TOO_BIG));

switch(part_type)
  {
     case PART_PRIMARY:

         if(frs.type==FREE_EXTENDED)
             throw(DiskException(ERR_PRIMARY_ON_EXTENDED));
         else if(frs.type==FREE_UNALLOCATED)
         {
             new_part.flags=PART_PRIMARY;
             mspec.fsid=FSID_DEFAULT;
             memcpy(new_part.data,&mspec,sizeof(mspec));
         }
         else throw(DiskException(ERR_UNKNOWN_ERROR));

     break;
     case PART_MBR_GPT:

         if(frs.length<sizeof(GPT)+sizeof(GPT_ENTRY)*128+(MB/GetDiskGeometry().bps))
             throw(DiskException(ERR_PART_TOO_SMALL));

         if(frs.type==FREE_UNALLOCATED)
           {
              new_part.flags=PART_MBR_GPT;
              mspec.fsid=FSID_GPT;
              memcpy(new_part.data,&mspec,sizeof(mspec));
           }
          else throw(DiskException(ERR_UNKNOWN_ERROR));
     break;
     case PART_GPT:
       //  mbr_extended_part=get_extended();
         if(frs.type==FREE_GPT)
           {
              GPT_SPECIFIC gspec;
              generate_guid(gspec.unique_guid);
              gspec.type_guid=guid_man->Get(5);

              new_part.flags=PART_GPT;
              memcpy(new_part.data,&gspec,sizeof(gspec));
           }
         else throw(DiskException(ERR_UNKNOWN_ERROR));

     break;
     case PART_LOGICAL:
         mbr_extended_part=part_man->GetExtendedPartition();
         if(frs.type==FREE_EXTENDED)
           {
              mspec.begin_sector_rel=mbr_extended_part.begin_sector;
              new_part.flags=PART_LOGICAL;
              memcpy(new_part.data,&mspec,sizeof(mspec));
           }
         else throw(DiskException(ERR_UNKNOWN_ERROR));
   break;

    case PART_EXTENDED:    // MBR extended partition
        if(frs.type==FREE_UNALLOCATED && frs.length>=size)
          {
            mspec.begin_sector_rel=mbr_extended_part.begin_sector;
            new_part.flags=PART_EXTENDED;
            memcpy(new_part.data,&mspec,sizeof(mspec));
          }
        else throw(DiskException(ERR_UNKNOWN_ERROR));
   break;

     default: throw(DiskException(ERR_UNKNOWN_ERROR));
  }


new_part.flags|=PART_NOT_FORMATTED;
new_part.fsid=FS_FAT16;
add_partition(new_part);

}



void Disk::find_free_space()
{
    frs_man->FindFreeSpace(part_man,LastSector(),GetDiskGeometry().bps,GetDiskGeometry().spt);
}




void Disk::Split(unsigned int _p,uint64_t size_of_first_part)
{
GEN_PART old_part=GetPartition(_p);
GEN_PART new_part;

if(size_of_first_part>=old_part.length)
    throw(DiskException(ERR_CANNOT_SPLIT));


new_part.begin_sector=old_part.begin_sector+size_of_first_part;
new_part.length=old_part.length-size_of_first_part;
new_part.flags=old_part.flags & (~PART_ACTIVE) &(~PART_EXTENDED) & (~PART_MBR_GPT);
old_part.length=size_of_first_part;

modify_partition(_p,old_part);
add_partition(new_part);

}



int Disk::Save()
{


// STEP 1: MBR
    save_mbr();
// STEP 2: EBR (if there are extended partitions)
    ebr_parser->WriteChanges();
// STEP 3: GPT (if exists)
    gpt_parser->WriteChanges();

do_pending();
diskio->Sync();
return true;
}









//
// Read partition tables
//
bool  Disk::load()
{

// load mbr
MBR mbr;
ReadMBR(mbr);
GEN_PART gpart;
MBR_SPECIFIC mspec;

for(int i=0;i<4;i++)
  {
     if(mbr.partition_table[i].lba_blocks==0)
         continue;
     gpart.begin_sector=mbr.partition_table[i].begin_lba;
     gpart.length=mbr.partition_table[i].lba_blocks;
     gpart.flags=0;
     mspec.fsid=mbr.partition_table[i].partition_type;

     gpart.fsid=fsid_man->GetByFsid(mbr.partition_table[i].partition_type).fsid_partmod;


     switch(gpart.fsid)
       {
          case FS_EXTENDED:
            gpart.flags|=PART_EXTENDED;
            break;
          case FS_GPT:
            gpart.flags|=PART_MBR_GPT;
            break;

         default:
            gpart.flags|=PART_PRIMARY;
            break;
       }

     mbr.partition_table[i].status==0x80 ? gpart.flags|=PART_ACTIVE : gpart.flags&=~PART_ACTIVE;

     add_partition(gpart);
     set_mbr_specific(CountPartitions()-1,mspec);

     if(gpart.flags&PART_EXTENDED)
         ebr_parser->ParsePartition(gpart);
     else if(gpart.flags&PART_MBR_GPT)
         gpt_parser->ParsePartition(gpart);

  }



is_open=true;
return true;
}








GEN_HANDLE Disk::GetPartitionHandle(unsigned int p)
{

GEN_HANDLE tmp;
memset(&tmp,0,sizeof(tmp));

tmp.hDisk=diskio;

tmp.begin_sector=GetPartition(p).begin_sector;
tmp.length=GetPartition(p).length;
tmp.fsid=GetPartition(p).fsid;
tmp.flags=GetPartition(p).flags;

memcpy(&tmp.disk_geometry,&GetDiskGeometry(),sizeof(GEOMETRY));
return tmp;
}

void Disk::Move(unsigned p,uint64_t new_pos)
{
GEN_PART gpart=GetPartition(p);
gpart.begin_sector=new_pos;
modify_partition(p,gpart);

}




uint64_t Disk::LastSector() const
{
uint64_t length;
int ret = diskio->GetDiskSize(length);
if(ret!=0)
    throw(DiskException(ret));
return length;

}




void Disk::DiskRead(uint64_t offset,void* data,int size)
{
if(diskio->Seek(offset)!=0)
    throw(DiskException(ERR_INVALID_SEEK));
if(diskio->Read(data,size)!=0)
    throw(DiskException(ERR_READ));

}



void Disk::DiskWrite(uint64_t offset,void* data,int size)
{
if(diskio->Seek(offset)!=0)
    throw(DiskException(ERR_INVALID_SEEK));
if(diskio->Write(data,size)!=0)
    throw(DiskException(ERR_WRITE));

}


void Disk::SetActive(unsigned int p, bool set_active)
{
   part_man->SetActive(p,set_active);
}

const FREE_SPACE &Disk::GetFreeSpace(unsigned int s)
{
  return frs_man->GetFreeSpace(s);
}


int Disk::CountFreeSpaces()
{
  return frs_man->CountFreeSpaces();
}



const GEN_PART &Disk::GetPartition(unsigned int _p)
{
   return part_man->GetPartition(_p);
}


int Disk::CountPartitions(unsigned int type) const
{
   return part_man->CountPartitions(type);
}


int Disk::CountPartitions() const
{
   return part_man->CountPartitions();
}


void Disk::DeletePartition(unsigned int _p)
{
   part_man->DeletePartition(_p);
   Sync(part_man,pending_man);
   find_free_space();
}




unsigned int Disk::GetDiskSignature()
{
  MBR mbr;
  ReadMBR(mbr);

  return mbr.disk_signature;
}




int Disk::GetDiskType() const
{
  unsigned part_gpt=CountPartitions(PART_MBR_GPT);
  unsigned part_mbr=CountPartitions(PART_PRIMARY|PART_EXTENDED);

  if(part_gpt==0)
      return DISK_MBR;
  else if(part_gpt!=0 && part_mbr!=0)
      return DISK_HYBRID;
  else return DISK_MBR;

}


string Disk::GetPhysicalDriveString() const
{
    return pd_str;
}

const GEOMETRY &Disk::GetDiskGeometry() const
{
    return diskio->GetDiskGeometry();
}


void Disk::modify_partition(unsigned int _p,GEN_PART new_data)
{
    part_man->ModifyPartition(_p,new_data);
}

void Disk::add_partition(GEN_PART new_part)
{
   part_man->AddPartition(new_part);
   find_free_space();
}






void Disk::CreateBackup(std::string filename)
{
BACKUP_FILE_HEADER bfh;
unsigned int n_partitions=CountPartitions();
ofstream file;
CRC32 crc32;

bfh.magic=BACKUP_MAGIC;
bfh.dg=GetDiskGeometry();
bfh.n_partitions=n_partitions;
bfh.n_sect=LastSector();
bfh.checksum=0;

crc32.Hash(&bfh,sizeof(bfh));
for(unsigned i=0;i<n_partitions;i++)
    crc32.Hash(&GetPartition(i),sizeof(GEN_PART));
bfh.checksum=crc32.Get();

file.open(filename.c_str(),ios::binary);

if(!file.is_open())
    throw(DiskException(ERR_CREATE_FILE));

file.write((char*)&bfh,sizeof(BACKUP_FILE_HEADER));

for(unsigned i=0;i<n_partitions;i++)
    file.write((char*)&GetPartition(i),sizeof(GEN_PART));

file.close();
}

void Disk::LoadBackup(std::string filename)
{
BACKUP_FILE_HEADER bfh;
GEN_PART gpart;
CRC32 crc32;
uint32_t old_checksum;
ifstream file(filename.c_str(),ios::binary);

if(!file.is_open())
    throw(DiskException(ERR_OPEN_FILE));

file.read((char*)&bfh,sizeof(BACKUP_FILE_HEADER));
if(file.fail())
    throw(DiskException(ERR_READ));

if(bfh.magic!=BACKUP_MAGIC)
    throw(DiskException(ERR_INVALID_BACKUP));
if(bfh.n_sect!=LastSector())
    throw(DiskException(ERR_NOT_THAT_BACKUP));

old_checksum=bfh.checksum;
bfh.checksum=0;
crc32.Hash(&bfh,sizeof(bfh));

part_man->DeleteAll();

for(unsigned i=0;i<bfh.n_partitions;i++)
  {
     file.read((char*)&gpart,sizeof(GEN_PART));
     crc32.Hash(&gpart,sizeof(GEN_PART));
     add_partition(gpart);
  }

if(old_checksum!=crc32.Get())
  {
      part_man->DeleteAll();
      throw(DiskException(ERR_INVALID_CHECKSUM));
  }



file.close();
}

//-------------------------------------------------------------------

void Disk::Wipe(unsigned int _p,int method)
{
    pending_man->WipePartition(part_man->GetPartitionUID(_p),method);
}


void Disk::do_pending()
{
GEN_PART gpart;
PENDING curr;
uint32_t uid_one;
for(unsigned i=0;i<pending_man->CountPendingActions();i++)
  {
      try
        {
           uid_one= GetPendingUID(i);
           gpart=GetPartitionByUID(uid_one);
           curr=GetPending(i);
        }
       catch(...)
         {
             continue;
         }
      switch(curr.action)
        {
            case PENDING_WIPE:
                wipe(part_man->WhichByUID(uid_one),gpart.begin_sector,gpart.length,curr.specific);
                break;

        }
  }
memset(&clbk,0,sizeof(clbk));
pending_man->DeleteAll();
}



void Disk::wipe(unsigned p,uint64_t begin_sect,uint64_t length,int method)
{
   int ret;
   int bps=GetDiskGeometry().bps;
   const int mul=128;
   int buff_size=mul*bps;
   uint8_t *buff=new uint8_t[buff_size];
   uint32_t liko=(length%mul);

   clbk.current=0;
   clbk.total=length;
   clbk.event=PENDING_WIPE;
   clbk.partition=p;

   ret=diskio->Seek(begin_sect*bps);
   if(ret!=0)
       throw(DiskException(ret));

   memset(buff,0,buff_size);

   length-=liko;

   while(length>0)
     {
        if(method!=WIPE_ZEROS)
          {

          }
        ret=diskio->Write(buff,buff_size);
        if(ret!=0)
            throw(DiskException(ret));

        length-=mul;
        clbk.current+=mul;
     }
// Wipe what's left
ret=diskio->Write(buff,liko*bps);
if(ret!=0)
    throw(DiskException(ret));

length-=liko;
clbk.current+=liko;
   delete[] buff;
}




void Disk::verify_geometry()
{
int bps=GetDiskGeometry().bps;
int n=2;
while(true)
  {
     n*=2;
     if(n==bps)
         break;
     else if(n>bps)
       {
          this->Close();
          throw(DiskException(ERR_BPS_NOT_POW_OF_TWO));
       }

  }
}


const GEN_PART &Disk::GetPartitionByUID(unsigned uid)
{
 return  part_man->GetPartitionByUID(uid);

}
uint32_t Disk::GetPartitionUID(unsigned p)
{
    return part_man->GetPartitionUID(p);
}

unsigned Disk::CountPendingActions()
{
    return pending_man->CountPendingActions();
}

const PENDING &Disk::GetPending(unsigned p)
{
    return pending_man->GetPendingAction(p);
}

uint32_t Disk::GetPendingUID(unsigned p)
{
    return pending_man->GetUID(p);
}
