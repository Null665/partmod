#ifndef PENDING_MANAGER_H
#define PENDING_MANAGER_H

#include "definitions.h"
#include <vector>
#include <utility>


struct PENDING
{
uint32_t action;
uint32_t flags;

GEN_PART gpart;     // the partition we want to modify
GEN_PART gpart_opt; // optional partition (example: copying dest.)

std::vector<GEN_PART> nested_part; // used only if PART_EXTENDED or PART_MBR_GPT flag is enabled in gpart.flags
uint32_t flags_old;
};


struct UID_PAIR
{
  uint32_t uid_one; // required
  uint32_t uid_two; // optional
};


class PendingActionManager
{
protected:
  std::vector<std::pair<UID_PAIR,PENDING> > pending;
  void Add(PENDING pnd,UID_PAIR uid_pair);
public:

  uint32_t CountPendingActions() const;
  const PENDING &GetPendingAction(uint32_t p) const;

  void DeleteAction(uint32_t a);
  void DeleteAssociated(uint32_t part_uid);
  void DeleteAll();

  void WipePartition(GEN_PART partition,uint32_t partition_uid,uint8_t method);

  void CreatePartition(const GEN_PART &partition,uint32_t partition_uid);

  void DeletePartition(GEN_PART partition);
  void DeletePartition(GEN_PART partition,const std::vector<GEN_PART> &nested_part);

  void ModifyFlags(uint32_t partition_uid,uint32_t flags,uint32_t flags_old);

  PENDING Undo();
  PENDING Undo(UID_PAIR &uid_pair);
};

#endif
