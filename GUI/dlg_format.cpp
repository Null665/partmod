#include "gui.h"
#include "resource.h"
#include <windowsx.h>


void DlgFormatDisk::InitDialog(Disk *disk,int part_selected,HINSTANCE hinst,HWND hwnd)
{
   this->disk=disk;
   this->part_selected=part_selected;
   DialogBoxParam(hinst,MAKEINTRESOURCE(IDD_FORMAT),hwnd,DlgProc,(LPARAM)this);
}


BOOL CALLBACK DlgFormatDisk::DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
DlgFormatDisk *dlg=get_this<DlgFormatDisk>(hwnd);
switch(message)
  {
     case WM_INITDIALOG:
          init_set_this<DlgFormatDisk>(hwnd,lParam);
          dlg=get_this<DlgFormatDisk>(hwnd);
          dlg->wm_init(hwnd,message,wParam,lParam);
          return true;
     case WM_COMMAND:
          dlg->msg_buttons(hwnd,message,wParam,lParam);
          break;

   default: return false;
  }

}


void DlgFormatDisk::wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
HWND hfs=GetDlgItem(hwnd,CMB_FS);
HWND hcs=GetDlgItem(hwnd,CMB_CLUSTER_SIZE);
GEN_PART gp=disk->GetPartition(part_selected);

if( gp.length<((2*(GB/disk->GetDiskGeometry().bps))+1) )
  ComboBox_AddString(hfs,"FAT");
if(gp.length<32*(GB/disk->GetDiskGeometry().bps))
ComboBox_AddString(hfs,"FAT32");
if(gp.length<32*(TB/disk->GetDiskGeometry().bps))
ComboBox_AddString(hfs,"NTFS");
ComboBox_SetCurSel(hfs,0);



ComboBox_AddString(hcs,"512");
ComboBox_SetCurSel(hcs,0);


}

void DlgFormatDisk::msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{

switch(LOWORD(wParam))
  {

     case IDOK:
     {

         HWND hfs=GetDlgItem(hwnd,CMB_FS);
         char *str=new char[32];
         unsigned int fs=0,flags=0;

         if(MessageBox(hwnd,"Are you sure want to format a partition? All data on this partition will be lost",
         "Warning",MB_YESNO|48)!=IDYES)
              break;


         if(SendMessage(GetDlgItem(hwnd,CHK_QUICKFMT), BM_GETSTATE,0,0)==BST_CHECKED)
             flags|=FF_QUICK;
         else
             flags&=~FF_QUICK;
         if(SendMessage(GetDlgItem(hwnd,CHK_NTFS_COMPRESS), BM_GETSTATE,0,0)==BST_CHECKED)
             flags|=FF_ENABLE_COMPRESSION;
         else
             flags&=~FF_ENABLE_COMPRESSION;

         ComboBox_GetText(hfs,str,32);

         if(strcmp("NTFS",str)==0)
             fs=FS_NTFS;
         else if(strcmp("FAT",str)==0)
             fs=FS_FAT16;
         else if(strcmp("FAT32",str)==0)
             fs=FS_FAT32;
/*
         if(disk->Format(part_selected,fs,flags)==true)
           {
                MessageBox(hwnd,"Finished formatting disk","Information",64);
                EndDialog(hwnd,IDD_FORMAT);
           }*/
         delete[] str;
     }
       break;
     case IDCANCEL:
         EndDialog(hwnd,IDD_FORMAT);
         break;
  }


}

