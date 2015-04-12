#include "main_frame.hpp"
#include <sstream>
using namespace std;

//(*InternalHeaders(MainFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/msgdlg.h>

#include "dlg_newpart.hpp"
#include "dlg_open_diskimage.h"
#include "dlg_guid_list.h"
#include "dlg_create_backup.h"
#include "dlg_restore_backup.h"
#include "dlg_save_changes.h"
#include "dlg_partition_properties.h"
#include "dlg_fsid_list.h"
#include "dlg_pending_list.h"



//(*IdInit(MainFrame)
const long MainFrame::ID_LISTVIEW1 = wxNewId();
const long MainFrame::ID_LISTVIEW2 = wxNewId();
const long MainFrame::ID_BUTTON1 = wxNewId();
const long MainFrame::ID_BUTTON2 = wxNewId();
const long MainFrame::ID_PANEL1 = wxNewId();
const long MainFrame::ID_SAVE_CHANGES = wxNewId();
const long MainFrame::ID_REFRESH_DISK_LIST = wxNewId();
const long MainFrame::ID_QUIT = wxNewId();
const long MainFrame::ID_CLOSE_DISK = wxNewId();
const long MainFrame::ID_OPEN_DISK_IMAGE = wxNewId();
const long MainFrame::ID_CREATE_PARTITION = wxNewId();
const long MainFrame::ID_DELETE_PARTITION = wxNewId();
const long MainFrame::ID_SET_ACTIVE = wxNewId();
const long MainFrame::ID_SET_INACTIVE = wxNewId();
const long MainFrame::ID_PARTITION_PROPERTIES = wxNewId();
const long MainFrame::ID_WIPE_PARTITION = wxNewId();
const long MainFrame::ID_MENUITEM3 = wxNewId();
const long MainFrame::ID_MENUITEM4 = wxNewId();
const long MainFrame::ID_MENUITEM5 = wxNewId();
const long MainFrame::ID_CREATE_BACKUP = wxNewId();
const long MainFrame::ID_RESTORE_BACKUP = wxNewId();
const long MainFrame::ID_LIST_GUID = wxNewId();
const long MainFrame::ID_MENUITEM1 = wxNewId();
const long MainFrame::ID_ABOUT = wxNewId();
const long MainFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(MainFrame,wxFrame)
	//(*EventTable(MainFrame)
	//*)
END_EVENT_TABLE()





