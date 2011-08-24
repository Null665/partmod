#include "dlg_newpart.hpp"
#include "../Partmod/definitions.h"
#include "../Partmod/disk_exception.h"
#include <wx/wx.h>
//(*InternalHeaders(DlgNewPart)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(DlgNewPart)
const long DlgNewPart::IDA_HSHDGSDHUF = wxNewId();
const long DlgNewPart::ID_SIZE_MULTIPLIER = wxNewId();
const long DlgNewPart::ID_STATICTEXT1 = wxNewId();
const long DlgNewPart::ID_OK = wxNewId();
const long DlgNewPart::ID_CHOICE1 = wxNewId();
const long DlgNewPart::ID_STATICTEXT2 = wxNewId();
const long DlgNewPart::ID_CHOICE2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgNewPart,wxDialog)
	//(*EventTable(DlgNewPart)
	//*)
END_EVENT_TABLE()

DlgNewPart::DlgNewPart(wxWindow* parent)
{
	//(*Initialize(DlgNewPart)
	Create(parent, wxID_ANY, _("Create a new partition"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(370,215));
	SpinCtrlPartitionSize = new wxSpinCtrl(this, IDA_HSHDGSDHUF, _T("0"), wxPoint(48,64), wxDefaultSize, 0, 0, 10000000, 0, _T("IDA_HSHDGSDHUF"));
	SpinCtrlPartitionSize->SetValue(_T("0"));
	ChoiceSizeMul = new wxChoice(this, ID_SIZE_MULTIPLIER, wxPoint(184,64), wxSize(112,21), 0, 0, 0, wxDefaultValidator, _T("ID_SIZE_MULTIPLIER"));
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Enter size of partition:"), wxPoint(48,40), wxSize(112,13), 0, _T("ID_STATICTEXT1"));
	ButtonOK = new wxButton(this, ID_OK, _("OK"), wxPoint(72,144), wxDefaultSize, 0, wxDefaultValidator, _T("ID_OK"));
	ButtonCancel = new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(184,144), wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
	ChoicePartitionType = new wxChoice(this, ID_CHOICE1, wxPoint(48,112), wxSize(120,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Select partition type:"), wxPoint(48,96), wxSize(112,13), 0, _T("ID_STATICTEXT2"));
	ChoiceFsType = new wxChoice(this, ID_CHOICE2, wxPoint(184,112), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));

	Connect(ID_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgNewPart::OnButtonOKClick);
	Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&DlgNewPart::OnChoicePartitionTypeSelect);
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
      ChoicePartitionType->Append(_("Primary"),(void*)PART_PRIMARY);
    //  ChoicePartitionType->Append(_("GPT container"),(void*)PART_MBR_GPT);
      ChoicePartitionType->Append(_("Extended"),(void*)PART_EXTENDED);
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
  uint64_t size_mul=(int)ChoiceSizeMul->GetClientObject(ChoiceSizeMul->GetSelection());
  int part_type=(int)ChoicePartitionType->GetClientObject(ChoicePartitionType->GetSelection());
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
