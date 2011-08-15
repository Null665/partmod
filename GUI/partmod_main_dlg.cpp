#include "gui.h"
#include "resource.h"


#include <commctrl.h>

#include <algorithm>
#include <sstream>
using namespace std;


#define ENABLE_WIPE_PARTITION  1
#define ENABLE_BACKUP_DLG      1
#define ENABLE_SAVE_DLG        1
#define ENABLE_NEWPART_DLG     1
#define ENABLE_DISKIMAGE_DLG   1
//#define ENABLE_FORMAT_DLG    0
#define ENABLE_CHECKDISK_DLG   1
#define ENABLE_BS_DLG          0



COLORREF clr_part_primary=RGB(1,6,203),       // dark blue
         clr_part_containers=RGB(240,0,0),    // red
         clr_part_subparts=RGB(140,90,140),
         clr_frs_unallocated=RGB(50,50,50),   // dark grey
         clr_frs_subparts=RGB(50,130,50);     // light green


/*
int GetSelectedPartition(HWND hList)
{
 int selection=0,i=0;
     i=SendMessage(hList,LVM_GETNEXTITEM,-1,LVNI_FOCUSED);

selection=listview_list[i].selection_type.type;
if(selection!=S_FREE_SPACE)
  {
      frs_selected=-1;
      part_selected=listview_list[i].num;
  }
else
  {
      frs_selected=listview_list[i].num;
      part_selected=-1;
  }
return selection;
}
*/


DlgPartmod::DlgPartmod()
{
  disk=new Disk;
  part_selected=-1;
  frs_selected=-1;
}

void DlgPartmod::InitDialog(HINSTANCE hinst, HWND hwnd)
{
  DialogBoxParam(hinst,MAKEINTRESOURCE(IDD_PARTMOD),hwnd,DlgProc,(LPARAM)this);

}


//
// Dialog callback function
//
BOOL CALLBACK DlgPartmod::DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
DlgPartmod *dlg=get_this<DlgPartmod>(hwnd);

  switch(message)
    {
       case WM_INITDIALOG:
         init_set_this<DlgPartmod>(hwnd,lParam);
         dlg->wm_init(hwnd,message,wParam,lParam);
         return true;
       case WM_NOTIFY:
    	 switch(LOWORD(wParam))
		   {
			    case ILV_PHYSICAL:
                     dlg->click_physical(hwnd,message,wParam,lParam);
			  	     break;
                 case ILV_LOGICAL:
                    dlg->click_logical(hwnd,message,wParam,lParam);
                    return dlg->customdraw_handler(hwnd,message,wParam,lParam);
                    break;
           }
            break;
     case WM_COMMAND:
     case WM_MENUCOMMAND:
          dlg->msg_buttons(hwnd,message,wParam,lParam);
          break;

    }

return false;
}

//
// WM_INIT handler
//
void DlgPartmod::wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
HWND hPhysical = GetDlgItem(hwnd, ILV_PHYSICAL);
HWND hLogical =  GetDlgItem(hwnd, ILV_LOGICAL);
ListView_SetExtendedListViewStyle(hPhysical, LVS_EX_FULLROWSELECT);
ListView_SetExtendedListViewStyle(hLogical, LVS_EX_FULLROWSELECT);

HMENU hMenu=LoadMenu(GetModuleHandle(NULL),MAKEINTRESOURCE(IDR_MENU));
SetMenu(hwnd,hMenu);


int ColumnMask = LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM|LVCF_WIDTH;
LVCOLUMN lc[] = {
    { ColumnMask, 0, 120, "Disk",                0, 0,0,0 },
    { ColumnMask, 0, 70,  "Type",                0, 1,0,0 },
    { ColumnMask, 0, 70 , "Size ",               0, 2,0,0 },
    { ColumnMask, 0, 70 , "Free",                0, 3,0,0 },
    { ColumnMask, 0, 70 , "Cylinders",           0, 4,0,0 },
    { ColumnMask, 0, 50 , "bps",                 0, 5,0,0 },
    { ColumnMask, 0, 50 , "spt",                 0, 6,0,0 },
    { ColumnMask, 0, 50 , "tpc",                 0, 7,0,0 },
    { ColumnMask, 0, 100 ,"Total sectors",       0, 8,0,0 },
    { ColumnMask, 0, 100, "Disk signature",      0, 9,0,0 },

};

