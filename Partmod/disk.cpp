//
// Disk class implementation
//

#include "disk.h"
#include "definitions.h"
#include "disk_exception.h"
#include "chs.hpp"
#include "crc.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>
#include <memory>
using namespace std;


///
/// \brief Align partition to megabyte, cylinder, or track boundary.
/// \return aligned sector
uint64_t align_to(uint64_t sector,const GEOMETRY &dg,unsigned align_type)
{
  uint64_t div;

  switch(align_type)
    {
      case ALIGN_SECTOR:
        return sector;
        break;
      case ALIGN_TRACK:
        div=dg.spt;
        break;
      case ALIGN_CYLINDER:
        div=dg.spt*dg.tpc;
        break;
      case ALIGN_MEGABYTE:
        div=MB/dg.bps;
        break;
    }
  return sector+=div-(sector%div);
}




//
//  initialize variables, constructors must call this function
//
void Disk::init()
{
  memset(&clbk,0,sizeof(clbk));

  ebr_helper=new EBRHelper(this);
  mbr_helper=new MBRHelper(this);
  gpt_helper=new GPTHelper(this);

  diskio=new DiskIO;

  part_man=new PartitionManager;
  frs_man=new FreeSpaceManager;
  pending_man=new PendingActionManager;
  guid_man=new GuidManager;
  fsid_man=new FsidManager;

  error_on_load=false;
}


Disk::Disk()
{
  init();
}

Disk::Disk(string dsk)
{
  init();
  Open(dsk);
}



Disk::~Disk()
{
Close();
delete mbr_helper;
delete ebr_helper;
delete gpt_helper;

delete part_man;
delete frs_man;
delete pending_man;
}




///
/// \brief Open a disk
/// \param drive string (\\.\PhysicalDriveX or /dev/[s,h]dX )
void Disk::Open(string dsk)
{
if(IsOpen())
    throw DiskException(ERR_ALREADY_OPEN);

error_on_load=false;

pd_str=dsk;

unsigned ret=diskio->Open(dsk.c_str());
/*
if(ret!=0)
    throw DiskException(ret);*/
if(!diskio->IsOpen())
    throw(DiskException(ERR_OPEN_DISK));

verify_geometry();
try{
    load();
}catch(DiskException &de)
{
   error_on_load=true;
   find_free_space(); // even though partition search fails, try to find free space
   throw de; // let soemone else handle this
}

}

///
/// \brief Open a disk image
/// \param dsk disk image file path
/// \param geom disk image geometry (bytes per sector, sectors per track, tracks per cylinder, number of cylinders)
void Disk::OpenDiskImage(string dsk,GEOMETRY geom)
{
if(IsOpen())
    throw DiskException(ERR_ALREADY_OPEN);

pd_str=dsk;

error_on_load=false;

diskio->Open(dsk.c_str(),geom);
if(!diskio->IsOpen())
    throw(DiskException(ERR_OPEN_DISK));
try{
    verify_geometry();
    load();
    find_free_space();
}
catch(...)
{
    error_on_load=true;
    throw;
}

}

///
/// \brief Check whether disk is open
/// \return true if open
bool Disk::IsOpen()
{
   return diskio->IsOpen();
}



