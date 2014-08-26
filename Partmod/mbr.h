//
// MBR specific sruff
//
//
#ifndef MBR_H
#define MBR_H

#include "definitions.h"
#include <stdint.h>

/*
#define MBR_FSID_GPT               0xEE
#define MBR_FSID_EXTENDED          0x0F
#define MBR_FSID_EXTENDED_LBA      0x05
#define MBR_FSID_DEFAULT           0x06
*/



//
// Partition record of MBR and EBR
//
struct alignas(1) PARTITION_RECORD
{
  uint8_t        status;
  MBR_CHS        begin_chs;
  uint8_t        partition_type;
  MBR_CHS        end_chs;
  uint32_t       begin_lba;
  uint32_t       lba_blocks;
};

//
// Master Boot Record
//
struct alignas(1) MBR
{
  uint8_t              boot_code[440];
  uint32_t             disk_signature;
  uint16_t             unknown;
  PARTITION_RECORD     partition_table[4];
  uint16_t             signature;          //AA55
};


//
// MBR-specific data (in GEN_PART::data )
//
struct alignas(1) MBR_SPECIFIC
{
   uint8_t  fsid;
   uint32_t begin_sector_rel;
};



class Disk;
class MBRHelper
{
public:
  MBRHelper(Disk *disk){this->disk=disk;}

  void ReadPartitionTables();
  void WriteChanges();
protected:
  Disk *disk;
};

#endif
