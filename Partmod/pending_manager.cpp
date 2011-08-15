#include "pending_manager.h"
#include "disk.h"
#include "disk_exception.h"
using namespace std;


PendingActionManager::~PendingActionManager()
{
   pending.clear();
   pending.resize(0);
}

uint32_t PendingActionManager::CountPendingActions()
{
    return pending.size();
}

const PENDING &PendingActionManager::GetPendingAction(uint32_t p)
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


uint32_t PendingActionManager::GetUID(uint32_t p)
{
  if(CountPendingActions()<p)
      throw(DiskException(ERR_OUT_OF_BOUNDS));
  return pending[p].first.uid_one;
}


uint32_t PendingActionManager::GetUID(uint32_t p,uint32_t &uid_two)
{
  if(CountPendingActions()<p)
      throw(DiskException(ERR_OUT_OF_BOUNDS));
  uid_two=pending[p].first.uid_two;
  return pending[p].first.uid_one;
}


void PendingActionManager::Add(PENDING pnd,UID_PAIR uid_pair)
{
    pending.push_back(make_pair(uid_pair,pnd));
}

void PendingActionManager::WipePartition(uint32_t partition_uid,uint8_t method)
{
PENDING pnd;
UID_PAIR uid_pair={partition_uid,0};


pnd.action=PENDING_WIPE;
pnd.specific=method;


Add(pnd,uid_pair);
}
