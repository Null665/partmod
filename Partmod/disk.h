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


class GPTParser;
class EBRParser;


class Disk
{
private:

  DiskIO *diskio;
  std::string pd_str;          // Physical drive string, eg. \\.\PhysicalDrive0

  bool is_open;


  PartitionManager *part_man;
  FreeSpaceManager *frs_man;
  PendingActionManager *pending_man;

protected:

  friend class EBRParser;
  friend class GPTParser;

  EBRParser *ebr_parser;
  GPTParser *gpt_parser;

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
  // TODO: remove this function; it's much easier and simpler to use DiskIO instead
  GEN_HANDLE GetDiskHandle();                     // Creates GEN_HANDLE for whole disk


  Disk(std::string _dsk); // Constructor: takes string, eg. \\.\PhysicalDrive0
  Disk(int _dsk);         // Constructor: takes disk number, eg. 0 and converts into string \\.\PhysicalDrive0
  Disk();
  ~Disk();

  void Open(std::string _dsk);                 // Open disk
  void Open(std::string _dsk,GEOMETRY geom);   // Open a disk image
  bool IsOpen();

  void Close(); // Close disk/disk image
  int  Save();  // Save changes to disk

  int  CountPartitions(unsigned int _type) const;          // Returns number of partitions by type
  int  CountPartitions() const;                            // Returns number of all partitions
  int  CountFreeSpaces();                            // Returns number of free spaces found on disk

  const GEN_PART   &GetPartitionByUID(unsigned uid);

  const GEN_PART   &GetPartition(unsigned int _p);
  const FREE_SPACE &GetFreeSpace(unsigned int s);


  void DiskRead(uint64_t offset,void* data,int size);   // Read from disk
  void DiskWrite(uint64_t offset,void* data,int size);  // Write to disk

  void DeletePartition(unsigned _p);

  void CreatePartition(FREE_SPACE _frs,int _part_type,uint64_t _size);


  void CreatePartitionPrimary(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid);
  void CreatePartitionExtended(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid);
  void CreatePartitionLogical(int which_frs,uint64_t size,uint64_t sect_before,uint8_t fsid);
  void CreatePartitionGPT(int which_frs,uint64_t size,uint64_t sect_before,__GUID type_guid);


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



// Base class for implementing EBR and GPT parsers
class PartitionParser
{
protected:
    Disk *disk;

public:
    PartitionParser(Disk *disk) {this->disk=disk;}
    virtual void ParsePartition(GEN_PART)=0;
    virtual void WriteChanges()=0;

   // virtual void FindFreeSpace();



};


class EBRParser:public PartitionParser
{
public:
  EBRParser(Disk *_disk) : PartitionParser(_disk)
  {}
  void ParsePartition(GEN_PART);
  void WriteChanges();

};



class GPTParser:public PartitionParser
{
public:
  GPTParser(Disk *_disk) : PartitionParser(_disk)
  {}
  void ParsePartition(GEN_PART);
  void WriteChanges();
  void WriteGPT(GEN_PART, GPT);

  bool IsValidGPT(GPT gpt);
  void DeleteAllEntries(GPT gpt);

  void WritePartitionEntries(GPT gpt);

  void UpdateChecksum(GPT &gpt);
  void RestoreFromBackup();

  void RestoreGPTFromBackup(GEN_PART gpt_part);

  GPT CreateGPT(GEN_PART gpt_part);
  void WriteBackup(GPT gpt);

};




#endif // DISK_H
