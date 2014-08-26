#ifndef CHS_HPP
#define CHS_HPP
// This class allows to convert LBA to CHS adressing used by MBR and vice versa
// Examples
/*
1:
    CHS chs(cyl,head,sect);
    chs.SetGeometry(sectors_per_track,tracks_per_cylinder);
    unsigned sector=chs.ToLBA();

2:
    CHS chs(1023); // secotr 1023
    chs.SetGeometry(sectors_per_track,tracks_per_cylinder);
    chs=2048;
    chs++; // now 2049
    MBR_CHS c=chs.ToMbrChs();


*/
#include <stdint.h>

// Cylinder-head-sector structure for MBR
struct alignas(1) MBR_CHS
{
  uint8_t head;             // head (<255)
  uint8_t sector:6;         // sector (<64)
  uint8_t cylinder_bits:2;  // 2 bits of cylinder value
  uint8_t cylinder   ;      // cylinder (<=1023)
};

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
