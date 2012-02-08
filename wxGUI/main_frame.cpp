#include "main_frame.hpp"
#include "dlg_newpart.hpp"
#include "dlg_open_diskimage.h"
#include "dlg_guid_list.h"
#include "dlg_create_backup.h"
#include "dlg_restore_backup.h"
#include "dlg_save_changes.h"

#include <sstream>
#include <algorithm>
using namespace std;

bool cmp_lv(lvlist a,lvlist b);


    const long MainFrame::ID_QUIT = wxNewId();
    const long MainFrame::ID_ABOUT = wxNewId();

    const long MainFrame::ID_SAVE_CHANGES = wxNewId();
    const long MainFrame::ID_CLOSE_DISK = wxNewId() ;
    const long MainFrame::ID_OPEN_DISK_IMAGE = wxNewId();
    const long MainFrame::ID_CREATE_BACKUP = wxNewId();
    const long MainFrame::ID_RESTORE_BACKUP = wxNewId();
    const long MainFrame::ID_CHECK_DISK = wxNewId();

    const long MainFrame::ID_CREATE_PARTITION = wxNewId() ;
    const long MainFrame::ID_DELETE_PARTITION = wxNewId();
    const long MainFrame::ID_SET_ACTIVE = wxNewId();
    const long MainFrame::ID_SET_INACTIVE = wxNewId();
    const long MainFrame::ID_EDIT_BOOTSECTOR = wxNewId();
    const long MainFrame::ID_CHECK_FS = wxNewId();
    const long MainFrame::ID_FORMAT = wxNewId();
    const long MainFrame::ID_WIPE_PARTITION = wxNewId();

    const long MainFrame::ID_DISK_LIST = wxNewId();
    const long MainFrame:: ID_PARTITION_LIST = wxNewId();

    const long MainFrame:: ID_LIST_GUID = wxNewId();



MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame(NULL, -1, title, pos, size)
{
    selected_partition=0,selected_frs=0;

    menuActions = new wxMenu;
    menuDisk = new wxMenu;
    menuPartition = new wxMenu;
    menuTools = new wxMenu;
    menuHelp = new wxMenu;

    mainPanel=new wxPanel(this,-1,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,"panel");
    diskList=new wxListCtrl(mainPanel,ID_DISK_LIST, wxPoint(10,30),wxSize(800,120),wxLC_REPORT,wxDefaultValidator,wxListCtrlNameStr);
    partitionList=new wxListCtrl(mainPanel,ID_PARTITION_LIST, wxPoint(10,180),wxSize(800,200),wxLC_REPORT,wxDefaultValidator,wxListCtrlNameStr);
    create_menus();

    create_disk_listctrl();
    create_partition_listctrl();


    CreateStatusBar();
    SetStatusText( _("idle") );

    disk=new Disk;

    refresh_disk_list();

    menuPartition->Enable(ID_DELETE_PARTITION,false);
    menuPartition->Enable(ID_SET_ACTIVE,false);
    menuPartition->Enable(ID_SET_INACTIVE,false);
    menuPartition->Enable(ID_CREATE_PARTITION,false);
    menuPartition->Enable(ID_WIPE_PARTITION,false);
    menuDisk->Enable(ID_CLOSE_DISK,false);

}

void MainFrame::OnSaveChangesClick(wxCommandEvent& event)
{

  int ret=wxMessageBox( _("Are you sure want to write changes to disk?"),
                        _("Warning"),wxYES_NO| wxICON_WARNING, this );
  if(ret!=wxYES)
      return;
/*
  DlgSaveChanges *dlg=new DlgSaveChanges(this);
  dlg->ShowModal(disk);

  refresh_partition_list();
  delete dlg;
*/

  try
  {
     disk->Save();
  }
  catch(exception &ex)
  {
     wxMessageBox( _(ex.what()),_("Error"),wxOK | wxICON_ERROR, this );
     return;
  }
   wxMessageBox( _("Finished succesfuly"),_("Information"),wxOK | wxICON_INFORMATION, this );
}

