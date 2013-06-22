#include "dlg_partition_properties.h"

//(*InternalHeaders(DlgPartitionProperties)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(DlgPartitionProperties)
const long DlgPartitionProperties::ID_STATICBOX1 = wxNewId();
const long DlgPartitionProperties::ID_STATICBOX2 = wxNewId();
const long DlgPartitionProperties::ID_STATICTEXT1 = wxNewId();
const long DlgPartitionProperties::ID_TEXTCTRL1 = wxNewId();
const long DlgPartitionProperties::ID_STATICTEXT2 = wxNewId();
const long DlgPartitionProperties::ID_TEXTCTRL2 = wxNewId();
const long DlgPartitionProperties::ID_STATICTEXT3 = wxNewId();
const long DlgPartitionProperties::ID_TEXTCTRL3 = wxNewId();
const long DlgPartitionProperties::ID_STATICTEXT4 = wxNewId();
const long DlgPartitionProperties::ID_TEXTCTRL4 = wxNewId();
const long DlgPartitionProperties::ID_STATICBOX3 = wxNewId();
const long DlgPartitionProperties::ID_STATICTEXT5 = wxNewId();
const long DlgPartitionProperties::ID_TEXTCTRL5 = wxNewId();
const long DlgPartitionProperties::ID_TEXTCTRL6 = wxNewId();
const long DlgPartitionProperties::ID_STATICTEXT6 = wxNewId();
const long DlgPartitionProperties::ID_TEXTCTRL7 = wxNewId();
const long DlgPartitionProperties::ID_STATICBOX4 = wxNewId();
const long DlgPartitionProperties::ID_STATICTEXT7 = wxNewId();
const long DlgPartitionProperties::ID_TEXTCTRL8 = wxNewId();
const long DlgPartitionProperties::ID_TEXTCTRL9 = wxNewId();
const long DlgPartitionProperties::ID_STATICTEXT8 = wxNewId();
const long DlgPartitionProperties::ID_TEXTCTRL10 = wxNewId();
const long DlgPartitionProperties::ID_STATICTEXT9 = wxNewId();
const long DlgPartitionProperties::ID_TEXTCTRL11 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgPartitionProperties,wxDialog)
	//(*EventTable(DlgPartitionProperties)
	//*)
END_EVENT_TABLE()