LVCOLUMN lc2[] = {
    { ColumnMask, 0,  70, "Partition#",        0, 0,0,0 },
    { ColumnMask, 0, 100, "Type",              0, 1,0,0 },
    { ColumnMask, 0, 100, "FS type",           0, 2,0,0 },
    { ColumnMask, 0,  70, "Size",              0, 3,0,0 },
    { ColumnMask, 0,  70, "Free",              0, 4,0,0 },
    { ColumnMask, 0,  80, "First sect",        0, 6,0,0 },
    { ColumnMask, 0,  80, "Last sect",         0, 7,0,0 },
    { ColumnMask, 0,  110,"Mount point",       0, 8,0,0 },
};

for(int i=0;i<sizeof(lc)/sizeof(LVCOLUMN);i++)
    ListView_InsertColumn(hPhysical, i, &lc[i]);
for(int i=0;i<sizeof(lc2)/sizeof(LVCOLUMN);i++)
    ListView_InsertColumn(hLogical, i, &lc2[i]);

ListDisks(GetDlgItem(hwnd,ILV_PHYSICAL));


EnableButtons(hwnd,false,IDB_NEWPART,IDB_WIPE,IDB_DELETE,IDB_FORMAT,IDB_BS_MOD,IDB_SET_ACTIVE,IDB_SET_INACTIVE,IDB_FS_CHECK,0);
EnableButtons(hwnd,false,IDB_BACKUP_CREATE,IDB_BACKUP_RESTORE,0);


}