void Disk::Close()
{
diskio->Close();
part_man->DeleteAll();
pending_man->DeleteAll();
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
    part_extended=part_man->GetPartition(0,PART_EXTENDED);
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


/** \brief Create a primary partition
 *
 * \param  which_frs - which free space slice to use
 * \param  size - partition size in disk sectors
 * \param  sect_before - number of sectors between beginning of the free space slice and partition
 * \param  fsid - MBR(msdos) file system id
 * \param alignment - align beginning of partition to boundary (megabyte, cylinder, ...)
 *
 */
void Disk::CreatePartitionPrimary(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid,unsigned alignment)
{
  GEN_PART new_part;
  MBR_SPECIFIC mspec;
  const FREE_SPACE &frs=GetFreeSpace(which_frs);

  if(frs.type!=FREE_UNALLOCATED)
      throw DiskException(ERR_UNKNOWN_ERROR);

  if(frs.length<size+sect_before)
      throw DiskException(ERR_PART_TOO_BIG);

  uint64_t begin_unaligned=frs.begin_sector+sect_before;
  new_part.begin_sector=align_to(begin_unaligned,GetDiskGeometry(),alignment);

//  new_part.begin_sector=frs.begin_sector+sect_before;
  new_part.length=size-(new_part.begin_sector-begin_unaligned);
  new_part.fsid=fsid_man->GetByFsid(fsid).fsid_partmod;
  new_part.flags=PART_PRIMARY;

  mspec.begin_sector_rel=0;
  mspec.fsid=fsid;

  memcpy(new_part.data,&mspec,sizeof(mspec));

  pending_man->CreatePartition(new_part,add_partition(new_part));
}

/** \brief Create an exztended partition
 *
 * \param  which_frs - which free space slice to use
 * \param  size - partition size in disk sectors
 * \param  sect_before - number of sectors between beginning of the free space slice and partition
 * \param  fsid - MBR(msdos) file system id
 * \param alignment - align beginning of partition to boundary (megabyte, cylinder, ...)
 *
 */
void Disk::CreatePartitionExtended(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid,unsigned alignment)
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

  uint64_t begin_unaligned=frs.begin_sector+sect_before;
  new_part.begin_sector=align_to(begin_unaligned,GetDiskGeometry(),alignment);
  new_part.length=size-(new_part.begin_sector-begin_unaligned);

  new_part.fsid=fsid_man->GetByFsid(fsid).fsid_partmod;
  new_part.flags=PART_EXTENDED;

  mspec.begin_sector_rel=0;
  mspec.fsid=fsid;

  memcpy(new_part.data,&mspec,sizeof(mspec));

  add_partition(new_part);
  pending_man->CreatePartition(new_part,get_partition_uid(CountPartitions()-1));
}


/** \brief Create a GUID partition table
 *
 * \param  which_frs - which free space slice to use
 * \param  size - partition size in disk sectors. If 0, then the partition will span entire disk
 */
void Disk::CreatePartitionPrimaryGPT(int which_frs,uint64_t size)
{
  GEN_PART new_part;
  MBR_SPECIFIC mspec;
  const FREE_SPACE &frs=GetFreeSpace(which_frs);

  if(frs.type!=FREE_UNALLOCATED)
      throw DiskException(ERR_UNKNOWN_ERROR);

  if(frs.length<size)
      throw DiskException(ERR_PART_TOO_BIG);

  //new_part.begin_sector=align_to(begin_unaligned,GetDiskGeometry(),ALIGN_MEGABYTE);
  new_part.begin_sector=frs.begin_sector;

  if(size==0)
    new_part.length=LastSector()-frs.begin_sector;
  else
      new_part.length=size-(new_part.begin_sector);

  new_part.fsid=FS_GPT;
  new_part.flags=PART_MBR_GPT;

  mspec.begin_sector_rel=0;
  mspec.fsid=fsid_man->GetByPartmodFsid(FS_GPT).fsid;

  memcpy(new_part.data,&mspec,sizeof(mspec));

  add_partition(new_part);
  pending_man->CreatePartition(new_part,get_partition_uid(CountPartitions()-1));
}

/** \brief Create a logical partition
 *
 * \param  which_frs - which free space slice to use
 * \param  size - partition size in disk sectors
 * \param  sect_before - number of sectors between beginning of the free space slice and partition
 * \param  fsid - MBR(msdos) file system id
 * \param alignment - align beginning of partition to boundary (megabyte, cylinder, ...)
 *
 */
void Disk::CreatePartitionLogical(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid,unsigned alignment)
{
  GEN_PART new_part;
  MBR_SPECIFIC mspec;
  const FREE_SPACE &frs=GetFreeSpace(which_frs);

  if(frs.type!=FREE_EXTENDED)
      throw DiskException(ERR_UNKNOWN_ERROR);

  if(frs.length<size+sect_before)
      throw DiskException(ERR_PART_TOO_BIG);

  uint64_t begin_unaligned=frs.begin_sector+sect_before;

  new_part.begin_sector=align_to(begin_unaligned,GetDiskGeometry(),alignment);
  new_part.begin_sector=begin_unaligned;

  new_part.length=size-(new_part.begin_sector-begin_unaligned);

  new_part.fsid=fsid_man->GetByFsid(fsid).fsid_partmod;
  new_part.flags=PART_LOGICAL;

  mspec.begin_sector_rel=0;
  mspec.fsid=fsid;

  memcpy(new_part.data,&mspec,sizeof(mspec));

  add_partition(new_part);
  pending_man->CreatePartition(new_part,get_partition_uid(CountPartitions()-1));
}


