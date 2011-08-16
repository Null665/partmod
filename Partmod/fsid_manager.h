#ifndef FSID_MANAGER_H
#define FSID_MANAGER_H

#include <stdint.h>
#include <string>
#include <vector>

#include "definitions.h"

#define MIN_SIZE   100*KB
#define MAX_LBA    ~(uint32_t)(0)

/** File system ID manager class
 * Manages MBR file system IDs and asscociates them with Partmod file system IDs
 *
*/
class FsidManager : public std::vector<FSID_INFO>
{
protected:
  void add_fsids();
public:
  FsidManager();

//
// Add file system ID to list
//
  bool Add(uint8_t fsid,                        // mbr fsid
           std::string description,
           uint64_t min_size,uint64_t max_size, // min and max size, -1 if unknown
           uint32_t partmod_fsid);
//
// Get FSID_INFO  by its index in vector
//
  const FSID_INFO &Get(unsigned i) const;
  std::string GetDescription(int i); // Get(i).description

//
// Get FSID_INFO structure by fsid
//
  const FSID_INFO &GetByFsid(int fsid) const;

//
// Get FSID_INFO structure of nth element with specidfied partmnod fsid
//
  const FSID_INFO &GetByPartmodFsid(int fsid_partmod,unsigned num) const ;

//
// Get 1st FSID_INFO structure with specified partmod fsid
// this function is equivalent to GetByPartmodFsid(fsid_partmod,0)
//
  const FSID_INFO &GetByPartmodFsid(int fsid_partmod) const;
  std::string GetDescription(int i,int fsid_partmod); // GetByPartmodFsid(...).description

  uint32_t Count(); // Get number of known file system IDs
  uint32_t Count(unsigned fsid_partmod); // Get number of File system IDs that have specified partmod FSID
};


#endif
