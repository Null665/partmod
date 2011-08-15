#include "gui.h"
#include "../Partmod/chs.hpp"
#include "../Partmod/disk.h"
#include <algorithm>
#include <cstdarg>
using namespace std;


//
// Enables or disables buttons and menu items
// the last parameter must be set to 0, eg. EnableButtons(hwnd,true,IDB_BTN1,IDB_BTN2,IDB_BTN3,0);
//
void EnableButtons(HWND hwnd,bool enable,UINT buttons,...)
{
   HMENU hmenu=GetMenu(hwnd);
   UINT button=buttons,i=0;
   va_list vl;
   va_start(vl,buttons);
   while(button!=0)
     {

        if(enable)
          {
             EnableWindow(GetDlgItem(hwnd,button),true);
             EnableMenuItem(hmenu,button,MF_ENABLED);
          }
        else
          {
             EnableWindow(GetDlgItem(hwnd,button),false);
             EnableMenuItem(hmenu,button,MF_GRAYED);
          }
       button=va_arg(vl,UINT);
       i++;
     }
    va_end(vl);
}




string GetFileName(bool open,HWND hwnd,const char *str_filter)
{
  const char *filter=str_filter;
  char filename[1024];
  memset(filename,0,sizeof(filename));

  OPENFILENAME ofn;
  memset(&ofn,0,sizeof(OPENFILENAME));

  ofn.lStructSize=sizeof(OPENFILENAME);
  ofn.hwndOwner=hwnd;
  ofn.lpstrFilter=filter;
  ofn.lpstrFile=filename;
  ofn.nMaxFile=1024;
  ofn.Flags= OFN_HIDEREADONLY | OFN_EXPLORER;

  if(!GetOpenFileName(&ofn))
    {
      return "";
    }
return filename;
}






bool cmp_lv(lvlist a,lvlist b)
  {
    return StrToU64((char*)a.begin_sect.c_str())<StrToU64((char*)b.begin_sect.c_str());
  }