void DlgPartmod::msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
switch(LOWORD(wParam))
  {

    case IDB_NEWPART:
    {
#ifdef ENABLE_NEWPART_DLG

          DlgNewPart np;
          np.InitDialog(disk,frs_selected,GetModuleHandle(0),hwnd);

          ListView_DeleteAllItems(GetDlgItem(hwnd,ILV_LOGICAL));
          ListPartitions(GetDlgItem(hwnd,ILV_LOGICAL));
#endif
         break;
    }
    case IDB_DELETE:
         if(disk->GetPartition(part_selected).flags&PART_EXTENDED)
            {
              int ret=MessageBox(hwnd,"This is an extended partition and it may contain more partitions. If you'll delete this partition, all logical volumes on this partition will be deleted too","Warning",48|MB_YESNO);
              if(ret!=IDYES) break;
            }
         else if(MessageBox(hwnd,"Do you really want to delete a partition? All data on that partition will be lost","Warning",48 | MB_YESNO)==IDNO) break;

         try{
             disk->DeletePartition(part_selected);
         }
         catch(DiskException &de){
            MessageBox(0,de.what(),"Runtime exception",16);
         }
         catch(...){
            MessageBox(0,"Unknown exception","Runtime exception",16);
         }

         ListPartitions(GetDlgItem(hwnd,ILV_LOGICAL));
         break;

    case IDB_SET_ACTIVE:

         if(disk->GetPartition(part_selected).flags&PART_ACTIVE)
           {
             MessageBox(hwnd,"This partition is already marked as active","Information",64);
             break;
           }

         if(disk->CountPartitions(PART_ACTIVE)>0)
         if(MessageBox(hwnd,"Are you sure want to do this?\nOne partition is already marked as active","Warning",48 | MB_YESNO)==IDNO) break;
         disk->SetActive(part_selected,true);
         ListPartitions(GetDlgItem(hwnd,ILV_LOGICAL));
         break;

    case IDB_SET_INACTIVE:

         if(!(disk->GetPartition(part_selected).flags&PART_ACTIVE))
           {
             MessageBox(hwnd,"This partition is already inactive","Information",64);
             break;
           }

         if(disk->CountPartitions(PART_ACTIVE)>0)
             if(MessageBox(hwnd,"Are you sure want to do this?\n","Warning",48 | MB_YESNO)==IDNO) break;
                 disk->SetActive(part_selected,false);
         ListPartitions(GetDlgItem(hwnd,ILV_LOGICAL));
         break;

    case IDB_BS_MOD:
#if ENABLE_BS_DLG!=0
         GEN_HANDLE gh;
         gh=disk->GetPartitionHandle(part_selected);
         DllBootInfo(gh);
#endif
         break;

    case IDB_DISKIMAGE:
        {
#ifdef ENABLE_DISKIMAGE_DLG

         ListView_DeleteAllItems(GetDlgItem(hwnd,ILV_LOGICAL));
         DlgDiskImage di;
            di.InitDialog(disk,GetModuleHandle(0),hwnd);

         if(disk->IsOpen())
              EnableButtons(hwnd,true,IDB_BACKUP_CREATE,IDB_BACKUP_RESTORE,IDB_CLOSE_DISK,0);
         else EnableButtons(hwnd,false,IDB_BACKUP_CREATE,IDB_BACKUP_RESTORE,IDB_CLOSE_DISK,0);


         ListPartitions(GetDlgItem(hwnd,ILV_LOGICAL));

#endif
         break;
        }
/*
    case IDB_FS_CHECK:
      {
        GEN_HANDLE gh;
        int ret;
        gh=disk->GetPartitionHandle(part_selected);
        string err;
        ret=DllFsCheck(gh,err);

        if(ret==0 && !err.empty())
          {
             MessageBox(hwnd,err.c_str(),"File system warnings",48);
          }
        else if(ret==0 && err.empty())
             MessageBox(hwnd,"No file system errors found!","Information",64);
        else if(ret>0)
          {
             MessageBox(hwnd,err.c_str(),"File system errors",16);
          }

        break;
      }*/
    case IDB_FORMAT:
        {
#ifdef ENABLE_FORMAT_DLG
            if(MessageBox(hwnd,"Don't use this function. If you don't want to be able to boot and/or install windows, "
                       "click OK!","Don't",48|MB_YESNO)!=IDYES)
                       break;
            FormatDisk fmt;
            fmt.InitDialog(disk,part_selected,GetModuleHandle(0),hwnd);

#endif
        }
         break;
    case IDCANCEL:

         disk->Close();
         listview_list.clear();
         ListView_DeleteAllItems(GetDlgItem(hwnd,ILV_LOGICAL));
         EndDialog(hwnd,IDD_PARTMOD);
         break;
    case IDB_WIPE:
#if ENABLE_WIPE_PARTITION==1
    {
      if(MessageBox(hwnd,"Are you sure want to wipe the partition? Partition will be wiped when you press 'Save changs' button","Warning",48|MB_YESNO)!=IDYES)
                 break;

         try
          {
             disk->Wipe(part_selected,WIPE_ZEROS);
          }
         catch(DiskException &de){
            MessageBox(0,de.what(),"Runtime exception",64);
         }
         catch(...){
            MessageBox(0,"Unknown exception","Runtime exception",16);
         }

         break;
    }
#else
    MessageBox(hwnd,"This function is not supported yet","Information",64);
    break;
#endif
    case IDB_SAVE:
    {
         if(!disk->IsOpen())
            {
                MessageBox(hwnd,"Select a disk first","Information",64);
                break;
            }
         if(MessageBox(hwnd,"Are you sure want to write changes to disk?","Warning",48|MB_YESNO)!=IDYES)
             break;
#ifdef ENABLE_SAVE_DLG
         DlgSave dlg;
         dlg.InitDialog(disk,GetModuleHandle(0),hwnd);
#else
         try{
             disk->Save();
         }
         catch(DiskException &de){
            MessageBox(0,de.what(),"Runtime exception",64);
         }
         catch(...){
            MessageBox(0,"Unknown exception","Runtime exception",16);
         }

#endif
         break;
    }
     case IDB_ABOUT:
         MessageBox(hwnd,"Version 0.1","Partmod",64);
         break;
     case IDB_CLOSE_DISK:
         disk->Close();
         listview_list.clear();
         ListView_DeleteAllItems(GetDlgItem(hwnd,ILV_LOGICAL));
         EnableButtons(hwnd,false,IDB_BACKUP_CREATE,IDB_BACKUP_RESTORE,IDB_CLOSE_DISK,0);
         break;
#ifdef ENABLE_BACKUP_DLG
      case IDB_BACKUP_CREATE:
      {
          DlgBackup dbk;
          dbk.InitDialog(disk,GetModuleHandle(0),hwnd,true);
          ListPartitions(GetDlgItem(hwnd,ILV_LOGICAL));
          break;
      }
      case IDB_BACKUP_RESTORE:
      {
          DlgBackup dbk;
          dbk.InitDialog(disk,GetModuleHandle(0),hwnd,false);
          ListPartitions(GetDlgItem(hwnd,ILV_LOGICAL));
          break;
      }
#endif
      case IDB_CHECK_DISK:
      {
          DlgCheckDisk dlg;
          dlg.InitDialog(disk,GetModuleHandle(0),hwnd);

          break;
      }
  }

}







