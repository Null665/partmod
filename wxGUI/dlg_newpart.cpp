#include "dlg_newpart.hpp"
#include "../Partmod/definitions.h"
#include "../Partmod/disk_exception.h"
#include <wx/wx.h>
//(*InternalHeaders(DlgNewPart)
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

//(*IdInit(DlgNewPart)
const long DlgNewPart::ID_SPINCTRL1 = wxNewId();
const long DlgNewPart::ID_CHOICE1 = wxNewId();
const long DlgNewPart::ID_STATICTEXT1 = wxNewId();
const long DlgNewPart::ID_CHOICE2 = wxNewId();
const long DlgNewPart::ID_STATICTEXT2 = wxNewId();
const long DlgNewPart::ID_CHOICE3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgNewPart,wxDialog)
	//(*EventTable(DlgNewPart)
	//*)
	EVT_BUTTON(wxID_OK, DlgNewPart::OnButtonOKClick)
END_EVENT_TABLE()

DlgNewPart::DlgNewPart(wxWindow* parent)
{
	//(*Initialize(DlgNewPart)
	wxGridSizer* GridSizer1;
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _("Create a new partition"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Partition size"));
	SpinCtrlPartitionSize = new wxSpinCtrl(this, ID_SPINCTRL1, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL1"));
	SpinCtrlPartitionSize->SetValue(_T("0"));
	StaticBoxSizer1->Add(SpinCtrlPartitionSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ChoiceSizeMul = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	StaticBoxSizer1->Add(ChoiceSizeMul, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer1 = new wxGridSizer(2, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Partition type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	GridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	ChoicePartitionType = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
	GridSizer1->Add(ChoicePartitionType, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("File system type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	GridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	ChoiceFsType = new wxChoice(this, ID_CHOICE3, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
	GridSizer1->Add(ChoiceFsType, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(GridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, _("Create")));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

DlgNewPart::~DlgNewPart()
{
	//(*Destroy(DlgNewPart)
	//*)
}

int DlgNewPart::ShowModal(Disk *disk, int selected_frs)
{
  this->selected_frs=selected_frs;
  this->disk=disk;

  ChoiceSizeMul->Append( _("KB"),(void*)KB);
  ChoiceSizeMul->Append( _("MB"),(void*)MB);
  ChoiceSizeMul->Append( _("GB"),(void*)GB);

  const GEOMETRY &dg=disk->GetDiskGeometry();

  ChoiceSizeMul->Append( _("Sectors"),(void*)dg.bps);
  ChoiceSizeMul->Append( _("Tracks (Heads)"),(void*)uint64_t(dg.bps*dg.spt));
  ChoiceSizeMul->Append( _("Cylinders"),(void*)uint64_t(dg.bps*dg.spt*dg.tpc));

  uint64_t nsect_size=disk->GetFreeSpace(selected_frs).length;

  if(nsect_size> (GB/disk->GetDiskGeometry().bps)*2)
    {
      ChoiceSizeMul->SetSelection(2);
      SpinCtrlPartitionSize->SetValue(nsect_size/(GB/disk->GetDiskGeometry().bps));
    }
  else if(nsect_size> (MB/disk->GetDiskGeometry().bps))
    {
      ChoiceSizeMul->SetSelection(1);
      SpinCtrlPartitionSize->SetValue(nsect_size/(MB/disk->GetDiskGeometry().bps));
    }
  else if(nsect_size> (KB/disk->GetDiskGeometry().bps))
    {
      ChoiceSizeMul->SetSelection(0);
      SpinCtrlPartitionSize->SetValue(nsect_size/(KB/disk->GetDiskGeometry().bps));
    }


  int frs_type=disk->GetFreeSpace(selected_frs).type;

  if(frs_type==FREE_UNALLOCATED)
    {
      if(disk->CountPartitions(PART_PRIMARY | PART_EXTENDED | PART_MBR_GPT)<4)
          ChoicePartitionType->Append(_("Primary"),(void*)PART_PRIMARY);
      if(disk->CountPartitions(PART_EXTENDED)==0)
          ChoicePartitionType->Append(_("Extended"),(void*)PART_EXTENDED);
      if(disk->CountPartitions(PART_MBR_GPT)==0)
          ChoicePartitionType->Append(_("GPT partition table"),(void*)PART_MBR_GPT);
    }
  else if(frs_type==FREE_EXTENDED)
    {
       ChoicePartitionType->Append(_("Logical MBR"),(void*)PART_LOGICAL);
    }
  else if(frs_type==FREE_GPT)
    {
       ChoicePartitionType->Append(_("Logical GPT"),(void*)PART_GPT);
    }
  ChoicePartitionType->SetSelection(0);
  wxCommandEvent wcmd;
  wcmd.SetClientData(ChoicePartitionType->GetClientData(0));
  OnChoicePartitionTypeSelect(wcmd);

  wxDialog::ShowModal();
}

void DlgNewPart::OnButtonOKClick(wxCommandEvent& event)
{
  uint64_t size_mul=(int)ChoiceSizeMul->GetClientData(ChoiceSizeMul->GetSelection());
  int part_type=(int)ChoicePartitionType->GetClientData(ChoicePartitionType->GetSelection());
  uint64_t size=SpinCtrlPartitionSize->GetValue()*(size_mul/disk->GetDiskGeometry().bps);
  unsigned fs_type=(unsigned)ChoiceFsType->GetClientData(ChoiceFsType->GetSelection());

  try
   {
       switch(part_type)
         {
           case PART_PRIMARY:
               disk->CreatePartitionPrimary(selected_frs,size,0,fs_type);
               break;
           case PART_EXTENDED:
               disk->CreatePartitionExtended(selected_frs,size,0,fs_type);
               break;
           case PART_LOGICAL:
               disk->CreatePartitionLogical(selected_frs,size,0,fs_type);
               break;
           case PART_MBR_GPT:
               disk->CreatePartitionPrimaryGPT(selected_frs,size,0);
               break;
           case PART_GPT:
               disk->CreatePartitionGPT(selected_frs,size,0,disk->guid_man->Get(fs_type));
               break;

         }
     //  disk->CreatePartition(disk->GetFreeSpace(selected_frs),part_type,size);
   }
  catch(DiskException &de)
   {
     wxMessageBox( _(de.what()),_("Error"),wxOK | wxICON_ERROR, this );
     return;
   }
  catch(...)
   {
     wxMessageBox( _("Unknown error"),_("Error"),wxOK | wxICON_ERROR, this );
     return;
   }
   this->Destroy();
}

void DlgNewPart::OnButtonCancelClick(wxCommandEvent& event)
{
   this->Destroy();
}

void DlgNewPart::OnChoicePartitionTypeSelect(wxCommandEvent& event)
{
   ChoiceFsType->Clear();
  if((int)event.GetClientData()==PART_GPT)
    {
       for(int i=0;i<disk->guid_man->Count();i++)
         {
           ChoiceFsType-> Append(_(disk->guid_man->GetDescription(i).c_str()));
           ChoiceFsType->SetClientData(i,(void*)i);
         }
       ChoiceFsType->SetSelection(5);
    }
  else if((int)event.GetClientData()==PART_PRIMARY)
    {
       for(int i=0;i<disk->fsid_man->Count();i++)
         {
           ChoiceFsType-> Append(_(disk->fsid_man->GetDescription(i).c_str()));
           ChoiceFsType->SetClientData(i,(void*)disk->fsid_man->Get(i).fsid);
         }

       ChoiceFsType->SetSelection(5);
    }
  else if((int)event.GetClientData()==PART_EXTENDED)
    {
       for(int i=0;i<disk->fsid_man->Count(FS_EXTENDED);i++)
         {
           ChoiceFsType-> Append(_(disk->fsid_man->GetByPartmodFsid(FS_EXTENDED,i).description.c_str()));
           ChoiceFsType->SetClientData(i,(void*)disk->fsid_man->GetByPartmodFsid(FS_EXTENDED,i).fsid);
         }

       ChoiceFsType->SetSelection(0);
    }
  else if((int)event.GetClientData()==PART_MBR_GPT)
    {
       for(int i=0;i<disk->fsid_man->Count(FS_GPT);i++)
         {
           ChoiceFsType-> Append(_(disk->fsid_man->GetByPartmodFsid(FS_GPT,i).description.c_str()));
           ChoiceFsType->SetClientData(i,(void*)disk->fsid_man->GetByPartmodFsid(FS_GPT,i).fsid);
         }

       ChoiceFsType->SetSelection(0);
    }
  else
    {
       for(int i=0;i<disk->fsid_man->Count();i++)
         {
           ChoiceFsType-> Append(_(disk->fsid_man->GetDescription(i).c_str()));
           ChoiceFsType->SetClientData(i,(void*)disk->fsid_man->Get(i).fsid);
         }

       ChoiceFsType->SetSelection(5);
    }
}

