#ifndef EBR_H
#define EBR_H

#include "definitions.h"
#include "mbr.h"

//
// Extended Boot Record
//
struct EBR
{
  uint8_t             boot_code[446];
  PARTITION_RECORD    partition_table[2];
  uint8_t             unused[32];
  uint16_t            signature;          //AA55
};


class Disk;
class EBRHelper
{
public:
  EBRHelper(Disk *disk){this->disk=disk;}

  void ReadPartitionTables(GEN_PART);
  void WriteChanges();
protected:
  Disk *disk;
};


#endif
