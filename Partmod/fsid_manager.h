#ifndef FSID_MANAGER_H
#define FSID_MANAGER_H

#include <stdint.h>
#include <string>
#include <vector>

#include "definitions.h"

#define MIN_SIZE   100*KB
#define MAX_LBA    ~(uint32_t)(0)


class FsidManager : public std::vector<FSID_INFO>
{
protected:
  void add_fsids();
public:
  FsidManager();

  bool Add(uint8_t fsid,
           std::string description,
           uint64_t min_size,uint64_t max_size,
           uint32_t partmod_fsid);

 FSID_INFO Get(unsigned i);
 std::string GetDescription(int i);
 std::string GetDescription(int i,int fsid_partmod);


 FSID_INFO GetByFsid(int fsid);
 FSID_INFO GetByPartmodFsid(int fsid_partmod);
 FSID_INFO GetByPartmodFsid(int fsid_partmod,int num);


 uint32_t Count();
 uint32_t Count(unsigned fsid_partmod);
};


#endif
