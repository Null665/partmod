#ifndef DLG_FSID_LIST_H
#define DLG_FSID_LIST_H

//(*Headers(DlgFsidList)
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/dialog.h>
//*)
#include "../Partmod/disk.h"

class DlgFsidList: public wxDialog
{
	public:

		DlgFsidList(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DlgFsidList();

		//(*Declarations(DlgFsidList)
		wxListCtrl* fsidList;
		wxStaticText* StaticText1;
		//*)
		int ShowModal(Disk *disk);
	protected:

		//(*Identifiers(DlgFsidList)
		static const long ID_STATICTEXT1;
		static const long ID_LISTVIEW1;
		//*)

	private:

		//(*Handlers(DlgFsidList)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
