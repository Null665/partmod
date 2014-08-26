/*
 * TODO: move Windows-specific stuff to a separate file
 *
*/
#include "definitions.h"

#if defined(_WIN32) || defined (WIN32)
    #include <windows.h>
    #include <winreg.h>
#endif

#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cstdio>
using namespace std;


bool Dump2File(const char *fn,const void *data,int size)
{
  FILE *fp=fopen(fn,"wb");
  if(fp==0)
    return false;
  fwrite(data,size,1,fp);
  fclose(fp);
  return true;
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
  else if(num_of_sect>TB/bps)
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


  struct alignas(1) reg_key
  {
    uint32_t disk_signature;
    uint64_t offset;
  } data;


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

  return "[Unknown]";
}

uint64_t GetFileSize(std::string filename)
{
  HANDLE hFile=CreateFile(filename.c_str(),GENERIC_READ|GENERIC_WRITE,7,0,OPEN_EXISTING,0,0);
  if(hFile==INVALID_HANDLE_VALUE)
      return 0;

  LARGE_INTEGER li,unused;
  memset(&li,0,sizeof(LARGE_INTEGER));
  memset(&unused,0,sizeof(LARGE_INTEGER));
  SetFilePointerEx(hFile,unused,&li,FILE_END);
  CloseHandle(hFile);
  return li.QuadPart;
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
  return "[not implemented]";
}

uint64_t GetFileSize(std::string filename)
{
    uint64_t file_size;
    FILE *fp=fopen(filename.c_str(),"rb");

    if(fp==0)
        return 0;

    fseek(fp,0,SEEK_END);
    file_size=ftello64(fp);

    fclose(fp);
    return file_size;
}


#endif
