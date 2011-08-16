/*
 * Various functions and definitions that are used by Disk class
 *
 * NOTE: I don't like the dependency between disk.h, definitions.h and diskio.hpp
 *
 */

class DiskIO;
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdint.h>
#include <string>


typedef uint32_t uid_t;


#define DLLEXPORT extern "C" __declspec(dllexport)

#ifndef __cdecl
  #define __cdecl
#endif


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
#define PART_LOGICAL      4  // partition is in extended partition ( logical partition )
#define PART_ACTIVE           8  // Active (bootable)
#define PART_NOT_FORMATTED    16 // FIXME: PART_NOT_FORMATTED flag is totally ignored
#define PART_GPT              32 // GPT partition
#define PART_MBR_GPT          64 // MBR partition with fsid 0xEE -- (How is it called? GPT primary partition or what?)
#define PART_DYNAMIC         128 // Windows dynamic partition/disk

#define PART_ALL              (unsigned int)( PART_PRIMARY | PART_EXTENDED | PART_LOGICAL | PART_GPT | PART_MBR_GPT )


// Disk types
#define DISK_MBR               1
#define DISK_GPT               2
#define DISK_DYNAMIC           3
#define DISK_HYBRID            4 // MBR + GPT


// Error list
#define ERR_OPEN_DISK            1  // Can't open disk
#define ERR_OPEN_FILE            2  // Can't open file
#define ERR_CREATE_FILE          3  // Can't create file
#define ERR_READ                 4  // Can't read from disk/partition/file
#define ERR_WRITE                5  // Can't write to disk/partiton/file
#define ERR_INVALID_SEEK         6  // Wrong seek
#define ERR_PART_NOT_EXIST       7  // This error is set when too big number is passed to Disk::GetPartition()
#define ERR_EXTENDED_NOT_FOUND   8  // Extedned partition not found
#define ERR_RESIZE_TOO_BIG       9  // Not enough of free space?
#define ERR_BAD_RESIZE           10 //
#define ERR_MBR_LIMIT            11 // There are 4 primary partitions
#define ERR_DISK_FULL            12 // No free(unpartitioned) space
#define ERR_UNKNOWN_ERROR        13 // Something's wrong
#define ERR_PART_TOO_BIG         14 // Partition is too big
#define ERR_PART_TOO_SMALL       15 // Partition is too small ( <1MB ? )
#define ERR_PRIMARY_ON_EXTENDED  16 // Priamry partition on extended partition
#define ERR_INVALID_BACKUP       17 // Invalid backup file
#define ERR_GPT_NOT_FOUND        18 // Probably someone called Disk::ReadGPT on non-GPT disk
#define ERR_GEOMETRY_NOT_FOUND   19 // Function that gets disk geometry has failed
#define ERR_CANNOT_SPLIT         20 // Can't split partition
#define ERR_PARTITIONS_OVERLAP   21 // Partitions overlap each other
#define ERR_GPT_ALREADY_EXISTS   22 // GPT partiton already exists
#define ERR_INVALID_CHECKSUM     23
#define ERR_NOT_THAT_BACKUP      24
#define ERR_BPS_NOT_POW_OF_TWO   25 // "Bytes per sector" value is not a power of 2
#define ERR_OUTSIDE_DISK         26 // Partition begins or ends outside disk
#define ERR_OUTSIDE_EXTENDED     27 // Logical volume is outside Extended partition
#define ERR_PENDING_NOT_EXIST    28
#define ERR_OUT_OF_BOUNDS        29
#define ERR_GET_DISK_SIZE        30 // Could not get disk size
#define ERR_ALREADY_OPEN         31 // Disk is already open




// Partmod file system IDs
#define FS_FAT12                  1
#define FS_FAT16                  2
#define FS_FAT32                  3
#define FS_NTFS                   4
#define FS_EXTENDED               5 // Extended partiton
#define FS_GPT                    6 // GPT partiton
#define FS_LINUX                  8 // Ext2, Ext3, Ext4, ReiserFS... -- Linux
#define FS_DYNAMIC                9 // Microsoft Windows dynamic volume manager partition

// TODO: FSID_<anything> should be removed
// MS-DOS partition IDs
//
#define FSID_GPT               0xEE
#define FSID_EXTENDED          0x0F
#define FSID_EXTENDED_LBA      0x05
#define FSID_DEFAULT           0x06

// Pending operations
#define PENDING_MOVE              1 // Move partition from one place to another
#define PENDING_COPY              2 // Copy partition
#define PENDING_WIPE              3 // Wipe out everything in selected partition

// Partition wiping method
#define WIPE_RANDOM               1 // Fill with random data
#define WIPE_ZEROS                2 // Fill with 0



// For U64ToStr() and StrToU64
#define STR_OCT                   8 // Decimal string
#define STR_DEC                  10 // Octal string
#define STR_HEX                  16 // Hex string


// Format FS flags
#define FF_QUICK                   1  // Quick format
#define FF_ENABLE_COMPRESSION      2  // Enable file compression (NTFS only?)

#define BACKUP_MAGIC               0x4E756C6C // Backup file magic number


extern const char *error_table[];


#pragma pack(1)

