//
// GPT specific functions
//

// TODO: Restore GPT from backup

#define GPT_VERSION        0x10000
#define GPT_MIN_ENTRIES    128

#include "disk.h"
#include "crc.hpp"
#include "disk_exception.h"

#include <cstring>
#include <memory>  // for std::unique_ptr
using namespace std;


void GPTHelper::WriteChanges()
{
if(disk->CountPartitions(PART_MBR_GPT)==0)
    return;

GPT gpt;
/*
disk->ReadGPT(gpt);

if(IsValidGPT(gpt)==false)
  {
    RestoreGPTFromBackup(disk->GetPartition(which_gpt));
    disk->ReadGPT(gpt);
    if(IsValidGPT(gpt)==false)
      {
         gpt=CreateGPT(disk->GetPartition(which_gpt));
      }
  }*/

gpt=CreateGPT(disk->GetPartition(0,PART_MBR_GPT));

gpt.entries_checksum=WritePartitionEntries(gpt);
gpt.checksum=0;

CRC32 crc;
crc.Hash(&gpt,gpt.header_size);
gpt.checksum=crc.Get();


WriteGPT(gpt);
WriteBackup(gpt);
}


GPT GPTHelper::ReadGPTFromBackup(const GEN_PART &gpt_part)
{
  GPT gpt;
  uint64_t seek_pos=(gpt_part.begin_sector+gpt_part.length)*disk->GetDiskGeometry().bps;
  disk->DiskRead( seek_pos,&gpt,sizeof(gpt));
  return gpt;
}



GPT GPTHelper::CreateGPT(GEN_PART gpart)
{
  return CreateGPT(gpart,GPT_MIN_ENTRIES);
}

//
// Create a new GPT
// Initializes all GPT header values except checksums.
GPT GPTHelper::CreateGPT(GEN_PART gpart,int n_gpt_entries)
{
    if(n_gpt_entries<GPT_MIN_ENTRIES)
        throw DiskException("GUID partition table must have at least 128 entries");

    GPT gpt;
    memset(&gpt,0,sizeof(GPT));

    int bps=disk->GetDiskGeometry().bps;

    generate_guid(gpt.disk_guid);
    memcpy(&gpt.signature,"EFI PART",8);
    gpt.version=GPT_VERSION;
    gpt.backup_lba=gpart.begin_sector+gpart.length; // FIXME/CHECK: Is this correct?
    gpt.unknown=0;
    gpt.n_entries=n_gpt_entries;
    gpt.entry_size=sizeof(GPT_ENTRY);


    gpt.header_size=sizeof(gpt)-sizeof(gpt.reserved); // == 92
    gpt.checksum=0;

    gpt.current_lba=gpart.begin_sector;
    gpt.last_usable_lba=gpart.begin_sector+gpart.length-sizeof(GPT)-sizeof(GPT_ENTRY)*gpt.n_entries;
   // gpt.last_usable_lba=gpart.begin_sector+gpart.length- ((sizeof(GPT)-sizeof(GPT_ENTRY)*gpt.n_entries)/bps);
    gpt.first_entry_lba=gpart.begin_sector+1;
    gpt.first_usable_lba=gpart.begin_sector+ ((gpt.entry_size*gpt.n_entries)/bps); // FIXE/CHECK: is this correct?

    return gpt;
}



bool GPTHelper::IsValidGPT(GPT gpt)
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
void GPTHelper::WriteGPT(const GPT &gpt)
{
    int bps=disk->GetDiskGeometry().bps;
    disk->DiskWrite(gpt.current_lba*bps,&gpt,sizeof(GPT));
}