void MainFrame::OnDiskCloseClick(wxCommandEvent& event)
{
    disk->Close();
    refresh_partition_list();

    menuPartition->Enable(ID_CREATE_PARTITION,false);
    menuPartition->Enable(ID_DELETE_PARTITION,false);
    menuPartition->Enable(ID_SET_ACTIVE,false);
    menuPartition->Enable(ID_SET_INACTIVE,false);
    menuPartition->Enable(ID_WIPE_PARTITION,false);
    menuDisk->Enable(ID_CLOSE_DISK,false);
}

void MainFrame::OnNewPartition(wxCommandEvent& event)
{
    DlgNewPart *dlg=new DlgNewPart(this);
    dlg->ShowModal(disk,selected_frs);

    refresh_partition_list();
    delete dlg;
}

void MainFrame::OnCreateBackupClick(wxCommandEvent& event)
{
    DlgCreateBackup *dlg=new DlgCreateBackup(this);
    dlg->ShowModal(disk);

    refresh_partition_list();
    delete dlg;
}


void MainFrame::OnRestoreBackupClick(wxCommandEvent& event)
{
    DlgRestoreBackup *dlg=new DlgRestoreBackup(this);
    dlg->ShowModal(disk);

    refresh_partition_list();
    delete dlg;
}

void MainFrame::OnSetActiveClick(wxCommandEvent& event)
{
   try{

   if(disk->GetPartition(selected_partition).flags&PART_ACTIVE)
     {
       wxMessageBox(_("This partition is already active"),_("Information"),wxICON_EXCLAMATION,this);
       return;
     }
   if(disk->CountPartitions(PART_ACTIVE)>0)
     {
       int r=wxMessageBox(_("One partition is already set as active. Do you want to continue?"),_("Information"), wxYES_NO,this);
       if(r==wxYES)
         {
           disk->SetActive(selected_partition,true);
           refresh_partition_list();
         }
       return;
     }
    disk->SetActive(selected_partition,true);
    refresh_partition_list();
   }
   catch(DiskException &de)
   {
       wxMessageBox(_(de.what()),_("Error"),wxICON_ERROR,this);
   }

}

void MainFrame::OnWipePartitionClick(wxCommandEvent& event)
{
   int ret=wxMessageBox( _("Do you really want to wipe all data on the selected partition? Data recovery will not be possible."),
                        _("Warning"),wxYES_NO| wxICON_WARNING, this );
   if(ret!=wxYES)
      return;

   try{
   disk->Wipe(selected_partition,false);
   }
   catch(DiskException &de)
   {
       wxMessageBox(_(de.what()),_("Error"),wxICON_ERROR,this);
   }
   refresh_partition_list();
}


void MainFrame::OnUnsetActiveClick(wxCommandEvent& event)
{
   try{
   disk->SetActive(selected_partition,false);
   }
   catch(DiskException &de)
   {
       wxMessageBox(_(de.what()),_("Error"),wxICON_ERROR,this);
   }
   refresh_partition_list();
}

void MainFrame::OnListGuidClick(wxCommandEvent& event)
{
    DlgGuidList *dlg=new DlgGuidList(this);
    dlg->ShowModal(disk);

    delete dlg;
}

void MainFrame::OnOpenDiskImage(wxCommandEvent& event)
{
    DlgOpenDiskImage *dlg=new DlgOpenDiskImage(this);
    dlg->ShowModal(disk);

    if(disk->IsOpen())
        menuDisk->Enable(ID_CLOSE_DISK,true);


    refresh_partition_list();
    delete dlg;
}