MainFrame::MainFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(MainFrame)
	Create(parent, wxID_ANY, _("Partmod"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(860,500));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(160,144), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	diskList = new wxListView(Panel1, ID_LISTVIEW1, wxPoint(16,24), wxSize(800,120), wxLC_REPORT, wxDefaultValidator, _T("ID_LISTVIEW1"));
	partitionList = new wxPartitionList(Panel1, ID_LISTVIEW2, wxPoint(16,168), wxSize(800,200), wxLC_REPORT, wxDefaultValidator, _T("ID_LISTVIEW2"));
	ButtonSaveChanges = new wxButton(Panel1, ID_BUTTON1, _(" Save changes "), wxPoint(344,392), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	ButtonQuit = new wxButton(Panel1, ID_BUTTON2, _("Quit"), wxPoint(352,424), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	menuBar = new wxMenuBar();
	menuActions = new wxMenu();
	MenuSaveChanges = new wxMenuItem(menuActions, ID_SAVE_CHANGES, _("Save changes"), _("Write partition table to disk"), wxITEM_NORMAL);
	menuActions->Append(MenuSaveChanges);
	MenuRefreshDiskList = new wxMenuItem(menuActions, ID_REFRESH_DISK_LIST, _("Refresh disk list"), _("Note: all changes will be discarded"), wxITEM_NORMAL);
	menuActions->Append(MenuRefreshDiskList);
	menuActions->AppendSeparator();
	MenuQuit = new wxMenuItem(menuActions, ID_QUIT, _("Exit"), _("Exit this program without saving any changes"), wxITEM_NORMAL);
	menuActions->Append(MenuQuit);
	menuBar->Append(menuActions, _("Actions"));
	menuDisk = new wxMenu();
	MenuCloseDisk = new wxMenuItem(menuDisk, ID_CLOSE_DISK, _("Close disk"), _("Close disk or a disk image without saving any changes"), wxITEM_NORMAL);
	menuDisk->Append(MenuCloseDisk);
	MenuOpenDiskImage = new wxMenuItem(menuDisk, ID_OPEN_DISK_IMAGE, _("Open a disk image..."), _("Open a disk image and modify partitions on it as on a physical disk"), wxITEM_NORMAL);
	menuDisk->Append(MenuOpenDiskImage);
	menuBar->Append(menuDisk, _("Disk"));
	menuPartition = new wxMenu();
	MenuCreatePartition = new wxMenuItem(menuPartition, ID_CREATE_PARTITION, _("Create partition..."), wxEmptyString, wxITEM_NORMAL);
	menuPartition->Append(MenuCreatePartition);
	MenuDeletePartition = new wxMenuItem(menuPartition, ID_DELETE_PARTITION, _("Delete partition"), wxEmptyString, wxITEM_NORMAL);
	menuPartition->Append(MenuDeletePartition);
	menuPartition->AppendSeparator();
	MenuSetActive = new wxMenuItem(menuPartition, ID_SET_ACTIVE, _("Set active (bootable)"), wxEmptyString, wxITEM_NORMAL);
	menuPartition->Append(MenuSetActive);
	MenuSetInactive = new wxMenuItem(menuPartition, ID_SET_INACTIVE, _("Set inactive"), wxEmptyString, wxITEM_NORMAL);
	menuPartition->Append(MenuSetInactive);
	menuPartition->AppendSeparator();
	MenuPartitionProperties = new wxMenuItem(menuPartition, ID_PARTITION_PROPERTIES, _("Properties..."), _("More information about the partition"), wxITEM_NORMAL);
	menuPartition->Append(MenuPartitionProperties);
	menuPartition->AppendSeparator();
	MenuWipePartition = new wxMenuItem(menuPartition, ID_WIPE_PARTITION, _("Wipe partition"), wxEmptyString, wxITEM_NORMAL);
	menuPartition->Append(MenuWipePartition);
	MenuWipePartition->Enable(false);
	menuBar->Append(menuPartition, _("Partition"));
	Menu1 = new wxMenu();
	MenuShowPending = new wxMenuItem(Menu1, ID_MENUITEM3, _("Show pending operations..."), wxEmptyString, wxITEM_NORMAL);
	Menu1->Append(MenuShowPending);
	MenuUndoLastPending = new wxMenuItem(Menu1, ID_MENUITEM4, _("Undo last"), _("Undo last pending operation"), wxITEM_NORMAL);
	Menu1->Append(MenuUndoLastPending);
	MenuUndoAllPending = new wxMenuItem(Menu1, ID_MENUITEM5, _("Undo all"), _("Undo all pending operations"), wxITEM_NORMAL);
	Menu1->Append(MenuUndoAllPending);
	menuBar->Append(Menu1, _("Pending"));
	menuBackup = new wxMenu();
	MenuCreateBackup = new wxMenuItem(menuBackup, ID_CREATE_BACKUP, _("Create partition table backup..."), _("Create a backup of the partition table structure"), wxITEM_NORMAL);
	menuBackup->Append(MenuCreateBackup);
	MenuRestoreBackup = new wxMenuItem(menuBackup, ID_RESTORE_BACKUP, _("Restore partition table..."), _("Restore partition table from backup"), wxITEM_NORMAL);
	menuBackup->Append(MenuRestoreBackup);
	menuBar->Append(menuBackup, _("Backup"));
	menuTools = new wxMenu();
	MenuListGuid = new wxMenuItem(menuTools, ID_LIST_GUID, _("List known GUIDs..."), wxEmptyString, wxITEM_NORMAL);
	menuTools->Append(MenuListGuid);
	MenuListFsid = new wxMenuItem(menuTools, ID_MENUITEM1, _("List known FSIDs..."), wxEmptyString, wxITEM_NORMAL);
	menuTools->Append(MenuListFsid);
	menuBar->Append(menuTools, _("Tools"));
	menuHelp = new wxMenu();
	MenuAbout = new wxMenuItem(menuHelp, ID_ABOUT, _("About..."), _("About this program"), wxITEM_NORMAL);
	menuHelp->Append(MenuAbout);
	menuBar->Append(menuHelp, _("Help"));
	SetMenuBar(menuBar);
	StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
	int __wxStatusBarWidths_1[1] = { -10 };
	int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
	StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
	StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
	SetStatusBar(StatusBar1);

	Connect(ID_LISTVIEW1,wxEVT_COMMAND_LIST_ITEM_ACTIVATED,(wxObjectEventFunction)&MainFrame::OndiskListItemActivated);
	Connect(ID_LISTVIEW2,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&MainFrame::OnPartitionListItemActivated);
	Connect(ID_LISTVIEW2,wxEVT_COMMAND_LIST_ITEM_DESELECTED,(wxObjectEventFunction)&MainFrame::OnPartitionListItemDeselect);
	Connect(ID_LISTVIEW2,wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&MainFrame::OnPartitionListItemRClick);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnMenuSaveChangesSelected);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnMenuQuitSelected);
	Connect(ID_SAVE_CHANGES,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuSaveChangesSelected);
	Connect(ID_REFRESH_DISK_LIST,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuRefreshDiskListSelected);
	Connect(ID_QUIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuQuitSelected);
	Connect(ID_CLOSE_DISK,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuCloseDiskSelected);
	Connect(ID_OPEN_DISK_IMAGE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuOpenDiskImageSelected);
	Connect(ID_CREATE_PARTITION,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuCreatePartitionSelected);
	Connect(ID_DELETE_PARTITION,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuDeletePartitionSelected);
	Connect(ID_SET_ACTIVE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuSetActiveSelected);
	Connect(ID_SET_INACTIVE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuSetInactiveSelected);
	Connect(ID_PARTITION_PROPERTIES,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuPartitionPropertiesSelected);
	Connect(ID_WIPE_PARTITION,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuWipePartitionSelected);
	Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuShowPendingSelected);
	Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuUndoPendingSelected);
	Connect(ID_MENUITEM5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuUndoAllPendingSelected);
	Connect(ID_CREATE_BACKUP,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuCreateBackupSelected);
	Connect(ID_RESTORE_BACKUP,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuRestoreBackupSelected);
	Connect(ID_LIST_GUID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuListGuidSelected);
	Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuListFsidSelected);
	Connect(ID_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuAboutSelected);
	//*)

    disk=new Disk;
    partitionList->AssignDisk(disk);

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

