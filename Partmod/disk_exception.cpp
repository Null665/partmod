#include "disk_exception.h"

const char *error_table[]=
{
  "",
  "Could not open disk",                             // 1
  "Could not open a file",
  "Could not create a file",
  "Could not read from disk",
  "Could not write to disk",                         // 5
  "Could not seek to specified position ",
  "Partition doesn't exist",
  "LBA Extended partition not found",
  "Impossible to resize partition",
  "Failed to resize a partition",                      //10
  "Cannot create more than 4 primary partitions",
  "Disk is full",
  "Unknown error",
  "Not enough space",
  "Partition is too small",                          // 15
  "Impossible to create a primary partition on extended partition",
  "File is not a valid backup file",
  "GPT primary partition not found",
  "Disk geometry is unknown",
  "Can't split partition",                           // 20
  "One or more partitions overlap each other",
  "GPT already exists on disk",
  "Invalid checksum",
  "Disk size registered in backup and size of selected disk don't mach",
  "\"Bytes per sector\" value is not a power of two", // 25
  "Partition begins or ends outside disk",
  "Logical volume is outside extended partition",
  "Pending action doesn't exist (array index out of bounds)",
  "Array (vector) index out of bounds",
  "Failed to get disk size",                        // 30
  "Close the current disk first",
  "Unknown file system ID",
  "",
};




 DiskException::DiskException(int error_code)
  {
    this->error_code=error_code;
  }
 DiskException::DiskException(int error_code,std::string message)
  {
    this->error_code=error_code;
    this->message=message;
  }
 DiskException::DiskException(std::string message)
  {
    this->error_code=0;
    this->message=message;
  }
 const char* DiskException::what() const noexcept
  {
    std::stringstream ss;
    if(this->error_code!=0)
        ss <<"Error code: "<<this->error_code<<"("<<error_table[this->error_code]<<")"<<'\n';
    if(!message.empty())
        ss <<"Message: "<<message<<'\n';

    this->ret=ss.str(); // returning stringstream::str::c_str() doesn't always work for some reason
    return ret.c_str();

  }
 DiskException::~DiskException() throw(){}
