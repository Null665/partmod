// A class toi deal with pending actions (move, wipe, copy, etc)
// all code will be rewritten soon

#include "pending_manager.h"
#include "disk.h"
#include "disk_exception.h"
using namespace std;

uint32_t PendingActionManager::CountPendingActions() const
{
    return pending.size();
}

const PENDING &PendingActionManager::GetPendingAction(uint32_t p) const
{
  if(p>CountPendingActions())
      throw(DiskException(ERR_OUT_OF_BOUNDS));
  return pending[p].second;
}


void PendingActionManager::DeleteAction(uint32_t a)
{
  if(a>CountPendingActions())
      throw(DiskException(ERR_OUT_OF_BOUNDS));

  pending.erase(pending.begin()+a);
}

void PendingActionManager::DeleteAll()
{
    pending.clear();
}

/*
uint32_t PendingActionManager::GetUID(uint32_t p)
{
  if(CountPendingActions()<p)
      throw(DiskException(ERR_OUT_OF_BOUNDS));
  return pending[p].first.uid_one;
}
*/
/*
uint32_t PendingActionManager::GetUID(uint32_t p,uint32_t &uid_two)
{
  if(CountPendingActions()<p)
      throw(DiskException(ERR_OUT_OF_BOUNDS));
  uid_two=pending[p].first.uid_two;
  return pending[p].first.uid_one;
}*/


void PendingActionManager::Add(PENDING pnd,UID_PAIR uid_pair)
{
    pending.push_back(make_pair(uid_pair,pnd));
}

void PendingActionManager::WipePartition(GEN_PART partition,uint32_t partition_uid,uint8_t method)
{
  PENDING pnd;
  UID_PAIR uid_pair={partition_uid,0};

  pnd.action=PENDING_WIPE;
  pnd.flags=method;
  pnd.gpart=partition;

  Add(pnd,uid_pair);
}

void PendingActionManager::ModifyFlags(uint32_t partition_uid,uint32_t flags,uint32_t flags_old)
{
  PENDING pnd;
  UID_PAIR uid_pair={partition_uid,0};

  pnd.action=PENDING_FLAGS;
  pnd.flags=flags;
  pnd.flags_old=flags_old;

  Add(pnd,uid_pair);
}


void PendingActionManager::DeletePartition(GEN_PART partition)
{
  PENDING pnd;
  UID_PAIR uid_pair={0,0};

  pnd.action=PENDING_DELETE;
  pnd.gpart=partition;

  Add(pnd,uid_pair);
}

void PendingActionManager::DeletePartition(GEN_PART partition,const std::vector<GEN_PART> &nested_part)
{
  PENDING pnd;
  UID_PAIR uid_pair={0,0};

  pnd.action=PENDING_DELETE;
  pnd.gpart=partition;
  pnd.nested_part=nested_part;

  Add(pnd,uid_pair);
}


void PendingActionManager::CreatePartition(const GEN_PART &partition,uint32_t partition_uid)
{
  PENDING pnd;
  UID_PAIR uid_pair={partition_uid,0};

  pnd.action=PENDING_CREATE;
  pnd.gpart=partition;

  Add(pnd,uid_pair);
}


PENDING PendingActionManager::Undo()
{
  PENDING p={0};
  if(CountPendingActions()==0)
        return p;

  p=GetPendingAction(CountPendingActions()-1);
  DeleteAction(CountPendingActions()-1);
  return p;
}

PENDING PendingActionManager::Undo(UID_PAIR &uid_pair)
{
  PENDING p;
  if(CountPendingActions()==0)
        return p;

  p=GetPendingAction(CountPendingActions()-1);
  uid_pair=pending[CountPendingActions()-1].first;

  DeleteAction(CountPendingActions()-1);
  return p;
}
