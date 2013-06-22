#ifndef GPT_H
#define GPT_H

#include "definitions.h"



// ============= GPT disk structs =================

#pragma pack(1)
// GPT partition table header
struct GPT
{
  uint8_t  signature[8];          // "EFI PART"
  uint32_t version;               // 00 00 01 00 for version 1.00
  uint32_t header_size;           // Usually 5C 00 00 00 == 92 bytes
  uint32_t checksum;              // Checsum (CRC32) of this header; this field must be zero before calculation
  uint32_t unknown;               // Or reserved
  uint64_t current_lba;           // Location of this header
  uint64_t backup_lba;            // Location of the backup
  uint64_t first_usable_lba;      // First usable lba for partitions
  uint64_t last_usable_lba;       // Last usable lba for partitions
  __GUID   disk_guid;             // Disk GUID
  uint64_t first_entry_lba;       // Where partition entries begin
  uint32_t n_entries;             // Number of partition entries, must be at least 128 according to EFI standard
  uint32_t entry_size;            // Usually 128
  uint32_t entries_checksum;      // CRC32 checksum of partition entries
  uint8_t  reserved[420];         // Should be zeros

};

// GPT partition entry
struct GPT_ENTRY
{
  __GUID   type_guid;              // GUID of partition type
  __GUID   unique_guid;            // GUID unique for this partition
  uint64_t begin_lba;              // Begining of partition in LBA blocks
  uint64_t end_lba;                // End lba of partition
  uint64_t  flags;                 // Various flags
  uint16_t name[36];               // Partition name -- 36 UTF-16LE chars
} ;



struct GPT_SPECIFIC
{
  __GUID type_guid;
  __GUID unique_guid;
  uint16_t name[36];
  uint64_t flags;
};

#pragma pack()

class Disk;
class GPTHelper
{
public:
  GPTHelper(Disk *disk) {this->disk=disk;}

  void ReadPartitionTables(GEN_PART);
  void WriteChanges();

  bool IsValidGPT(GPT gpt);

  GPT CreateGPT(GEN_PART gpt_part);
  GPT CreateGPT(GEN_PART gpt_part,int n_gpt_entries);

/* TODO:
 * GPT_ENTRY ReadGPTEntryFromDisk(uint32_t n);
 * GPT_ENTRY WriteGPTEntryToDisk(const GPT_ENTRY &e,uint32_t n);
 * bool IsBackupIdentical();
 *
*/

protected:
  void WriteGPT(const GPT&);
  uint32_t WritePartitionEntries(GPT gpt);
  void WriteBackup(GPT gpt);

  GPT ReadGPTFromBackup(const GEN_PART &gpt_part);

  Disk *disk;
};



#endif
