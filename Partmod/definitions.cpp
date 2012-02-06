/*
 * TODO: move Windows-specific stuff to a separate file
 *
*/
#include "definitions.h"
#include "dllcall.hpp"


#if defined(_WIN32) || defined (WIN32)
#include <windows.h>
#include <winreg.h>
#include <winioctl.h>
#endif

#include "../diskio/diskio.hpp"
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cstdio>
using namespace std;

//
// Error strings
//
const char *error_table[]=
{
  "", // Success
  "Could not open disk",                             // 1
  "Could not open a file",
  "Could not create a file",
  "Could not read from disk",
  "Could not write to disk",                         // 5
  "Colud not seek to specified position ",
  "Partition doesn't exist",
  "LBA Extended partition not found",
  "Impossible to resize partition",
  "Failed to resize a partiton",                      //10
  "Cannot create more than 4 primary partitions",
  "Disk is full",
  "Unknown error",
  "Not enough space",
  "Partition is too small",                          // 15
  "Impossible to create a primary partition on extended partition",
  "File is not a valid backup file",
  "GPT primary partition not found",
  "Disk geometry is unknown",
  "Can't split partition",                           // 20
  "One or more partitions overlap each other",
  "GPT already exists on disk",
  "Invalid checksum",
  "Disk size registered in backup and size of selected disk don't mach",
  "\"Bytes per sector\" value is not a power of two", // 25
  "Partition begins or ends outside disk",
  "Logical volume is outside extended partition",
  "Pending action doesn't exist (array index out of bounds)",
  "Array (vector) index out of bounds",
  "Failed to get disk size",                        // 30
  "Close the current disk first",
};



unsigned short  GET_CYLINDER (MBR_CHS chs)
{
  return ( (chs.cylinder_bits<<8)|(chs.cylinder));
}
unsigned short  GET_HEAD     (MBR_CHS chs)
{
  return (chs.head);
}
unsigned short  GET_SECTOR   (MBR_CHS chs)
{
  return (chs.sector);
}

void SET_CYLINDER (MBR_CHS &chs, short cyl)
{
  chs.cylinder_bits=cyl>>8;
  chs.cylinder=cyl&0xFF;
}
void SET_HEAD     (MBR_CHS &chs, unsigned char head)
{
  chs.head=head;
}
void SET_SECTOR   (MBR_CHS &chs, unsigned char sector)
{
  chs.sector=sector;
}


string GetErrorDescription(int error_code)
{
  return error_table[error_code];
}

bool Dump2File(const char *fn,const void *data,int size)
{
  FILE *fp=fopen(fn,"wb");
  if(fp==0)
    return false;
  fwrite(data,size,1,fp);
  fclose(fp);
  return true;
}



// ==> reiktu sukurti 2 versijas, viena MBR CHS, o kita apskritai CHS (kad rodytu ir virs mbr limitu)
MBR_CHS lba_to_chs(uint64_t _lba,GEOMETRY _dg)
{
  MBR_CHS chs= {0};
  if(_lba>=CHS_LIMIT)
    {
      SET_CYLINDER(chs,1023);
      SET_HEAD(chs,254);
      SET_SECTOR(chs,63);
      return chs;
    }
  unsigned int
  tpc=_dg.tpc,
      spt=_dg.spt,
          cylinder,head,sector,
          temp=0;

  cylinder =_lba / (tpc * spt);
  temp     = _lba % (tpc * spt);
  head     = temp / spt;
  sector   = temp % spt + 1;

  SET_CYLINDER(chs,cylinder);
  SET_HEAD(chs,head);
  SET_SECTOR(chs,sector);


  return chs;
}

uint64_t chs_to_lba(MBR_CHS _chs,GEOMETRY _dg)
{
  uint64_t lba;

  unsigned int
  tpc=_dg.tpc,
      spt=_dg.spt;

  lba= ( ( _chs.cylinder * tpc + _chs.head ) * spt ) + _chs.sector - 1;

  return lba;
}






