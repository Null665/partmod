#include "gui.h"
#include "resource.h"
#include <windowsx.h>

void DlgNewPart::InitDialog(Disk *disk, int frs_selected,HINSTANCE hinst,HWND hwnd)
{

   this->frs_selected=frs_selected;
   this->disk=disk;

   DialogBoxParam(hinst,MAKEINTRESOURCE(IDD_CREATE),hwnd,DlgProc,(LPARAM)this);
}


// Dialog's callback function
BOOL CALLBACK  DlgNewPart::DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
DlgNewPart *newpart=get_this<DlgNewPart>(hwnd);
switch(message)
  {
     case WM_INITDIALOG:
          init_set_this<DlgNewPart>(hwnd,lParam);
          newpart=get_this<DlgNewPart>(hwnd);

          newpart->wm_init(hwnd,message,wParam,lParam);
          return true;
          break;
     case WM_COMMAND:
          newpart->msg_buttons(hwnd,message,wParam,lParam);
          break;

   default: return false;
  }

}




void  DlgNewPart::wm_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

HWND hList=GetDlgItem(hwnd,CMB_SIZE);
ComboBox_AddString(hList,"MB");
ComboBox_AddString(hList,"GB");
ComboBox_AddString(hList,"TB");
ComboBox_AddString(hList,"Sectors");
ComboBox_AddString(hList,"Cylinders");
ComboBox_SetCurSel(hList,0);



if(disk->CountPartitions(PART_EXTENDED)!=0)
    EnableWindow(GetDlgItem(hwnd,IDR_EXTENDED),false);
else
    EnableWindow(GetDlgItem(hwnd,IDR_EXTENDED),true);
if(disk->CountPartitions(PART_PRIMARY | PART_EXTENDED)==4)
    EnableWindow(GetDlgItem(hwnd,IDR_PRIMARY),false);
else
    EnableWindow(GetDlgItem(hwnd,IDR_PRIMARY),true);



EnableWindow(GetDlgItem(hwnd,IDR_EXTENDED),false);
EnableWindow(GetDlgItem(hwnd,IDR_PRIMARY),false);
EnableWindow(GetDlgItem(hwnd,IDR_LOGICAL),false);
//EnableWindow(GetDlgItem(hwnd,IDOK),false);

     if(disk->GetFreeSpace(frs_selected).type==FREE_EXTENDED)
        {
            EnableWindow(GetDlgItem(hwnd,IDR_PRIMARY),false);
            EnableWindow(GetDlgItem(hwnd,IDR_LOGICAL),true);

            SendDlgItemMessage(hwnd, IDR_PRIMARY,  BM_SETCHECK, 0, 0);
            SendDlgItemMessage(hwnd, IDR_LOGICAL,  BM_SETCHECK, 1, 0);
            SendDlgItemMessage(hwnd, IDR_EXTENDED, BM_SETCHECK, 0, 0);
        }
      else
        {
           if(disk->CountPartitions(PART_PRIMARY | PART_EXTENDED)<4)
             {
                  EnableWindow(GetDlgItem(hwnd,IDR_PRIMARY),true);
                  SendDlgItemMessage(hwnd, IDR_PRIMARY, BM_SETCHECK, 1, 0);
             }
            if(disk->CountPartitions(PART_EXTENDED)==0)
              {
                 EnableWindow(GetDlgItem(hwnd,IDR_EXTENDED),true);
              }
             EnableWindow(GetDlgItem(hwnd,IDR_LOGICAL),false);

            SendDlgItemMessage(hwnd, IDR_LOGICAL, BM_SETCHECK, 0, 0);
            SendDlgItemMessage(hwnd, IDR_EXTENDED, BM_SETCHECK, 0, 0);
        }




}



