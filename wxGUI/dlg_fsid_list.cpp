#include "dlg_fsid_list.h"
#include "../Partmod/numstr.h"

//(*InternalHeaders(DlgFsidList)
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)
#include <wx/msgdlg.h>

//(*IdInit(DlgFsidList)
const long DlgFsidList::ID_STATICTEXT1 = wxNewId();
const long DlgFsidList::ID_LISTVIEW1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgFsidList,wxDialog)
	//(*EventTable(DlgFsidList)
	//*)
END_EVENT_TABLE()

DlgFsidList::DlgFsidList(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DlgFsidList)
	wxFlexGridSizer* FlexGridSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _("Known MBR file system IDs"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(2, 1, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	fsidList = new wxListView(this, ID_LISTVIEW1, wxDefaultPosition, wxSize(478,336), wxLC_REPORT, wxDefaultValidator, _T("ID_LISTVIEW1"));
	FlexGridSizer1->Add(fsidList, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	FlexGridSizer1->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	//*)
}

DlgFsidList::~DlgFsidList()
{
	//(*Destroy(DlgFsidList)
	//*)
}

int DlgFsidList::ShowModal(Disk *disk)
{
    fsidList->InsertColumn(0,_("FSID (hex)"),0,50);
    fsidList->InsertColumn(1,_("Min. size"),0,80);
    fsidList->InsertColumn(2,_("Max. size"),0,80);
    fsidList->InsertColumn(3,_("Description"),0,200);

    for(unsigned i=0;i<disk->fsid_man->Count();i++)
      {
          fsidList->InsertItem(i,to_string(disk->fsid_man->Get(i).fsid,STR_HEX));

          if(disk->fsid_man->Get(i).min_size>-1)
              fsidList->SetItem(i,1,size_str(disk->fsid_man->Get(i).min_size,1));
          else fsidList->SetItem(i,1,"-");

          if(disk->fsid_man->Get(i).max_size>-1)
              fsidList->SetItem(i,2,size_str(disk->fsid_man->Get(i).max_size,1));
          else fsidList->SetItem(i,2,"-");

          fsidList->SetItem(i,3,disk->fsid_man->GetDescription(i));
      }

    return wxDialog::ShowModal();
}
