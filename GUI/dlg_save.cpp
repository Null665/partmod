#include "gui.h"
#include "resource.h"
#include <commctrl.h>
#include <sstream>
using namespace std;

#define ID_START 12568

HWND DlgSave::hwnd=0;
bool DlgSave::finished=true;
HANDLE hThread;
DWORD dwThreadID;

CALLBACK_DATA cdt;


void DlgSave::InitDialog(Disk *disk,HINSTANCE hinst,HWND hwnd)
{
   this->disk=disk;
   finished=false;
   DialogBoxParam(hinst,MAKEINTRESOURCE(IDD_SAVE_PROGRESS),hwnd,DlgProc,(LPARAM)this);
}


BOOL CALLBACK DlgSave::DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
DlgSave *dlg=get_this<DlgSave>(hwnd);
switch(message)
  {
     case WM_INITDIALOG:
          init_set_this<DlgSave>(hwnd,lParam);
          dlg=get_this<DlgSave>(hwnd);

          dlg->wm_init(hwnd,message,wParam,lParam);
          return true;

     case WM_COMMAND:
          dlg->msg_buttons(hwnd,message,wParam,lParam);
          break;

   default: return false;
  }

}





void DlgSave::wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
this->hwnd=hwnd;
SendMessage(hwnd,WM_COMMAND,ID_START,0);
}


DWORD WINAPI DlgSave::start(LPVOID lp)
{
Disk *disk=static_cast<Disk*>(lp);

SendDlgItemMessage(hwnd,IDP_PROGRESS, PBM_SETRANGE, 0, MAKELPARAM(0, 1));
SendDlgItemMessage(hwnd,IDP_PROGRESS, PBM_SETPOS, 1, 1);

SetDlgItemText(hwnd,IDT_DESCRIPTION,"Saving changes");
SetDlgItemText(hwnd,IDT_CURRENT_SECTOR,"");
SetDlgItemText(hwnd,IDT_MIDDLE_TEXT,"");
SetDlgItemText(hwnd,IDT_SECTORS_TOTAL,"");

CreateThread(0,0,Callback,(void*)&disk->clbk,0,0);


try
  {
    disk->Save();
  }
catch(DiskException &de)
  {
    MessageBox(hwnd,de.what(),"Runtime exception",16);
  }
finished=true;
}



void DlgSave::msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{

switch(LOWORD(wParam))
  {
      case ID_START:
           finished=false;
           hThread=CreateThread(0,0,start,disk,0,&dwThreadID);
          break;
      case IDCANCEL:
        if(finished==false)
          {
            SuspendThread(hThread);
            if(MessageBox(hwnd,"Are you sure want to cancel?","Warning",48|MB_YESNO)!=IDYES)
              {
                ResumeThread(hThread);
                break;
              }
            else
              {
                ExitThread(dwThreadID);
                break;
              }
          }
        EndDialog(hwnd,IDD_SAVE_PROGRESS);
        break;
  }

}


DWORD WINAPI DlgSave::Callback(LPVOID lp)
{
CALLBACK_DATA *cdt=static_cast<CALLBACK_DATA*>(lp);

while(true)
{
std::stringstream ss;

SendDlgItemMessage(hwnd,IDP_PROGRESS, PBM_SETRANGE, 0, MAKELPARAM(0,100));

SendDlgItemMessage(hwnd,IDP_PROGRESS, PBM_SETPOS, ((long double)cdt->current/cdt->total)*100, 0);
switch(cdt->event)
  {
      case PENDING_WIPE:
        ss<<"Action: Wiping partition "<<cdt->partition;
        break;
      case PENDING_COPY:
        ss<<"Action: Copying partition "<<cdt->partition;
        break;
  }
SetDlgItemText(hwnd,IDT_DESCRIPTION,ss.str().c_str());

SetDlgItemText(hwnd,IDT_CURRENT_SECTOR,U64ToStr(cdt->current).c_str());
SetDlgItemText(hwnd,IDT_MIDDLE_TEXT,"out of");
SetDlgItemText(hwnd,IDT_SECTORS_TOTAL,U64ToStr(cdt->total).c_str());

ss.str("");

if(finished==true)
    SetDlgItemText(hwnd,IDCANCEL,"Done");

Sleep(100);
}

}



