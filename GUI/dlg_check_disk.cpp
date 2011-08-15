#include "gui.h"
#include "../Partmod/disk.h"
#include "resource.h"
#include <string>
#include <commctrl.h>
using namespace std;

void DlgCheckDisk::InitDialog(Disk *disk,HINSTANCE hinst,HWND hwnd)
{
   this->disk=disk;
   DialogBoxParam(hinst,MAKEINTRESOURCE(IDD_CHECK_DISK),hwnd,DlgProc,(LPARAM)this);
}



BOOL CALLBACK DlgCheckDisk::DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
DlgCheckDisk *chkdsk=get_this<DlgCheckDisk>(hwnd);

switch(message)
  {
     case WM_INITDIALOG:
          init_set_this<DlgCheckDisk>(hwnd,lParam);
          chkdsk=get_this<DlgCheckDisk>(hwnd);

          chkdsk->wm_init(hwnd,message,wParam,lParam);
          return true;

     case WM_COMMAND:
          chkdsk->msg_buttons(hwnd,message,wParam,lParam);
          break;


   default: return false;
  }


}


void DlgCheckDisk::wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
int ColumnMask = LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM|LVCF_WIDTH;
HWND hdlg=GetDlgItem(hwnd,ILV_ERROR_LIST);

ListView_SetExtendedListViewStyle(hdlg, LVS_EX_FULLROWSELECT);

LVCOLUMN lc[] = {
    { ColumnMask, 0, 70,  "Error code",           0, 0,0,0 },
    { ColumnMask, 0, 70,  "Partition",            0, 1,0,0 },
    { ColumnMask, 0, 100, "Relevant part.",       0, 2,0,0 },
    { ColumnMask, 0, 300, "Error description",    0, 3,0,0 },

};

for(int i=0;i<sizeof(lc)/sizeof(LVCOLUMN);i++)
    ListView_InsertColumn(hdlg, i, &lc[i]);


refresh(hwnd,message,wParam,lParam);
}


void DlgCheckDisk::msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{

switch(LOWORD(wParam))
  {
     case IDCANCEL:

         EndDialog(hwnd,IDD_CHECK_DISK);
         break;
  }

}




void DlgCheckDisk::refresh(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{

HWND hErrorList=GetDlgItem(hwnd,ILV_ERROR_LIST);

lv_list.clear();
ListView_DeleteAllItems(hErrorList);
lv_errors tmp;

vector<DISK_ERROR> disk_errors;
disk->CheckDisk(disk_errors);

if(disk_errors.size()==0)
    return;

for(int i=0;i<disk_errors.size();i++)
  {
      tmp.error_code=U64ToStr(disk_errors[i].error_code);

      if(disk_errors[i].partition==-1)
          tmp.partition="-";
      else tmp.partition=U64ToStr(disk_errors[i].partition+1);

      if(disk_errors[i].partition_nxt==-1)
          tmp.partition_relevant="-";
      else tmp.partition_relevant=U64ToStr(disk_errors[i].partition_nxt+1);


      tmp.error_description=GetErrorDescription(disk_errors[i].error_code);
      tmp.de=disk_errors[i];
      lv_list.push_back(tmp);
  }

for(int i=0;i<lv_list.size();i++)
      LV_InsertItem(hErrorList,"");

for(int i=0;i<lv_list.size();i++)
  {
      LV_SetItem(hErrorList,lv_list[i].error_code.c_str(),i,0);
      LV_SetItem(hErrorList,lv_list[i].partition.c_str(),i,1);
      LV_SetItem(hErrorList,lv_list[i].partition_relevant.c_str(),i,2);
      LV_SetItem(hErrorList,lv_list[i].error_description.c_str(),i,3);


  }



}