//
// Called when a physical drive is clicked
//
void DlgPartmod::click_physical(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
LVITEM LvItem={0};
HWND hList=GetDlgItem(hwnd,ILV_PHYSICAL);

if(((LPNMHDR)lParam)->code == NM_DBLCLK)
  {
    char dsk[32]={0};
    int selected=-1;
    selected=SendMessage(hList,LVM_GETNEXTITEM,-1,LVNI_FOCUSED);

    if(selected==-1)
      return;
    LvItem.mask=LVIF_TEXT;
    LvItem.iSubItem=0;
    LvItem.pszText=dsk;
    LvItem.cchTextMax=32;
    LvItem.iItem=selected;
    SendMessage(hList,LVM_GETITEMTEXT, selected, (LPARAM)&LvItem);

    if(disk->IsOpen())
        disk->Close();
    try
      {
        disk->Open(dsk);
      }
    catch(DiskException &de)
      {
         EnableButtons(hwnd,false,IDB_BACKUP_CREATE,IDB_BACKUP_RESTORE,IDB_CLOSE_DISK,0);
         MessageBox(hwnd,de.what(),"Runtime exceptioon",16);
         return;
      }

    vector<DISK_ERROR> tmp;
    disk->CheckDisk(tmp);
    if(tmp.size()!=0)
      {
          DlgCheckDisk d;
          d.InitDialog(disk,GetModuleHandle(0),hwnd);
      }



    EnableButtons(hwnd,true,IDB_BACKUP_CREATE,IDB_BACKUP_RESTORE,IDB_CLOSE_DISK,0);
    if(disk->GetDiskType()==DISK_GPT)
            MessageBox(hwnd,"This disk has GPT partition layout which is not supported now "
            "so you will see only one partition on disk.\nPlease do NOT modify it!","Warning",48);

    ListPartitions(GetDlgItem(hwnd,ILV_LOGICAL));

  }


}

void DlgPartmod::click_logical(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{

sel_type selection;

HWND hList=GetDlgItem(hwnd,ILV_LOGICAL);
int selected_count=ListView_GetSelectedCount(hList);

if(selected_count==0)
  {
    EnableButtons(hwnd,false,IDB_FORMAT,IDB_NEWPART,IDB_DELETE,IDB_RESIZE,IDB_BS_MOD,
                  IDB_SET_ACTIVE,IDB_SET_INACTIVE,IDB_FS_CHECK,IDB_WIPE,0);
    return;
  }


selection=get_sel_type(hList);

uint32_t flag_primary=PART_PRIMARY;
uint32_t flag_containers=PART_EXTENDED | PART_MBR_GPT;
uint32_t flag_nested=PART_LOGICAL | PART_GPT;
uint32_t flag_mbr=PART_PRIMARY | PART_EXTENDED | PART_MBR_GPT;

if(((LPNMHDR)lParam)->code == NM_RCLICK)
  {

    HMENU hPopupMenu = CreatePopupMenu();
    if(selection.type!=S_FREE_SPACE)
      {
        if(!(selection.flags&flag_containers))
            InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDB_BS_MOD, (LPCSTR)"Modify boot sector...");
        if(selection.flags&flag_mbr)
            InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDB_SET_ACTIVE, (LPCSTR)"Mark as active");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDB_DELETE, (LPCSTR)"Delete this partition");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDB_WIPE, (LPCSTR)"Wipe partition");

        if( !(selection.flags&flag_containers))
          {
            InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDB_FS_CHECK, (LPCSTR)"Check file system");
          //  InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDB_FORMAT, (LPCSTR)"Format...");
          }
      }
    else InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDB_NEWPART, (LPCSTR)"New partition...");

    SetForegroundWindow(hwnd);
    POINT cp;
    GetCursorPos(&cp);
    TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, cp.x, cp.y, 0, hwnd, NULL);
  };

if(selection.type!=S_FREE_SPACE)
  {
    EnableButtons(hwnd,false,IDB_NEWPART,0);
    EnableButtons(hwnd,true,IDB_DELETE,IDB_RESIZE,IDB_WIPE,0);

    if(!(selection.flags&flag_containers))
          EnableButtons(hwnd,false,IDB_FORMAT,IDB_BS_MOD,IDB_FS_CHECK,IDB_WIPE,0);
    else
          EnableButtons(hwnd,true,IDB_FORMAT,IDB_BS_MOD,IDB_FS_CHECK,0);

    if(selection.flags&flag_nested)
          EnableButtons(hwnd,false,IDB_SET_ACTIVE,IDB_SET_INACTIVE,0);
    else  EnableButtons(hwnd,true,IDB_SET_ACTIVE,IDB_SET_INACTIVE,0);

  }
