#include "main_frame.hpp"
#include <sstream>
#include <algorithm>
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



//(*IdInit(MainFrame)
const long MainFrame::ID_LISTVIEW1 = wxNewId();
const long MainFrame::ID_LISTVIEW2 = wxNewId();
const long MainFrame::ID_BUTTON1 = wxNewId();
const long MainFrame::ID_BUTTON2 = wxNewId();
const long MainFrame::ID_PANEL1 = wxNewId();
const long MainFrame::ID_SAVE_CHANGES = wxNewId();
const long MainFrame::ID_QUIT = wxNewId();
const long MainFrame::ID_CLOSE_DISK = wxNewId();
const long MainFrame::ID_OPEN_DISK_IMAGE = wxNewId();
const long MainFrame::ID_CREATE_BACKUP = wxNewId();
const long MainFrame::ID_RESTORE_BACKUP = wxNewId();
const long MainFrame::ID_CREATE_PARTITION = wxNewId();
const long MainFrame::ID_DELETE_PARTITION = wxNewId();
const long MainFrame::ID_SET_ACTIVE = wxNewId();
const long MainFrame::ID_SET_INACTIVE = wxNewId();
const long MainFrame::ID_PARTITION_PROPERTIES = wxNewId();
const long MainFrame::ID_WIPE_PARTITION = wxNewId();
const long MainFrame::ID_LIST_GUID = wxNewId();
const long MainFrame::ID_ABOUT = wxNewId();
const long MainFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(MainFrame,wxFrame)
	//(*EventTable(MainFrame)
	//*)
END_EVENT_TABLE()


bool cmp_lv(lvlist a,lvlist b)
  {
    return StrToU64((char*)a.begin_sect.c_str())<StrToU64((char*)b.begin_sect.c_str());
  }



