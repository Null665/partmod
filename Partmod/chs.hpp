#ifndef CHS_HPP
#define CHS_HPP

#include <stdint.h>

#pragma pack(1)
// Cylinder-head-sector structure for MBR
struct MBR_CHS
{
  uint8_t head;             // head (<255)
  uint8_t sector:6;         // sector (<64)
  uint8_t cylinder_bits:2;  // 2 bits of cylinder value
  uint8_t cylinder   ;      // cylinder (<=1023)
};
#pragma pack()

//
// A class for CHS manipulation
//

class CHS
{
  long long cylinder,
            head,
            sector;
  unsigned int spt, // sectors per track
               tpc; // tracks per cylinder

  void calc(long long);

public:

  CHS(int c,int h, int s);
  CHS(long long lba);
  CHS();

  bool SetCylinder(long long);
  bool SetHead(unsigned int);
  bool SetSector(unsigned int);

  unsigned long GetCylinder();
  unsigned int GetHead();
  unsigned int GetSector();

  bool SetGeometry(int spt,int tpc);
  int GetSPT();
  int GetTPC();
  void SetSPT(int);
  void SetTPC(int);

  long long ToLBA();

  CHS &operator =(long long);
  bool operator ==(CHS &chs);
  CHS &operator ++();
  CHS &operator ++(int);

  MBR_CHS ToMbrChs();


};

#endif
