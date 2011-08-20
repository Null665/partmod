//
// GPT specific functions
//

#include "disk.h"
#include <cstring>
#include "crc.hpp"
#include "disk_exception.h"

#include <iostream>
using namespace std;


void GPTParser::WriteChanges()
{
if(disk->CountPartitions(PART_GPT)==0)
    return;

int which_gpt=0;
for(int i=0;i<disk->CountPartitions();i++)
    if(disk->GetPartition(i).flags&PART_MBR_GPT)
        which_gpt=i;


GPT gpt;
disk->ReadGPT(gpt);

if(IsValidGPT(gpt)==false)
  {
    RestoreGPTFromBackup(disk->GetPartition(which_gpt));
    disk->ReadGPT(gpt);
    if(IsValidGPT(gpt)==false)
      {
         gpt=CreateGPT(disk->GetPartition(which_gpt));
      }


  }

DeleteAllEntries(gpt);
WritePartitionEntries(gpt);


WriteGPT(disk->GetPartition(which_gpt),gpt);
WriteBackup(gpt);
}




void GPTParser::RestoreGPTFromBackup(GEN_PART gpt_part)
{

}

//
// Create a new GPT
//
GPT GPTParser::CreateGPT(GEN_PART gpart)
{
GPT gpt;
int bps=disk->GetDiskGeometry().bps;

memset(&gpt,0,sizeof(GPT));
generate_guid(gpt.disk_guid);
memcpy(&gpt.signature,"EFI PART",8);
gpt.backup_lba=gpart.begin_sector+gpart.length; // FIXME/CHECK: Is this correct?
gpt.unknown=0;
gpt.n_entries=128;
gpt.entry_size=sizeof(GPT_ENTRY);


gpt.header_size=sizeof(gpt)-sizeof(gpt.reserved); // == 92
gpt.checksum=0;

gpt.current_lba=gpart.begin_sector;
gpt.last_usable_lba=gpart.begin_sector+gpart.length;
gpt.first_entry_lba=gpart.begin_sector+1;
gpt.first_usable_lba=gpart.begin_sector+ ((gpt.entry_size*gpt.n_entries)/bps); // FIXE/CHECK: is this correct?

CRC32 crc;
crc.Hash(&gpt,gpt.header_size);
gpt.checksum=crc.Get();


return gpt;
}

void Disk::ReadGPT(GPT &gpt)
{
int n_gpt_parts=0,which_gpt=0;

memset(&gpt,0,sizeof(GPT));

for(int i=0;i<CountPartitions();i++)
    if(GetPartition(i).flags&PART_MBR_GPT)
      {
         ++n_gpt_parts;
         which_gpt=i;
      }

if(n_gpt_parts==1)
     DiskRead(GetPartition(which_gpt).begin_sector*diskio->GetDiskGeometry().bps,&gpt,sizeof(GPT));
else if(n_gpt_parts==0)
    throw(DiskException(ERR_GPT_NOT_FOUND));
else // More than one GPT?
    throw(DiskException(ERR_UNKNOWN_ERROR,"There's more than one GPT (?)"));

}


bool GPTParser::IsValidGPT(GPT gpt)
{
// Check the GPT signature
if(memcmp(gpt.signature,"EFI PART",8)!=0)
    return false;

// Check the checksum
CRC32 crc;
uint32_t old_checksum=gpt.checksum;
uint32_t calculated_checksum;

gpt.checksum=0;
crc.Hash(&gpt,gpt.header_size);

calculated_checksum=crc.Get();

if(calculated_checksum==old_checksum)
    return true;
else
    return false;

}





//
// Write GPT header to disk
//
void GPTParser::WriteGPT(GEN_PART gpart,GPT gpt)
{
    int bps=disk->GetDiskGeometry().bps;
    disk->DiskWrite(gpart.begin_sector*bps,&gpt,sizeof(GPT));
}

