#ifndef FREE_SPACE_MANAGER_H
#define FREE_SPACE_MANAGER_H

#include "definitions.h"
#include "partition_manager.h"
#include <vector>

#ifdef GetFreeSpace
  #undef GetFreeSpace
#endif



class FreeSpaceManager
{
protected:
  std::vector<FREE_SPACE> free_space;

  void sort_vectors();
  void find_in(PartitionManager *partman,uint64_t num_sect,uint32_t parent_flag,uint32_t child_flag,
                uint32_t sect_between,uint32_t free_spece_type,uint32_t resolution,uint32_t reserved_space=0);
 // void add(FREE_SPACE frs,uint32_t )


public:
  ~FreeSpaceManager();

  void FindFreeSpace(PartitionManager *partman,uint64_t num_sect,int bps,int spt);

  uint32_t CountFreeSpaces();
  const FREE_SPACE &GetFreeSpace(uint32_t f);

};


#endif
