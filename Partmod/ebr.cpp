//
// EBR specific functions
//



#include "disk.h"
#include "mbr.h"
#include "chs.hpp"
#include <cstring>
using namespace std;

void EBRHelper::ReadPartitionTables(GEN_PART extended)
{
  EBR ebr;
  uint64_t next_lba=0;
  MBR_SPECIFIC mspec;
  GEN_PART gpart;

  int bps=disk->GetDiskGeometry().bps;

  do
    {
       memset(&ebr,0,sizeof(EBR));
       memset(&gpart,0,sizeof(GEN_PART));

       // Read EBR from disk
       disk->DiskRead( (uint64_t)(extended.begin_sector+next_lba)*bps,&ebr,sizeof(EBR));

       // If no logical volumes on extended partition
       if(ebr.partition_table[0].lba_blocks==0 || ebr.partition_table[0].partition_type==0)
           break;

       // First sect. of partition =
       gpart.begin_sector=ebr.partition_table[0].begin_lba+extended.begin_sector+next_lba;
       gpart.length=ebr.partition_table[0].lba_blocks;

       gpart.flags|=PART_LOGICAL;
       ebr.partition_table[0].status==0x80 ? gpart.flags|=PART_ACTIVE : gpart.flags&=~PART_ACTIVE;
       gpart.fsid=disk->fsid_man->GetByFsid(ebr.partition_table[0].partition_type).fsid_partmod;

       // MBR-specific data
       mspec.begin_sector_rel=ebr.partition_table[0].begin_lba;
       mspec.fsid=ebr.partition_table[0].partition_type;

       disk->add_partition(gpart);
       disk->set_mbr_specific(disk->CountPartitions()-1,mspec);

       next_lba=ebr.partition_table[1].begin_lba;

    }while(ebr.partition_table[1].lba_blocks!=0 || ebr.partition_table[1].begin_lba!=0);



}

void EBRHelper::WriteChanges()
{

int spt=disk->GetDiskGeometry().spt;

//
// Check whether extended partition exists
//
if(disk->CountPartitions(PART_EXTENDED)==0)
    return;


//
// If there are no logical volumes, clear the ext. part. boot sector and exit
//
if(disk->CountPartitions(PART_LOGICAL)==0)
  {
      EBR ebr;
      memset(&ebr,0,sizeof(EBR));
      ebr.signature=BS_MAGIC;
      disk->DiskWrite(disk->GetPartition(0,PART_EXTENDED).begin_sector*disk->GetDiskGeometry().bps,&ebr,sizeof(EBR));
      return;
  }

//
// NOW LET'S FUCK UP THE PARTITION TABLE!!!1
// Well, finally this peace of code works

uint64_t begin_sector_extd=disk->GetPartition(0,PART_EXTENDED).begin_sector;
uint64_t ebr_sector=begin_sector_extd;

CHS chs;
chs.SetGeometry(disk->GetDiskGeometry().spt,disk->GetDiskGeometry().tpc);

for(unsigned i=0;i<disk->CountPartitions();)
  {
    const GEN_PART &curr_part=disk->GetPartition(i);

    if((curr_part.flags&PART_LOGICAL)==false)
      {
        i++;
        continue;
      }


    MBR_SPECIFIC mspec=disk->GetMBRSpecific(i);
    EBR ebr;
    memset(&ebr,0,sizeof(EBR));


    ebr.partition_table[0].begin_lba=curr_part.begin_sector-ebr_sector;
    ebr.partition_table[0].lba_blocks=curr_part.length;
    ebr.partition_table[0].partition_type=mspec.fsid;
    curr_part.flags&PART_ACTIVE ?
        ebr.partition_table[0].status=0x80:
        ebr.partition_table[0].status=0x00;


    chs=curr_part.begin_sector;
    ebr.partition_table[0].begin_chs=chs.ToMbrChs();
    chs=curr_part.begin_sector+curr_part.length;
    ebr.partition_table[0].end_chs=chs.ToMbrChs();

    unsigned which_curr=i;
    ++i;
    while(i<disk->CountPartitions() && (disk->GetPartition(i).flags&PART_LOGICAL)==false)
        i++;

    if(i==disk->CountPartitions())
      {
        if(disk->GetPartition(which_curr-1).flags&PART_EXTENDED)
            ebr_sector=disk->GetPartition(which_curr-1).begin_sector;
        else
            ebr_sector=curr_part.begin_sector-spt;

        ebr.signature=BS_MAGIC;
        disk->DiskWrite(ebr_sector*disk->GetDiskGeometry().bps,&ebr,sizeof(EBR));
        return;
      }

    const GEN_PART &next_part=disk->GetPartition(i);

    ebr.partition_table[1].begin_lba=next_part.begin_sector-begin_sector_extd-spt;
    ebr.partition_table[1].lba_blocks=next_part.length+spt;
    ebr.partition_table[1].partition_type=0x05;

    chs=next_part.begin_sector;
    ebr.partition_table[1].begin_chs=chs.ToMbrChs();
    chs=next_part.begin_sector+curr_part.length;
    ebr.partition_table[1].end_chs=chs.ToMbrChs();

    ebr.signature=BS_MAGIC;
    disk->DiskWrite(ebr_sector*disk->GetDiskGeometry().bps,&ebr,sizeof(EBR));

    ebr_sector=next_part.begin_sector-spt;

  }
}
