/* Disk class
 *
 * Curerently:
 *  Creates, deletes, sets active partition, can create and restore partition tables
 *
 *
 *  TODO:
 *   -> Resize partition
 *   -> Store EBR boot codes somewhere (are they used at all?)
 *   -> Split partition
 *   -> ...
 *
 *
 *  NOTES:
 *    -> Tested only with 512-byte sectors
 *
*/


#ifndef DISK_H
#define DISK_H

#include "mbr.h"
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


class GPTHelper;
class EBRHelper;


class Disk
{
private:

  DiskIO *diskio;
  std::string pd_str;          // Physical drive string, eg. \\.\PhysicalDrive0

  bool is_open;

  PartitionManager *part_man;
  FreeSpaceManager *frs_man;
  PendingActionManager *pending_man;

  void init();

protected:

  friend class EBRHelper;
  friend class GPTHelper;

  EBRHelper *ebr_helper;
  GPTHelper *gpt_helper;

  bool load();                // find partitoons on disk
  void find_free_space();     // find free space on disk

  GEN_PART  get_extended();   // TODO: remove this function

  void save_mbr();  // write MBR to disk

  void modify_partition(unsigned int _p,GEN_PART new_data);
  void add_partition(GEN_PART new_part);

  void set_gpt_specific(unsigned p,GPT_SPECIFIC spec);
  void set_mbr_specific(unsigned p,MBR_SPECIFIC spec);

  void do_pending();

  void wipe(unsigned p,uint64_t begin_sect,uint64_t length,int method);

  void verify_geometry();


public:
  GuidManager *guid_man;
  FsidManager *fsid_man;

  CALLBACK_DATA clbk;
 // ------------- Public functions -------------------------

  uint64_t         LastSector()       const;      // Returns number of sectors on disk
  unsigned         GetDiskSignature();            // Returns MBR disk signature

  GEN_HANDLE GetPartitionHandle(unsigned int p);  // Creates GEN_HANDLE for partition


  Disk(std::string dsk); // Constructor: takes string, eg. \\.\PhysicalDrive0
  Disk(int dsk);         // Constructor: takes disk number, eg. 0 and converts into string \\.\PhysicalDrive0
  Disk();
  ~Disk();

  void Open(std::string dsk);                 // Open disk
  void Open(std::string dsk,GEOMETRY geom);   // Open a disk image
  bool IsOpen();

  void Close(); // Close disk/disk image
  int  Save();  // Save changes to disk

  int  CountPartitions(unsigned int type) const;          // Returns number of partitions by type
  int  CountPartitions() const;                            // Returns number of all partitions
  int  CountFreeSpaces();                            // Returns number of free spaces found on disk

  const GEN_PART   &GetPartitionByUID(unsigned uid);

  const GEN_PART   &GetPartition(unsigned int _p);
  const FREE_SPACE &GetFreeSpace(unsigned int s);


  void DiskRead(uint64_t offset,void* data,unsigned size);   // Read from disk
  void DiskWrite(uint64_t offset,const void* data,unsigned size);  // Write to disk

  void DeletePartition(unsigned p);

  void CreatePartition(FREE_SPACE _frs,int _part_type,uint64_t _size);


  void CreatePartitionPrimary(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid);
  void CreatePartitionExtended(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid);
  void CreatePartitionLogical(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid);
  void CreatePartitionGPT(int which_frs,uint64_t size,uint64_t sect_before,__GUID type_guid);
  void CreatePartitionPrimaryGPT(int which_frs,uint64_t size,uint64_t sect_before);


  void SetActive(unsigned  p,bool set_active);

  void Wipe(unsigned int _p,int method);

// These  functions aren't finished yet
  void Split(unsigned int _p,uint64_t size_of_first_part);
  void Move(unsigned p,uint64_t new_pos);

// Create and Restore partition table backup
  void CreateBackup(std::string filename);
  void LoadBackup(std::string filename);

// Read disk structures
  void ReadGPT(GPT &gpt); // Reads GPT only if it exists, otherwise throws an exception
  void ReadMBR(MBR &mbr); // Reads the first sector of the disk

  int GetDiskType() const;                      // returns Disk::disk_type
  const GEOMETRY &GetDiskGeometry() const;      // returns Disk::disk_geometry
  std::string GetPhysicalDriveString() const;   // returns Disk::pd_str

  void CheckDisk(std::vector<DISK_ERROR> &error_list);

  MBR_SPECIFIC GetMBRSpecific(unsigned p);
  GPT_SPECIFIC GetGPTSpecific(unsigned p);

  uint32_t GetPartitionUID(unsigned p);

  unsigned CountPendingActions();
  const PENDING &GetPending(unsigned p);
  uint32_t GetPendingUID(unsigned p);

 // void ChangeFSID();


};




class PartitionHelper
{
protected:
    Disk *disk;

public:
    PartitionHelper(Disk *disk) {this->disk=disk;}
    virtual void ReadPartitionTables(GEN_PART)=0;
    virtual void WriteChanges()=0;

};


class EBRHelper:public PartitionHelper
{
public:
  EBRHelper(Disk *disk) : PartitionHelper(disk)
  {}
  void ReadPartitionTables(GEN_PART);
  void WriteChanges();

};



class GPTHelper:public PartitionHelper
{
public:
  GPTHelper(Disk *disk) : PartitionHelper(disk)
  {}
  void ReadPartitionTables(GEN_PART);
  void WriteChanges();
  bool IsValidGPT(GPT gpt);
  GPT CreateGPT(GEN_PART gpt_part);

/* TODO:
 * GPT_ENTRY ReadGPTEntryFromDisk(uint32_t n);
 * GPT_ENTRY WriteGPTEntryToDisk(const GPT_ENTRY &e,uint32_t n);
*/

//
// I think there's no reason to call rhese functions dirtectly
//
protected:
  void WriteGPT(const GPT&);
  uint32_t WritePartitionEntries(GPT gpt);
  void RestoreGPTFromBackup(GEN_PART gpt_part);
  void WriteBackup(GPT gpt);


};




#endif // DISK_H
