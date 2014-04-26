#ifndef PARTITION_MANAGER_H
#define PARTITION_MANAGER_H
// Free space manager - an internal helper class for class Disk;
// the purpose of this class is to keep a list of partition and check that they do not overlap,
// are created correctly (no primary partition in extended partition, etc)

// each partition has its own unique ID, which is required for PendingActionManager, to avoid things like
// 1. select wipe partition #3
// 2. delete partition #3 (1GB)
// 3. create partition #3 (100GB)
// 4. apply pending actions and delete 100GB of data
// unique id allows to check that partition exists, and iof it doesn't remove pending actions associated with it

#include "definitions.h"
#include <vector>
#include <stdint.h>


class PartitionManager
{
private:
  uint32_t least_uid;

protected:
  std::vector<std::pair<GEN_PART,uint32_t> > partition;

  void sort_vectors();


public:
  PartitionManager();
 ~PartitionManager();

  uint32_t AddPartition(GEN_PART new_part);
  void DeletePartition(uint32_t p);
  void DeleteAll();

  void SetActive(uint32_t p,bool set_active);

  uint32_t CountPartitions() const;
  uint32_t CountPartitions(uint32_t type) const;

  const GEN_PART &GetPartition(uint32_t p) const;
  const GEN_PART &GetPartition(uint32_t p,uint32_t flag) const;

  const GEN_PART &GetPartitionByUID(uint32_t uid) const;

  void ModifyPartition(uint32_t p,GEN_PART data);

  uint32_t GetPartitionUID(uint32_t p )const;
  uint32_t WhichByUID(uint32_t uid) const;

  void SetSpecificData(uint32_t p,void *data,uint32_t size);
};


#endif
