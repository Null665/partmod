#include "free_space_manager.h"
#include "disk.h"
#include "disk_exception.h"
#include <algorithm>
using namespace std;


 bool cmp_frs(FREE_SPACE a,FREE_SPACE b)
{
    return a.begin_sector < b.begin_sector;
}


void FreeSpaceManager::sort_vectors()
{
    sort(free_space.begin(),free_space.end(),cmp_frs);
}





FreeSpaceManager::~FreeSpaceManager()
{
  free_space.clear();
  free_space.resize(0);
}

uint32_t FreeSpaceManager::CountFreeSpaces()
{
    return free_space.size();
}


const FREE_SPACE &FreeSpaceManager::GetFreeSpace(uint32_t f)
{
  if(f>CountFreeSpaces())
      throw DiskException(ERR_OUT_OF_BOUNDS);

  return free_space[f];

}

//
// FINALLY THIS SHIT WORKS!!!
//
void FreeSpaceManager::FindFreeSpace(PartitionManager *partman,uint64_t num_sect,int bps,int spt)
{
  free_space.clear();
  uint64_t NSECT_MB=MB/bps; // Numbert of disk sectors in megabyte

  bool found_gpt;
  try
  {
      partman->GetPartition(0).flags&PART_MBR_GPT ? found_gpt=true : found_gpt=false;
  }
  catch(...) {found_gpt=false;}

  if(found_gpt)
      find_in(partman,num_sect,0,PART_PRIMARY | PART_EXTENDED | PART_MBR_GPT,0,FREE_UNALLOCATED,NSECT_MB,1);
  else
      find_in(partman,num_sect,0,PART_PRIMARY | PART_EXTENDED | PART_MBR_GPT,0,FREE_UNALLOCATED,NSECT_MB,spt);

  find_in(partman,num_sect,PART_EXTENDED,PART_LOGICAL,spt,FREE_EXTENDED,NSECT_MB);
  find_in(partman,num_sect,PART_MBR_GPT,PART_GPT,spt,FREE_GPT,NSECT_MB);

  this->sort_vectors();
  return;
}





void FreeSpaceManager::find_in(PartitionManager *partman,
                               uint64_t num_sect,          // Number of sectors on disk
                               uint32_t parent_flag,       // Parent flag: (optional)PART_EXTENDED, PART_MBR_GPT
                               uint32_t child_flag,        // Child flag: PART_LOGICAL, PART_GPT
                               uint32_t sect_between,      // Number of sectors between partition and the free space
                               uint32_t free_space_type,   // FREE_EXTENDED, FREE_GPT
                               uint32_t resolution,        // Min size of the free space
                               uint32_t reserved_space     // Number of reseved sectors at the beginning of disk
                               )
{

GEN_PART part_curr,part_next;
FREE_SPACE tmp;

for(unsigned i=0,j=1;i<partman->CountPartitions()-1,j<partman->CountPartitions(); i++,j++)
  {
    part_curr=partman->GetPartition(i);
    if( !(part_curr.flags&child_flag))
          continue;

    part_next=partman->GetPartition(j);
    if( !(part_next.flags&child_flag))
      {
           i--;
          continue;
      }

    tmp.begin_sector=part_curr.begin_sector+part_curr.length;
    tmp.length=part_next.begin_sector-tmp.begin_sector;
    tmp.type=free_space_type;

    if(tmp.length>resolution)
      {
        tmp.begin_sector+=sect_between;
        tmp.length-=sect_between;
        free_space.push_back(tmp);
      }
  }

// ----------

GEN_PART part_parent,last_logical;
bool found=false;



if(parent_flag==0)
  {
      part_parent.begin_sector=reserved_space;
      part_parent.length=num_sect-part_parent.begin_sector;

  }
else
  {
    for(unsigned i=0;i<partman->CountPartitions();i++)
        if(partman->GetPartition(i).flags&parent_flag)
          {
            part_parent=partman->GetPartition(i);
            found=true;
            break;
          }
    if(!found)
        return;

  }


found=false;
for(unsigned i=0;i<partman->CountPartitions();i++)
   if(partman->GetPartition(i).flags&child_flag)
    {
        last_logical=partman->GetPartition(i);
        found=true;
    }


if(found==false)
  {
    tmp.begin_sector=part_parent.begin_sector;
    tmp.length=part_parent.length;
    tmp.type=free_space_type;

    if(tmp.length>resolution)
      {
        tmp.begin_sector+=sect_between;
        tmp.length-=sect_between;
        free_space.push_back(tmp);
      }
  }
else
  {
// Find free space between the parent and last child partition
    tmp.begin_sector=last_logical.begin_sector+last_logical.length;
    tmp.length= (part_parent.begin_sector+part_parent.length)-tmp.begin_sector;
    tmp.type=free_space_type;
    if(tmp.length>resolution)
      {
        tmp.begin_sector+=sect_between;
        tmp.length-=sect_between;
        free_space.push_back(tmp);
      }
// Find free space between the parent and first child partition
     GEN_PART first_logical;
      for(unsigned i=0;i<partman->CountPartitions();i++)
          if(partman->GetPartition(i).flags&child_flag)
            {
                first_logical=partman->GetPartition(i);
                break;
            }
      tmp.begin_sector=part_parent.begin_sector;
      tmp.length= first_logical.begin_sector-tmp.begin_sector;
      tmp.type=free_space_type;

      if(tmp.length>resolution)
        {
          tmp.begin_sector+=sect_between;
          tmp.length-=sect_between;
          free_space.push_back(tmp);
        }
  }

}