// -----

    menuPartition->Enable(ID_DELETE_PARTITION,false);
    menuPartition->Enable(ID_SET_ACTIVE,false);
    menuPartition->Enable(ID_SET_INACTIVE,false);
    menuPartition->Enable(ID_CREATE_PARTITION,false);
    menuPartition->Enable(ID_WIPE_PARTITION,false);
    menuPartition->Enable(ID_PARTITION_PROPERTIES,false);

    menuDisk->Enable(ID_CLOSE_DISK,false);
    menuActions->Enable(ID_SAVE_CHANGES,false);
    menuBackup->Enable(ID_CREATE_BACKUP,false);
    menuBackup->Enable(ID_RESTORE_BACKUP,false);

// ---
    refresh_disk_list();
}

MainFrame::~MainFrame()
{
	//(*Destroy(MainFrame)
	//*)
	disk->Close();
	delete disk;
}






void MainFrame::refresh_disk_list()
{
diskList->DeleteAllItems();

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
    catch(...)
    {
        continue;
    }
    diskList->InsertItem(item_index,ss.str().c_str());

    if( disk->GetDiskType()==DISK_MBR)
        diskList->SetItem(item_index,1,_("MBR"),-1);
    else if( disk->GetDiskType()==DISK_GPT)
        diskList->SetItem(item_index,1,_("GPT"),-1);
    else
        diskList->SetItem(item_index,1,_("unknown/empty"),-1);

    diskList->SetItem(item_index,2,size_str(disk->LastSector(),disk->GetDiskGeometry().bps).c_str(),-1);

    unsigned long long free=0;
    for(int i=0;i<disk->CountFreeSpaces();i++)
        free+=disk->GetFreeSpace(i).length;

    diskList->SetItem(item_index,3,size_str(free,disk->GetDiskGeometry().bps).c_str(),-1);
    diskList->SetItem(item_index,4,U64ToStr(disk->GetDiskGeometry().cylinders).c_str(),-1);
    diskList->SetItem(item_index,5,U64ToStr(disk->GetDiskGeometry().bps).c_str(),-1);
    diskList->SetItem(item_index,6,U64ToStr(disk->GetDiskGeometry().spt).c_str(),-1);
    diskList->SetItem(item_index,7,U64ToStr(disk->GetDiskGeometry().tpc).c_str(),-1);
    diskList->SetItem(item_index,8,U64ToStr(disk->LastSector()).c_str(),-1);
    diskList->SetItem(item_index,9,U64ToStr(disk->GetDiskSignature(),STR_HEX).c_str(),-1);

    disk->Close();
    j++;
    item_index++;

  }
