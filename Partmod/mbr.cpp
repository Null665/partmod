//
// MBR spceific functions
//

#include "disk.h"
#include "mbr.h"
#include "chs.hpp"
#include "disk_exception.h"
#include <cstring>
#include <vector>
using namespace std;

void MBRHelper::ReadPartitionTables()
{
MBR mbr;
disk->ReadMBR(mbr);

GEN_PART gpart;
MBR_SPECIFIC mspec;

for(int i=0;i<4;i++)
  {
     if(mbr.partition_table[i].lba_blocks==0)
         continue;
     gpart.begin_sector=mbr.partition_table[i].begin_lba;
     gpart.length=mbr.partition_table[i].lba_blocks;
     gpart.flags=0;
     mspec.fsid=mbr.partition_table[i].partition_type;

     if(gpart.begin_sector>disk->LastSector() || (gpart.begin_sector+gpart.length)>disk->LastSector())
        throw DiskException(ERR_OUTSIDE_DISK);

     try{
         gpart.fsid=disk->fsid_man->GetByFsid(mbr.partition_table[i].partition_type).fsid_partmod;
     }
     catch(DiskException &de)
     {
         if(de.error_code==ERR_UNKNOWN_FSID)
         {
            disk->fsid_man->Add(mbr.partition_table[i].partition_type,"[Unknown]",-1,-1,0);
            gpart.fsid=disk->fsid_man->GetByFsid(mbr.partition_table[i].partition_type).fsid_partmod;
         }

     }

     switch(gpart.fsid)
       {
          case FS_EXTENDED:
            gpart.flags|=PART_EXTENDED;
            break;
          case FS_GPT:
            gpart.flags|=PART_MBR_GPT;
            break;
         default:
            gpart.flags|=PART_PRIMARY;
            break;
       }

     mbr.partition_table[i].status==0x80 ? gpart.flags|=PART_ACTIVE : gpart.flags&=~PART_ACTIVE;

     disk->add_partition(gpart);
     disk->set_mbr_specific(disk->CountPartitions()-1,mspec);
  }


}

void MBRHelper::WriteChanges()
{
GEN_PART gpart;
MBR mbr;
MBR_SPECIFIC mspec;

CHS chs;
chs.SetGeometry(disk->GetDiskGeometry().spt,disk->GetDiskGeometry().tpc);

if( disk->CountPartitions(PART_PRIMARY|PART_EXTENDED|PART_MBR_GPT)>4)
    throw(DiskException(ERR_MBR_LIMIT));

disk->ReadMBR(mbr);

for(int i=0;i<4;i++)
    memset(&mbr.partition_table[i],0,sizeof(PARTITION_RECORD)); // Clear only partition records, not whole MBR

for(int i=0,j=0;i<disk->CountPartitions();i++)
  {
     gpart=disk->GetPartition(i);
     if( (gpart.flags&PART_PRIMARY) || (gpart.flags&PART_EXTENDED || (gpart.flags&PART_MBR_GPT) ) )
       {

          mspec=disk->GetMBRSpecific(i);
          mbr.partition_table[j].begin_lba=gpart.begin_sector;
          mbr.partition_table[j].lba_blocks=gpart.length;

          if(gpart.flags&PART_EXTENDED)
              mbr.partition_table[j].partition_type=FSID_EXTENDED;
          else if(gpart.flags&PART_MBR_GPT)
              mbr.partition_table[j].partition_type=FSID_GPT;
          else mbr.partition_table[j].partition_type=mspec.fsid;

          gpart.flags&PART_ACTIVE ? mbr.partition_table[j].status=0x80 : mbr.partition_table[j].status=0x00;


          chs=gpart.begin_sector;
          mbr.partition_table[j].begin_chs=chs.ToMbrChs();

          chs=gpart.begin_sector+gpart.length;
          mbr.partition_table[j].end_chs=chs.ToMbrChs();

          j++;
       }
  }
if(mbr.disk_signature==0)
    mbr.disk_signature=rand();
mbr.signature=BS_MAGIC; // AA55

disk->DiskWrite(0,&mbr,sizeof(MBR));

}
