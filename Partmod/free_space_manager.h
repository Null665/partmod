#ifndef FREE_SPACE_MANAGER_H
#define FREE_SPACE_MANAGER_H
// Free space manager - an internal helper class for class Disk;
// the purpose of this class is to find free disk space (unallocated) between partitions and store info as FREE_SPACE struct
// this includes space between primary partitions, inside extended partition, and in GPT

// FindFreeSpace() must be called after a change is made to partition structure (after partition creation, deletion..)
// in Disk class, FreeSpaceManager::FindFreeSpace(...) is called via find_spee_space() function

// FREE_SPACE is defined in definitions.h

#include "definitions.h"
#include "partition_manager.h"
#include <vector>

// avoid errors due to WinAPI
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