MainFrame::MainFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(MainFrame)
	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(860,500));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(160,144), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	diskList = new wxListView(Panel1, ID_LISTVIEW1, wxPoint(16,24), wxSize(800,120), wxLC_REPORT, wxDefaultValidator, _T("ID_LISTVIEW1"));
	partitionList = new wxListView(Panel1, ID_LISTVIEW2, wxPoint(16,168), wxSize(800,200), wxLC_REPORT, wxDefaultValidator, _T("ID_LISTVIEW2"));
	Button1 = new wxButton(Panel1, ID_BUTTON1, _(" Save changes "), wxPoint(328,392), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	Button2 = new wxButton(Panel1, ID_BUTTON2, _("Quit"), wxPoint(336,424), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	menuBar = new wxMenuBar();
	menuActions = new wxMenu();
	MenuSaveChanges = new wxMenuItem(menuActions, ID_SAVE_CHANGES, _("Save changes"), wxEmptyString, wxITEM_NORMAL);
	menuActions->Append(MenuSaveChanges);
	menuActions->AppendSeparator();
	MenuQuit = new wxMenuItem(menuActions, ID_QUIT, _("Exit"), wxEmptyString, wxITEM_NORMAL);
	menuActions->Append(MenuQuit);
	menuBar->Append(menuActions, _("Actions"));
	menuDisk = new wxMenu();
	MenuCloseDisk = new wxMenuItem(menuDisk, ID_CLOSE_DISK, _("Close disk"), _("Close disk or a disk image without saving any changes"), wxITEM_NORMAL);
	menuDisk->Append(MenuCloseDisk);
	MenuOpenDiskImage = new wxMenuItem(menuDisk, ID_OPEN_DISK_IMAGE, _("Open a disk image..."), wxEmptyString, wxITEM_NORMAL);
	menuDisk->Append(MenuOpenDiskImage);
	menuDisk->AppendSeparator();
	MenuCreateBackup = new wxMenuItem(menuDisk, ID_CREATE_BACKUP, _("Create partition table backup..."), wxEmptyString, wxITEM_NORMAL);
	menuDisk->Append(MenuCreateBackup);
	MenuRestoreBackup = new wxMenuItem(menuDisk, ID_RESTORE_BACKUP, _("Restore partition table..."), wxEmptyString, wxITEM_NORMAL);
	menuDisk->Append(MenuRestoreBackup);
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
	MenuPartitionProperties = new wxMenuItem(menuPartition, ID_PARTITION_PROPERTIES, _("Properties..."), wxEmptyString, wxITEM_NORMAL);
	menuPartition->Append(MenuPartitionProperties);
	menuPartition->AppendSeparator();
	MenuWipePartition = new wxMenuItem(menuPartition, ID_WIPE_PARTITION, _("Wipe partition"), wxEmptyString, wxITEM_NORMAL);
	menuPartition->Append(MenuWipePartition);
	menuBar->Append(menuPartition, _("Partition"));
	menuTools = new wxMenu();
	MenuListGuid = new wxMenuItem(menuTools, ID_LIST_GUID, _("List known GUIDs..."), wxEmptyString, wxITEM_NORMAL);
	menuTools->Append(MenuListGuid);
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
	Connect(ID_SAVE_CHANGES,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuSaveChangesSelected);
	Connect(ID_QUIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuQuitSelected);
	Connect(ID_CLOSE_DISK,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuCloseDiskSelected);
	Connect(ID_OPEN_DISK_IMAGE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuOpenDiskImageSelected);
	Connect(ID_CREATE_BACKUP,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuCreateBackupSelected);
	Connect(ID_RESTORE_BACKUP,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuRestoreBackupSelected);
	Connect(ID_CREATE_PARTITION,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuCreatePartitionSelected);
	Connect(ID_DELETE_PARTITION,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuDeletePartitionSelected);
	Connect(ID_SET_ACTIVE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuSetActiveSelected);
	Connect(ID_SET_INACTIVE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuSetInactiveSelected);
	Connect(ID_PARTITION_PROPERTIES,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuPartitionPropertiesSelected);
	Connect(ID_WIPE_PARTITION,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuWipePartitionSelected);
	Connect(ID_LIST_GUID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuListGuidSelected);
	Connect(ID_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuAboutSelected);
	//*)

    disk=new Disk;

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

    partitionList->InsertColumn(0,_("Partition #"),0,70);
    partitionList->InsertColumn(1,_("Type"),0,100);
    partitionList->InsertColumn(2,_("FS type"),0,90);
    partitionList->InsertColumn(3,_("Size"),0,70);
    //partitionList->InsertColumn(4,_("Free"),0,70);
    partitionList->InsertColumn(5,_("First sect."),0,80);
    partitionList->InsertColumn(6,_("Last sect."),0,80);
    partitionList->InsertColumn(7,_("Mount point"),0,100);

// -----

    menuPartition->Enable(ID_DELETE_PARTITION,false);
    menuPartition->Enable(ID_SET_ACTIVE,false);
    menuPartition->Enable(ID_SET_INACTIVE,false);
    menuPartition->Enable(ID_CREATE_PARTITION,false);
    menuPartition->Enable(ID_WIPE_PARTITION,false);
    menuPartition->Enable(ID_PARTITION_PROPERTIES,false);

    menuDisk->Enable(ID_CLOSE_DISK,false);
    menuActions->Enable(ID_SAVE_CHANGES,false);
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

    if(disk->IsOpen()==false)
        return;

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


void MainFrame::OndiskListItemActivated(wxListEvent& event)
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
    {
       menuDisk->Enable(ID_CLOSE_DISK,true);
       menuActions->Enable(ID_SAVE_CHANGES,true);
    }


   refresh_partition_list();
}


void MainFrame::OnMenuCloseDiskSelected(wxCommandEvent& event)
{
    disk->Close();
    refresh_partition_list();

    menuPartition->Enable(ID_CREATE_PARTITION,false);
    menuPartition->Enable(ID_DELETE_PARTITION,false);
    menuPartition->Enable(ID_SET_ACTIVE,false);
    menuPartition->Enable(ID_SET_INACTIVE,false);
    menuPartition->Enable(ID_WIPE_PARTITION,false);
    menuPartition->Enable(ID_PARTITION_PROPERTIES,false);

    menuDisk->Enable(ID_CLOSE_DISK,false);
    menuActions->Enable(ID_SAVE_CHANGES,false);
}

void MainFrame::OnMenuSaveChangesSelected(wxCommandEvent& event)
{
  int ret=wxMessageBox( _("Are you sure want to write changes to disk?"),
                        _("Warning"),wxYES_NO| wxICON_WARNING, this );
  if(ret!=wxYES)
      return;

  DlgSaveChanges *dlg=new DlgSaveChanges(this);
  dlg->ShowModal(disk);
  delete dlg;
  refresh_partition_list();
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
    }


    refresh_partition_list();
}