void MainFrame::OnPartitionListClick(wxListEvent& event)
{

	if(disk_structure[event.GetIndex()].selection_type==S_FREE_SPACE)
	  {
	      menuPartition->Enable(ID_DELETE_PARTITION,false);
	      menuPartition->Enable(ID_SET_ACTIVE,false);
	      menuPartition->Enable(ID_SET_INACTIVE,false);
	      menuPartition->Enable(ID_CREATE_PARTITION,true);
	      menuPartition->Enable(ID_WIPE_PARTITION,false);

	      selected_frs=disk_structure[event.GetIndex()].num;
	  }
	else
	  {
	     menuPartition->Enable(ID_DELETE_PARTITION,true);
	     menuPartition->Enable(ID_SET_ACTIVE,true);
	     menuPartition->Enable(ID_SET_INACTIVE,true);
	     menuPartition->Enable(ID_CREATE_PARTITION,false);
	     menuPartition->Enable(ID_WIPE_PARTITION,true);
	     selected_partition=disk_structure[event.GetIndex()].num;
	  }
}

void MainFrame::OnPartitionListDeselect(wxListEvent& event)
{
    menuPartition->Enable(ID_DELETE_PARTITION,false);
    menuPartition->Enable(ID_SET_ACTIVE,false);
    menuPartition->Enable(ID_SET_INACTIVE,false);
    menuPartition->Enable(ID_CREATE_PARTITION,false);
    menuPartition->Enable(ID_WIPE_PARTITION,false);
}

void MainFrame::OnPartitionListRightClick(wxListEvent& event)
{
	void *data = reinterpret_cast<void *>(event.GetItem().GetData());
	wxMenu menu;
	menu.SetClientData( data );
	if(disk_structure[event.GetIndex()].selection_type==S_FREE_SPACE)
	  {
	      selected_frs=disk_structure[event.GetIndex()].num;
          menu.Append(ID_CREATE_PARTITION,"Create new partition...");

	  }
	else
	  {
	     selected_partition=disk_structure[event.GetIndex()].num;
      //   menu.Append(ID_WIPE_PARTITION,"Wipe partition");
         menu.Append(ID_DELETE_PARTITION,"Delete partition");

	  }

	menu.Connect(wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnPartitionListPopupClick, NULL, this);
	PopupMenu(&menu);
}


void MainFrame::OnPartitionListPopupClick(wxListEvent& event)
{
//	void *data=static_cast<wxMenu *>(event.GetEventObject())->GetClientData();
    int eid=event.GetId();

	if(eid==ID_DELETE_PARTITION)
        disk->DeletePartition(selected_partition);
	else if(eid==ID_CREATE_PARTITION)
		OnNewPartition(event);

    refresh_partition_list();
}

void MainFrame::OnDiskListClick(wxListEvent& event)
{
   wxString diskname=diskList->GetItemText ( event.GetIndex());

   if(disk->IsOpen())
        disk->Close();
   try{
   disk->Open(diskname.ToAscii());
   }
   catch(DiskException &de)
   {
       wxMessageBox(_(de.what()),_("Error"),wxICON_ERROR,this);
   }
    if(disk->IsOpen())
        menuDisk->Enable(ID_CLOSE_DISK,true);

   refresh_partition_list();
}

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox( _("Version 0.2"),_("About"),wxOK | wxICON_INFORMATION, this );
}






//
// -------------------------------------------------------------------------------
//


