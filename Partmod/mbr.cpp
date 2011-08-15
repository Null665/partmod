// MBR spceific functions
//
//

#include "disk.h"
#include "mbr.h"
#include "disk_exception.h"
#include <cstring>
#include <vector>
using namespace std;



void Disk::save_mbr()
{
GEN_PART gpart;
MBR_CHS chs;
MBR mbr;
MBR_SPECIFIC mspec;

if( CountPartitions(PART_PRIMARY|PART_EXTENDED|PART_MBR_GPT)>4)
    throw(DiskException(ERR_MBR_LIMIT));

ReadMBR(mbr);

for(int i=0;i<4;i++)
    memset(&mbr.partition_table[i],0,sizeof(PARTITION_RECORD)); // Clear only partition records, not whole MBR

for(int i=0,j=0;i<CountPartitions();i++)
  {
     gpart=GetPartition(i);
     if( (gpart.flags&PART_PRIMARY) || (gpart.flags&PART_EXTENDED || (gpart.flags&PART_MBR_GPT) ) )
       {

          mspec=GetMBRSpecific(i);
          mbr.partition_table[j].begin_lba=gpart.begin_sector;
          mbr.partition_table[j].lba_blocks=gpart.length;
          if(gpart.flags&PART_EXTENDED)
              mbr.partition_table[j].partition_type=FSID_EXTENDED;
          else if(gpart.flags&PART_MBR_GPT)
              mbr.partition_table[j].partition_type=FSID_GPT;
          else mbr.partition_table[j].partition_type=mspec.fsid;

          gpart.flags&PART_ACTIVE ? mbr.partition_table[j].status=0x80 : mbr.partition_table[j].status=0x00;


               chs=lba_to_chs(gpart.begin_sector,diskio->GetDiskGeometry());
               memcpy(&mbr.partition_table[j].begin_chs,&chs,sizeof(MBR_CHS));

               chs=lba_to_chs(gpart.begin_sector+gpart.length,diskio->GetDiskGeometry());
               memcpy(&mbr.partition_table[j].end_chs,&chs,sizeof(MBR_CHS));

          j++;
       }
  }
mbr.signature=BS_MAGIC; // AA55

DiskWrite(0,&mbr,sizeof(MBR));

}


void Disk::ReadMBR(MBR &mbr)
{
  DiskRead(0,&mbr,sizeof(MBR));
}



void Disk::set_mbr_specific(unsigned p,MBR_SPECIFIC spec)
{
   GEN_PART gpart=GetPartition(p);
   memcpy(gpart.data,&spec,sizeof(MBR_SPECIFIC));
   modify_partition(p,gpart);
}

MBR_SPECIFIC Disk::GetMBRSpecific(unsigned p)
{
    MBR_SPECIFIC mspec;
    memcpy(&mspec,GetPartition(p).data,sizeof(MBR_SPECIFIC));
    return mspec;
}
