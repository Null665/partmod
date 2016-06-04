#ifndef DISK_H
#define DISK_H

#include "mbr.h"
#include "ebr.h"
#include "gpt.h"


#include <vector>
#include <exception>
#include <sstream>
#include <utility>
#include "definitions.h"
#include "../diskio/diskio.hpp"

#include "partition_manager.h"
#include "free_space_manager.h"
#include "pending_manager.h"
#include "guid_manager.h"
#include "fsid_manager.h"


// Windows defines this so it conflicts with Disk::GetFreeSpace
#ifdef GetFreeSpace
  #undef GetFreeSpace
#endif


struct DISK_ERROR
{
  int error_code;

  int partition;
  int partition_nxt;

};


struct CALLBACK_DATA
{
   int event;
   int partition;

   uint64_t current;
   uint64_t total;
};


class Disk
{
private:

  DiskIO *diskio;
  std::string pd_str;          // Physical drive string, eg. \\.\PhysicalDrive0 or /dev/sda

  PartitionManager *part_man;
  FreeSpaceManager *frs_man;
  PendingActionManager *pending_man;

  bool error_on_load; // Were there any errors while loading partition tables?

  void init();

protected:

  friend class EBRHelper;
  friend class MBRHelper;
  friend class GPTHelper;

  EBRHelper *ebr_helper;
  MBRHelper *mbr_helper;
  GPTHelper *gpt_helper;

  bool load();                // find partitoons on disk
  void find_free_space();     // find free space on disk

  void modify_partition(unsigned int p,GEN_PART new_data);
  uint32_t add_partition(GEN_PART new_part);

  void set_gpt_specific(unsigned p,GPT_SPECIFIC spec);
  void set_mbr_specific(unsigned p,MBR_SPECIFIC spec);

  void do_pending();
  void do_wipe(uint64_t begin_sect,uint64_t length,int method);

  void verify_geometry();

  uint32_t get_partition_uid(unsigned p);

public:
  GuidManager *guid_man;
  FsidManager *fsid_man;

  CALLBACK_DATA clbk;
 // ------------- Public functions -------------------------

  uint64_t         LastSector()       const;      // Returns number of sectors on disk
  unsigned         GetDiskSignature();            // Returns MBR disk signature


  Disk(std::string dsk); // Constructor: takes string, eg. \\.\PhysicalDrive0
  Disk();
  ~Disk();

  void Open(std::string dsk);
  void OpenDiskImage(std::string dsk,GEOMETRY geom);
  bool IsOpen();
  bool IsReadOnly();

  void Close(); // Close disk/disk image
  int Save();  // Save changes to disk

  unsigned  CountPartitions(unsigned flags) const;       // Returns number of partitions of specified type
  unsigned  CountPartitions() const;                     // nnumber of all partitions
  unsigned  CountFreeSpaces();                           // Returns number of free space slices found on disk

  const GEN_PART   &GetPartition(unsigned int p);
  const GEN_PART   &GetPartition(unsigned int p,uint32_t flag);

  const FREE_SPACE &GetFreeSpace(unsigned int s);


  void DiskRead(uint64_t offset,void* data,unsigned size);   // Read from disk
  void DiskWrite(uint64_t offset,const void* data,unsigned size);  // Write to disk

  void DeletePartition(unsigned p);

  void CreatePartitionPrimary(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid,unsigned alignment);
  void CreatePartitionExtended(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid,unsigned alignment);
  void CreatePartitionLogical(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid,unsigned alignment);
  void CreatePartitionGPT(int which_frs,uint64_t size,uint64_t sect_before,__GUID type_guid,unsigned alignment);
  void CreatePartitionPrimaryGPT(int which_frs,uint64_t size=0); // Create a new GPT partition table

  void SetActive(unsigned  p,bool set_active);



  void Wipe(unsigned p,int method);
  void Move(unsigned p,uint64_t sect_new_pos);  // not finished yet

// Create and Restore partition table backup
  void CreateBackup(std::string filename, std::string description="");
  void LoadBackup(std::string filename,bool ignore_size_mismatch=false);

// Read disk structures
  void ReadGPT(GPT &gpt); // Reads GPT only if it exists, otherwise throws an exception
  void ReadMBR(MBR &mbr); // Reads the first sector of the disk

  int GetDiskType() const;                      // returns Disk::disk_type
  const GEOMETRY &GetDiskGeometry() const;      // returns Disk::disk_geometry
  std::string GetPhysicalDriveString() const;   // returns Disk::pd_str

  void CheckDisk(std::vector<DISK_ERROR> &error_list);

  MBR_SPECIFIC GetMBRSpecific(unsigned p);
  MBR_SPECIFIC GetMBRSpecific(const GEN_PART &gpart);

  GPT_SPECIFIC GetGPTSpecific(unsigned p);
  GPT_SPECIFIC GetGPTSpecific(const GEN_PART &gpart);

// Pending actions
  void Undo();
  void UndoAll();

  unsigned CountPendingActions();
  const PENDING &GetPending(unsigned p);

 // void ChangeFSID();

 bool ErrorOnLoad();


};



#endif // DISK_H
