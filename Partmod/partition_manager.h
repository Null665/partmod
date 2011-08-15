#ifndef PARTITION_MANAGER_H
#define PARTITION_MANAGER_H

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

  void AddPartition(GEN_PART new_part);
  void DeletePartition(uint32_t p);
  void DeleteAll();

  uint32_t CountPartitions() const;
  uint32_t CountPartitions(uint32_t type) const;

  const GEN_PART &GetPartition(uint32_t p) const;
  const GEN_PART &GetPartitionByUID(uint32_t uid) const;
  const GEN_PART &GetExtendedPartition() const;

  void ModifyPartition(uint32_t p,GEN_PART data);

  uint32_t GetPartitionUID(uint32_t p )const;

  void SetActive(uint32_t p,bool set_active);

  uint32_t WhichByUID(uint32_t uid) const;

  void SetSpecificData(uint32_t p,void *data,uint32_t size);


};


#endif