void MainFrame::refresh_disk_list()
{
stringstream ss;
Disk *disk=new Disk;
int item_index=0;


#if defined(_WIN32) || defined(WIN32)
for(int i=0,j=0;i<32;i++)
  {
    ss.str("");
    ss<<"\\\\.\\PhysicalDrive"<<i;
#else // Linux
for(char i='a',j=0;i<='z';i++)
  {
    ss.str("");
    ss<<"/dev/sd"<<i;
#endif
    try
      {
          disk->Open(ss.str());
      }
    catch(DiskException&de)
    {

       if(de.error_code==ERR_OPEN_DISK)
          {
              disk->Close();
              continue;
          }
     }

    diskList->InsertItem(item_index,_(ss.str().c_str()));

    if( disk->GetDiskType()==DISK_MBR)
        diskList->SetItem(item_index,1,_("MBR"),-1);
    else if( disk->GetDiskType()==DISK_GPT)
        diskList->SetItem(item_index,1,_("GPT"),-1);
    else
        diskList->SetItem(item_index,1,_("empty?"),-1);

    diskList->SetItem(item_index,2,_(size_str(disk->LastSector(),disk->GetDiskGeometry().bps).c_str()),-1);

    unsigned long long free=0;
    for(int i=0;i<disk->CountFreeSpaces();i++)
        free+=disk->GetFreeSpace(i).length;

    diskList->SetItem(item_index,3,_(size_str(free,disk->GetDiskGeometry().bps).c_str()),-1);
    diskList->SetItem(item_index,4,_(U64ToStr(disk->GetDiskGeometry().cylinders).c_str()),-1);
    diskList->SetItem(item_index,5,_(U64ToStr(disk->GetDiskGeometry().bps).c_str()),-1);
    diskList->SetItem(item_index,6,_(U64ToStr(disk->GetDiskGeometry().spt).c_str()),-1);
    diskList->SetItem(item_index,7,_(U64ToStr(disk->GetDiskGeometry().tpc).c_str()),-1);
    diskList->SetItem(item_index,8,_(U64ToStr(disk->LastSector()).c_str()),-1);
    diskList->SetItem(item_index,9,_(U64ToStr(disk->GetDiskSignature(),STR_HEX).c_str()),-1);

    disk->Close();
    j++;
    item_index++;

  }
delete disk;
}




void MainFrame::refresh_partition_list()
{
    partitionList->DeleteAllItems();
    disk_structure.clear();

    for(int i=0;i<disk->CountPartitions();i++)
      {
         GEN_PART gpart=disk->GetPartition(i);
         lvlist tmp;

         if(gpart.flags&PART_PRIMARY && gpart.flags&PART_ACTIVE)
             tmp.type="Primary (Active)";
         else if(gpart.flags&PART_PRIMARY)
             tmp.type="Primary";
         else if(gpart.flags&PART_EXTENDED)
             tmp.type="Extended MBR";
         else if(gpart.flags&PART_LOGICAL)
             tmp.type="Logical";
         else if(gpart.flags&PART_MBR_GPT)
             tmp.type="GPT part. table";
         else if(gpart.flags&PART_GPT)
             tmp.type="GPT partition";

         if(gpart.flags&PART_EXTENDED || gpart.flags&PART_MBR_GPT)
             tmp.mountpoint="";
         else
             tmp.mountpoint=get_mount_point(gpart,disk->GetDiskSignature());

         tmp.partition=U64ToStr(i+1);
         tmp.begin_sect=U64ToStr(gpart.begin_sector);
         tmp.last_sect=U64ToStr(gpart.begin_sector+gpart.length);
         tmp.free="N/A";
         if(gpart.flags&PART_PRIMARY || gpart.flags&PART_EXTENDED || gpart.flags&PART_MBR_GPT || gpart.flags&PART_LOGICAL)
             tmp.fs_type=disk->fsid_man->GetByFsid(disk->GetMBRSpecific(i).fsid).description;
         else if(gpart.flags&PART_GPT)
           {
             try{
                tmp.fs_type=disk->guid_man->GetDescription(disk->GetGPTSpecific(i).type_guid);
             }catch(...)
             {
                 tmp.fs_type="[unknown guid]";
             }
           }
         else tmp.fs_type="[unknown]";
         tmp.num=i;
         tmp.selection_type=S_PARTITION;
         tmp.flags=gpart.flags;
         tmp.size=size_str(gpart.length,disk->GetDiskGeometry().bps).c_str();

         disk_structure.push_back(tmp);
      }
    for(int i=0;i<disk->CountFreeSpaces();i++)
      {
         FREE_SPACE frs=disk->GetFreeSpace(i);
         lvlist tmp;

         tmp.partition="-";
         tmp.type="Free space";
         tmp.fs_type="";
         tmp.mountpoint="";
         tmp.begin_sect=U64ToStr(frs.begin_sector);
         tmp.last_sect=U64ToStr(frs.begin_sector+frs.length);
         tmp.flags=frs.type;
         tmp.selection_type=S_FREE_SPACE;
         tmp.num=i;
         tmp.size=size_str(frs.length,disk->GetDiskGeometry().bps).c_str();
         disk_structure.push_back(tmp);
      }

    sort(disk_structure.begin(),disk_structure.end(),cmp_lv);

    for(int i=0;i<disk_structure.size();i++)
      {
          lvlist tmp=disk_structure[i];

          partitionList->InsertItem(i,_(tmp.partition.c_str()),-1);
          partitionList->SetItem(i,1,_(tmp.type.c_str()),-1);
          partitionList->SetItem(i,2,_(tmp.fs_type.c_str()),-1);
          partitionList->SetItem(i,3,_(tmp.size.c_str()),-1);
         // partitionList->SetItem(i,4,_(tmp.free.c_str()),-1);
          partitionList->SetItem(i,4,_(tmp.begin_sect.c_str()),-1);
          partitionList->SetItem(i,5,_(tmp.last_sect.c_str()),-1);
          partitionList->SetItem(i,6,_(tmp.mountpoint.c_str()),-1);

          if(tmp.selection_type==S_FREE_SPACE)
            {
               if(tmp.flags!=FREE_UNALLOCATED)
                   partitionList->SetItemTextColour(i,wxColor(50,130,50));
               else partitionList->SetItemTextColour(i,wxColor(50,50,50));
            }

          else
            {
               if(tmp.flags&PART_PRIMARY)
                   partitionList->SetItemTextColour(i,wxColor(1,6,203));
               else if(tmp.flags&PART_EXTENDED || tmp.flags&PART_MBR_GPT)
                   partitionList->SetItemTextColour(i,wxColor(240,0,0));
               else if(tmp.flags&PART_LOGICAL || tmp.flags&PART_GPT)
                   partitionList->SetItemTextColour(i,wxColor(140,90,140));
            }
      }


}


void MainFrame::create_disk_listctrl()
{
    diskList->InsertColumn(0,_("Disk"),0,120);
    diskList->InsertColumn(1,_("Type"),0,50);
    diskList->InsertColumn(2,_("Size"),0,70);
    diskList->InsertColumn(3,_("Free"),0,70);
    diskList->InsertColumn(4,_("Cylinders"),0,70);
    diskList->InsertColumn(5,_("bps"),0,50);
    diskList->InsertColumn(6,_("spt"),0,50);
    diskList->InsertColumn(7,_("tpc"),0,50);
    diskList->InsertColumn(8,_("Total sectors"),0,100);
    diskList->InsertColumn(9,_("MBR signature"),0,100);
}

void MainFrame::create_partition_listctrl()
{
    partitionList->InsertColumn(0,_("Partition #"),0,70);
    partitionList->InsertColumn(1,_("Type"),0,100);
    partitionList->InsertColumn(2,_("FS type"),0,90);
    partitionList->InsertColumn(3,_("Size"),0,70);
    //partitionList->InsertColumn(4,_("Free"),0,70);
    partitionList->InsertColumn(5,_("First sect."),0,80);
    partitionList->InsertColumn(6,_("Last sect."),0,80);
    partitionList->InsertColumn(7,_("Mount point"),0,100);

}

void MainFrame::create_menus()
{

// Actions menu
    menuActions->Append(ID_SAVE_CHANGES,_("Save changes"),_("Write changes to disk"));
    menuActions->AppendSeparator();
    menuActions->Append( ID_QUIT, _("E&xit"),_("Exit without saving changes") );
// Disk menu
    menuDisk->Append(ID_CLOSE_DISK,_("Close disk (don\'t save changes)"),_("Close the disk handle"));
    menuDisk->Append(ID_OPEN_DISK_IMAGE,_("Open a disk image"),_("Open a disk image and partition it as a physical disk"));
    menuDisk->AppendSeparator();
    menuDisk->Append(ID_CREATE_BACKUP,_("Create partition table backup"));
    menuDisk->Append(ID_RESTORE_BACKUP,_("Restore partition table"));
    menuDisk->AppendSeparator();
   // menuDisk->Append(ID_CHECK_DISK,_("Check disk"));
// Partitions menu
    menuPartition->Append(ID_CREATE_PARTITION,_("Create new"),_("Create a new partition"));
    menuPartition->Append(ID_DELETE_PARTITION,_("Delete"));
    menuPartition->AppendSeparator();

    menuPartition->Append(ID_SET_ACTIVE,_("Set active"),_("Mark partition as active (bootable) partition"));
    menuPartition->Append(ID_SET_INACTIVE,_("Set inactive"),_("Remove active partition flag"));
 //   menuPartition->Append(ID_EDIT_BOOTSECTOR,_("Modify bootsector"));
 //   menuPartition->Append(ID_CHECK_FS,_("Check file system"));
 //   menuPartition->Append(ID_FORMAT,_("Format"));
    menuPartition->AppendSeparator();
    menuPartition->Append(ID_WIPE_PARTITION,_("Wipe"),_("Wipe all data on the partition"));

    menuTools->Append(ID_LIST_GUID,_("List known GUIDs"),_("List known Partition Globally Unique Identifiers"));



// Help
    menuHelp->Append( ID_ABOUT, _("&About..."),_("About the program") );


    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuActions, _("&Actions") );
    menuBar->Append( menuDisk, _("&Disk") );
    menuBar->Append( menuPartition, _("&Partition") );
    menuBar->Append( menuTools, _("&Tools") );
   // TODO: separator here
    menuBar->Append( menuHelp, _("Help") );

    SetMenuBar( menuBar );
}