delete disk;
}

void MainFrame::btn_disable_readonly()
{
  if(disk->IsReadOnly())
  {
    menuPartition->Enable(ID_DELETE_PARTITION,false);
    menuPartition->Enable(ID_SET_ACTIVE,false);
    menuPartition->Enable(ID_SET_INACTIVE,false);
    menuPartition->Enable(ID_CREATE_PARTITION,false);
    menuPartition->Enable(ID_WIPE_PARTITION,false);

    menuActions->Enable(ID_SAVE_CHANGES,false);
    menuBackup->Enable(ID_CREATE_BACKUP,false);
  }

}



void MainFrame::OndiskListItemActivated(wxListEvent& event)
{
  wxString diskname=diskList->GetItemText ( event.GetIndex());


   try{
   if(disk->IsOpen())
        disk->Close();

   disk->Open(diskname.ToStdString());
   }
   catch(DiskException &de)
   {
       wxMessageBox(de.what(),_("Error"),wxICON_ERROR,this);

   }
   catch(...)
   {
      wxMessageBox(_("Unknown error"),_("Error"),wxICON_ERROR,this);

   }
    if(disk->IsOpen())
    {
       menuDisk->Enable(ID_CLOSE_DISK,true);
       menuActions->Enable(ID_SAVE_CHANGES,true);
       menuBackup->Enable(ID_CREATE_BACKUP,true);
       menuBackup->Enable(ID_RESTORE_BACKUP,true);
       btn_disable_readonly();
    }

   if(disk->ErrorOnLoad())
   {
       wxMessageBox(_("An error occurRed while loading partition tables."
                      "This means that either the partition table is not valid or that there is a bug in Partmod.\r\n"
                      "If you are not sure, do not modify the partition table -> DO NOT click \'Save\'"),_("Warning"),wxICON_WARNING,this);
   }
   if(disk->IsReadOnly())
   {
      wxMessageBox(_("Cannot open disk in writable mode. The disk is open in read-only mode.\n"
                     "If you need full access, make sure that:\n"
                     "* the disk is not used by other programs\n"
                     "* you have sufficient access rights (linux: run as root)\n"
                     "* the media is not write-protected"),_("Disk is read-only"),wxICON_WARNING,this);
   }
   partitionList->Refresh();
}


void MainFrame::OnMenuCloseDiskSelected(wxCommandEvent& event)
{
    disk->Close();
    partitionList->Refresh();

    menuPartition->Enable(ID_CREATE_PARTITION,false);
    menuPartition->Enable(ID_DELETE_PARTITION,false);
    menuPartition->Enable(ID_SET_ACTIVE,false);
    menuPartition->Enable(ID_SET_INACTIVE,false);
    menuPartition->Enable(ID_WIPE_PARTITION,false);
    menuPartition->Enable(ID_PARTITION_PROPERTIES,false);

    menuDisk->Enable(ID_CLOSE_DISK,false);
    menuActions->Enable(ID_SAVE_CHANGES,false);
    menuBackup->Enable(ID_CREATE_BACKUP,false);
    menuBackup->Enable(ID_RESTORE_BACKUP,false);
}

void MainFrame::OnMenuSaveChangesSelected(wxCommandEvent& event)
{
  DlgSaveChanges *dlg=new DlgSaveChanges(this);
  dlg->ShowModal(disk);
  delete dlg;
  partitionList->Refresh();
}

void MainFrame::OnMenuQuitSelected(wxCommandEvent& event)
{
  Close(true);
}

void MainFrame::OnMenuOpenDiskImageSelected(wxCommandEvent& event)
{
    DlgOpenDiskImage *dlg=new DlgOpenDiskImage(this);
    dlg->ShowModal(disk);
    delete dlg;

    if(disk->IsOpen())
    {
        menuDisk->Enable(ID_CLOSE_DISK,true);
        menuActions->Enable(ID_SAVE_CHANGES,true);
        menuBackup->Enable(ID_CREATE_BACKUP,true);
        menuBackup->Enable(ID_RESTORE_BACKUP,true);
        btn_disable_readonly();
    }

   if(disk->ErrorOnLoad())
   {
       wxMessageBox(_("An error occured while loading partition tables."
                      "This means that either the partition table is not valid or that there is a bug in Partmod.\r\n"
                      "If you are not sure, do not modify the partition table -> DO NOT click \'Save\'"),_("Warning"),wxICON_WARNING,this);
   }

   partitionList->Refresh();
}