string U64ToStr(uint64_t number,unsigned int base)
{
  stringstream ss;

  switch (base)
    {
    case STR_OCT:
      ss<<oct;
      break;
    case STR_HEX:
      ss<<hex;
      break;
    }
  ss <<number;
  return ss.str();
}

uint64_t StrToU64(string str,unsigned base)
{
  uint64_t result;
  stringstream ss;

  switch (base)
    {
    case STR_OCT:
      ss<<oct;
      break;
    case STR_HEX:
      ss<<hex;
      break;
    }

  ss<<str;
  ss>>result;
  return result;
}

string size_str(uint64_t num_of_sect,int bps)
{
  string suffix;
  uint64_t div=0;
  stringstream tmp;

  if(num_of_sect>PB/bps)
    {
      suffix=" PB";
      div=PB/bps;
    }
  if(num_of_sect>TB/bps)
    {
      suffix=" TB";
      div=TB/bps;
    }
  else if(num_of_sect>GB/bps)
    {
      suffix=" GB";
      div=GB/bps;
    }
  else if(num_of_sect>MB/bps)
    {
      suffix=" MB";
      div=MB/bps;
    }
  else if(num_of_sect>KB/bps)
    {
      suffix=" KB";
      div=KB/bps;
    };

  if(num_of_sect==0)
    {
      tmp <<0<< " KB";
      return tmp.str();
    }
  if(div==0)
    return "[error]";

  tmp<<setprecision(4)<<((long double)num_of_sect/div)<<suffix;
  return tmp.str();
}

void set_guid(__GUID &guid,uint32_t one,uint16_t two, uint16_t three,uint64_t four)
{
  guid.one=one;
  guid.two=two;
  guid.three=three;
  guid.four=four;

}

//
// TODO: Write a real GUID generation algorithm
//
void generate_guid(__GUID &guid)
{
  guid.one=rand()% ~(uint32_t)(0);
  guid.two=rand()% ~(uint16_t)(0);
  guid.three=rand()% ~(uint16_t)(0);
  guid.four=rand()% ~(uint64_t)(0)*rand();
}



///////// Windows Specific ////////////////////

#if defined(_WIN32) || defined (WIN32)

//
// TODO: improve this function to show UNC paths too
//
string get_mount_point(GEN_PART gpart,unsigned int disk_signature)
{

#pragma pack(1)
  struct reg_key
  {
    uint32_t disk_signature;
    uint64_t offset;
  } data;
#pragma pack(0)

  memset(&data,0,sizeof(data));

  HKEY hkey;
  DWORD type=REG_BINARY;
  stringstream ss;
  string ret;

  for(char i='C'; i<='Z'; ++i)
    {
      DWORD size=sizeof(reg_key);
      ss.str("");
      memset(&data,0,sizeof(data));

      ss<<"\\DosDevices\\"<<i<<':';
      RegOpenKey(HKEY_LOCAL_MACHINE,"SYSTEM\\MountedDevices",&hkey);

      if(hkey==INVALID_HANDLE_VALUE)  continue;
      RegQueryValueEx(hkey,ss.str().c_str(),0,&type,(BYTE*)&data,&size);

      if(disk_signature==data.disk_signature && gpart.begin_sector==(data.offset/512) ) return ss.str();
      RegCloseKey(hkey);
    }

  return "Unknown";
}


void clrscr(void)
{
  COORD coordScreen = { 0, 0 };  // upper left corner
  DWORD cCharsWritten;
  DWORD dwConSize;
  HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO  csbi;

  GetConsoleScreenBufferInfo(hCon, &csbi);
  dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  // fill with spaces
  FillConsoleOutputCharacter(hCon, TEXT(' '), dwConSize, coordScreen, &cCharsWritten);
  GetConsoleScreenBufferInfo(hCon, &csbi);
  FillConsoleOutputAttribute(hCon, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
  // cursor to upper left corner
  SetConsoleCursorPosition(hCon, coordScreen);
}

#else

string get_mount_point(GEN_PART gpart,unsigned int disk_signature)
{
  return "";
}


#endif