/** \brief Create a GPT partition
 *
 * \param  which_frs - which free space slice to use
 * \param  size - partition size in disk sectors
 * \param  sect_before - number of sectors between beginning of the free space slice and partition
 * \param  type_guid - GUID that identifies partition type (not unique GUID)
 * \param alignment - align beginning of partition to boundary (megabyte, cylinder, ...)
 *
 */
void Disk::CreatePartitionGPT(int which_frs,uint64_t size,uint64_t sect_before,__GUID type_guid,unsigned alignment)
{
  GEN_PART new_part;
  GPT_SPECIFIC gspec;
  const FREE_SPACE &frs=GetFreeSpace(which_frs);

  if(frs.type!=FREE_GPT)
      throw DiskException(ERR_UNKNOWN_ERROR);

  if(frs.length<size+sect_before)
      throw DiskException(ERR_PART_TOO_BIG);

  uint64_t begin_unaligned=frs.begin_sector+sect_before;
  new_part.begin_sector=align_to(begin_unaligned,GetDiskGeometry(),alignment);
  new_part.length=size-(new_part.begin_sector-begin_unaligned);

  new_part.fsid=FS_FAT16;
  new_part.flags=PART_GPT;

  gspec.type_guid=type_guid;
  generate_guid(gspec.unique_guid);
  gspec.flags=0;

  wchar_t name[]={L"PartmodPartition"};
  memcpy(&gspec.name,name,sizeof(name));

  memcpy(new_part.data,&gspec,sizeof(gspec));

  add_partition(new_part);
  pending_man->CreatePartition(new_part,get_partition_uid(CountPartitions()-1));
}


int Disk::Save()
{
// STEP 1:
// process pending actions (wipe, move, ...) but not PENDING_CREATE and PENDING_DELETE
// those are handled by helpers
   do_pending();
// STEP 2: MBR
    mbr_helper->WriteChanges();
// STEP 3: EBR (if there are extended partitions)
    ebr_helper->WriteChanges();
// STEP 4: GPT (if exists)
    gpt_helper->WriteChanges();


diskio->Sync();
return true;
}




///
/// \brief Move partition
/// \param p - partition index
/// \param new_pos - new beginning sector of the partition
void Disk::Move(unsigned p,uint64_t new_pos)
{
GEN_PART gpart=GetPartition(p);
gpart.begin_sector=new_pos;
modify_partition(p,gpart);

}


///
/// \brief Get number of sectors on disk
///
uint64_t Disk::LastSector() const
{
uint64_t length;
int ret = diskio->GetDiskSize(length);
if(ret!=0)
    throw(DiskException(ret));
return length;

}


/** \brief Read data from disk
 *
 * \param offset - where to begin reading data
 * \param data - data buffer
 * \param size - size of the data buffer, number of bytes to read
 *
 */
void Disk::DiskRead(uint64_t offset,void* data,unsigned size)
{
if(diskio->Seek(offset)!=0)
    throw(DiskException(ERR_INVALID_SEEK));
if(diskio->Read(data,size)!=0)
    throw(DiskException(ERR_READ));

}


/** \brief Write data from disk
 * \warning This function does not create a pending operation! Data will be written INSTANTLY
 * \param offset - where to begin writing data
 * \param data - data buffer
 * \param size - size of the data buffer, number of bytes to read
 *
 */
void Disk::DiskWrite(uint64_t offset,const void* data,unsigned size)
{
if(diskio->Seek(offset)!=0)
    throw(DiskException(ERR_INVALID_SEEK));
if(diskio->Write(data,size)!=0)
    throw(DiskException(ERR_WRITE));
}

///
/// \brief Set active (bootable) partition
/// \param p - partition index
/// \param set_active - if true, partition will be set active, if false 'active' flag will be disabled
void Disk::SetActive(unsigned int p, bool set_active)
{
   uint32_t flags_old=GetPartition(p).flags;
   part_man->SetActive(p,set_active);
   pending_man->ModifyFlags(part_man->GetPartitionUID(p),GetPartition(p).flags,flags_old);
}

