//
// EBR specific functions
//



#include "disk.h"
#include "mbr.h"
#include <cstring>
using namespace std;

void EBRParser::ParsePartition(GEN_PART extended)
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

       // No logical volumes on extended partition
       if(ebr.partition_table[0].lba_blocks==0)
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


void EBRParser::WriteChanges()
{
vector <EBR_DATA> ebrs;

int spt=disk->GetDiskGeometry().spt;

//
// Find out which partition is the extended partition
//
int extended_part=-1;
for(int i=0;i<disk->CountPartitions();i++)
    if(disk->GetPartition(i).flags&PART_EXTENDED)
      {
         extended_part=i;
         break;
      }
if(extended_part==-1) // If extended partition doesn't exist
    return;


//
// Find out how many logical partitions exist
//
int first_logical=extended_part+1;
int n_logical=0;
for(int i=extended_part;i<disk->CountPartitions();i++)
  {
      if(disk->GetPartition(i).flags&PART_LOGICAL)
        n_logical++;
  }

//
// If there are no logical volumes, clear the ext. part. boot sector
//
if(n_logical==0)
  {
      EBR ebr;
      memset(&ebr,0,sizeof(EBR));
      ebr.signature=BS_MAGIC;

      disk->DiskWrite(disk->GetPartition(extended_part).begin_sector,&ebr,sizeof(EBR));
      return;
  }

//
// NOW LET'S FUCK UP THE PARTITION TABLE!!!1
//

uint64_t begin_sector_extd=disk->GetPartition(extended_part).begin_sector;
uint64_t ebr_sector;

uint64_t prev_end_sect=begin_sector_extd;

for(int i=first_logical-1;i<first_logical+n_logical;i++)
  {
      GEN_PART gpart=disk->GetPartition(i);
      MBR_SPECIFIC mspec=disk->GetMBRSpecific(i);


      //
      // Location of the EBR
      //
      ebr_sector=gpart.begin_sector-spt;

      EBR ebr;
      memset(&ebr,0,sizeof(EBR));

      // Fill the 1st partition entry
    // ebr.partition_table[0].begin_lba=spt;
      ebr.partition_table[0].begin_lba=spt;
      ebr.partition_table[0].lba_blocks=gpart.length;
      ebr.partition_table[0].partition_type=mspec.fsid;
      gpart.flags&PART_ACTIVE ?
          ebr.partition_table[i].status=0x80 :
          ebr.partition_table[i].status=0x00;

      MBR_CHS chs=lba_to_chs(gpart.begin_sector,disk->GetDiskGeometry());
      memcpy(&ebr.partition_table[0].begin_chs,&chs,sizeof(MBR_CHS));
      chs=lba_to_chs(gpart.begin_sector+gpart.length,disk->GetDiskGeometry());
      memcpy(&ebr.partition_table[0].end_chs,&chs,sizeof(MBR_CHS));


      prev_end_sect=gpart.begin_sector+gpart.length;

      // Fill, the 2nd partition entry
      // if there are more logical volumes
      if(i<(first_logical+n_logical-1))
        {
          GEN_PART next_part=disk->GetPartition(i+1);

          ebr.partition_table[1].begin_lba=next_part.begin_sector-begin_sector_extd-spt;
          ebr.partition_table[1].lba_blocks=next_part.length+spt;
          ebr.partition_table[1].partition_type=0x05;

          MBR_CHS chs=lba_to_chs(next_part.begin_sector,disk->GetDiskGeometry());
          memcpy(&ebr.partition_table[1].begin_chs,&chs,sizeof(MBR_CHS));
          chs=lba_to_chs(next_part.begin_sector+gpart.length,disk->GetDiskGeometry());
          memcpy(&ebr.partition_table[1].end_chs,&chs,sizeof(MBR_CHS));

        }
       else memset(&ebr.partition_table[1],0,sizeof(ebr.partition_table[1]));
     ebr.signature=BS_MAGIC;

     // EBR location in bytes
     ebr_sector*=disk->GetDiskGeometry().bps;
     // Read the boot code (just in case)
  //   disk->DiskRead(ebr_sector,&ebr.boot_code,sizeof(ebr.boot_code));

     disk->DiskWrite(ebr_sector,&ebr,sizeof(EBR));


  }


}