void MainFrame::OnMenuCreateBackupSelected(wxCommandEvent& event)
{
   DlgCreateBackup *dlg=new DlgCreateBackup(this);
   dlg->ShowModal(disk);
   delete dlg;

   partitionList->Refresh();
}

void MainFrame::OnMenuRestoreBackupSelected(wxCommandEvent& event)
{
   DlgRestoreBackup *dlg=new DlgRestoreBackup(this);
   dlg->ShowModal(disk);
   delete dlg;

   partitionList->Refresh();
}

void MainFrame::OnMenuCreatePartitionSelected(wxCommandEvent& event)
{
   DlgNewPart *dlg=new DlgNewPart(this);
   dlg->ShowModal(disk,partitionList->GetSelectedFreeSpace());
   delete dlg;

   partitionList->Refresh();
}

void MainFrame::OnMenuDeletePartitionSelected(wxCommandEvent& event)
{
  try
    {
        disk->DeletePartition(partitionList->GetSelectedPartition());
    }
  catch(DiskException &de)
    {
      wxMessageBox(de.what(),_("Error"),wxICON_ERROR,this);
    }
  partitionList->Refresh();
}

void MainFrame::OnMenuSetActiveSelected(wxCommandEvent& event)
{
   try{

   if(disk->GetPartition(partitionList->GetSelectedPartition()).flags&PART_ACTIVE)
     {
       wxMessageBox(_("This partition is already active"),_("Information"),wxICON_EXCLAMATION,this);
       return;
     }
   if(disk->CountPartitions(PART_ACTIVE)>0)
     {
       int r=wxMessageBox(_("One partition is already set as active. Do you want to continue?"),_("Information"), wxYES_NO,this);
       if(r==wxYES)
         {
           disk->SetActive(partitionList->GetSelectedPartition(),true);
           partitionList->Refresh();
         }
       return;
     }
    disk->SetActive(partitionList->GetSelectedPartition(),true);
    partitionList->Refresh();
   }
   catch(DiskException &de)
   {
       wxMessageBox(de.what(),_("Error"),wxICON_ERROR,this);
   }

}

void MainFrame::OnMenuSetInactiveSelected(wxCommandEvent& event)
{
   try{
       disk->SetActive(partitionList->GetSelectedPartition(),false);
   }
   catch(DiskException &de)
   {
       wxMessageBox(de.what(),_("Error"),wxICON_ERROR,this);
   }
   partitionList->Refresh();
}

void MainFrame::OnMenuWipePartitionSelected(wxCommandEvent& event)
{
   const char *msg_usual= _("Do you really want to wipe all data on the selected partition? Data recovery will not be possible.");
   const char *msg_extended=_("By wiping extended partition you will wipe data on ALL logical partitions in it. Are you sure?");
   const char *msg_guid=_("By wiping GUID partition table you will wipe data on ALL GPT partitions in it. Are you sure?");

   const char *displayed_msg=msg_usual;

   try{
     unsigned flags=disk->GetPartition(partitionList->GetSelectedPartition()).flags;

     if(flags&PART_EXTENDED)
         displayed_msg=msg_extended;
     else if(flags&PART_MBR_GPT)
         displayed_msg=msg_guid;


     int ret=wxMessageBox( displayed_msg,
                          _("Warning"),wxYES_NO| wxICON_WARNING, this );
     if(ret!=wxYES)
        return;


     disk->Wipe(partitionList->GetSelectedPartition(),false);
   }
   catch(DiskException &de)
   {
       wxMessageBox(de.what(),_("Error"),wxICON_ERROR,this);
   }
   partitionList->Refresh();
}

void MainFrame::OnMenuListGuidSelected(wxCommandEvent& event)
{
    DlgGuidList *dlg=new DlgGuidList(this);
    dlg->ShowModal(disk);

    delete dlg;
}

void MainFrame::OnMenuAboutSelected(wxCommandEvent& event)
{
  wxMessageBox( _("Version 0.3.1"),_("About"),wxOK | wxICON_INFORMATION, this );
}

// -------------------------------------------------------