///
/// \brief Get a free space slice
/// \param s - free space slice index
/// \return FREE_SPACE struct that describes the free space slice
/// \throw DiskException error code ERR_OUT_OF_BOUNDS if index is too large
const FREE_SPACE &Disk::GetFreeSpace(unsigned int s)
{
  return frs_man->GetFreeSpace(s);
}

///
/// \brief Return number of free space slices on disk
///
unsigned Disk::CountFreeSpaces()
{
  return frs_man->CountFreeSpaces();
}


///
/// \brief Get a partition
/// \param s - partition index
/// \return GEN_PART struct that describes the partition
const GEN_PART &Disk::GetPartition(unsigned int p)
{
   return part_man->GetPartition(p);
}

///
/// \brief Get nth partition that has specified flag
/// \param s partition index
/// \param flag partition flag
/// \return GEN_PART struct that describes the partition
const GEN_PART &Disk::GetPartition(unsigned int p,uint32_t flags)
{
   return part_man->GetPartition(p,flags);
}

///
/// \brief Get number of partitions on disk
unsigned Disk::CountPartitions() const
{
   return part_man->CountPartitions();
}


///
/// \brief Get number of partitions on diskthat have specified flag
/// \param flag partition flag (GEN_PART::flags)
unsigned Disk::CountPartitions(unsigned int part_flags) const
{
   return part_man->CountPartitions(part_flags);
}


///
/// \brief Delete a partition from disk
/// \param p partition index
/// \warning deletion of the extended partition will cause deletion of all logical partitions
void Disk::DeletePartition(unsigned p)
{
  GEN_PART gpart=GetPartition(p);

  if( (gpart.flags&PART_EXTENDED)==false && (gpart.flags&PART_MBR_GPT)==false)
    {
       pending_man->DeletePartition(GetPartition(p));
       part_man->DeletePartition(p);

       find_free_space();
       return;
    }

  int nflag=0;
  std::vector<GEN_PART>nested_part;

  if(gpart.flags&PART_EXTENDED)
      nflag=PART_LOGICAL;
  else if(gpart.flags&PART_MBR_GPT)
      nflag=PART_GPT;
  else return;

  for(int i=0;i<CountPartitions(nflag);i++)
      nested_part.push_back(GetPartition(i,nflag));

  pending_man->DeletePartition(GetPartition(p),nested_part);
  part_man->DeletePartition(p);
  find_free_space();
}



/// \brief Get MBR disk signature (MBR::disk_signature)
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

  if(part_gpt==1)
      return DISK_GPT;
  else if(part_gpt!=0 && part_mbr!=0)
      return DISK_HYBRID;
  else return DISK_MBR;

}

/// Get physical drive string (/dev/[s,h]dX or \\.\PhysicalDriveX )
string Disk::GetPhysicalDriveString() const
{
    return pd_str;
}

/// \brief Get disk geometry information
const GEOMETRY &Disk::GetDiskGeometry() const
{
    return diskio->GetDiskGeometry();
}


/** \brief Create a partition table backup
 *
 * \param filename File name(path) of the backup file
 * \param description A short description of the backup file (optional)
 * \throw DiskexcEption error code ERR_CREATE_FILE if failed to create the backup file
 */
void Disk::CreateBackup(std::string filename, std::string description)
{
    BACKUP_FILE_HEADER bfh;
    memset(&bfh,0,sizeof(bfh));

    unsigned int n_partitions=CountPartitions();
    ofstream file;
    CRC32 crc32;

    bfh.magic=BACKUP_MAGIC;
    bfh.header_size=sizeof(BACKUP_FILE_HEADER);
    bfh.dg=GetDiskGeometry();
    bfh.n_partitions=n_partitions;
    bfh.n_sect=LastSector();
    bfh.checksum=0;
    strncpy(bfh.description,description.c_str(),sizeof(bfh.description));

    crc32.Hash(&bfh,sizeof(bfh));
    for(unsigned i=0; i<n_partitions; i++)
        crc32.Hash(&GetPartition(i),sizeof(GEN_PART));
    bfh.checksum=crc32.Get();

    file.open(filename.c_str(),ios::binary);

    if(!file.is_open())
        throw(DiskException(ERR_CREATE_FILE));

    file.write((char*)&bfh,sizeof(BACKUP_FILE_HEADER));

    for(unsigned i=0; i<n_partitions; i++)
        file.write((char*)&GetPartition(i),sizeof(GEN_PART));

    file.close();
}