else
  {
    EnableButtons(hwnd,true,IDB_NEWPART,0);
    EnableButtons(hwnd,false,IDB_DELETE,IDB_FORMAT,IDB_BS_MOD,IDB_SET_ACTIVE,IDB_SET_INACTIVE,IDB_FS_CHECK,IDB_WIPE,0);
  }

}



//
// CustomDraw handler for ILV_LOGICAL
//
BOOL DlgPartmod::customdraw_handler(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
LPNMLISTVIEW  pnm    = (LPNMLISTVIEW)lParam;

switch (pnm->hdr.code)
 {
    case NM_CUSTOMDRAW:
      LPNMLVCUSTOMDRAW  lplvcd;
                        lplvcd = (LPNMLVCUSTOMDRAW)lParam;

     switch(lplvcd->nmcd.dwDrawStage)
       {
          case CDDS_PREPAINT :
             SetWindowLong(hwnd,DWL_MSGRESULT,CDRF_NOTIFYITEMDRAW);
             return CDRF_NOTIFYITEMDRAW;
           break;

          case CDDS_ITEMPREPAINT:
            {
               int  sel=lplvcd->nmcd.dwItemSpec;

               if( listview_list[sel].selection_type.type==S_FREE_SPACE)
                 {
                    uint32_t flags=listview_list[sel].selection_type.flags;

                    if(flags==FREE_EXTENDED || flags==FREE_GPT)
                      lplvcd->clrText=clr_frs_subparts;
                    else
                        lplvcd->clrText=clr_frs_unallocated;
                 }
               else
                 {
                    uint32_t flags=listview_list[sel].selection_type.flags;
                    if(flags&PART_PRIMARY)
                      lplvcd->clrText=clr_part_primary;
                    else if(flags&PART_EXTENDED || flags&PART_MBR_GPT)
                      lplvcd->clrText=clr_part_containers;
                    else if(flags&PART_LOGICAL || flags&PART_GPT)
                      lplvcd->clrText=clr_part_subparts;
                 }

               return CDRF_NEWFONT;
            }
            break;

       }
   break;

  }

return FALSE;
}




//
// Creates a list of partition in specified control
//
int DlgPartmod::ListPartitions(HWND hdlg)
{

if(disk==0)
   return 1;
ListView_DeleteAllItems(hdlg);
GEN_PART gpart;
FREE_SPACE frs;
GEN_HANDLE gh;
GEOMETRY geom=disk->GetDiskGeometry();
lvlist tmp;
listview_list.clear();
unsigned __int64 free;


MBR_SPECIFIC mspec;

for(int i=0;i<disk->CountPartitions();i++)
  {
     tmp.partition=U64ToStr(i+1);
     gpart=disk->GetPartition(i);
     mspec=disk->GetMBRSpecific(i);

     if(gpart.flags&PART_PRIMARY)
         tmp.type="Primary";
     else if(gpart.flags&PART_EXTENDED)
         tmp.type="Extended MBR";
     else if(gpart.flags&PART_LOGICAL)
         tmp.type="Logical";
     else if(gpart.flags&PART_MBR_GPT)
         tmp.type="GPT";

     tmp.fs_type=disk->fsid_man->GetByFsid(mspec.fsid).description;
     tmp.size=size_str(gpart.length,geom.bps).c_str();

     gh=disk->GetPartitionHandle(i);
     /*
     if(DllCalculateFree(gh,&free)==0)
         tmp.free=size_str(free,geom.bps);
     else */tmp.free="N/A";

     if(gpart.flags&PART_ACTIVE)
         tmp.type+=" (Active)";

     tmp.begin_sect=U64ToStr(gpart.begin_sector);
     tmp.last_sect=U64ToStr(gpart.begin_sector+gpart.length);

     if(gpart.flags&PART_EXTENDED) tmp.mountpoint="";
     else tmp.mountpoint=get_mount_point(gpart,disk->GetDiskSignature());

     tmp.num=i;
     tmp.selection_type.type=S_PARTITION;
     tmp.selection_type.flags=gpart.flags;

     listview_list.push_back(tmp);


  };

for(int i=0;i<disk->CountFreeSpaces();i++)
  {
    frs=disk->GetFreeSpace(i);
    tmp.partition="-";

    tmp.type="Free space";
    tmp.fs_type="";
    tmp.mountpoint="";

    tmp.size=size_str(frs.length,geom.bps).c_str();
    tmp.free="N/A";
    tmp.begin_sect=U64ToStr(frs.begin_sector);
    tmp.last_sect=U64ToStr(frs.begin_sector+frs.length);


    tmp.num=i;
    tmp.selection_type.type=S_FREE_SPACE;
    tmp.selection_type.flags=frs.type;

    listview_list.push_back(tmp);

  }

// sort the list
sort(listview_list.begin(),listview_list.end(),cmp_lv);


for(int i=0;i<listview_list.size();i++)
    LV_InsertItem(hdlg,"");

for(int i=0;i<listview_list.size();i++)
  {
     listview_list[i].pos=i;
     LV_SetItem(hdlg,(char*)listview_list[i].partition.c_str(),i,0);
     LV_SetItem(hdlg,(char*)listview_list[i].type.c_str(),i,LVC_TYPE);

     LV_SetItem(hdlg,(char*)listview_list[i].fs_type.c_str(),i,LVC_FS_TYPE);
     LV_SetItem(hdlg,(char*)listview_list[i].size.c_str(),i,LVC_SIZE);

     LV_SetItem(hdlg,(char*)listview_list[i].free.c_str(),i,LVC_FREE);

     LV_SetItem(hdlg,(char*)listview_list[i].begin_sect.c_str(),i,LVC_BEGIN);
     LV_SetItem(hdlg,(char*)listview_list[i].last_sect.c_str(),i,LVC_END);

     LV_SetItem(hdlg,(char*)listview_list[i].mountpoint.c_str(),i,LVC_MOUNTPOINT);


  }


}


