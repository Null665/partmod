#ifndef PENDING_MANAGER_H
#define PENDING_MANAGER_H

#include "definitions.h"
#include <vector>
#include <utility>


struct UID_PAIR
{
  uint32_t uid_one; // required
  uint32_t uid_two; // optional
};


class PendingActionManager
{
protected:
  std::vector<std::pair<UID_PAIR,PENDING> > pending;

public:

  ~PendingActionManager();

  void Add(PENDING pnd,UID_PAIR uid_pair);

  uint32_t CountPendingActions();
  const PENDING &GetPendingAction(uint32_t p);

  uint32_t GetUID(uint32_t p);
  uint32_t GetUID(uint32_t p,uint32_t &uid_two);

  void DeleteAction(uint32_t a);

  void DeleteAll();

  void WipePartition(uint32_t partition_uid,uint8_t method);
  void CopyPartition();
  void MovePartition();


};


#endif