void MainFrame::OnPartitionListItemActivated(wxListEvent& event)
{

	if(partitionList->GetSelectedPartition()==-1)
	  {
	      menuPartition->Enable(ID_DELETE_PARTITION,false);
	      menuPartition->Enable(ID_SET_ACTIVE,false);
	      menuPartition->Enable(ID_SET_INACTIVE,false);
	      menuPartition->Enable(ID_CREATE_PARTITION,true);
	      menuPartition->Enable(ID_WIPE_PARTITION,false);
          menuPartition->Enable(ID_PARTITION_PROPERTIES,false);
	  }
	else
	  {
	     menuPartition->Enable(ID_DELETE_PARTITION,true);
	     menuPartition->Enable(ID_SET_ACTIVE,true);
	     menuPartition->Enable(ID_SET_INACTIVE,true);
	     menuPartition->Enable(ID_CREATE_PARTITION,false);
	     menuPartition->Enable(ID_WIPE_PARTITION,true);
	     menuPartition->Enable(ID_PARTITION_PROPERTIES,true);
	     btn_disable_readonly();
	  }
}

void MainFrame::OnPartitionListItemRClick(wxListEvent& event)
{
	void *data = reinterpret_cast<void *>(event.GetItem().GetData());
	wxMenu menu;
	menu.SetClientData( data );
	if(partitionList->GetSelectedFreeSpace()>-1)
	  {
         if(disk->IsReadOnly()==false)
             menu.Append(ID_CREATE_PARTITION,"Create new partition...");
          else return;
	  }
	else
	  {
	     if(disk->IsReadOnly()==false)
	     {
           menu.Append(ID_WIPE_PARTITION,"Wipe partition");
           menu.Append(ID_DELETE_PARTITION,"Delete partition");
           menu.AppendSeparator();
	     }
         menu.Append(ID_PARTITION_PROPERTIES,"Properties...");
	  }

	menu.Connect(wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnPartitionListPopupClick, NULL, this);
	PopupMenu(&menu);
}

void MainFrame::OnPartitionListPopupClick(wxListEvent& event)
{
    int eid=event.GetId();

	if(eid==ID_DELETE_PARTITION)
        OnMenuDeletePartitionSelected(event);
	else if(eid==ID_CREATE_PARTITION)
		OnMenuCreatePartitionSelected(event);
	else if(eid==ID_PARTITION_PROPERTIES)
		OnMenuPartitionPropertiesSelected(event);
	else if(eid==ID_WIPE_PARTITION)
		OnMenuWipePartitionSelected(event);

   partitionList->Refresh();
}

void MainFrame::OnMenuPartitionPropertiesSelected(wxCommandEvent& event)
{
  DlgPartitionProperties *dlg=new DlgPartitionProperties(this);
  dlg->ShowModal(disk,partitionList->GetSelectedPartition());

  delete dlg;
}

void MainFrame::OnPartitionListItemDeselect(wxListEvent& event)
{
    menuPartition->Enable(ID_DELETE_PARTITION,false);
    menuPartition->Enable(ID_SET_ACTIVE,false);
    menuPartition->Enable(ID_SET_INACTIVE,false);
    menuPartition->Enable(ID_CREATE_PARTITION,false);
    menuPartition->Enable(ID_WIPE_PARTITION,false);
    menuPartition->Enable(ID_PARTITION_PROPERTIES,false);
}

void MainFrame::OnMenuRefreshDiskListSelected(wxCommandEvent& event)
{
   disk->Close();
   refresh_disk_list();
   partitionList->Refresh();
}

void MainFrame::OnMenuListFsidSelected(wxCommandEvent& event)
{
    DlgFsidList *dlg=new DlgFsidList(this);
    dlg->ShowModal(disk);

    delete dlg;
}

void MainFrame::OnMenuUndoPendingSelected(wxCommandEvent& event)
{
  disk->Undo();
  partitionList->Refresh();
}

void MainFrame::OnMenuShowPendingSelected(wxCommandEvent& event)
{
    DlgPendingList *dlg=new DlgPendingList(this);
    dlg->ShowModal(disk);
    delete dlg;
    partitionList->Refresh();
}


void MainFrame::OnMenuUndoAllPendingSelected(wxCommandEvent& event)
{
    try
    {
        disk->UndoAll();
    }
    catch(DiskException &de)
    {
        wxMessageBox( de.what(),_("Error"),wxOK | wxICON_ERROR, this );
        return;
    }

  partitionList->Refresh();
}
