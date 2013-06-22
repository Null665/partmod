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
const long DlgNewPart::ID_CUSTOM1 = wxNewId();
const long DlgNewPart::ID_STATICTEXT4 = wxNewId();
const long DlgNewPart::ID_PARTITION_SIZE = wxNewId();
const long DlgNewPart::ID_CHOICE1 = wxNewId();
const long DlgNewPart::ID_STATICTEXT5 = wxNewId();
const long DlgNewPart::ID_FREE_PREPENDING = wxNewId();
const long DlgNewPart::ID_STATICTEXT6 = wxNewId();
const long DlgNewPart::ID_FREE_FOLLOWING = wxNewId();
const long DlgNewPart::ID_STATICTEXT1 = wxNewId();
const long DlgNewPart::ID_CHOICE2 = wxNewId();
const long DlgNewPart::ID_STATICTEXT2 = wxNewId();
const long DlgNewPart::ID_CHOICE3 = wxNewId();
const long DlgNewPart::ID_STATICTEXT3 = wxNewId();
const long DlgNewPart::ID_CHOICE4 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgNewPart,wxDialog)
	//(*EventTable(DlgNewPart)
	//*)
	EVT_BUTTON(wxID_OK, DlgNewPart::OnButtonOKClick)
END_EVENT_TABLE()