//
// Write GPT header and GPT entries to the end of the disk
//
void GPTHelper::WriteBackup(GPT gpt)
{
    int bps=disk->GetDiskGeometry().bps;
    CRC32 crc;

    gpt.current_lba=gpt.backup_lba;
    gpt.backup_lba=gpt.backup_lba;
    gpt.checksum=0;

   // gpt.first_entry_lba=gpt.current_lba-1;

    crc.Hash(&gpt,gpt.header_size);
    gpt.checksum=crc;

    disk->DiskWrite( (gpt.backup_lba)*bps,&gpt,sizeof(GPT));

    unsigned buff_size=gpt.entry_size*gpt.n_entries;
    uint8_t *buff[buff_size];

    disk->DiskRead(gpt.first_entry_lba*bps,buff,buff_size);
    disk->DiskWrite((gpt.backup_lba*bps)-(buff_size-buff_size%bps),buff,buff_size);

}

//
// Write GPT entries to disk.
//
uint32_t GPTHelper::WritePartitionEntries(GPT gpt)
{
GEN_PART gpart;
int bps=disk->GetDiskGeometry().bps;
CRC32 crc;

int n_gpt_entry=0; // number of gpt entries
for(int i=0;i<disk->CountPartitions();i++)
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

     crc.Hash(&entry,sizeof(entry));
  }

//
// Clear unused entries and finish calculating checksum
//
  uint32_t unused_bytes= ((gpt.n_entries-n_gpt_entry)*gpt.entry_size);
  unique_ptr<uint8_t[]> buff(new uint8_t[unused_bytes]);
  memset(buff.get(),0,unused_bytes);

  uint64_t write_loc=gpt.first_entry_lba*bps+(n_gpt_entry*gpt.entry_size);
  disk->DiskWrite(write_loc,buff.get(),unused_bytes);
  crc.Hash(buff.get(),unused_bytes);

  return crc.Get();
}



void GPTHelper::ReadPartitionTables(GEN_PART gpt_part)
{
int bps=disk->GetDiskGeometry().bps;

GPT gpt;
//disk->ReadGPT(gpt);
disk->DiskRead(1*bps,&gpt,sizeof(GPT));

bool use_backup=false;

if(!IsValidGPT(gpt))
  {
      use_backup=true;
      gpt=ReadGPTFromBackup(gpt_part);

      if(!IsValidGPT(gpt))
        {
          throw(DiskException("Invalid main GPT header and backup GPT header"));
        }

  }


unique_ptr<GPT_ENTRY[]> entries(new GPT_ENTRY[gpt.n_entries]);

if(use_backup) // Invalid main GPT header - assume that entries are invalid too - read entries from backup
  {
    uint64_t read_loc=(gpt.backup_lba*bps)-sizeof(GPT)-sizeof(GPT_ENTRY)*gpt.n_entries;
    disk->DiskRead(read_loc ,entries.get(),sizeof(GPT_ENTRY)*gpt.n_entries);
  }
else
    disk->DiskRead(gpt.first_entry_lba*bps,entries.get(),sizeof(GPT_ENTRY)*gpt.n_entries);

for(unsigned i=0;i<gpt.n_entries;++i)
  {
     if(entries[i].begin_lba==0 && entries[i].end_lba==0)
         continue;
     else if(entries[i].begin_lba>=gpt.first_usable_lba && entries[i].begin_lba<=gpt.last_usable_lba)
       {
         if(entries[i].begin_lba<entries[i].end_lba)
           {
              GEN_PART gpart;
              gpart.begin_sector=entries[i].begin_lba;
              gpart.length=entries[i].end_lba-entries[i].begin_lba;
              gpart.flags=PART_GPT;

              disk->add_partition(gpart);

              GPT_SPECIFIC gspec;
              gspec.flags=entries[i].flags;
              gspec.type_guid=entries[i].type_guid;
              gspec.unique_guid=entries[i].unique_guid;
              memcpy(gspec.name,entries[i].name,32*sizeof(uint16_t));
              disk->set_gpt_specific(disk->CountPartitions()-1,gspec);
           }
          else throw(DiskException("Invalid GPT entry: begin_lba>end_lba"));
       }
     else throw DiskException("Invalid GPT entry: begin_lba<first_usable_lba or  begin_lba>last_usable_lba");
  }

}