//
// Creates a list of physical disks in specified control
//
void DlgPartmod::ListDisks(HWND hdlg)
{
stringstream ss;
Disk *disk=new Disk;
for(int i=0,j=0;i<32;i++)
  {
    ss.str("");
    ss<<"\\\\.\\PhysicalDrive"<<i;

    try
      {
          disk->Open(ss.str());
      }
    catch(DiskException&) { continue; }

        LV_InsertItem(hdlg,(char*)ss.str().c_str());

        if( disk->GetDiskType()==DISK_MBR)
           {
             LV_SetItem(hdlg,"MBR",j,LVC_DISK_TYPE);
           }
        else if( disk->GetDiskType()==DISK_GPT)
          {
            LV_SetItem(hdlg,"GPT",j,LVC_DISK_TYPE);
          }
        else LV_SetItem(hdlg,"Empty?",j,LVC_DISK_TYPE);

        LV_SetItem(hdlg,(char*)size_str(disk->LastSector(),disk->GetDiskGeometry().bps).c_str(),j,LVC_DISK_SIZE);

        unsigned __int64 free=0;
        for(int i=0;i<disk->CountFreeSpaces();i++) free+=disk->GetFreeSpace(i).length;

        LV_SetItem(hdlg,(char*)size_str(free,disk->GetDiskGeometry().bps).c_str(),j,LVC_DISK_FREE);
        LV_SetItem(hdlg,U64ToStr(disk->GetDiskGeometry().cylinders).c_str(),j,LVC_DISK_CYLINDERS);
        LV_SetItem(hdlg,U64ToStr(disk->GetDiskGeometry().bps).c_str(),j,LVC_DISK_BPS);
        LV_SetItem(hdlg,U64ToStr(disk->GetDiskGeometry().spt).c_str(),j,LVC_DISK_SPT);
        LV_SetItem(hdlg,U64ToStr(disk->GetDiskGeometry().tpc).c_str(),j,LVC_DISK_TPC);
        LV_SetItem(hdlg,U64ToStr(disk->LastSector()).c_str(),j,LVC_DISK_SECTORS);
        LV_SetItem(hdlg,U64ToStr(disk->GetDiskSignature(),STR_HEX).c_str(),j,LVC_DISK_SIGNATURE);

        disk->Close();
        j++;

  }

}

sel_type DlgPartmod::get_sel_type(HWND hList)
{
int selection=0,i=0;
     i=SendMessage(hList,LVM_GETNEXTITEM,-1,LVNI_FOCUSED);


selection=listview_list[i].selection_type.type;
if(selection!=S_FREE_SPACE)
  {
      frs_selected=-1;
      part_selected=listview_list[i].num;
  }
else
  {
      frs_selected=listview_list[i].num;
      part_selected=-1;
  }


return listview_list[i].selection_type;

}