void DlgNewPart::msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{

switch(LOWORD(wParam))
{
  case IDR_PRIMARY:
        switch (HIWORD(wParam))
        {
            case BN_CLICKED:
                if (SendDlgItemMessage(hwnd, IDR_PRIMARY, BM_GETCHECK, 0, 0) == 0)
                {
                    SendDlgItemMessage(hwnd, IDR_PRIMARY, BM_SETCHECK, 1, 0);
                    SendDlgItemMessage(hwnd, IDR_EXTENDED, BM_SETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd, IDR_LOGICAL, BM_SETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd, IDR_GPT, BM_SETCHECK, 0, 0);
                }
            break;
        }
     break;
  case IDR_GPT:
        switch (HIWORD(wParam))
        {
            case BN_CLICKED:
                if (SendDlgItemMessage(hwnd, IDR_GPT, BM_GETCHECK, 0, 0) == 0)
                {
                    SendDlgItemMessage(hwnd, IDR_GPT, BM_SETCHECK, 1, 0);
                    SendDlgItemMessage(hwnd, IDR_PRIMARY, BM_SETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd, IDR_EXTENDED, BM_SETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd, IDR_LOGICAL, BM_SETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd, IDR_GPT_HOLDER, BM_SETCHECK, 0, 0);
                }
            break;
        }
     break;
  case IDR_GPT_HOLDER:
        switch (HIWORD(wParam))
        {
            case BN_CLICKED:
                if (SendDlgItemMessage(hwnd, IDR_GPT_HOLDER, BM_GETCHECK, 0, 0) == 0)
                {
                    SendDlgItemMessage(hwnd, IDR_GPT_HOLDER, BM_SETCHECK, 1, 0);
                    SendDlgItemMessage(hwnd, IDR_EXTENDED, BM_SETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd, IDR_LOGICAL, BM_SETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd, IDR_GPT, BM_SETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd, IDR_PRIMARY, BM_SETCHECK, 0, 0);
                }
            break;
        }
     break;
  case IDR_EXTENDED:
        switch (HIWORD(wParam))
        {
            case BN_CLICKED:
                if (SendDlgItemMessage(hwnd, IDR_EXTENDED, BM_GETCHECK, 0, 0) == 0)
                {
                    SendDlgItemMessage(hwnd, IDR_PRIMARY, BM_SETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd, IDR_EXTENDED, BM_SETCHECK, 1, 0);
                    SendDlgItemMessage(hwnd, IDR_LOGICAL, BM_SETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd, IDR_GPT, BM_SETCHECK, 1, 0);
                }
        }
            break;
  case IDR_LOGICAL:
        switch (HIWORD(wParam))
        {
            case BN_CLICKED:
                if (SendDlgItemMessage(hwnd, IDR_LOGICAL, BM_GETCHECK, 0, 0) == 0)
                {
                    SendDlgItemMessage(hwnd, IDR_PRIMARY, BM_SETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd, IDR_EXTENDED, BM_SETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd, IDR_LOGICAL, BM_SETCHECK, 1, 0);
                    SendDlgItemMessage(hwnd, IDR_GPT, BM_SETCHECK, 1, 0);
                }
            break;
        }

      break;


  case IDOK:
    this->btn_idok_click(hwnd,message,wParam,lParam);
    break;

  case IDCANCEL:
       EndDialog(hwnd,IDD_CREATE);
    break;
}

}




void DlgNewPart::btn_idok_click(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
      BOOL b;
      int part_type;
      FREE_SPACE frs;
      uint64_t size;
      int sel;

      try{
        frs=disk->GetFreeSpace(frs_selected);
      }
      catch(DiskException &de){
         MessageBox(0,de.what(),"Runtime exception",64);
      }
      catch(...){
         MessageBox(0,"Unknown exception","Runtime exception",16);
      }

      size=GetDlgItemInt(hwnd,EDT_SIZE,&b,0);
      sel=ComboBox_GetCurSel(GetDlgItem(hwnd,CMB_SIZE));

      // Partition size in...
      switch(sel)
        {
          case 0: // MB
            size*=MB/disk->GetDiskGeometry().bps;
            break;
          case 1: // GB
            size*=GB/disk->GetDiskGeometry().bps;
            break;
          case 2: // TB
            size*=TB/disk->GetDiskGeometry().bps;
            break;
          case 3: // Sectors
            break;
          case 4: // Cylinders
            size*=( disk->GetDiskGeometry().spt * disk->GetDiskGeometry().tpc);
            break;
        }
     if(size>frs.length)
       {
           // Fit partition to free space if difference in their sizes is less than MB
           uint64_t NSECT_MB=MB/disk->GetDiskGeometry().bps;
           if(size-frs.length<NSECT_MB)
             {
                 size=frs.length;
             }
           else
             {
               MessageBox(0,"Too large partition","Error",16);
               return;
             }

       }
     if(size==0)
       {
           MessageBox(0,"Cannot create zero length partition","Error",16);
           return;
       };
    if(SendDlgItemMessage(hwnd, IDR_PRIMARY, BM_GETCHECK, 0, 0)   ==1)     part_type=PART_PRIMARY;
    else if(SendDlgItemMessage(hwnd, IDR_LOGICAL, BM_GETCHECK, 0, 0)==1)   part_type=PART_LOGICAL;
    else if(SendDlgItemMessage(hwnd, IDR_EXTENDED, BM_GETCHECK, 0, 0)==1)  part_type=PART_EXTENDED;
    else if(SendDlgItemMessage(hwnd, IDR_GPT, BM_GETCHECK, 0, 0)   ==1)    part_type=PART_GPT;

    try
    {
        disk->CreatePartition(frs,part_type,size);
    }
    catch(DiskException &de)
    {
      MessageBox(0,de.what(),"Runtime exception",64);
    }
    catch(...)
    {
      MessageBox(0,"Unknown exception","Runtime exception",16);
    }



    EndDialog(hwnd,IDD_CREATE);
}