//
// Write GPT header and GPT entries to the end of the disk
//
void GPTParser::WriteBackup(GPT gpt)
{
    int bps=disk->GetDiskGeometry().bps;
    disk->DiskWrite( (gpt.backup_lba)*bps,&gpt,sizeof(GPT));


    GEN_PART gpart;
    for(int i=0,n_gpt_entry=0;i<disk->CountPartitions();i++)
      {
         gpart=disk->GetPartition(i);
         if( !(gpart.flags&PART_GPT) )
             continue;
         GPT_ENTRY entry;
         GPT_SPECIFIC gspec;

         gspec=disk->GetGPTSpecific(i);

         entry.begin_lba=gpart.begin_sector;
         entry.end_lba=gpart.begin_sector+gpart.length;
         entry.flags=gspec.flags;
         memcpy(entry.name,gspec.name,32*sizeof(uint16_t));
         entry.type_guid=gspec.type_guid;
         entry.unique_guid=gspec.unique_guid;

         uint64_t write_loc=gpt.last_usable_lba*bps-(n_gpt_entry*gpt.entry_size);
         disk->DiskWrite(write_loc,reinterpret_cast<char*>(&entry),sizeof(entry));

          n_gpt_entry++;
      }


}


void GPTParser::WritePartitionEntries(GPT gpt)
{
GEN_PART gpart;
int bps=disk->GetDiskGeometry().bps;


for(int i=0,n_gpt_entry=0;i<disk->CountPartitions();i++)
  {
     gpart=disk->GetPartition(i);
     if( !(gpart.flags&PART_GPT) )
         continue;
     GPT_ENTRY entry;
     GPT_SPECIFIC gspec;


     gspec=disk->GetGPTSpecific(i);

     entry.begin_lba=gpart.begin_sector;
     entry.end_lba=gpart.begin_sector+gpart.length;
     entry.flags=gspec.flags;
     memcpy(entry.name,gspec.name,32*sizeof(uint16_t));
     entry.type_guid=gspec.type_guid;
     entry.unique_guid=gspec.unique_guid;

     uint64_t write_loc=gpt.first_entry_lba*bps+(n_gpt_entry*gpt.entry_size);

     disk->DiskWrite(write_loc,&entry,sizeof(entry));
     n_gpt_entry++;
  }



}


void GPTParser::DeleteAllEntries(GPT gpt)
{
int bps=disk->GetDiskGeometry().bps;
uint32_t nsect=(sizeof(GPT_ENTRY)*gpt.n_entries)/bps;

uint8_t *buff=new uint8_t[bps];
memset(buff,0,bps);

for(unsigned i=0;i<nsect;i++)
     disk->DiskWrite( (gpt.first_entry_lba+i)*bps,buff,bps);

for(unsigned i=0;i<nsect;i++)
    disk->DiskWrite( (gpt.last_usable_lba-nsect)*bps,buff,bps);

delete[] buff;
}


void GPTParser::ParsePartition(GEN_PART gpt_part)
{
GPT gpt;
GPT_ENTRY *entries;
GEN_PART gpart;
GPT_SPECIFIC gspec;

int bps=disk->GetDiskGeometry().bps;

disk->ReadGPT(gpt);

if(!IsValidGPT(gpt))
  {
      throw(DiskException("Invalid GPT"));
  }


entries=new GPT_ENTRY[gpt.n_entries];
disk->DiskRead(gpt.first_entry_lba*bps,entries,sizeof(GPT_ENTRY)*gpt.n_entries);


for(unsigned i=0;i<gpt.n_entries;++i)
  {
     if(entries[i].begin_lba>=gpt.first_usable_lba && entries[i].begin_lba<=gpt.last_usable_lba)
         if(entries[i].begin_lba<entries[i].end_lba)
           {
              gpart.begin_sector=entries[i].begin_lba;
              gpart.length=entries[i].end_lba-entries[i].begin_lba;
              gpart.flags=PART_GPT;

              disk->add_partition(gpart);

              gspec.flags=entries[i].flags;
              gspec.type_guid=entries[i].type_guid;
              gspec.unique_guid=entries[i].unique_guid;
              memcpy(gspec.name,entries[i].name,32*sizeof(uint16_t));
              disk->set_gpt_specific(disk->CountPartitions()-1,gspec);
           }
         else throw(DiskException(ERR_UNKNOWN_ERROR));
  }
 delete[] entries;
}

GPT_SPECIFIC Disk::GetGPTSpecific(unsigned p)
{
    GPT_SPECIFIC gspec;
    memcpy(&gspec,GetPartition(p).data,sizeof(GPT_SPECIFIC));
    return gspec;

}

void Disk::set_gpt_specific(unsigned p,GPT_SPECIFIC gspec)
{
   part_man->SetSpecificData(p,reinterpret_cast<void*>(&gspec),sizeof(gspec));
}
