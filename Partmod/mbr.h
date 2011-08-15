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


#pragma pack(1)

//
// Partition record of MBR and EBR
//
typedef struct __PARTITION_RECORD_
{
  uint8_t        status;
  MBR_CHS        begin_chs;
  uint8_t        partition_type;
  MBR_CHS        end_chs;
  uint32_t       begin_lba;
  uint32_t       lba_blocks;

} PARTITION_RECORD;

//
// Extended Boot Record
//
typedef struct __EBR_
{
  uint8_t             boot_code[446];
  PARTITION_RECORD    partition_table[2];
  uint8_t             unused[32];
  uint16_t            signature;          //AA55
} EBR;

//
// Master Boot Record
//
typedef struct __MBR_
{
  uint8_t              boot_code[440];
  uint32_t             disk_signature;
  uint16_t             unknown;
  PARTITION_RECORD     partition_table[4];
  uint16_t             signature;          //AA55

} MBR;


#pragma pack(0)


// Used when converting GEN_PART into EBR
struct EBR_DATA
{
  uint64_t sector;  // Where to write the ebr
  EBR ebr;          // The ebr
};



#endif
