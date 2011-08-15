#include "gui.h"
#include "../Partmod/chs.hpp"
#include "../Partmod/disk.h"
#include "resource.h"
#include <string>
using namespace std;

void DlgDiskImage::InitDialog(Disk *disk,HINSTANCE hinst,HWND hwnd)
{
   this->disk=disk;
 //  old_app_state=GetWindowLong(hwnd,DWL_USER);
   DialogBoxParam(hinst,MAKEINTRESOURCE(IDD_DISKIMAGE),hwnd,DlgProc,(LPARAM)this);
}




BOOL CALLBACK DlgDiskImage::DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
DlgDiskImage *diskimage=get_this<DlgDiskImage>(hwnd);

switch(message)
  {
     case WM_INITDIALOG:
          init_set_this<DlgDiskImage>(hwnd,lParam);
          diskimage=get_this<DlgDiskImage>(hwnd);

          diskimage->wm_init(hwnd,message,wParam,lParam);
          return true;

     case WM_COMMAND:
          diskimage->msg_buttons(hwnd,message,wParam,lParam);
          break;


   default: return false;
  }


}


void DlgDiskImage::wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{


}

void DlgDiskImage::set_fields(const char*filename,HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
uint64_t size;
int bps;
CHS chs;

SetDlgItemText(hwnd,EDT_FILENAME,filename);

HANDLE hFile=CreateFile(filename,GENERIC_READ|GENERIC_WRITE,7,0,OPEN_EXISTING,0,0);
if(hFile==INVALID_HANDLE_VALUE)
  {
      MessageBox(hwnd,"Could not open a file","Error",16);
      return;

  }
LARGE_INTEGER li,unused;
SetFilePointerEx(hFile,unused,&li,FILE_END);
CloseHandle(hFile);

//--------------------------

size=li.QuadPart;
bps=512;
chs=li.QuadPart/bps;

SetDlgItemInt(hwnd,EDT_BPS,bps,0);
SetDlgItemInt(hwnd,EDT_SPT,chs.GetSPT(),0);

if(li.QuadPart/bps<10485760)
  {
      chs.SetTPC(16);
      SetDlgItemInt(hwnd,EDT_TPC,chs.GetTPC(),0);
  }
else if(li.QuadPart/bps<68719476736)
  {
      chs.SetTPC(255);
      SetDlgItemInt(hwnd,EDT_TPC,chs.GetTPC(),0);
  }

SetDlgItemInt(hwnd,EDT_CYLINDERS,chs.GetCylinder(),0);

}




void DlgDiskImage::msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{

switch(LOWORD(wParam))
  {
     case IDB_BROWSE:
       {
          set_fields(GetFileName(true,hwnd,"All files\0*.*\0\0").c_str(),hwnd,message,wParam,lParam);
       }
         break;
     case IDOK:
         btn_idok_click(hwnd,message,wParam,lParam);

         SendMessage(hwnd,WM_COMMAND,IDCANCEL,0);
         break;
     case IDCANCEL:

       //  SetWindowLong(hwnd,DWL_USER,(LONG_PTR)old_app_state);
         EndDialog(hwnd,IDD_DISKIMAGE);
         break;
  }


}




void DlgDiskImage::btn_idok_click(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
         GEOMETRY geom;
         BOOL  b;
         geom.bps=GetDlgItemInt(hwnd,EDT_BPS,&b,0);
         geom.spt=GetDlgItemInt(hwnd,EDT_SPT,&b,0);
         geom.tpc=GetDlgItemInt(hwnd,EDT_TPC,&b,0);
         geom.cylinders=GetDlgItemInt(hwnd,EDT_CYLINDERS,&b,0);

         char filename[512];
         GetDlgItemText(hwnd,EDT_FILENAME,filename,512);

         try
           {
             if(disk->IsOpen())
                 disk->Close();
             disk->Open(filename,geom);
           }
          catch(DiskException& de)
            {
              MessageBox(hwnd,de.what(),"Runtime exception",16);
              return;
            }
          catch(...)
            {
               MessageBox(0,"Unknown exception","Runtime exception",16);
               return;
            }
            vector<DISK_ERROR> tmp;
            disk->CheckDisk(tmp);

            if(tmp.size()!=0)
              {
                  DlgCheckDisk d;
                  d.InitDialog(disk,GetModuleHandle(0),hwnd);
              }

}
