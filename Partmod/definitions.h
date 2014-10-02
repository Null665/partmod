/*
 * Various functions and definitions that are used by Disk class and other classes
 *
 */
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdint.h>
#include <string>
#include <vector>
#include "chs.hpp"


typedef uint32_t uid_t;


#define BS_MAGIC           0xAA55 // last 2 bytes of mbr,ebr, or boot sector
#define CHS_LIMIT        16370046 // number of sectors that can be addressed using MBR CHS

#define KB                1024ULL // bytes in Kilobyte
#define MB             1048576ULL // bytes in Megabyte
#define GB          1073741824ULL // ...
#define TB       1099511627776ULL
#define PB    1125899906842624ULL


// Type of free space
#define FREE_GPT              1
#define FREE_EXTENDED         2
#define FREE_UNALLOCATED      3

//
// Flags for GEN_PART::flags
//
#define PART_UNKNOWN          0  // Entry is unused
#define PART_PRIMARY          1  // Primary partition
#define PART_EXTENDED         2  // Partition is the "Extended LBA" partition
#define PART_LOGICAL          4  // partition is in extended partition ( logical partition )
#define PART_ACTIVE           8  // Active (bootable)
//#define PART_NOT_FORMATTED    16
#define PART_GPT              32 // GPT partition
#define PART_MBR_GPT          64 // MBR partition with fsid 0xEE -- (How is it called? GPT primary partition?)
//#define PART_DYNAMIC         128 // Windows dynamic partition/disk

#define PART_ON_MBR           (unsigned int)( PART_PRIMARY | PART_EXTENDED | PART_MBR_GPT )


// Disk types
#define DISK_UNKNOWN           0
#define DISK_MBR               1
#define DISK_GPT               2
#define DISK_DYNAMIC           3
#define DISK_HYBRID            4 // MBR + GPT


// Values for partition alignment
#define ALIGN_SECTOR    0
#define ALIGN_CYLINDER  1
#define ALIGN_TRACK     2
#define ALIGN_MEGABYTE  3


// Partmod file system IDs
#define FS_FAT12                  1
#define FS_FAT16                  2
#define FS_FAT32                  3
#define FS_NTFS                   4
#define FS_EXTENDED               5 // Extended partiton
#define FS_GPT                    6 // GPT partiton
#define FS_LINUX                  8 // Ext2, Ext3, Ext4, ReiserFS... -- Linux
#define FS_DYNAMIC                9 // Microsoft Windows dynamic volume manager partition


// MS-DOS partition IDs
#define FSID_GPT               0xEE
#define FSID_EXTENDED          0x0F
#define FSID_EXTENDED_LBA      0x05
#define FSID_DEFAULT           0x06
#define FSID_F_HIDDEN          0x10

//
// Pending operations
//
#define PENDING_MOVE              1 // Move partition from one place to another
#define PENDING_COPY              2 // Copy partition
#define PENDING_WIPE              3 // Erase all data
#define PENDING_CREATE            4 // Create a new partition
#define PENDING_DELETE            5 // Delete partition
#define PENDING_FLAGS             6 // Modify partition flags


// Partition wiping method
#define WIPE_RANDOM               1 // Fill with random data
#define WIPE_ZEROS                2 // Fill with 0



// For U64ToStr() and StrToU64
#define STR_OCT                   8 // Octal string
#define STR_DEC                  10 // Decimal string
#define STR_HEX                  16 // Hex string

#define BACKUP_MAGIC               0x4E756C6C // Backup file magic number



#pragma pack(1)

struct __GUID
{
  uint32_t one;
  uint16_t two;
  uint16_t three;
  uint64_t four;  // Note: Must be in big endian
};


// Disk geometry -- also required when converting MBR_CHS to LBA or LBA to MBR_CHS
struct GEOMETRY
{
  uint64_t cylinders;   /* number of cylinders  */
  uint16_t tpc;         /* tracks per cylinder  */
  uint16_t spt;         /* sectors per track    */
  uint16_t bps;         /* bytes per sector     */
};



//
// Generic partition
//
struct GEN_PART
{
  uint8_t  mount_point[256];    // Mount point. Currently unused, might be removed in future
  uint64_t begin_sector;        // In which sector partition starts
  uint64_t length;              // size of partition (in sectors)
  uint32_t fsid;                // File system ID (system-independet)
  uint32_t flags;               // flags (is active, is extended...)
  uint16_t reserved_a[65];
  uint8_t  data[128];           // Data specific to the partition table
};


struct BACKUP_FILE_HEADER
{
  uint32_t magic;             // BACKUP_MAGIC
  uint32_t header_size;       // size of backup file header in bytes
  GEOMETRY dg;                // Disk geometry
  uint32_t n_partitions;      // Number of partitions
  uint32_t n_sect;            // Number of sectors on disk
  uint32_t checksum;          // CRC32 of the backup file. Must be zero before calculating the checksum
  char     description[128];
  char     reserved[128];
};

#pragma pack()

// describes where free space blocks are on disk
struct FREE_SPACE
{
  uint64_t begin_sector;  /* Where free space begins */
  uint64_t length;        /* length of free space */
  uint32_t type;          /* type of free space */
};



// String to number and number to string
std::string U64ToStr(uint64_t number,unsigned int base=STR_DEC);
uint64_t StrToU64(std::string str,unsigned int base=STR_DEC);

// Convert number of sectors to a menaingful size interpretation
std::string size_str(uint64_t num_of_sect,int bps); // returns a string, eg: "100 GB" or "785.55 MB"


uint64_t chs_to_lba(MBR_CHS _chs,GEOMETRY _dg);

// Get partition mount point from Windows registry
//  Windows only now
std::string get_mount_point(GEN_PART gpart,unsigned int disk_signature);

// GUID fuctions
void generate_guid(__GUID &);
void set_guid(__GUID &guid,uint32_t one,uint16_t two, uint16_t three,uint64_t four);

bool Dump2File(const char *fn,const void *data,int size);

uint64_t GetFileSize(std::string filename);



#endif // definitions.h
