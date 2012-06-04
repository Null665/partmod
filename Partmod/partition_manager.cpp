#include "partition_manager.h"
#include "disk.h"
#include "disk_exception.h"
#include <algorithm>
#include <cstring>
using namespace std;

//
// Vector sorting functions
//
static bool cmp_genpart(pair<GEN_PART,int> a, pair<GEN_PART,int>  b)
{
  return (a.first.begin_sector) < (b.first.begin_sector) ;
}

void PartitionManager::sort_vectors()
{
  sort(partition.begin(),partition.end(),cmp_genpart);
}


//
// Default constructor
//
PartitionManager::PartitionManager()
{
  least_uid=1;
}

// Destructor
PartitionManager::~PartitionManager()
{
    partition.clear();
    partition.resize(0);
}

//
// Get partition from vector
//
const GEN_PART &PartitionManager::GetPartition(uint32_t p)  const
{
  if(CountPartitions()<=p)
      throw(DiskException(ERR_OUT_OF_BOUNDS));
  return partition[p].first;
}

//
// Get partition by its Unique IDentifier
//
const GEN_PART &PartitionManager::GetPartitionByUID(uint32_t uid)  const
{
  return GetPartition(WhichByUID(uid));
}



uint32_t PartitionManager::GetPartitionUID(uint32_t p)  const
{
  if(CountPartitions()<=p)
      throw(DiskException(ERR_OUT_OF_BOUNDS));
  return partition[p].second;
}




//
// Return total number of partitions
//
uint32_t PartitionManager::CountPartitions()  const
{
    return partition.size();
}

//
// Return number of partitions by type
//
uint32_t PartitionManager::CountPartitions(uint32_t type)  const
{
  uint32_t num_part=0;
  for(unsigned int i=0;i<CountPartitions();i++)
    {
       if(GetPartition(i).flags &type)
           num_part++;
    }
  return num_part;
}



//
// Add a partition to vector
//
void PartitionManager::AddPartition(GEN_PART new_part)
{
   if(new_part.flags&PART_PRIMARY)
     {
         if(CountPartitions(PART_PRIMARY|PART_EXTENDED|PART_MBR_GPT)>=4)
             throw(DiskException(ERR_MBR_LIMIT));
     }
   else if (new_part.flags&PART_EXTENDED)
     {
         if(CountPartitions(PART_EXTENDED))
             throw(DiskException("Only one extended partition is allowed"));
         if(CountPartitions(PART_PRIMARY|PART_EXTENDED|PART_MBR_GPT)>=4)
             throw(DiskException(ERR_MBR_LIMIT));
     }
   else if (new_part.flags&PART_LOGICAL)
     {
         if(CountPartitions(PART_EXTENDED)!=1)
             throw(DiskException(ERR_EXTENDED_NOT_FOUND));
     }
   else if (new_part.flags&PART_MBR_GPT)
     {
         if(CountPartitions(PART_PRIMARY|PART_EXTENDED|PART_MBR_GPT)>=4)
             throw(DiskException(ERR_MBR_LIMIT));
         if(CountPartitions(PART_MBR_GPT)!=0)
             throw(DiskException(ERR_GPT_ALREADY_EXISTS));
     }

   ++least_uid; // Do not remove this line!

   partition.push_back(make_pair(new_part,least_uid));

   sort_vectors();
}


void PartitionManager::DeletePartition(uint32_t p)
{
if(GetPartition(p).flags&PART_EXTENDED)
    for(unsigned int i=0;i<CountPartitions();i++)
        if(GetPartition(i).flags&PART_LOGICAL)
           {
             partition.erase(partition.begin()+i);
             i--;
           }
if(GetPartition(p).flags&PART_MBR_GPT)
    for(unsigned int i=0;i<CountPartitions();i++)
        if(GetPartition(i).flags&PART_GPT)
           {
             partition.erase(partition.begin()+i);
             i--;
           }
partition.erase(partition.begin()+p);
}

void PartitionManager::DeleteAll()
{
    partition.clear();
}



const GEN_PART &PartitionManager::GetExtendedPartition()  const
{
for(unsigned i=0;i<CountPartitions();i++)
  {
      if(GetPartition(i).flags&PART_EXTENDED)
          return GetPartition(i);
  }
throw DiskException(ERR_EXTENDED_NOT_FOUND);
}


void PartitionManager::ModifyPartition(uint32_t p,GEN_PART data)
{
  DeletePartition(p);
  AddPartition(data);
}

void PartitionManager::SetActive(uint32_t p,bool set_active)
{
if(p>CountPartitions())
    throw(DiskException(ERR_PART_NOT_EXIST));

for(unsigned int i=0;i<CountPartitions();i++)
    if(GetPartition(i).flags&PART_ACTIVE)
        partition[i].first.flags&= ~PART_ACTIVE;

if(set_active)
    partition[p].first.flags|=PART_ACTIVE;
}


uint32_t PartitionManager::WhichByUID(uint32_t uid)  const
{
for(unsigned int i=0;i<CountPartitions();i++)
    if(GetPartitionUID(i)==uid)
        return i;
throw DiskException(ERR_PART_NOT_EXIST);
}


void PartitionManager::SetSpecificData(uint32_t p,void *data,uint32_t size)
{
    if(p>=CountPartitions())
        throw DiskException(ERR_OUT_OF_BOUNDS);
    if(size>=128) // TODO: delete this hard-coded value (must be equal to size of GEN_PART::data)
        throw DiskException(ERR_UNKNOWN_ERROR);

    memcpy(partition[p].first.data,data,size);

}