void MainFrame::OnMenuCreateBackupSelected(wxCommandEvent& event)
{
    DlgCreateBackup *dlg=new DlgCreateBackup(this);
    dlg->ShowModal(disk);
    delete dlg;

    refresh_partition_list();
}

void MainFrame::OnMenuRestoreBackupSelected(wxCommandEvent& event)
{
    DlgRestoreBackup *dlg=new DlgRestoreBackup(this);
    dlg->ShowModal(disk);
    delete dlg;

    refresh_partition_list();
}

void MainFrame::OnMenuCreatePartitionSelected(wxCommandEvent& event)
{
    DlgNewPart *dlg=new DlgNewPart(this);
    dlg->ShowModal(disk,selected_frs);
    delete dlg;

    refresh_partition_list();
}

void MainFrame::OnMenuDeletePartitionSelected(wxCommandEvent& event)
{
  try
    {
        disk->DeletePartition(selected_partition);
    }
  catch(DiskException &de)
    {
      wxMessageBox(_(de.what()),_("Error"),wxICON_ERROR,this);
    }
  refresh_partition_list();
}

void MainFrame::OnMenuSetActiveSelected(wxCommandEvent& event)
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

void MainFrame::OnMenuSetInactiveSelected(wxCommandEvent& event)
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

void MainFrame::OnMenuWipePartitionSelected(wxCommandEvent& event)
{
   wxMessageBox(_("Action not implemented"),_("Not yet"),wxICON_INFORMATION,this);
    /*
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
   refresh_partition_list();*/
}

void MainFrame::OnMenuListGuidSelected(wxCommandEvent& event)
{
    DlgGuidList *dlg=new DlgGuidList(this);
    dlg->ShowModal(disk);

    delete dlg;
}

void MainFrame::OnMenuAboutSelected(wxCommandEvent& event)
{
  wxMessageBox( _("Version 0.2"),_("About"),wxOK | wxICON_INFORMATION, this );
}

// -------------------------------------------------------

void MainFrame::OnPartitionListItemActivated(wxListEvent& event)
{

	if(disk_structure[event.GetIndex()].selection_type==S_FREE_SPACE)
	  {
	      menuPartition->Enable(ID_DELETE_PARTITION,false);
	      menuPartition->Enable(ID_SET_ACTIVE,false);
	      menuPartition->Enable(ID_SET_INACTIVE,false);
	      menuPartition->Enable(ID_CREATE_PARTITION,true);
	      menuPartition->Enable(ID_WIPE_PARTITION,false);
          menuPartition->Enable(ID_PARTITION_PROPERTIES,false);

	      selected_frs=disk_structure[event.GetIndex()].num;
	  }
	else
	  {
	     menuPartition->Enable(ID_DELETE_PARTITION,true);
	     menuPartition->Enable(ID_SET_ACTIVE,true);
	     menuPartition->Enable(ID_SET_INACTIVE,true);
	     menuPartition->Enable(ID_CREATE_PARTITION,false);
	     menuPartition->Enable(ID_WIPE_PARTITION,true);
	     menuPartition->Enable(ID_PARTITION_PROPERTIES,true);

	     selected_partition=disk_structure[event.GetIndex()].num;
	  }
}

void MainFrame::OnPartitionListItemRClick(wxListEvent& event)
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
         menu.Append(ID_WIPE_PARTITION,"Wipe partition");
         menu.Append(ID_DELETE_PARTITION,"Delete partition");
         menu.AppendSeparator();
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
    refresh_partition_list();
}

void MainFrame::OnMenuPartitionPropertiesSelected(wxCommandEvent& event)
{
  DlgPartitionProperties *dlg=new DlgPartitionProperties(this);
  dlg->ShowModal(disk,selected_partition);

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