/** \brief Restore partition table from the bacup file
 *
 * \param filename Path to the backup file
 * \param ignore_size_mismatch Continue if cureent disk size and size registered in backup don't match
 * \throw DiskException ERR_OPEN_FILE if backup file does not exist or is not accessible
 * \throw DiskException ERR_READ if failed reading data from the backup file
 * \throw DiskException ERR_INVALID_BACKUP if magic nuber is wrong
 * \throw DiskException ERR_NOT_THAT_BACKUP if backup was created for a different disk. Ignored if ignore_size_mismatch param is true
 */
void Disk::LoadBackup(std::string filename,bool ignore_size_mismatch)
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
    if(bfh.n_sect!=LastSector() && ignore_size_mismatch==false)
        throw(DiskException(ERR_NOT_THAT_BACKUP));

    old_checksum=bfh.checksum;
    bfh.checksum=0;
    crc32.Hash(&bfh,sizeof(bfh));

    part_man->DeleteAll();

    for(unsigned i=0; i<bfh.n_partitions; i++)
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


///
/// \brief Undo pending action.
/// When this function is called, a single pending action is removed from list.
/// If pending action is PENDING_CREATE or PENDING_DELETE, changes to partition tables are reverted
void Disk::Undo()
{
  UID_PAIR uid_pair;
  PENDING pending=pending_man->Undo(uid_pair);

  switch(pending.action)
  {
    case PENDING_DELETE:
      {
          part_man->AddPartition(pending.gpart);

          for(unsigned i=0;i<pending.nested_part.size();i++)
              part_man->AddPartition(pending.nested_part[i]);

          find_free_space();
      }
        break;
    case PENDING_CREATE:
      {
          part_man->DeletePartition(part_man->WhichByUID(uid_pair.uid_one));
          find_free_space();
      }
        break;
    case PENDING_FLAGS:
      {
          GEN_PART gpart=GetPartition(part_man->WhichByUID(uid_pair.uid_one));
          gpart.flags=pending.flags;
          part_man->ModifyPartition(part_man->WhichByUID(uid_pair.uid_one),gpart);
          find_free_space();
      }
        break;
    default:
        // do nothing; other actions (move, wipe) do not affect partition tables
        break;
  }

}

void Disk::UndoAll()
{
  unsigned n_pending= CountPendingActions();
  for(unsigned i=0;i<n_pending;i++)
      Undo();
}


/** \brief Erase all data on partition
 *
 * \param p Partition index
 * \param method erasing method, see definitions.h for #defines beginning with WIPE_
 * \throw DiskException error code ERR_OUT_OF_BOUNDS if index is too large
 */
void Disk::Wipe(unsigned int p,int method)
{
    pending_man->WipePartition(GetPartition(p),get_partition_uid(p),method);
}


/** \brief Get information specific to GUID partition table partitions
 *
 * \param p partition index
 * \throw DiskException error code ERR_OUT_OF_BOUNDS if index is too large
 */
GPT_SPECIFIC Disk::GetGPTSpecific(unsigned p)
{
    GPT_SPECIFIC gspec;
    memcpy(&gspec,GetPartition(p).data,sizeof(GPT_SPECIFIC));
    return gspec;

}

MBR_SPECIFIC Disk::GetMBRSpecific(unsigned p)
{
    MBR_SPECIFIC mspec;
    memcpy(&mspec,GetPartition(p).data,sizeof(MBR_SPECIFIC));
    return mspec;
}




void Disk::ReadMBR(MBR &mbr)
{
  DiskRead(0,&mbr,sizeof(MBR));
}