DlgNewPart::DlgNewPart(wxWindow* parent)
{
	//(*Initialize(DlgNewPart)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer2;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _("Create a new partition"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(3, 1, 0, 0);
	FlexGridSizer1->AddGrowableRow(1);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Partition size"));
	partitionView = new wxPartition(this,ID_CUSTOM1,wxDefaultPosition,wxSize(400,74),0,_T("ID_CUSTOM1"));
	StaticBoxSizer1->Add(partitionView, 0, wxALL|wxEXPAND|wxSHAPED|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3 = new wxFlexGridSizer(3, 3, 0, 0);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Partition size:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer3->Add(StaticText4, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlPartitionSize = new wxSpinCtrl(this, ID_PARTITION_SIZE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100000000, 0, _T("ID_PARTITION_SIZE"));
	SpinCtrlPartitionSize->SetValue(_T("0"));
	FlexGridSizer3->Add(SpinCtrlPartitionSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ChoiceSizeMul = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	ChoiceSizeMul->SetToolTip(_("Select units that are used to display partition size, free space prepending and free space following"));
	FlexGridSizer3->Add(ChoiceSizeMul, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Free space prepending:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer3->Add(StaticText5, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlFreePrepending = new wxSpinCtrl(this, ID_FREE_PREPENDING, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100000000, 0, _T("ID_FREE_PREPENDING"));
	SpinCtrlFreePrepending->SetValue(_T("0"));
	FlexGridSizer3->Add(SpinCtrlFreePrepending, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(-1,-1,1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Free space following:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer3->Add(StaticText6, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlFreeFollowing = new wxSpinCtrl(this, ID_FREE_FOLLOWING, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100000000, 0, _T("ID_FREE_FOLLOWING"));
	SpinCtrlFreeFollowing->SetValue(_T("0"));
	FlexGridSizer3->Add(SpinCtrlFreeFollowing, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer3, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Basic properties"));
	FlexGridSizer2 = new wxFlexGridSizer(3, 2, 0, 0);
	FlexGridSizer2->AddGrowableRow(1);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Partition type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer2->Add(StaticText1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	ChoicePartitionType = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
	ChoicePartitionType->SetHelpText(_("Can be one of the following: \"Primary\", \"Extended\", \"Logical\"\nOn an empty disk, \"GPT partition table\" option exists, which allows to create an empty GUID partition table"));
	FlexGridSizer2->Add(ChoicePartitionType, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("File system type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer2->Add(StaticText2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	ChoiceFsType = new wxChoice(this, ID_CHOICE3, wxDefaultPosition, wxSize(166,29), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
	ChoiceFsType->SetHelpText(_("File system ID for MBR partitions.\nGUID for GPT partitions."));
	FlexGridSizer2->Add(ChoiceFsType, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Alignment"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer2->Add(StaticText3, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	ChoiceAlignTo = new wxChoice(this, ID_CHOICE4, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
	FlexGridSizer2->Add(ChoiceAlignTo, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	FlexGridSizer1->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	partitionView->Connect(wxEVT_MOTION,(wxObjectEventFunction)&DlgNewPart::OnPartitionViewMouseMove,0,this);
	Connect(ID_PARTITION_SIZE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DlgNewPart::OnSpinCtrlPartitionSizeChange);
	Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&DlgNewPart::OnChoiceSizeMulSelect);
	Connect(ID_FREE_PREPENDING,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DlgNewPart::OnSpinCtrlFreePrependingChange);
	Connect(ID_FREE_FOLLOWING,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DlgNewPart::OnSpinCtrlFreeFollowingChange);
	Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&DlgNewPart::OnChoicePartitionTypeSelect);
	//*)



	ChoiceAlignTo->Append(_("Sector"),(void*)ALIGN_SECTOR);
	ChoiceAlignTo->Append(_("Track (head)"),(void*)ALIGN_TRACK);
	ChoiceAlignTo->SetSelection(ChoiceAlignTo->Append(_("Megabyte"),(void*)ALIGN_MEGABYTE));
	ChoiceAlignTo->Append(_("Cylinder"),(void*)ALIGN_CYLINDER);
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

  const GEOMETRY &dg=disk->GetDiskGeometry();

  ChoiceSizeMul->Append( _("KB"),(void*)KB);
  ChoiceSizeMul->Append( _("MB"),(void*)MB);
  ChoiceSizeMul->Append( _("GB"),(void*)GB);
  ChoiceSizeMul->Append( _("Sectors"),(void*)dg.bps);
  ChoiceSizeMul->Append( _("Tracks (Heads)"),(void*)uint64_t(dg.bps*dg.spt));
  ChoiceSizeMul->Append( _("Cylinders"),(void*)uint64_t(dg.bps*dg.spt*dg.tpc));

  uint64_t nsect_size=disk->GetFreeSpace(selected_frs).length;
/*
  if(nsect_size> (GB/disk->GetDiskGeometry().bps)*2)
    {
      ChoiceSizeMul->SetSelection(2);
      SpinCtrlPartitionSize->SetValue(nsect_size/(GB/disk->GetDiskGeometry().bps));
    }
  else*/ if(nsect_size> (MB/disk->GetDiskGeometry().bps))
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
      if(disk->CountPartitions()==0)
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

  return wxDialog::ShowModal();
}

void DlgNewPart::OnButtonOKClick(wxCommandEvent& event)
{
  uint64_t size_mul=(uint64_t)ChoiceSizeMul->GetClientData(ChoiceSizeMul->GetSelection());
  int part_type=(int)ChoicePartitionType->GetClientData(ChoicePartitionType->GetSelection());
  uint64_t size=SpinCtrlPartitionSize->GetValue()*(size_mul/disk->GetDiskGeometry().bps);
  unsigned fs_type=(unsigned)ChoiceFsType->GetClientData(ChoiceFsType->GetSelection());
  unsigned alignment_type=(unsigned)ChoiceAlignTo->GetClientData(ChoiceAlignTo->GetSelection());

  uint64_t sect_before=SpinCtrlFreePrepending->GetValue()*(size_mul/disk->GetDiskGeometry().bps);


  try
   {
       switch(part_type)
         {
           case PART_PRIMARY:
               disk->CreatePartitionPrimary(selected_frs,size,sect_before,fs_type,alignment_type);
               break;
           case PART_EXTENDED:
               disk->CreatePartitionExtended(selected_frs,size,sect_before,fs_type,alignment_type);
               break;
           case PART_LOGICAL:
               disk->CreatePartitionLogical(selected_frs,size,sect_before,fs_type,alignment_type);
               break;
           case PART_MBR_GPT:
               disk->CreatePartitionPrimaryGPT(selected_frs,size);
               break;
           case PART_GPT:
               disk->CreatePartitionGPT(selected_frs,size,sect_before,disk->guid_man->Get(fs_type),alignment_type);
               break;

         }
   }
  catch(DiskException &de)
   {
     wxMessageBox( de.what(),_("Error"),wxOK | wxICON_ERROR, this );
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
           ChoiceFsType-> Append(disk->guid_man->GetDescription(i).c_str());
           ChoiceFsType->SetClientData(i,(void*)i);
         }
       ChoiceFsType->SetSelection(5);
    }
  else if((int)event.GetClientData()==PART_PRIMARY)
    {
       for(int i=0;i<disk->fsid_man->Count();i++)
         {
           ChoiceFsType-> Append(disk->fsid_man->GetDescription(i).c_str());
           ChoiceFsType->SetClientData(i,(void*)disk->fsid_man->Get(i).fsid);
         }

       ChoiceFsType->SetSelection(5);
    }
  else if((int)event.GetClientData()==PART_EXTENDED)
    {
       for(int i=0;i<disk->fsid_man->Count(FS_EXTENDED);i++)
         {
           ChoiceFsType-> Append(disk->fsid_man->GetByPartmodFsid(FS_EXTENDED,i).description.c_str());
           ChoiceFsType->SetClientData(i,(void*)disk->fsid_man->GetByPartmodFsid(FS_EXTENDED,i).fsid);
         }

       ChoiceFsType->SetSelection(1);
    }
  else if((int)event.GetClientData()==PART_MBR_GPT)
    {
       for(int i=0;i<disk->fsid_man->Count(FS_GPT);i++)
         {
           ChoiceFsType-> Append(disk->fsid_man->GetByPartmodFsid(FS_GPT,i).description.c_str());
           ChoiceFsType->SetClientData(i,(void*)disk->fsid_man->GetByPartmodFsid(FS_GPT,i).fsid);
         }
       SpinCtrlPartitionSize->SetValue(U64ToStr(disk->GetFreeSpace(selected_frs).length));
       ChoiceSizeMul->Select(3);
       ChoiceSizeMul->Disable();
       SpinCtrlPartitionSize->Disable();

       ChoiceFsType->SetSelection(0);
    }
  else
    {
       for(int i=0;i<disk->fsid_man->Count();i++)
         {
           ChoiceFsType-> Append(disk->fsid_man->GetDescription(i).c_str());
           ChoiceFsType->SetClientData(i,(void*)disk->fsid_man->Get(i).fsid);
         }

       ChoiceFsType->SetSelection(5);
    };

    if((int)event.GetClientData()!=PART_MBR_GPT)
      {
        ChoiceSizeMul->Enable();
        SpinCtrlPartitionSize->Enable();
      }

}


void DlgNewPart::OnPartitionViewMouseMove(wxMouseEvent& event)
{
 if(event.LeftIsDown())
   {
     int lpos= partitionView->GetLeftSliderPos();
     int rpos= partitionView->GetRightSliderPos();
     int n_steps=partitionView->GetNumberOfSteps();

     uint64_t units=(uint64_t)ChoiceSizeMul->GetClientData(ChoiceSizeMul->GetSelection());


     double step_value=(long double)(disk->GetFreeSpace(selected_frs).length*disk->GetDiskGeometry().bps)/(double)n_steps;

     uint64_t free_prepending=step_value*lpos/units;
     uint64_t free_following=step_value*(n_steps-rpos)/units;
     uint64_t part_size=step_value*(rpos-lpos)/units;

     SpinCtrlFreePrepending->SetValue(free_prepending);
     SpinCtrlFreeFollowing->SetValue(free_following);
     SpinCtrlPartitionSize->SetValue(part_size);

    // let the partitionView control to process the message
    event.Skip();
   }
}

void DlgNewPart::OnSpinCtrlFreePrependingChange(wxSpinEvent& event)
{
 uint64_t free_prepending= SpinCtrlFreePrepending->GetValue();
 int n_steps=partitionView->GetNumberOfSteps();

 uint64_t units=(uint64_t)ChoiceSizeMul->GetClientData(ChoiceSizeMul->GetSelection())/disk->GetDiskGeometry().bps;
 double step_value=(long double)(disk->GetFreeSpace(selected_frs).length)/(double)n_steps;

 uint64_t lpos=(free_prepending*units/step_value);
 partitionView->SetLeftSliderPos(lpos);

}



void DlgNewPart::OnChoiceSizeMulSelect(wxCommandEvent& event)
{
     int lpos= partitionView->GetLeftSliderPos();
     int rpos= partitionView->GetRightSliderPos();
     int n_steps=partitionView->GetNumberOfSteps();

     uint64_t units=(uint64_t)ChoiceSizeMul->GetClientData(ChoiceSizeMul->GetSelection());


     double step_value=(long double)(disk->GetFreeSpace(selected_frs).length*disk->GetDiskGeometry().bps)/(double)n_steps;

     uint64_t free_prepending=step_value*lpos/units;
     uint64_t free_following=step_value*(n_steps-rpos)/units;
     uint64_t part_size=step_value*(rpos-lpos)/units;

     SpinCtrlFreePrepending->SetValue(free_prepending);
     SpinCtrlFreeFollowing->SetValue(free_following);
     SpinCtrlPartitionSize->SetValue(part_size);
}

void DlgNewPart::OnSpinCtrlFreeFollowingChange(wxSpinEvent& event)
{
 uint64_t free_following= SpinCtrlFreeFollowing->GetValue();
 int n_steps=partitionView->GetNumberOfSteps();

 uint64_t units=(uint64_t)ChoiceSizeMul->GetClientData(ChoiceSizeMul->GetSelection())/disk->GetDiskGeometry().bps;
 double step_value=(long double)(disk->GetFreeSpace(selected_frs).length)/(double)n_steps;

 uint64_t rpos=(free_following*units/step_value);
 partitionView->SetRightSliderPos(n_steps-rpos);

}

void DlgNewPart::OnSpinCtrlPartitionSizeChange(wxSpinEvent& event)
{
  uint64_t free_following=SpinCtrlFreeFollowing->GetValue();
  uint64_t free_prepending=SpinCtrlFreePrepending->GetValue();
  uint64_t partition_size=SpinCtrlPartitionSize->GetValue();

  static uint64_t partition_size_old=SpinCtrlPartitionSize->GetValue();
  if(partition_size_old<partition_size)
  {
      if(free_following>0)
        {
          SpinCtrlFreeFollowing->SetValue(free_following-(partition_size-partition_size_old));

          wxSpinEvent unused_param;
          unused_param.SetInt(free_following-(partition_size-partition_size_old));
          OnSpinCtrlFreeFollowingChange(unused_param);
        }
       else if(free_prepending>0)
         {
           SpinCtrlFreePrepending->SetValue(free_prepending-(partition_size-partition_size_old));

           wxSpinEvent unused_param;
           unused_param.SetInt(free_prepending-(partition_size-partition_size_old));
           OnSpinCtrlFreePrependingChange(unused_param);
         }
       else
         {
            SpinCtrlPartitionSize->SetValue(partition_size_old);
         }
  }
  else
  {
      if(free_following>=0)
        {
          SpinCtrlFreeFollowing->SetValue(free_following+(partition_size_old-partition_size));

          wxSpinEvent unused_param;
          OnSpinCtrlFreeFollowingChange(unused_param);
        }
       else if(free_prepending>=0)
         {
           SpinCtrlFreePrepending->SetValue(free_prepending+(partition_size_old-partition_size));

           wxSpinEvent unused_param;
           OnSpinCtrlFreePrependingChange(unused_param);
         }
       else
         {
            SpinCtrlPartitionSize->SetValue(partition_size);
         }
  }

   partition_size_old=SpinCtrlPartitionSize->GetValue();
}