// --------------------------------------------
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_QUIT,  MainFrame::OnQuit)
    EVT_MENU(ID_ABOUT, MainFrame::OnAbout)

    EVT_MENU(ID_CREATE_PARTITION,MainFrame::OnNewPartition)
    EVT_MENU(ID_OPEN_DISK_IMAGE,MainFrame::OnOpenDiskImage)
    EVT_MENU(ID_CLOSE_DISK, MainFrame::OnDiskCloseClick)
    EVT_MENU(ID_SAVE_CHANGES, MainFrame::OnSaveChangesClick)

    EVT_MENU(ID_SET_ACTIVE, MainFrame::OnSetActiveClick)
    EVT_MENU(ID_SET_INACTIVE, MainFrame::OnUnsetActiveClick)

    EVT_MENU(ID_LIST_GUID, MainFrame::OnListGuidClick)

    EVT_MENU(ID_CREATE_BACKUP, MainFrame::OnCreateBackupClick)
    EVT_MENU(ID_RESTORE_BACKUP, MainFrame::OnRestoreBackupClick)

    EVT_MENU(ID_WIPE_PARTITION, MainFrame::OnWipePartitionClick)


// Disk list events
    EVT_LIST_ITEM_ACTIVATED(ID_DISK_LIST, MainFrame::OnDiskListClick)
    EVT_LIST_ITEM_SELECTED(ID_PARTITION_LIST, MainFrame::OnPartitionListClick)
    EVT_LIST_ITEM_DESELECTED(ID_PARTITION_LIST, MainFrame::OnPartitionListDeselect)
// Partition list events
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PARTITION_LIST, MainFrame::OnPartitionListRightClick)
END_EVENT_TABLE()


// ----------------------
bool cmp_lv(lvlist a,lvlist b)
  {
    return StrToU64((char*)a.begin_sect.c_str())<StrToU64((char*)b.begin_sect.c_str());
  }