// Cylinder-head-sector structure for MBR
struct MBR_CHS
{
uint8_t head;             // head (<255)
uint8_t sector:6;         // sector (<64)
uint8_t cylinder_bits:2;  // 2 bits of cylinder value
uint8_t cylinder   ;      // cylinder (<=1023)
};


typedef struct __GUID_
{
uint32_t one;
uint16_t two;
uint16_t three;
uint64_t four;  // Note: Must be in big endian
}__GUID;



#pragma pack(0)

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
uint8_t  mount_point[256];    // C: D: ... Z:  maybe even an UNC path -- NOT IMPLEMENTED
uint64_t begin_sector;        // In which sector partition starts
uint64_t length;              // size of partition (in sectors)
uint32_t fsid;                // File system ID (system-independet)
uint32_t flags;               // special flags (is active, is extended...)

uint16_t reserved_a[65];
uint8_t  data[128];           // Data specific to partition layout
};

// generic handle
struct GEN_HANDLE
{
DiskIO * hDisk;
uint64_t begin_sector;
uint64_t length;
uint8_t  fsid;
uint32_t flags;
GEOMETRY disk_geometry;
};


// describes where free space blocks  on disk
struct FREE_SPACE
  {
     uint64_t begin_sector;  /* Where free space begins */
     uint64_t length;        /* length of free space */
     uint32_t type;          /* type of free space */

  };


// Used when formatting a partition
struct FORMAT_DISK
  {
    GEN_HANDLE gh;
    unsigned int spc;       /* sectors per cluster */
    unsigned int fs;        /* What file sysytem to use, also tells which DLL must be called */
    unsigned int flags;     /* quick format,enable compression */
  };



struct BACKUP_FILE_HEADER
  {
     uint32_t magic;         // BACKUP_MAGIC
     GEOMETRY dg;            // Disk geometry
     uint32_t n_partitions;  // Number of partitions
     uint32_t n_sect;        // Number of sectors on disk
     uint32_t checksum;      // CRC32 of the backup fle. Must be zero before calculating the checksum
  };



struct FSID_INFO
{
	uint8_t fsid;
    std::string description;
    int64_t min_size;
    int64_t max_size;
    unsigned fsid_partmod;
};




//
// Partition type specific data (in GEN_PART::data )
//
struct MBR_SPECIFIC
{
   uint8_t  fsid;
   uint32_t begin_sector_rel;
};


struct PENDING
{
uint32_t action;
uint32_t specific;

};

// --------------------------------

// String to number and number to string
std::string U64ToStr(uint64_t number,unsigned int base=STR_DEC);
uint64_t StrToU64(std::string str,unsigned int base=STR_DEC);

std::string size_str(uint64_t num_of_sect,int bps); // returns a string, eg: "100 GB" or "1021.55 MB"


std::string GetErrorDescription(int error_code);

//
// The following functions will be removed someday (replaced with class CHS )
//---------------------------------------------------------
MBR_CHS lba_to_chs(uint64_t _lba,GEOMETRY _dg);
uint64_t chs_to_lba(MBR_CHS _chs,GEOMETRY _dg);

unsigned short  GET_CYLINDER (MBR_CHS _chs);
unsigned short  GET_HEAD     (MBR_CHS _chs);
unsigned short  GET_SECTOR   (MBR_CHS _chs);

void SET_CYLINDER (MBR_CHS &_chs, short _cyl);
void SET_HEAD     (MBR_CHS &_chs, unsigned char _head);
void SET_SECTOR   (MBR_CHS &_chs, unsigned char _sector);
//---------------------------------------------------------

/*
int Write(GEN_HANDLE _gh,int64_t _offset_rel,void *_buff,int _size);
int Read (GEN_HANDLE _gh,int64_t _offset_rel,void *_buff,int _size);


typedef int(__cdecl *DLL_BOOT_INFO)(GEN_HANDLE _gh);                   // Boot sector editing
typedef int(__cdecl *DLL_RESIZE)(GEN_HANDLE ,uint64_t,bool);   // Resize partition
typedef int(__cdecl *DLL_FORMAT)(FORMAT_DISK);                         // Format disk
typedef int(__cdecl *DLL_CALC_FREE)(GEN_HANDLE ,uint64_t*);    // Calculate free space
typedef int(__cdecl *DLL_FS_CHECK)(GEN_HANDLE, std::string&);          // Check file system for errors


// Functions that call DLLs
int DllBootInfo(GEN_HANDLE);
int DllResizePartition(GEN_HANDLE,uint64_t _sectors,bool increase);
int DllFormatPartition(FORMAT_DISK _fd);
int DllCalculateFree(GEN_HANDLE _gh,uint64_t *ret);
int DllFsCheck(GEN_HANDLE _gh,std::string &_err_log);
*/

// Get partition mount point from Windows registry
std::string get_mount_point(GEN_PART gpart,unsigned int disk_signature);

// GUID fuctions
void generate_guid(__GUID &);
void set_guid(__GUID &guid,uint32_t one,uint16_t two, uint16_t three,uint64_t four);

bool Dump2File(const char *fn,const void *data,int size);
#endif // definitions.h