//
// NOTE: maybe I shouldn't do any kind of checks, just read 1st sector of the disk?
// NOTE: maybe move this function to GPTHelper?
void Disk::ReadGPT(GPT &gpt)
{
memset(&gpt,0,sizeof(GPT));

int n_gpt_parts=CountPartitions(PART_MBR_GPT);

if(n_gpt_parts==1)
     DiskRead(GetPartition(0,PART_MBR_GPT).begin_sector*diskio->GetDiskGeometry().bps,&gpt,sizeof(GPT));
else if(n_gpt_parts==0)
    throw(DiskException(ERR_GPT_NOT_FOUND));
else // More than one GPT?
    throw(DiskException(ERR_UNKNOWN_ERROR,"There's more than one GPT (?)"));

}








///
/// \brief Get number of pending operations
///
unsigned Disk::CountPendingActions()
{
    return pending_man->CountPendingActions();
}

///
/// \brief Get a pending action
/// \param p Pending action index
/// \throw DiskException code ERR_OUT_OF_BOUNDS if index is too large
const PENDING &Disk::GetPending(unsigned p)
{
    return pending_man->GetPendingAction(p);
}

///
/// \brief Returns true if there was an error while loading partition tables
///
bool Disk::ErrorOnLoad()
{
    return error_on_load;
}



//
// ------------------------------------------------------------------------
// private/protected functions
// ------------------------------------------------------------------------
//

void Disk::set_mbr_specific(unsigned p,MBR_SPECIFIC spec)
{
   GEN_PART gpart=GetPartition(p);
   memcpy(gpart.data,&spec,sizeof(MBR_SPECIFIC));
   modify_partition(p,gpart);
}

void Disk::find_free_space()
{
    frs_man->FindFreeSpace(part_man,LastSector(),GetDiskGeometry().bps,GetDiskGeometry().spt);
}

//
// Read partition tables
//
bool  Disk::load()
{
  mbr_helper->ReadPartitionTables();

  if(CountPartitions(PART_EXTENDED)==1)
      ebr_helper->ReadPartitionTables(GetPartition(0,PART_EXTENDED));
  if(CountPartitions(PART_MBR_GPT)==1)
      gpt_helper->ReadPartitionTables(GetPartition(0,PART_MBR_GPT));

  return true;
}











uint32_t Disk::get_partition_uid(unsigned p)
{
    return part_man->GetPartitionUID(p);
}



void Disk::do_pending()
{
PENDING curr;
for(unsigned i=0;i<CountPendingActions();i++)
  {
      try
        {
           curr=GetPending(i);
        }
       catch(...)
         {
             continue;
         }
      switch(curr.action)
        {
            case PENDING_WIPE:
                do_wipe(curr.gpart.begin_sector,curr.gpart.length,curr.flags);
                break;
        }
  }
memset(&clbk,0,sizeof(clbk));
pending_man->DeleteAll();
}



void Disk::do_wipe(uint64_t begin_sect,uint64_t length,int method)
{
   int ret;
   int bps=GetDiskGeometry().bps;

   const int mul=128;

   int buff_size=mul*bps;
   unique_ptr<uint8_t[]> buff(new uint8_t[buff_size]);
   uint32_t liko=(length%mul);

   clbk.current=0;
   clbk.total=length;
   clbk.event=PENDING_WIPE;
 //  clbk.partition=p;

   ret=diskio->Seek(begin_sect*bps);
   if(ret!=0)
       throw(DiskException(ret));

   memset(buff.get(),0,buff_size);

   length-=liko;

   while(length>0)
     {
        if(method!=WIPE_ZEROS)
          {

          }
        ret=diskio->Write(buff.get(),buff_size);
        if(ret!=0)
            throw(DiskException(ret));

        length-=mul;
        clbk.current+=mul;
     }
// Wipe what's left
ret=diskio->Write(buff.get(),liko*bps);
if(ret!=0)
    throw(DiskException(ret));

length-=liko;
clbk.current+=liko;
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


void Disk::modify_partition(unsigned int p,GEN_PART new_data)
{
    part_man->ModifyPartition(p,new_data);
}

uint32_t Disk::add_partition(GEN_PART new_part)
{
   uint32_t uid=part_man->AddPartition(new_part);
   find_free_space();
   return uid;
}

void Disk::set_gpt_specific(unsigned p,GPT_SPECIFIC gspec)
{
   part_man->SetSpecificData(p,reinterpret_cast<void*>(&gspec),sizeof(gspec));
}



