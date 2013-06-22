#ifndef DLG_PARTITION_PROPERTIES_H
#define DLG_PARTITION_PROPERTIES_H

//(*Headers(DlgPartitionProperties)
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)
#include "../Partmod/disk.h"

class DlgPartitionProperties: public wxDialog
{
	public:
        int ShowModal(Disk *disk, int part);


		DlgPartitionProperties(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DlgPartitionProperties();

		//(*Declarations(DlgPartitionProperties)
		wxStaticText* StaticText9;
		wxStaticText* StaticText2;
		wxButton* Button1;
		wxStaticText* StaticText6;
		wxStaticText* StaticText8;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxStaticBox* StaticBox1;
		wxTextCtrl* TextFsIdString;
		wxTextCtrl* TextPartitionName;
		wxTextCtrl* TextUniqueGuid;
		wxStaticBox* StaticBox2;
		wxTextCtrl* TextFsId;
		wxTextCtrl* TextPartitionType;
		wxStaticText* StaticText5;
		wxStaticText* StaticText7;
		wxTextCtrl* TextTypeGuid;
		wxTextCtrl* TextBeginSector;
		wxStaticBox* StaticBox3;
		wxTextCtrl* TextLength;
		wxTextCtrl* TextBootableFlag;
		wxStaticText* StaticText4;
		wxTextCtrl* TextTypeGuidString;
		wxTextCtrl* TextEndSector;
		wxStaticBox* StaticBox4;
		//*)

	protected:

		//(*Identifiers(DlgPartitionProperties)
		static const long ID_STATICBOX1;
		static const long ID_STATICBOX2;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL3;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL4;
		static const long ID_STATICBOX3;
		static const long ID_STATICTEXT5;
		static const long ID_TEXTCTRL5;
		static const long ID_TEXTCTRL6;
		static const long ID_STATICTEXT6;
		static const long ID_TEXTCTRL7;
		static const long ID_STATICBOX4;
		static const long ID_STATICTEXT7;
		static const long ID_TEXTCTRL8;
		static const long ID_TEXTCTRL9;
		static const long ID_STATICTEXT8;
		static const long ID_TEXTCTRL10;
		static const long ID_STATICTEXT9;
		static const long ID_TEXTCTRL11;
		//*)

	private:

		//(*Handlers(DlgPartitionProperties)
		//*)
        Disk *disk;
		DECLARE_EVENT_TABLE()
};

#endif
