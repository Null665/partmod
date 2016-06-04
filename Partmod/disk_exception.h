#ifndef DISK_EXCEPTION_H
#define DISK_EXCEPTION_H

#include <string>
#include <exception>
#include <sstream>


// Error codes
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
#define ERR_GET_DISK_GEOMETRY    19 // Function that gets disk geometry has failed
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
#define ERR_UNKNOWN_FSID         32
#define ERR_BKUP_BPS_MISMATCH    33


// Error strings
extern const char *error_table[];


class DiskException:public std::exception
{
private:
  mutable std::string ret; // string returned by what().
public:

 int error_code;
 std::string message;


 DiskException(int error_code);
 DiskException(int error_code,std::string message);
 DiskException(std::string message);

 const char* what() const noexcept;
  ~DiskException();

};

#endif