DlgPartitionProperties::DlgPartitionProperties(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DlgPartitionProperties)
	Create(parent, wxID_ANY, _("Partition properties"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(538,500));
	StaticBox1 = new wxStaticBox(this, ID_STATICBOX1, _("Label"), wxPoint(112,104), wxSize(1,1), 0, _T("ID_STATICBOX1"));
	StaticBox2 = new wxStaticBox(this, ID_STATICBOX2, _("Basic properties"), wxPoint(16,24), wxSize(504,144), 0, _T("ID_STATICBOX2"));
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Begins at"), wxPoint(24,56), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	TextBeginSector = new wxTextCtrl(this, ID_TEXTCTRL1, _("#undefined"), wxPoint(24,72), wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	TextBeginSector->Disable();
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Ends at"), wxPoint(152,56), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	TextEndSector = new wxTextCtrl(this, ID_TEXTCTRL2, _("#undefined"), wxPoint(152,72), wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	TextEndSector->Disable();
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Length"), wxPoint(280,56), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	TextLength = new wxTextCtrl(this, ID_TEXTCTRL3, _("#undefined"), wxPoint(280,72), wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	TextLength->Disable();
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Partition type"), wxPoint(24,112), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	TextPartitionType = new wxTextCtrl(this, ID_TEXTCTRL4, _("#undefined"), wxPoint(24,128), wxSize(104,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	TextPartitionType->Disable();
	StaticBox3 = new wxStaticBox(this, ID_STATICBOX3, _("MBR partition data"), wxPoint(16,184), wxSize(504,88), 0, _T("ID_STATICBOX3"));
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("File system ID"), wxPoint(24,216), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	TextFsId = new wxTextCtrl(this, ID_TEXTCTRL5, _("#n/a"), wxPoint(24,232), wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	TextFsId->Disable();
	TextFsIdString = new wxTextCtrl(this, ID_TEXTCTRL6, _("#n/a"), wxPoint(128,232), wxSize(224,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL6"));
	TextFsIdString->Disable();
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Bootable flag"), wxPoint(376,216), wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	TextBootableFlag = new wxTextCtrl(this, ID_TEXTCTRL7, _("#n/a"), wxPoint(376,232), wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL7"));
	TextBootableFlag->Disable();
	StaticBox4 = new wxStaticBox(this, ID_STATICBOX4, _("GPT partition data"), wxPoint(16,288), wxSize(504,168), 0, _T("ID_STATICBOX4"));
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Type GUID"), wxPoint(24,312), wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	TextTypeGuid = new wxTextCtrl(this, ID_TEXTCTRL8, _("#n/a"), wxPoint(24,328), wxSize(208,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL8"));
	TextTypeGuid->Disable();
	TextTypeGuidString = new wxTextCtrl(this, ID_TEXTCTRL9, _("#n/a"), wxPoint(240,328), wxSize(256,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL9"));
	TextTypeGuidString->Disable();
	StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("Unique GUID"), wxPoint(24,360), wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	TextUniqueGuid = new wxTextCtrl(this, ID_TEXTCTRL10, _("#n/a"), wxPoint(24,376), wxSize(208,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL10"));
	TextUniqueGuid->Disable();
	StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("Partition name"), wxPoint(24,408), wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	TextPartitionName = new wxTextCtrl(this, ID_TEXTCTRL11, _("#n/a"), wxPoint(24,424), wxSize(208,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL11"));
	TextPartitionName->Disable();
	Button1 = new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(216,464), wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
	//*)
}

DlgPartitionProperties::~DlgPartitionProperties()
{
	//(*Destroy(DlgPartitionProperties)
	//*)
}

int DlgPartitionProperties::ShowModal(Disk *disk,int part)
{
  this->disk=disk;
  GEN_PART gpart=disk->GetPartition(part);

  TextBeginSector->SetValue(U64ToStr(gpart.begin_sector));
  TextEndSector->SetValue(U64ToStr(gpart.begin_sector+gpart.length));
  TextLength->SetValue(U64ToStr(gpart.length));

  if(gpart.flags&PART_PRIMARY)
      TextPartitionType->SetValue(_("Primary"));
  else if(gpart.flags&PART_EXTENDED)
      TextPartitionType->SetValue(_("Extended"));
  else if(gpart.flags&PART_LOGICAL)
      TextPartitionType->SetValue(_("Logical MBR"));
  else if(gpart.flags&PART_MBR_GPT)
      TextPartitionType->SetValue(_("GPT partition table"));
  else if(gpart.flags&PART_GPT)
      TextPartitionType->SetValue(_("GPT"));

  if(gpart.flags&PART_PRIMARY || gpart.flags&PART_EXTENDED || gpart.flags&PART_LOGICAL)
    {
      TextFsId->SetValue(U64ToStr(disk->GetMBRSpecific(part).fsid));
      TextFsIdString->SetValue(disk->fsid_man->GetByFsid(disk->GetMBRSpecific(part).fsid).description);

      if(gpart.flags&PART_ACTIVE)
           TextBootableFlag->SetValue("True");
      else TextBootableFlag->SetValue("False");
    }
  else if(gpart.flags&PART_GPT)
    {
      TextTypeGuid->SetValue(disk->guid_man->GuidToString(disk->GetGPTSpecific(part).type_guid));
      TextTypeGuidString->SetValue(disk->guid_man->GetDescription(disk->GetGPTSpecific(part).type_guid));

      TextUniqueGuid->SetValue(disk->guid_man->GuidToString(disk->GetGPTSpecific(part).unique_guid));

      TextPartitionName->SetValue((wxChar*)disk->GetGPTSpecific(part).name);


    }




  return wxDialog::ShowModal();
}
